/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_UDPDRIVER_H
#define DCPLIB_UDPDRIVER_H

#define ASIO_STANDALONE

#include <dcp/driver/ethernet/udp/helper/UdpHelper.hpp>

#include <dcp/driver/DcpDriver.hpp>

class UdpDriver : public Logable {
public:
    UdpDriver(std::string host, uint16_t port) : mainPort(port), mainHost(host) {}

    ~UdpDriver() {}

    DcpDriver getDcpDriver() {
        return {[this](DcpPdu &msg) { this->send(msg); },
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
                std::bind(&UdpDriver::startReceiving, this),
                [this](dcpId_t) {/*nothing to do for connectionless UDP_IPv4*/ },
                [this](dcpId_t) {/*nothing to do for connectionless UDP_IPv4*/ },
                [this](DcpManager manager) {
                    this->dcpManager = manager;
                },

                [this](const LogManager &logManager) {
                    setLogManager(logManager);
                },
                std::bind(&UdpDriver::registerSuccessfull, this),
                std::bind(&UdpDriver::openPorts, this),
                [this]() {/*nothing to do for connectionless UDP_IPv4*/ },
                std::bind(&UdpDriver::closeConfiguredPorts, this),
                [this]() {/*nothing to do for connectionless UDP_IPv4*/ },
                std::bind(&UdpDriver::stopReceiving, this)
        };
    }

private:

    asio::io_service io_service;

    DcpManager dcpManager;
    uint16_t mainPort;
    std::string mainHost;

    asio::ip::udp::endpoint masterEndpoint;
    std::shared_ptr<Socket> mainSocket;

    std::map<dcpId_t, asio::ip::udp::endpoint> otherSlaves;
    std::map<dataId_t, asio::ip::udp::endpoint> ioOut;
    std::map<dataId_t, std::shared_ptr<Socket>> ioIn;
    std::map<paramId_t, std::shared_ptr<Socket>> paramIn;
    std::map<paramId_t, asio::ip::udp::endpoint> paramOut;

    inline std::shared_ptr<Socket>
    getSocket(port_t port, ip_address_t ip) {
        asio::ip::udp::endpoint endpoint(asio::ip::address_v4(ip), port);
        if (mainSocket->getEndpoint() == endpoint ||
          (mainSocket->getEndpoint().address().to_string() == "0.0.0.0" && mainSocket->getEndpoint().port() == port)) {
            return mainSocket;
        }
        for (const auto &it : ioIn) {
            if (it.second->getEndpoint() == endpoint) {
                return it.second;
            }
        }
        for (const auto &it : paramIn) {
            if (it.second->getEndpoint() == endpoint) {
                return it.second;
            }
        }
        return std::make_shared<Socket>(io_service, endpoint, dcpManager, logManager);
    }

    void send(DcpPdu &msg) {
        asio::ip::udp::endpoint endpoint;
        switch (msg.getTypeId()) {
            case DcpPduType::DAT_input_output: {
                DcpPduDatInputOutput &data = static_cast<DcpPduDatInputOutput &>(msg);
                endpoint = ioOut[data.getDataId()];
                break;
            }
            case DcpPduType::DAT_parameter: {
                DcpPduDatParameter &param = static_cast<DcpPduDatParameter &>(msg);
                endpoint = paramOut[param.getParamId()];
                break;
            }
            case DcpPduType::NTF_state_changed:
            case DcpPduType::NTF_log: {
                endpoint = masterEndpoint;
                break;
            }
            case DcpPduType::RSP_ack:
            case DcpPduType::RSP_nack:
            case DcpPduType::RSP_state_ack:
            case DcpPduType::RSP_error_ack:
            case DcpPduType::RSP_log_ack: {
                endpoint = mainSocket->getLastAccess();
                break;
            }
            default: {
                DcpPduBasic &basic = static_cast<DcpPduBasic &>(msg);
                endpoint = otherSlaves[basic.getReceiver()];
                break;
            }
        }
        mainSocket->send(msg, endpoint);
    }

    void setSlaveNetworkInformation(dcpId_t dcpId, port_t port, ip_address_t ip) {
        otherSlaves[dcpId] = asio::ip::udp::endpoint(asio::ip::address_v4(ip), port);
    }

    void setSourceNetworkInformation(dataId_t dataId, port_t port, ip_address_t ip) {
        ioIn[dataId] = getSocket(port, ip);
    }

    void setTargetNetworkInformation(dataId_t dataId, port_t port, ip_address_t ip) {
        ioOut[dataId] = asio::ip::udp::endpoint(asio::ip::address_v4(ip), port);
    }

    void setParamNetworkInformation(paramId_t paramId, port_t port, ip_address_t ip) {
        paramIn[paramId] = getSocket(port, ip);
    }

    void setTargetParamNetworkInformation(paramId_t paramId, port_t port, ip_address_t ip) {
        paramOut[paramId] = asio::ip::udp::endpoint(asio::ip::address_v4(ip), port);
    }

    void startReceiving() {
        for (auto &pos: ioIn) {
            pos.second->setLogManager(logManager);
        }
        for (auto &pos: paramIn) {
            pos.second->setLogManager(logManager);
        }
        mainSocket = std::make_shared<Socket>(io_service, asio::ip::udp::endpoint(asio::ip::address_v4::from_string(mainHost), mainPort), dcpManager, logManager);
        mainSocket->start();
        asio::io_service::work work(io_service);
        io_service.run();
    }

    void stopReceiving() {
        io_service.stop();
    }

    void registerSuccessfull() {
        masterEndpoint = mainSocket->getLastAccess();
#if defined(DEBUG)
        Log(NEW_MASTER_ENDPOINT, Udp::protocolName,
            to_string(masterEndpoint));
#endif
    }

    void openPorts() {
        for (auto &pos: ioIn) {
            pos.second->start();
        }
        for (auto &pos: paramIn) {
            pos.second->start();
        }
    }

    void closeConfiguredPorts() {
        for (auto &pos: ioIn) {
            pos.second->close();
        }
        ioIn.clear();
        for (auto &pos: paramIn) {
            pos.second->close();
        }
        paramIn.clear();
    }
};

#endif //DCPLIB_UDPDRIVER_H