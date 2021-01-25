#include "UdpHelper.hpp"

#include <asio.hpp>
#include <dcp/driver/ethernet/ErrorCodes.hpp>
#include <dcp/logic/DcpManager.hpp>
#include <dcp/model/pdu/DcpPduFactory.hpp>

Socket::Socket(asio::io_service &ios, asio::ip::udp::endpoint endpoint, DcpManager &dcpManager, LogManager &_logManager) :
        io_service(ios), endpoint(endpoint), dcpManager(dcpManager), started(false) {
    setLogManager(_logManager);
}

void Socket::send(DcpPdu &msg, asio::ip::udp::endpoint endpoint) {
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

void Socket::handle_receive(const std::error_code &error, std::size_t bytes_transferred) {
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

void Socket::setup_receive() {
    socket->async_receive_from(asio::buffer(data + 4, maxLength), lastAccess,
                                std::bind(&Socket::handle_receive, this,
                                            std::placeholders::_1,
                                            std::placeholders::_2));
}

void Socket::start() {
    if(!started){
        socket = std::unique_ptr<asio::ip::udp::socket>(new asio::ip::udp::socket(io_service, endpoint));
        setup_receive();
#if defined(DEBUG)
        Log(NEW_SOCKET, Udp::protocolName, to_string(endpoint));
#endif
        started = true;
    }
}

void Socket::close() {
    //one in asio queue, one existing in driver
    if(shared_from_this().use_count() == 2){
        if (started && socket != nullptr) {
            socket->close();
            started = false;
        }
#if defined(DEBUG)
        Log(SOCKET_CLOSED, Udp::protocolName, to_string(endpoint));
#endif
    }
}