/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPERROR_HPP
#define DCPLIB_DCPERROR_HPP

#include <string>
#include <sstream>

/**
 * Errors which can occour in a co-simulation run
 */
enum class DcpError : uint16_t {
    /**
     * Indicates that no error is currently present.
     */
    NONE = 0x00,
    /**
     * Indicates that an error has occurred which is not specified in this document.
     */
    PROTOCOL_ERROR_GENERIC = 0x1001,
    /**
     * Indicates that the DCP slave did not receive a PDU INF_state within the maximum periodic interval ti_max
     * defined in the DCP slave description.
     */
    PROTOCOL_ERROR_HEARTBEAT_MISSED = 0x1002,
    /**
     * Indicates that a received PDU is not allowed in the current state.
     */
    PROTOCOL_ERROR_PDU_NOT_ALLOWED_IN_THIS_STATE = 0x1003,
    /**
     * Indicates that one of the following properties specified in the DCP slave description has been violated:
     * min, max, preEdge, postEdge, gradient, maxConsecMissedPdus.
     */
    PROTOCOL_ERROR_PROPERTY_VIOLATED = 0x1004,
    /**
     * Indicates that a received state change request PDU has already been acknowledged, but the current state still
     * differs from the requested state.
     */
    PROTOCOL_ERROR_STATE_TRANSITION_IN_PROGRESS = 0x1005,

    /**
     * Indicates that the received PDU has a valid type_id, but its length does not match.
     */
    INVALID_LENGTH = 0x2001,
    /**
     * A requested log_category is not a log category of the slave
     */
    INVALID_LOG_CATEGORY = 0x2002,
    /**
     * log_level is not a valid log level
     */
    INVALID_LOG_LEVEL = 0x2003,
    /**
     * log_mode is not valid log mode
     */
    INVALID_LOG_MODE = 0x2004,
    /**
     * major_version as set by the master is not allowed according to the major version of the DCP specification
     * defined in the DCP slave description.
     */
    INVALID_MAJOR_VERSION = 0x2005,
    /**
     * minor_version as set by the master is not allowed according to the minor version of the DCP specification
     * defined in the DCP slave description.
     */
    INVALID_MINOR_VERSION = 0x2006,
    /**
     * Indicates that the network information provided through a configuration request PDU is not valid.
     */
    INVALID_NETWORK_INFORMATION = 0x2007,
    /**
     * Indicates that the operating mode requested through STC_register is not supported by this DCP slave.
     */
    INVALID_OP_MODE = 0x2008,
    /**
     * Indicates that the length of a string is not equal to a defined fixedLengthBytes or the length of a binary value
     * is greater than a defined maxSize.
     */
    INVALID_PAYLOAD = 0x2009,
    /**
     * A given scope is invalid.
     */
    INVALID_SCOPE = 0x200A,
    /**
     * source_data_type is not compatible with the inputs data type. For a list of data types.
     */
    INVALID_SOURCE_DATA_TYPE = 0x200B,
    /**
     * Indicates that the start time provided in STC_run is invalid, e.g. in the past.
     */
    INVALID_START_TIME = 0x200C,
    /**
     * state_id is not equal to the slave’s state.
     */
    INVALID_STATE_ID = 0x200D,
    /**
     * Indicates that the number of steps in CFG_steps is not supported.
     */
    INVALID_STEPS = 0x200E,
    /**
     * Indicates that the time resolution expressed by numerator and denominator is not valid.
     */
    INVALID_TIME_RESOLUTION = 0x200F,
    /**
     * The given transport_protocol is not supported by the slave
     */
    INVALID_TRANSPORT_PROTOCOL = 0x2010,
    /**
     * slave_uuid is not equal to slave’s uuid.
     */
    INVALID_UUID = 0x2011,
    /**
     * Indicates that the value reference in CFG_input, CFG_output, CFG_parameter, or CFG_tunable_parameter is not
     * available within that DCP slave.
     */
    INVALID_VALUE_REFERENCE = 0x2012,
    /**
     * Violated PDU sequence ID check.
     */
    INVALID_SEQUENCE_ID = 0x2013,
    /**
     * There are gaps in the configured PDU Data payload field.
     * Note: No gap means if pos n is not the last pos, there exists a pos n+1.
     */
    INCOMPLETE_CONFIG_GAP_INPUT_POS = 0x3001,
    /**
     * There are gaps in the pos of the received CFG_output PDUs.
     */
    INCOMPLETE_CONFIG_GAP_OUTPUT_POS = 0x3002,
    /**
     * here are gaps in the pos of the received CFG_tunable_parameter PDUs.
     */
    INCOMPLETE_CONFIG_GAP_TUNABLE_POS = 0x3003,
    /**
     * For the PDU Data payload field identified by data_id no or no valid CFG_source_network_information has been
     * received.
     */
    INCOMPLETE_CONFIG_NW_INFO_INPUT = 0x3004,
    /**
     * For the PDU Data payload field identified by data_id no or no valid CFG_target_network_information has been
     * received.
     */
    INCOMPLETE_CONFIG_NW_INFO_OUTPUT = 0x3005,
    /**
     * Not for each param_id which occurs in the PDUs CFG_tunable_parameter a valid CFG_param_network_information with
     * the same param_id is received.
     */
    INCOMPLETE_CONFIG_NW_INFO_TUNABLE = 0x3006,
    /**
     * At least one data_id is missing the setting of the scope.
     */
    INCOMPLETE_CONFIG_SCOPE = 0x3007,
    /**
     * For every data_id which occurs in a CFG_output at least one valid CFG_steps with the same data_id must have been
     * received.
     */
    INCOMPLETE_CONFIG_STEPS = 0x3008,
    /**
     * No time resolution is specified. Neither through the DCP slave description, nor through CFG_time_res.
     */
    INCOMPLETE_CONFIG_TIME_RESOLUTION = 0x3009,
    /**
     * Indicates that a DCP slave cannot leave CONFIGURATION state due to missing configuration information.
     */
    INCOMPLETE_CONFIGURATION = 0x300A,

    /**
     * log_mode logOnNotification is not supported by the slave, i.e. in DCP slave description
     * canProvideLogOnNotification = false
     */
    NOT_SUPPORTED_LOG_ON_NOTIFICATION = 0x4001,
    /**
     * log_mode logOnRequest is not supported by the slave i.e. in DCP slave description canProvideLogOnRequest = false
     */
    NOT_SUPPORTED_LOG_ON_REQUEST = 0x4002,
    /**
     * Steps differ from previous ones (if the slave does not support variable step sizes).
     */
    NOT_SUPPORTED_VARIABLE_STEPS = 0x4003,
    /**
     * Indicates that the transport protocol number provided through a configuration request PDU is not supported.
     */
    NOT_SUPPORTED_TRANSPORT_PROTOCOL = 04004,
    /**
     * Indicates that this type of PDU (identified through field type_id) is defined within this specification but is
     * not supported by this DCP slave.
     */
    NOT_SUPPORTED_PDU = 0x4005,
    /**
     * A data PDU was configured such that it exceeds the maximum allowed PDU size specified in maxPduSize in the DCP
     * slave description.
     */
    NOT_SUPPORTED_PDU_SIZE = 0x4006,
};

/**
 * Adds an DcpError to an osstream in a human readable format.
 * @param str
 * @param error
 */
static std::ostream &operator<<(std::ostream &os, DcpError error) {
    switch (error) {
        case DcpError::NONE:
            return os << "NONE";
        case DcpError::PROTOCOL_ERROR_GENERIC:
            return os << "PROTOCOL_ERROR_GENERIC";
        case DcpError::PROTOCOL_ERROR_HEARTBEAT_MISSED:
            return os << "PROTOCOL_ERROR_HEARTBEAT_MISSED";
        case DcpError::INCOMPLETE_CONFIG_GAP_INPUT_POS:
            return os << "INCOMPLETE_CONFIG_GAP_INPUT_POS";
        case DcpError::INCOMPLETE_CONFIG_GAP_OUTPUT_POS:
            return os << "INCOMPLETE_CONFIG_GAP_OUTPUT_POS";
        case DcpError::INCOMPLETE_CONFIG_GAP_TUNABLE_POS:
            return os << "INCOMPLETE_CONFIG_GAP_TUNABLE_POS";
        case DcpError::INCOMPLETE_CONFIG_NW_INFO_OUTPUT:
            return os << "INCOMPLETE_CONFIG_NW_INFO_OUTPUT";
        case DcpError::INCOMPLETE_CONFIG_NW_INFO_INPUT:
            return os << "INCOMPLETE_CONFIG_NW_INFO_INPUT";
        case DcpError::INCOMPLETE_CONFIG_NW_INFO_TUNABLE:
            return os << "INCOMPLETE_CONFIG_NW_INFO_TUNABLE";
        case DcpError::INCOMPLETE_CONFIG_SCOPE:
            return os << "INCOMPLETE_CONFIG_SCOPE";
        case DcpError::INCOMPLETE_CONFIG_STEPS:
            return os << "INCOMPLETE_CONFIG_STEPS";
        case DcpError::INCOMPLETE_CONFIG_TIME_RESOLUTION:
            return os << "INCOMPLETE_CONFIG_TIME_RESOLUTION";
        case DcpError::INCOMPLETE_CONFIGURATION:
            return os << "INCOMPLETE_CONFIGURATION";
        case DcpError::INVALID_LENGTH:
            return os << "INVALID_LENGTH";
        case DcpError::INVALID_LOG_CATEGORY:
            return os << "INVALID_LOG_CATEGORY";
        case DcpError::INVALID_LOG_LEVEL:
            return os << "INVALID_LOG_LEVEL";
        case DcpError::INVALID_LOG_MODE:
            return os << "INVALID_LOG_MODE";
        case DcpError::INVALID_MAJOR_VERSION:
            return os << "INVALID_MAJOR_VERSION";
        case DcpError::INVALID_MINOR_VERSION:
            return os << "INVALID_MINOR_VERSION";
        case DcpError::INVALID_NETWORK_INFORMATION:
            return os << "INVALID_NETWORK_INFORMATION";
        case DcpError::INVALID_OP_MODE:
            return os << "INVALID_OP_MODE";
        case DcpError::INVALID_SCOPE:
            return os << "INVALID_SCOPE";
        case DcpError::INVALID_START_TIME:
            return os << "INVALID_START_TIME";
        case DcpError::INVALID_STATE_ID:
            return os << "INVALID_STATE_ID";
        case DcpError::INVALID_STEPS:
            return os << "INVALID_STEPS";
        case DcpError::INVALID_TIME_RESOLUTION:
            return os << "INVALID_TIME_RESOLUTION";
        case DcpError::INVALID_TRANSPORT_PROTOCOL:
            return os << "INVALID_TRANSPORT_PROTOCOL";
        case DcpError::INVALID_UUID:
            return os << "INVALID_UUID";
        case DcpError::INVALID_VALUE_REFERENCE:
            return os << "INVALID_VALUE_REFERENCE";
        case DcpError::INVALID_SEQUENCE_ID:
            return os << "INVALID_SEQUENCE_ID";
        case DcpError::NOT_SUPPORTED_LOG_ON_NOTIFICATION:
            return os << "NOT_SUPPORTED_LOG_ON_NOTIFICATION";
        case DcpError::NOT_SUPPORTED_LOG_ON_REQUEST:
            return os << "NOT_SUPPORTED_LOG_ON_REQUEST";
        case DcpError::NOT_SUPPORTED_VARIABLE_STEPS:
            return os << "NOT_SUPPORTED_VARIABLE_STEPS";
        case DcpError::NOT_SUPPORTED_PDU_SIZE:
            return os << "NOT_SUPPORTED_PDU_SIZE";
        case DcpError::PROTOCOL_ERROR_PDU_NOT_ALLOWED_IN_THIS_STATE:
            return os << "PROTOCOL_ERROR_PDU_NOT_ALLOWED_IN_THIS_STATE";
        case DcpError::PROTOCOL_ERROR_PROPERTY_VIOLATED:
            return os << "PROTOCOL_ERROR_PROPERTY_VIOLATED";
        case DcpError::PROTOCOL_ERROR_STATE_TRANSITION_IN_PROGRESS:
            return os << "PROTOCOL_ERROR_STATE_TRANSITION_IN_PROGRESS";
        case DcpError::NOT_SUPPORTED_PDU:
            return os << "NOT_SUPPORTED_PDU";
        case DcpError::INVALID_PAYLOAD:
            return os << "INVALID_PAYLOAD";
        case DcpError::INVALID_SOURCE_DATA_TYPE:
            return os << "INVALID_SOURCE_DATA_TYPE";
        case DcpError::NOT_SUPPORTED_TRANSPORT_PROTOCOL:
            return os << "NOT_SUPPORTED_TRANSPORT_PROTOCOL";
        default:
            return os << "UNKNOWN(" << ((uint16_t) error) << ")";
    }
    return os;
}

/**
 * Concatenate an DcpError to an string in a human readable format.
 * @param str
 * @param type
 */
static std::string to_string(DcpError type) {
    std::ostringstream oss;
    oss << type;
    return oss.str();
}

#endif //DCPLIB_DCPERROR_HPP