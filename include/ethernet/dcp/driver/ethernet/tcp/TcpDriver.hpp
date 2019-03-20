/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universit�t Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_TCP_DRIVER_H
#define DCPLIB_TCP_DRIVER_H

#define ASIO_STANDALONE


#include <dcp/driver/ethernet/tcp/helper/TcpHelper.hpp>
#include <iostream>
#include <chrono>
#include <thread>

class TcpDriver : public Logable {
public:

    TcpDriver(std::string host, uint16_t port) : mainPort(port), mainHost(host) {}

    ~TcpDriver() {
        closeConfiguredPorts();
        disconnect();
    }

    DcpDriver getDcpDriver() {
        return {[this](DcpPdu &msg) {
            this->send(msg);
        },
                [this](dcpId_t dcpId, uint8_t *info) {
                    setSlaveNetworkInformation(dcpId, *((uint16_t *) info), *((ip_address_t *) (info + 2)));
                },
                [this](dataId_t dataId, uint8_t *info) {
                    setSourceNetworkInformation(dataId, *((uint16_t *) info), *((ip_address_t *) (info + 2)));
                },
                [this](dataId_t dataId, uint8_t *info) {
                    setTargetNetworkInformation(dataId, *((uint16_t *) info), *((ip_address_t *) (info + 2)));
                },
                [this](paramId_t paramId, uint8_t *info) {
                    setParamNetworkInformation(paramId, *((uint16_t *) info), *((ip_address_t *) (info + 2)));
                },
                [this](paramId_t paramId, uint8_t *info) {
                    setTargetParamNetworkInformation(paramId, *((uint16_t *) info), *((ip_address_t *) (info + 2)));
                },
                std::bind(&TcpDriver::startReceiving, this),
                std::bind(&TcpDriver::connectToSlave, this, std::placeholders::_1),
                std::bind(&TcpDriver::disconnectFromSlave, this, std::placeholders::_1),
                [this](DcpManager manager) {
                    this->dcpManager = manager;
                },

                [this](const LogManager &logManager) {
                    setLogManager(logManager);
                },
                std::bind(&TcpDriver::registerSuccessfull, this),
                std::bind(&TcpDriver::openPorts, this),
                std::bind(&TcpDriver::connectToConfiguredPorts, this),
                std::bind(&TcpDriver::closeConfiguredPorts, this),
                std::bind(&TcpDriver::disconnect, this),
        };
    }

private:

    asio::io_service io_service;

    DcpManager dcpManager;
    uint16_t mainPort;
    std::string mainHost;

    std::shared_ptr<Server> mainServer;
    size_t mainSession;

    std::map<dataId_t, std::shared_ptr<Server>> ioServers;
    std::map<paramId_t, std::shared_ptr<Server>> parameterServers;
    std::map<dcpId_t, std::shared_ptr<Client>> otherSlaves;
    std::map<dataId_t, std::shared_ptr<Client>> ioClients;
    std::map<paramId_t, std::shared_ptr<Client>> parameterClients;

    inline std::shared_ptr<Client> getClient(asio::io_service &, port_t port, ip_address_t ip, DcpManager &dcpManager) {
        asio::ip::tcp::endpoint endpoint(asio::ip::address_v4(ip), port);
        for (const auto &it : otherSlaves) {
            if (it.second->getEndpoint() == endpoint) {
                return it.second;
            }
        }
        for (const auto &it : ioClients) {
            if (it.second->getEndpoint() == endpoint) {
                return it.second;
            }
        }
        for (const auto &it : parameterClients) {
            if (it.second->getEndpoint() == endpoint) {
                return it.second;
            }
        }
        return std::make_shared<Client>(io_service, endpoint, dcpManager, logManager);
    }

    inline std::shared_ptr<Server>
    getServer(asio::io_service &ios, port_t port, ip_address_t ip, DcpManager &manager, LogManager &_logManager) {
        asio::ip::tcp::endpoint endpoint(asio::ip::address_v4(ip), port);
        if (mainServer->getEndpoint() == endpoint ||
            (mainServer->getEndpoint().address().to_string() == "0.0.0.0" && mainServer->getEndpoint().port() == port)) {
            return mainServer;
        }
        for (const auto &it : ioServers) {
            if (it.second->getEndpoint() == endpoint) {
                return it.second;
            }
        }
        for (const auto &it : parameterServers) {
            if (it.second->getEndpoint() == endpoint) {
                return it.second;
            }
        }
        return std::make_shared<Server>(io_service, asio::ip::tcp::endpoint(asio::ip::address_v4(ip), port), dcpManager,
                                        logManager);
    }

    void send(DcpPdu &msg) {
        switch (msg.getTypeId()) {
            case DcpPduType::DAT_input_output: {
                DcpPduDatInputOutput &data = static_cast<DcpPduDatInputOutput &>(msg);
                if (ioClients.count(data.getDataId()) && ioClients[data.getDataId()]->getSession() != nullptr) {
                    ioClients[data.getDataId()]->getSession()->send(msg);
                }
                break;
            }
            case DcpPduType::DAT_parameter: {
                DcpPduDatParameter &param = static_cast<DcpPduDatParameter &>(msg);
                if (parameterClients.count(param.getParamId()) &&
                    parameterClients[param.getParamId()]->getSession() != nullptr) {
                    parameterClients[param.getParamId()]->getSession()->send(msg);
                }
                break;
            }
            case DcpPduType::NTF_state_changed:
            case DcpPduType::NTF_log: {
                if (mainServer->getSession(mainSession) != nullptr) {
                    mainServer->getSession(mainSession)->send(msg);
                }
                break;
            }
            case DcpPduType::RSP_ack:
            case DcpPduType::RSP_nack:
            case DcpPduType::RSP_state_ack:
            case DcpPduType::RSP_error_ack:
            case DcpPduType::RSP_log_ack: {
                if (mainServer->getLatestSession() != nullptr) {
                    mainServer->getLatestSession()->send(msg);
                }
                break;
            }
            default: {
                DcpPduBasic &basic = static_cast<DcpPduBasic &>(msg);
                if (otherSlaves.count(basic.getReceiver())) {
                    std::shared_ptr<Client> cl = otherSlaves[basic.getReceiver()];
                    if (!cl->isConnected()) {
                        cl->start();
                    }
                    cl->getSession()->send(msg);
                }
                break;
            }
        }
    }

    void setSlaveNetworkInformation(dcpId_t dcpId, port_t port, ip_address_t ip) {
        otherSlaves.insert(std::make_pair(dcpId, getClient(io_service, port, ip, dcpManager)));
    }

    void setSourceNetworkInformation(dataId_t dataId, port_t port, ip_address_t ip) {
        ioServers.insert(std::make_pair(dataId, getServer(io_service, port, ip, dcpManager, logManager)));
    }

    void setTargetNetworkInformation(dataId_t dataId, port_t port, ip_address_t ip) {
        ioClients.insert(std::make_pair(dataId, getClient(io_service, port, ip, dcpManager)));
    }

    void setParamNetworkInformation(paramId_t paramid, port_t port, ip_address_t ip) {
        parameterServers.insert(std::make_pair(paramid, getServer(io_service, port, ip, dcpManager, logManager)));
    }

    void setTargetParamNetworkInformation(paramId_t paramId, port_t port, ip_address_t ip) {
        parameterClients.insert(std::make_pair(paramId, getClient(io_service, port, ip, dcpManager)));
    }

    void startReceiving() {
        for (auto &pos: ioClients) {
            pos.second->setLogManager(logManager);
        }
        for (auto &pos: parameterClients) {
            pos.second->setLogManager(logManager);
        }
        for (auto &pos: ioServers) {
            pos.second->setLogManager(logManager);
        }
        for (auto &pos: parameterServers) {
            pos.second->setLogManager(logManager);
        }
        for (auto &pos: otherSlaves) {
            pos.second->setLogManager(logManager);
        }
        try {
            mainServer = std::make_shared<Server>(io_service,
                                                  asio::ip::tcp::endpoint(asio::ip::address_v4::from_string(mainHost),
                                                                          mainPort),
                                                  dcpManager,
                                                  logManager);
            mainServer->start();
            asio::io_service::work work(io_service);
            io_service.run();
        } catch (std::exception &e) {
            dcpManager.reportError(DcpError::PROTOCOL_ERROR_GENERIC);
#if defined(DEBUG) || defined(LOGGING)
            Log(NETWORK_PROBLEM, Tcp::protocolName, e.what());
#endif
        }
    }

    void connectToSlave(dcpId_t dcpId) {
        otherSlaves[dcpId]->start();
    }

    void disconnectFromSlave(dcpId_t dcpId) {
        otherSlaves.erase(dcpId);
    }

    void registerSuccessfull() {
        mainSession = mainServer->getLastSessionAccess();
#if defined(DEBUG)
        Log(NEW_MASTER_ENDPOINT, Tcp::protocolName,
            to_string(mainServer->getSession(mainSession)->getSocket().remote_endpoint()));
#endif
    }

    void openPorts() {
        try {
            for (auto &pos: ioServers) {
                pos.second->start();
            }
            for (auto &pos: parameterServers) {
                pos.second->start();
            }
        } catch (std::exception &e) {
            dcpManager.reportError(DcpError::PROTOCOL_ERROR_GENERIC);
#if defined(DEBUG) || defined(LOGGING)
            Log(NETWORK_PROBLEM, Tcp::protocolName, e.what());
#endif
        }
    }

    void connectToConfiguredPorts() {
        try {
            for (auto &pos: ioClients) {
                pos.second->start();
            }
            for (auto &pos: parameterClients) {
                pos.second->start();
            }
        } catch (std::exception &e) {
            dcpManager.reportError(DcpError::PROTOCOL_ERROR_GENERIC);
#if defined(DEBUG) || defined(LOGGING)
            Log(NETWORK_PROBLEM, Tcp::protocolName, e.what());
#endif
        }
    }

    void closeConfiguredPorts() {
        for (auto &pos: ioServers) {
            pos.second->cancel();
        }
        for (auto &pos: parameterServers) {
            pos.second->cancel();
        }
        ioClients.clear();
        parameterClients.clear();
        ioServers.clear();
        parameterServers.clear();
        mainServer->clearNonMainSessions(mainSession);
    }

    void disconnect() {
        mainServer->clearSessions();
    }

};

#endif //DCPLIB_TCP_DRIVER_H
