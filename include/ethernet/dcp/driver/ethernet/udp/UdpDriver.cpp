#include "UdpDriver.hpp"

#include <dcp/driver/ethernet/ErrorCodes.hpp>
#include <dcp/model/pdu/DcpPduDatInputOutput.hpp>
#include <dcp/model/pdu/DcpPduDatParameter.hpp>
#include <dcp/model/pdu/DcpPduBasic.hpp>

#define ASIO_STANDALONE

DcpDriver UdpDriver::getDcpDriver() {
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

void UdpDriver::send(DcpPdu &msg) {
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

void UdpDriver::setSlaveNetworkInformation(dcpId_t dcpId, port_t port, ip_address_t ip) {
    otherSlaves[dcpId] = asio::ip::udp::endpoint(asio::ip::address_v4(ip), port);
}

void UdpDriver::setSourceNetworkInformation(dataId_t dataId, port_t port, ip_address_t ip) {
    ioIn[dataId] = getSocket(port, ip);
}

void UdpDriver::setTargetNetworkInformation(dataId_t dataId, port_t port, ip_address_t ip) {
    ioOut[dataId] = asio::ip::udp::endpoint(asio::ip::address_v4(ip), port);
}

void UdpDriver::setParamNetworkInformation(paramId_t paramId, port_t port, ip_address_t ip) {
    paramIn[paramId] = getSocket(port, ip);
}

void UdpDriver::setTargetParamNetworkInformation(paramId_t paramId, port_t port, ip_address_t ip) {
    paramOut[paramId] = asio::ip::udp::endpoint(asio::ip::address_v4(ip), port);
}

void UdpDriver::startReceiving() {
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

void UdpDriver::stopReceiving() {
    io_service.stop();
}

void UdpDriver::registerSuccessfull() {
    masterEndpoint = mainSocket->getLastAccess();
#if defined(DEBUG)
    Log(NEW_MASTER_ENDPOINT, Udp::protocolName,
        to_string(masterEndpoint));
#endif
}

void UdpDriver::openPorts() {
    for (auto &pos: ioIn) {
        pos.second->start();
    }
    for (auto &pos: paramIn) {
        pos.second->start();
    }
}

void UdpDriver::closeConfiguredPorts() {
    for (auto &pos: ioIn) {
        pos.second->close();
    }
    ioIn.clear();
    for (auto &pos: paramIn) {
        pos.second->close();
    }
    paramIn.clear();
}