/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUTYPE_HPP
#define DCPLIB_DCPPDUTYPE_HPP

#include <string>
#if defined(DEBUG) || defined(LOGGING)
#include <sstream>
#endif

/**
 * PDU types of the DCP
 */
enum class DcpPduType : uint8_t {
    /**
     * A DCP master shall register a DCP slave to integrate it into a simulation scenario and use it for a simulation
     * task.
     */
    STC_register = 0x01,
    /**
     * A DCP master deregisters a DCP slave to release it from a simulation scenario.
     */
    STC_deregister = 0x02,
    /**
     * The transport protocol should be prepared.
     */
    STC_prepare = 0x03,
    /**
     * The DCP slave has received configuration information and shall start to realize the configuration.
     */
    STC_configure = 0x04,
    /**
     * The DCP slave starts to establish a consistent initial state with all other connected DCP slaves.
     */
    STC_initialize = 0x05,
    /**
     * This transition indicates the start of the simulation run.
     */
    STC_run = 0x06,
    /**
     * The DCP slave starts one computational step.
     */
    STC_do_step = 0x07,
    /**
     * The DCP slave sends its computational results.
     */
    STC_send_outputs = 0x08,
    /**
     * The DCP master tells the DCP slave to halt the simulation or abort the configuration or initialization phase by
     * sending PDU STC_stop. The DCP slave proceeds to STOPPING.
     */
    STC_stop = 0x09,
    /**
     * The DCP slave is commanded by the DCP master to go back to state CONFIGURATION. All previously configured
     * settings are reset, this also includes shutdown of connections configured by PDUs of the configuration family.
     */
    STC_reset = 0x0A,
    /**
     * This PDU requests a DCP slave to set its time resolution.
     */
    CFG_time_res = 0x20,
    /**
     * This PDU requests a DCP slave to set its communication step size. The number of steps must be larger or equal
     * than 1.
     */
    CFG_steps = 0x21,
    /**
     * In order to set up slave-to-slave DAT_input_output PDU communication, the DCP master must inform all DCP slaves
     * that have inputs to be received in which format they may expect DAT_input_output PDUs. For that purpose,
     * CFG_input is used.
     */
    CFG_input = 0x22,
    /**
     * In order to set up slave-to-slave DAT_input_output PDU communication, the master must inform all DCP slaves that
     * have outputs to be sent to which input and at which DCP slave the value must be sent. For that purpose, the PDU
     * CFG_output is used.
     */
    CFG_output = 0x23,
    /**
     * The DCP slave must reset all configurations set earlier by configuration request PDUs.
     */
    CFG_clear = 0x24,
    /**
     * The PDU CFG_target_network_information is used to distribute network information.
     */
    CFG_target_network_information = 0x25,
    /**
     * The message CFG_source_network_information is used to distribute network information.
     */
    CFG_source_network_information = 0x26,
    /**
     * Sets the value of a parameter
     */
    CFG_parameter = 0x27,
    /**
     * This PDU is used to inform the DCP slave about the parameter format to expect.
     */
    CFG_tunable_parameter = 0x28,
    /**
     * The message CFG_param_network_information is used to distribute network information.
     */
    CFG_param_network_information = 0x29,
    /**
     * This PDU is used to set up the DCP logging mechanisms.
     */
    CFG_logging = 0x2A,
    /**
     * This PDU is used to set the scope of the configurations of a Data PDU identified by data_id.
     */
    CFG_scope = 0x2B,

    /**
     * This PDU requests a DCP slave’s current state.
     */
    INF_state = 0x80,
    /**
     * This PDU requests a DCP slave’s reported error.
     */
    INF_error = 0x81,
    /**
     * This PDU requests the slave to send it’s logging entries of the category log_category. The number of returned
     * logging entries is limited to log_max_num.
     */
    INF_log = 0x82,

    /**
     * This PDU is used for general acknowledgment of any requests.
     */
    RSP_ack = 0xB0,
    /**
     * RSP_nack shall be sent whenever a received Control PDU was not understood correctly, cannot be executed at the
     * time, or contains an unexpected PDU sequence identifier .
     */
    RSP_nack = 0xB1,
    /**
     * This PDU is used by a DCP master to report the current state in the field state_id. The sender field holds the
     * slave id of the slave. In case the slave is in state ALIVE i.e. it has not been registered and thus has not been
     * given a slave id, the slave uses the DCP slave id from the receiver field of PDU INF_state to answer with the
     * sender field of PDU RSP_state_ack.
     */
    RSP_state_ack = 0xB2,
    /**
     * This PDU is used by a DCP slave to report the current error.
     */
    RSP_error_ack = 0xB3,
    /**
     * Returns the requested log entries
     */
    RSP_log_ack = 0xB4,
    /**
     * This PDU indicates a successful state transition.
     */
    NTF_state_changed = 0xE0,
    /**
     * This PDU is used to send a single log entry.
     */
    NTF_log = 0xE1,

    /**
     * This PDU contains output values of a DCP slave
     */
    DAT_input_output = 0xF0,
    /**
     * This PDU contains parameter values for a DCP slave
     */
    DAT_parameter = 0xF1,


};

#if defined(DEBUG) || defined(LOGGING)
/**
 * Adds an DcpPduType to an osstream in a human readable format.
 * @param str
 * @param type
 */
static std::ostream &operator<<(std::ostream &os, DcpPduType type) {
    switch (type) {
        case DcpPduType::STC_configure:
            return os << "STC_configure";
        case DcpPduType::STC_prepare:
            return os << "STC_prepare";
        case DcpPduType::STC_initialize:
            return os << "STC_initialize";
        case DcpPduType::STC_run:
            return os << "STC_run";
        case DcpPduType::STC_do_step:
            return os << "STC_do_step";
        case DcpPduType::STC_send_outputs:
            return os << "STC_send_outputs";
        case DcpPduType::STC_stop:
            return os << "STC_stop";
        case DcpPduType::STC_reset:
            return os << "STC_reset";
        case DcpPduType::INF_state:
            return os << "INF_state";
        case DcpPduType::INF_error:
            return os << "INF_error";
        case DcpPduType::INF_log:
            return os << "INF_log";
        case DcpPduType::NTF_state_changed:
            return os << "NTF_state_changed";
        case DcpPduType::NTF_log:
            return os << "NTF_log";
        case DcpPduType::CFG_time_res:
            return os << "CFG_time_res";
        case DcpPduType::CFG_steps:
            return os << "CFG_steps";
        case DcpPduType::STC_register:
            return os << "STC_register";
        case DcpPduType::STC_deregister:
            return os << "STC_deregister";
        case DcpPduType::CFG_input:
            return os << "CFG_input";
        case DcpPduType::CFG_output:
            return os << "CFG_output";
        case DcpPduType::CFG_parameter:
            return os << "CFG_parameter";
        case DcpPduType::CFG_tunable_parameter:
            return os << "CFGtunable_parameter";
        case DcpPduType::CFG_param_network_information:
            return os << "CFG_param_network_information";
        case DcpPduType::CFG_clear:
            return os << "CFG_clear";
        case DcpPduType::CFG_logging:
            return os << "CFG_logging";
        case DcpPduType::DAT_input_output:
            return os << "DAT_input_output";
        case DcpPduType::DAT_parameter:
            return os << "DAT_parameter";
        case DcpPduType::RSP_ack:
            return os << "RSP_ack";
        case DcpPduType::RSP_nack:
            return os << "RSP_nack";
        case DcpPduType::RSP_state_ack:
            return os << "RSP_state_ack";
        case DcpPduType::RSP_error_ack:
            return os << "RSP_error_ack";
        case DcpPduType::RSP_log_ack:
            return os << "RSP_log_ack";
        case DcpPduType::CFG_target_network_information:
            return os << "CFG_target_network_information";
        case DcpPduType::CFG_source_network_information:
            return os << "CFG_source_network_information";
        case DcpPduType::CFG_scope:
            return os << "CFG_scope";
        default:
            return os << "UNKNOWN(" << (unsigned((uint8_t) type)) << ")";
    }
    return os;
}

/**
 * Concatenate an DcpPduType to an string in a human readable format.
 * @param str
 * @param type
 */
static std::string to_string(DcpPduType type) {
    std::ostringstream oss;
    oss << type;
    return oss.str();
}
#endif
#endif //DCPLIB_DCPPDUTYPE_HPP
