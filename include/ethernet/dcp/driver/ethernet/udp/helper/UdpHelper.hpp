/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universit�t Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_UDPHELPER_H
#define DCPLIB_UDPHELPER_H

#include <asio.hpp>
#include <dcp/logic/Logable.hpp>
#include <dcp/logic/DcpManager.hpp>

namespace Udp {
    static std::string protocolName = "UDP_IPv4";
}

static std::string to_string(const asio::ip::udp::endpoint &remote_endpoint) {
    std::ostringstream oss;
    oss << remote_endpoint;
    return oss.str();
}

class Socket : public Logable, public std::enable_shared_from_this<Socket> {
public:

    Socket(asio::io_service &ios, asio::ip::udp::endpoint endpoint, DcpManager &dcpManager, LogManager &_logManager);

    ~Socket() {}

    void send(DcpPdu &msg, asio::ip::udp::endpoint endpoint);

    void handle_receive(const std::error_code &error, std::size_t bytes_transferred);

    void setup_receive();

    const asio::ip::udp::endpoint &getLastAccess() const {
        return lastAccess;
    }

    void start();

    void close();

    const asio::ip::udp::endpoint &getEndpoint() const {
        return endpoint;
    }

private:
    asio::io_service &io_service;
    asio::ip::udp::endpoint endpoint;
    std::unique_ptr<asio::ip::udp::socket> socket;
    DcpManager dcpManager;
    asio::ip::udp::endpoint lastAccess;
    enum {
        maxLength = 1024
    };
    uint8_t data[maxLength];
    bool started;
};

#endif //DCPLIB_UDPHELPER_H