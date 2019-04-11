/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#include <string>
#include <sstream>

#ifndef DCPLIB_DCPSTATE_HPP
#define DCPLIB_DCPSTATE_HPP

/**
 * States of the DCP state machine
 */
enum class DcpState : uint8_t {
    /**
     * The DCP slave is connected to communication media and waits for a DCP master to take ownership.
     * While being in this state, the DCP slave is not assigned to a DCP master yet. A DCP master may
     * take control of a DCP slave by sending the PDU STC_register.
     */
    ALIVE = 0x00,
    /**
     * A DCP master has taken ownership of the DCP slave. In this state, the DCP slave shall accept configuration
     * request PDUs (CFG). A configuration received in this state shall be applied before reaching the state
     * CONFIGURED at the latest.
     */
    CONFIGURATION = 0x01,
    /**
     * Slave must prepare the transport protocol to allow to connect and/or to receive data. This needs to be
     * done for every received CFG_source_network_information.
     */
    PREPARING = 0x02,
    /**
     * The slave has prepared the transport protocol and is ready to communicate or establish connections.
     */
    PREPARED = 0x03,
    /**
     * For connection oriented transport protocols a connection is established for every CFG_target_network_information.
     * For connectionless transport protocols no specific actions are necessary.The DCP slave realizes a start condition
     * depending on parameters, but not on input values.
     */
    CONFIGURING = 0x04,
    /**
     * At entry to this state coming from CONFIGURING, a start condition depending on parameters, but not on input
     * values has been realized by the DCP slave.
     */
    CONFIGURED = 0x05,
    /**
     * In INITIALIZING an internal initial state of the DCP slave, which is consistent to its inputs, shall be
     * established and the outputs shall be computed. The input values from the most recent data PDU are used for
     * internal computation. If no inputs have been received, start values defined in DCP slave description shall be
     * used.
     *
     * Simulation models: Simulation time stays at start time, simulation models are not computed over time,
     * but at start time.
     */
    INITIALIZING = 0x06,
    /**
     * In INITIALIZED an internal initial state of the DCP slave, which is consistent to its inputs, is established and
     * the outputs are available.
     *
     * In INITIALIZED the slave remains in its consistent internal initial state. If the slave fails to keep the
     * consistent internal initial state, it must perform the transition to the superstate Error.
     */
    INITIALIZED = 0x07,
    /**
     * In this state the DCP slave sends its outputs.
     */
    SENDING_I = 0x08,
    /**
     * For real-time operating modes SRT and HRT: The DCP slave is running and inputs/outputs are exchanged.
     * Simulation time is mapped to absolute time.
     *
     * For non-real-time operating mode (NRT): Simulation time is not advanced but can be increased by transitioning to
     * the NRT-specific state COMPUTING. The DCP slave can receive inputs.
     */
    SYNCHRONIZING = 0x09,
    /**
     * For real-time operating modes SRT and HRT: The DCP slave is running and inputs/outputs are exchanged.
     * Simulation time is mapped to absolute time.
     *
     * For non-real-time operating mode (NRT): Simulation time is not advanced but can be increased by transitioning to
     * the NRT-specific state COMPUTING. The DCP slave can receive inputs.
     */
    SYNCHRONIZED = 0x0A,
    /**
     * For real-time operating modes SRT and HRT: The DCP slave is running and inputs/outputs are exchanged.
     * Simulation time is mapped to absolute time.
     *
     * For non-real-time operating mode (NRT): Simulation time is not advanced but can be increased by transitioning to
     * the NRT-specific state COMPUTING. The DCP slave can receive inputs.
     */
    RUNNING = 0x0B,
    /**
     * In this state one computational step is performed. The values from the most recent Data PDUs are used for
     * internal computation. The virtual simulation time is incremented by the number of steps given in the field steps
     * of the PDU STC_do_step multiplied by resolution.
     */
    COMPUTING = 0x0C,
    /**
     * In this state all computations were performed and the DCP slave is ready to send computation results. The DCP
     * slave can receive inputs.
     */
    COMPUTED = 0x0D,
    /**
     * In this state the DCP slave sends its outputs.
     */
    SENDING_D = 0x0E,
    /**
     * The simulation run has finished and is now being stopped.
     */
    STOPPING = 0x0F,
    /**
     * The DCP slave has come to a stop.
     */
    STOPPED = 0x10,
    /**
     * The DCP slave tries to resolve an error.
     */
    ERROR_HANDLING = 0x11,
    /**
     * The DCP slave has finished its error handling procedure and successfully mitigated the hazardous condition.
     */
    ERROR_RESOLVED = 0x12,
};

/**
 * Adds an DcpState to an osstream in a human readable format.
 * @param str
 * @param type
 */
static std::ostream &operator<<(std::ostream &os, DcpState type) {
    switch (type) {
        case DcpState::ALIVE:
            return os << "ALIVE";
        case DcpState::CONFIGURATION:
            return os << "CONFIGURATION";
        case DcpState::PREPARING:
            return os << "PREPARING";
        case DcpState::PREPARED:
            return os << "PREPARED";
        case DcpState::CONFIGURING:
            return os << "CONFIGURING";
        case DcpState::CONFIGURED:
            return os << "CONFIGURED";
        case DcpState::INITIALIZING:
            return os << "INITIALIZING";
        case DcpState::INITIALIZED:
            return os << "INITIALIZED";
        case DcpState::SENDING_I:
            return os << "SENDING_I I";
        case DcpState::SYNCHRONIZING:
            return os << "SYNCHRONIZING";
        case DcpState::SYNCHRONIZED:
            return os << "SYNCHRONIZED";
        case DcpState::RUNNING:
            return os << "RUNNING";
        case DcpState::COMPUTING:
            return os << "COMPUTING";
        case DcpState::COMPUTED:
            return os << "COMPUTED";
        case DcpState::SENDING_D:
            return os << "SENDING_D D";
        case DcpState::STOPPING:
            return os << "STOPPING";
        case DcpState::STOPPED:
            return os << "STOPPED";
        case DcpState::ERROR_HANDLING:
            return os << "ERROR_HANDLING";
        case DcpState::ERROR_RESOLVED:
            return os << "ERROR_HANDLING";
        default:
            return os << "UNKNOWN(" << (unsigned((uint8_t) type)) << ")";
    }
    return os;
}
/**
 * Concatenate an DcpState to an string in a human readable format.
 * @param str
 * @param type
 */
static std::string to_string(DcpState type) {
    std::ostringstream oss;
    oss << type;
    return oss.str();
}
#endif //DCPLIB_DCPSTATE_HPP