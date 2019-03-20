/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universit�t Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */
#ifndef DCPLIB_TCPHELPER_H
#define DCPLIB_TCPHELPER_H

#include <dcp/driver/DcpDriver.hpp>
#include <dcp/logic/Logable.hpp>
#include <dcp/driver/ethernet/ErrorCodes.hpp>
#include <asio.hpp>

namespace Tcp {
    static std::string protocolName = "TCP_IPv4";
}

static std::string to_string(const asio::ip::tcp::endpoint &remote_endpoint) {
    std::ostringstream oss;
    oss << remote_endpoint;
    return oss.str();
}

class SessionManager {
public:
    virtual ~SessionManager() {}

    virtual void removeSession(size_t key) = 0;

    virtual void setLastSessionAccess(size_t lastSessionAccess) = 0;
};

class IClient {
public:
    virtual ~IClient() {}

    virtual void connectionLost() = 0;

};

class Session : public Logable, public std::enable_shared_from_this<Session> {
public:
    Session(asio::io_service &ios, DcpManager &manager, std::shared_ptr<SessionManager> _sessionManager, size_t cId)
            : dcpManager(manager), sessionManager(_sessionManager), id(cId) {
        this->socket = std::make_shared<asio::ip::tcp::socket>(ios);
        this->client = nullptr;
    }

    Session(std::shared_ptr<asio::ip::tcp::socket> socket, DcpManager &manager, std::shared_ptr<IClient> client)
            : dcpManager(manager), id(0) {
        this->socket = socket;
        this->sessionManager = nullptr;
        this->client = client;
    }

    ~Session(){}

    asio::ip::tcp::socket &getSocket() {
        return *socket;
    }

    void start() {
        prepareRead();
    }

    void prepareRead() {
        asio::async_read(*socket,
                         asio::buffer(data, maxLength),
                         std::bind(&Session::completion_condition, this,
                                   std::placeholders::_1,
                                   std::placeholders::_2),
                         std::bind(&Session::handleRead, this,
                                   shared_from_this(),
                                   std::placeholders::_1,
                                   std::placeholders::_2));
    }

    std::size_t completion_condition(const std::error_code &error, std::size_t bytes_transferred) {
        if (error && !connectionLost){
            if (sessionManager != nullptr) {
                sessionManager->removeSession(id);
            }
            if(client != nullptr){
                client->connectionLost();
            }
            connectionLost = true;
            return 0;
        }
        if (!error && bytes_transferred >= 4) {
            return (*((uint32_t *) data) + 4) - bytes_transferred;
        }
        return 4 - bytes_transferred;
    }

    void handleRead(std::shared_ptr<Session> s, const std::error_code &error, size_t bytes_transferred) {
        if(connectionLost){
            return;
        }
        if (!error) {
            if (sessionManager != nullptr) {
                sessionManager->setLastSessionAccess(id);
            }
            DcpPdu *pdu = makeDcpPdu(data, bytes_transferred - 4);
#if defined(DEBUG)
            Log(PDU_RECEIVED, pdu->to_string());
#endif
            dcpManager.receive(*pdu);
            delete pdu;

            prepareRead();


        } else {
            dcpManager.reportError(DcpError::PROTOCOL_ERROR_GENERIC);
#if defined(DEBUG) || defined(LOGGING)
            Log(NETWORK_PROBLEM, Tcp::protocolName, error.message());
#endif
            if (sessionManager != nullptr) {
                sessionManager->removeSession(id);
            }
            return;

        }
    }

    void send(DcpPdu &msg) {
#if defined(DEBUG)
        Log(PDU_SEND, msg.to_string());
#endif
        std::error_code error;
        try {
            std::size_t size = socket->send(
                    asio::buffer(msg.serialize(), msg.getSerializedSize()),
                    0, error);
            if (error && error != asio::error::message_size) {
#if defined(DEBUG) || defined(LOGGING)
                Log(NETWORK_PROBLEM, Tcp::protocolName, error.message());
#endif
                dcpManager.reportError(DcpError::PROTOCOL_ERROR_GENERIC);
            }
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            dcpManager.reportError(DcpError::PROTOCOL_ERROR_GENERIC);
#if defined(DEBUG) || defined(LOGGING)
            Log(NETWORK_PROBLEM, Tcp::protocolName, e.what());
#endif
        }
    }

    size_t getId() const {
        return id;
    }


private:
    std::shared_ptr<asio::ip::tcp::socket> socket;
    enum {
        maxLength = 1024
    };
    size_t id;
    uint8_t data[maxLength];
    DcpManager &dcpManager;
    std::shared_ptr<SessionManager> sessionManager;
    std::shared_ptr<IClient> client;
    bool connectionLost = false;
};

class Server : public Logable, public SessionManager, public std::enable_shared_from_this<Server> {
public:
    Server(asio::io_service &ios, asio::ip::tcp::endpoint _endpoint, DcpManager &manager, LogManager &_logManager) :
            ios(ios), endpoint(_endpoint), acceptor(ios, _endpoint), dcpManager(manager), started(false) {
        setLogManager(_logManager);
    }

    ~Server() {
        acceptor.cancel();
        clearSessions();
#if defined(DEBUG)
        Log(SOCKET_CLOSED, Tcp::protocolName, to_string(endpoint));
#endif
    }

    void start() {
        if (!started) {
            prepareAccept();
#if defined(DEBUG)
            Log(NEW_SOCKET, Tcp::protocolName, to_string(endpoint));
#endif
            started = true;
        }
    }

    void prepareAccept() {
        sessionCounter++;
        std::shared_ptr<Session> session = std::make_shared<Session>(ios, dcpManager, shared_from_this(),
                                                                     sessionCounter);
        acceptor.async_accept(session->getSocket(),
                              std::bind(&Server::handle_accept,
                                        this,
                                        session,
                                        std::placeholders::_1));
    }

    void handle_accept(std::shared_ptr<Session> session,
                       const std::error_code &error) {
        if (!error) {
            sessions[session->getId()] = session->shared_from_this();

            session->setLogManager(logManager);
            session->start();
#if defined(DEBUG)
            Log(NEW_TCP_CONNECTION_IN, to_string(session->getSocket().remote_endpoint()));
#endif
            prepareAccept();
        } else {
            std::cerr << "err: " + error.message() << std::endl;
        }
    }

    virtual void removeSession(size_t key) {
        if(sessions[key] != nullptr){
            sessions[key]->getSocket().close();
        }
        sessions.erase(key);
    }

    std::shared_ptr<Session> getSession(size_t id) {
        return sessions[id];
    }

    std::shared_ptr<Session> getLatestSession() {
        return sessions[lastSessionAccess];
    }

    const asio::ip::tcp::endpoint &getEndpoint() const {
        return endpoint;
    }

    size_t getLastSessionAccess() const {
        return lastSessionAccess;
    }

    virtual void setLastSessionAccess(size_t lastSessionAccess) {
        Server::lastSessionAccess = lastSessionAccess;
    }

    void clearNonMainSessions(size_t mainSession) {
        for (auto it = sessions.cbegin(); it != sessions.cend() /* not hoisted */; /* no increment */) {
            if (it->first != mainSession) {
#if defined(DEBUG)
                Log(TCP_CONNECTION_CLOSED, to_string(it->second->getSocket().remote_endpoint()));
#endif
                if(it->second != nullptr){
                    it->second->getSocket().close();
                }
                sessions.erase(it++);
            } else {
                ++it;
            }
        }
    }

    void clearSessions() {
        for (auto it = sessions.cbegin(); it != sessions.cend() /* not hoisted */; /* no increment */) {
#if defined(DEBUG)
            Log(TCP_CONNECTION_CLOSED, to_string(it->second->getSocket().remote_endpoint()));
#endif
            if(it->second != nullptr){
                it->second->getSocket().close();
            }
            sessions.erase(it++);
        }
    }

    void cancel(){
        clearSessions();
        acceptor.cancel();
    }

private:
    asio::io_service &ios;
    asio::ip::tcp::endpoint endpoint;
    asio::ip::tcp::acceptor acceptor;
    DcpManager &dcpManager;
    size_t sessionCounter;
    std::map<size_t, std::shared_ptr<Session>> sessions;
    bool started;
    size_t lastSessionAccess;
};


class Client : public IClient, public Logable, public std::enable_shared_from_this<Client>{
public:
    Client(asio::io_service &_ios, asio::ip::tcp::endpoint _endpoint, DcpManager &manager, LogManager &logManager) : dcpManager(
            manager),
                                                                                                                     endpoint(
                                                                                                                             _endpoint),
                                                                                                                     connected(
                                                                                                                             false),
                                                                                                                     ios(_ios) {
        setLogManager(logManager);
    }

    ~Client() {}

    bool start() {
        if (!connected) {
            try {
                socket = std::make_shared<asio::ip::tcp::socket>(ios);
                socket->connect(endpoint);
#if defined(DEBUG)
                Log(NEW_TCP_CONNECTION_OUT, to_string(endpoint));
#endif
                session = std::make_shared<Session>(socket, dcpManager, shared_from_this());
                session->setLogManager(logManager);
                session->start();
                connected = true;
            } catch (std::exception &e) {
                connected = false;
                dcpManager.reportError(DcpError::PROTOCOL_ERROR_GENERIC);
#if defined(DEBUG) || defined(LOGGING)
                Log(NETWORK_PROBLEM, Tcp::protocolName, e.what());
#endif
                return false;
            }

        }
        return true;
    }

    const asio::ip::tcp::endpoint &getEndpoint() const {
        return endpoint;
    }

    const std::shared_ptr<Session> &getSession() const {
        return session;
    }

    bool isConnected() const {
        return connected;
    }

    void connectionLost() {
        socket->close();
        connected = false;
    }

private:
    std::shared_ptr<asio::ip::tcp::socket> socket;
    asio::ip::tcp::endpoint endpoint;
    DcpManager &dcpManager;
    asio::io_service & ios;

    bool connected;
    std::shared_ptr<Session> session;

};

#endif //DCPLIB_TCPHELPER_H
