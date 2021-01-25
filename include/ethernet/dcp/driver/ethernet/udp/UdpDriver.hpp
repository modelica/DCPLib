/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universit�t Hannover, Germany
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

    DcpDriver getDcpDriver();

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

    void send(DcpPdu &msg);

    void setSlaveNetworkInformation(dcpId_t dcpId, port_t port, ip_address_t ip);

    void setSourceNetworkInformation(dataId_t dataId, port_t port, ip_address_t ip);

    void setTargetNetworkInformation(dataId_t dataId, port_t port, ip_address_t ip);

    void setParamNetworkInformation(paramId_t paramId, port_t port, ip_address_t ip);

    void setTargetParamNetworkInformation(paramId_t paramId, port_t port, ip_address_t ip);

    void startReceiving();

    void stopReceiving();

    void registerSuccessfull();

    void openPorts();

    void closeConfiguredPorts();
};

#endif //DCPLIB_UDPDRIVER_H