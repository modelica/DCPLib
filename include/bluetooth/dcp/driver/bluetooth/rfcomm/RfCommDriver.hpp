//
// Created by kater on 03.09.18.
//

#ifndef DCPLIB_RFCOMMDRIVER_H
#define DCPLIB_RFCOMMDRIVER_H

#include <dcp/driver/DcpDriver.hpp>
#include <dcp/logic/Logable.hpp>

class RFCommDriver : public Logable {
public:

    RFCommDriver(uint8_t port, uint64_t bluetoothAddress) : mainPort(port), mainBluetoothAddress(bluetoothAddress) {}

    ~RFCommDriver() {}

    DcpDriver getDcpDriver() {
        return {std::bind(&RFCommDriver::send, this, std::placeholders::_1),
                [this](dcpId_t dcpId, uint8_t *info) {
                    setSlaveNetworkInformation(dcpId, *((uint8_t *) info), *((uint64_t*) (info + 1)));
                },
                [this](dataId_t dataId, uint8_t *info) {
                    setSourceNetworkInformation(dataId, *((uint8_t *) info), *((uint64_t*) (info + 1)));
                },
                [this](dataId_t dataId, uint8_t *info) {
                    setTargetNetworkInformation(dataId, *((uint8_t *) info), *((uint64_t*) (info + 1)));
                },
                [this](paramId_t paramId, uint8_t *info) {
                    setParamNetworkInformation(paramId, *((uint8_t *) info), *((uint64_t*) (info + 1)));
                },
                [this](paramId_t paramId, uint8_t *info) {
                    setTargetParamNetworkInformation(paramId, *((uint8_t *) info), *((uint64_t*) (info + 1)));
                },
                std::bind(&RFCommDriver::startReceiving, this),
                std::bind(&RFCommDriver::connectToSlave, this, std::placeholders::_1),
                std::bind(&RFCommDriver::disconnectFromSlave, this, std::placeholders::_1),
                [this](DcpManager manager) {
                    this->dcpManager = manager;
                },
                std::bind(&RFCommDriver::setLogManager, this, std::placeholders::_1),
                std::bind(&RFCommDriver::registerSuccessfull, this),
                std::bind(&RFCommDriver::openPorts, this),
                std::bind(&RFCommDriver::connectToConfiguredPorts, this),
                std::bind(&RFCommDriver::closeConfiguredPorts, this),
                std::bind(&RFCommDriver::disconnect, this),
        };
    }

private:

    DcpManager dcpManager;
    uint8_t mainPort; // Main port on which the slave listens for the master or vice versa
    uint64_t mainBluetoothAddress; // Bluetooth address of this device

    void send(DcpPdu &) {
        /*send PDU to master or targeted slave */
    }

    void setSlaveNetworkInformation(dcpId_t dcpId, uint8_t port, uint64_t bluetoothAddress) {
        /* Store Information to which host PDUs with *receiver=dcpId* has to be send */
    }

    void setSourceNetworkInformation(dataId_t dataId, uint8_t port, uint64_t bluetoothAddress) {
        /*  Store Information to listen for incomming DAT_input_output PDUs */
    }

    void setTargetNetworkInformation(dataId_t dataId, uint8_t port, uint64_t bluetoothAddress) {
        /* Store Information to which *receiver* certain PDUs has to be send */
    }

    void setParamNetworkInformation(paramId_t paramid, uint8_t port, uint64_t bluetoothAddress) {
        /*  Store Information to listen for incomming DAT_parameter PDUs */
    }

    void setTargetParamNetworkInformation(paramId_t paramid, uint8_t port, uint64_t bluetoothAddress) {
        /*  Store Information to listen for incomming DAT_parameter PDUs */
    }

    void startReceiving() {
        /* Start Listening on mainPort*/
    }

    void connectToSlave(dcpId_t dcpId) {

    }

    void disconnectFromSlave(dcpId_t dcpId) {

    }

    void registerSuccessfull() {
        /* Slave is takover by the master. The last connection from which a PDU was received in this thread can be
         * marked as master connection */
    }

    void openPorts() {
        /* Open all ports which was configured by  setSourceNetworkInformation & setParamNetworkInformation*/
    }

    void connectToConfiguredPorts() {
        /* Connect to all ports which was configured in setTargetNetworkInformation */
    }

    void closeConfiguredPorts() {
        /*close all connections & ports configured by setSourceNetworkInformation, setTargetNetworkInformation &
         * setParamNetworkInformation */
    }

    void disconnect(){

    }

};

#endif //DCPLIB_RFCOMMDRIVER_H
