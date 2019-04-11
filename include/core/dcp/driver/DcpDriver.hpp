/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPDRIVER_H
#define DCPLIB_DCPDRIVER_H

#include <dcp/logic/LogManager.hpp>
#include <dcp/model/pdu/DcpPdu.hpp>
#include <dcp/logic/DcpManager.hpp>

/**
 * Interface for a DCP driver.
 * A DCP driver maps the PDUs to transport protocol and
 * manages all relevant information regarding the transport protocol.
 *
 * @author Christian Kater <kater@sim.uni-hannover.de>
 */
struct DcpDriver{
    /**
     *  Sending the given PDU over transport protocol.
     *
     *  @pre Depending on PDU type one of the follwoing function was called first:
     *  setSlaveNetworkInformation, setSourceNetworkInformation, setTargetNetworkInformation,
     *  setParamNetworkInformation, setTargetParamNetworkInformation
     *
     */
    std::function<void(DcpPdu&)> send;
    /**
     * Setting the network information for a remote slave.
     * This will be nessecary to set, to be able to send control PDUs.
     * The format of the data inside the uint8_t* is as defined in the
     * corresponding PDU definition of CFG_target_network_information of the transport protocol
     */
    std::function<void(dcpId_t, uint8_t*)> setSlaveNetworkInformation;
    /**
     * Setting the network information which is used by sending DAT_input_output PDU.
     * The format of the data inside the uint8_t* is as defined in the
     * corresponding PDU definition of CFG_target_network_information of the transport protocol
     */
    std::function<void(dataId_t, uint8_t*)> setSourceNetworkInformation;
    /**
     * Setting the transport protocol to listen to DAT_input_output PDU.
     * The format of the data inside the uint8_t* is as defined in the
     * corresponding PDU definition of CFG_source_network_information of the transport protocol
     */
    std::function<void(dataId_t, uint8_t*)> setTargetNetworkInformation;
    /**
     * Setting the transport protocol to listen to DAT_parameter PDU.
     * The format of the data inside the uint8_t* is as defined in the
     * corresponding PDU definition of CFG_param_network_information of the transport protocol
     */
    std::function<void(paramId_t, uint8_t*)> setParamNetworkInformation;
    /**
     * Setting the network information which is used by sending DAT_parameter PDU.
     * The format of the data inside the uint8_t* is as defined in the
     * corresponding PDU definition of CFG_target_network_information of the transport protocol
     */
    std::function<void(paramId_t, uint8_t*)> setTargetParamNetworkInformation;
    /**
     * The DCP driver should start listening to the transport protocol
     */
    std::function<void()> startReceiving;
    /**
     * Establish a connection to another slave.
     * Only necessary for connection-oriented transport protocols.
     */
    std::function<void(dcpId_t)> connectToSlave;
    /**
     * Disconnect a connection to another slave.
     * Only necessary for connection-oriented transport protocols.
     */
    std::function<void(dcpId_t)> disconnectFromSlave;
    /**
     * Set callback of master or slave manager
     * Can be used to pass recieved PDUs or errors.
     */
    std::function<void(DcpManager)> setDcpManager;
    /**
     * Set callback of logManager.
     */
    std::function<void(LogManager)> setLogManager;
    /**
     * A slave was registered successfully.
     */
    std::function<void()> registerSuccessfull;
    /**
     * Open endpoints
     */
    std::function<void()> prepare;
    /**
     * Establish Connections
     */
    std::function<void()> configure;
    /**
     * Disconnect and close configured endpoints
     */
    std::function<void()> stop;
    /**
     * Disconnect all connections
     */
    std::function<void()> disconnect;
    /**
    * The DCP driver should stop listening to the transport protocol
    */
    std::function<void()> stopReceiving;
};

#endif //DCPLIB_DCPDRIVER_H