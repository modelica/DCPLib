/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
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
#include <dcp/driver/ethernet/ErrorCodes.hpp>
#include <dcp/logic/DcpManager.hpp>
#include <dcp/model/pdu/DcpPduFactory.hpp>

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

    Socket(asio::io_service &ios, asio::ip::udp::endpoint endpoint, DcpManager &dcpManager, LogManager &_logManager) :
            io_service(ios), endpoint(endpoint), dcpManager(dcpManager), started(false) {
        setLogManager(_logManager);
    }

    ~Socket() {}

    void send(DcpPdu &msg, asio::ip::udp::endpoint endpoint) {
#if defined(DEBUG)
        Log(PDU_SEND, msg.to_string());
#endif
        std::error_code error;
        try {
            socket->send_to(
                    asio::buffer(msg.serializePdu(), msg.getPduSize()),
                    endpoint, 0, error);
            if (error && error != asio::error::message_size) {
#if defined(DEBUG) || defined(LOGGING)
                Log(NETWORK_PROBLEM, Udp::protocolName, error.message());
#endif
                dcpManager.reportError(DcpError::PROTOCOL_ERROR_GENERIC);
            }
        } catch (std::exception &e) {
            dcpManager.reportError(DcpError::PROTOCOL_ERROR_GENERIC);
#if defined(DEBUG) || defined(LOGGING)
            Log(NETWORK_PROBLEM, Udp::protocolName, e.what());
#endif
        }
    }

    void handle_receive(const std::error_code &error, std::size_t bytes_transferred) {
        if (asio::error::connection_reset == error || asio::error::operation_aborted == error ||
            asio::error::eof == error) {
            //Socket is closed => stop receiving
            return;
        }

        if (error) {
            dcpManager.reportError(DcpError::PROTOCOL_ERROR_GENERIC);
#if defined(DEBUG) || defined(LOGGING)
            Log(NETWORK_PROBLEM, Udp::protocolName, error.message());
#endif
            return;
        }

        DcpPdu *pdu = makeDcpPdu(data, bytes_transferred);

#if defined(DEBUG)
        Log(PDU_RECEIVED, pdu->to_string());
#endif
        dcpManager.receive(*pdu);
        delete pdu;
        setup_receive();
    }

    void setup_receive() {
        socket->async_receive_from(asio::buffer(data + 4, maxLength), lastAccess,
                                   std::bind(&Socket::handle_receive, this,
                                             std::placeholders::_1,
                                             std::placeholders::_2));
    }

    const asio::ip::udp::endpoint &getLastAccess() const {
        return lastAccess;
    }

    void start() {
        if(!started){
            socket = std::unique_ptr<asio::ip::udp::socket>(new asio::ip::udp::socket(io_service, endpoint));
            setup_receive();
#if defined(DEBUG)
            Log(NEW_SOCKET, Udp::protocolName, to_string(endpoint));
#endif
            started = true;
        }
    }

    void close() {
        //one in asio queue, one existing in driver
        if(shared_from_this().use_count() == 2){
            socket->close();
#if defined(DEBUG)
            Log(SOCKET_CLOSED, Udp::protocolName, to_string(endpoint));
#endif
        }
    }

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