/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPLOGLEVEL_HPP
#define DCPLIB_DCPLOGLEVEL_HPP

#include <string>
#include <sstream>

enum class DcpLogLevel : uint8_t {
    /**
     * The simulation cannot be continued. The DCP slave will transition to the error superstate.
     */
    LVL_FATAL = 0,
    /**
     * The current action cannot be continued.
     */
    LVL_ERROR = 1,
    /**
     * The current action can be continued, but simulation results could be affected.
     */
    LVL_WARNING = 2,
    /**
     * This log level reflects the status of a DCP slave.
     */
    LVL_INFORMATION = 3,
    /**
     * This log level is intended for debug information.
     */
    LVL_DEBUG = 4,
};

static std::ostream &operator<<(std::ostream &os, DcpLogLevel level) {
    switch (level) {
        case DcpLogLevel::LVL_FATAL:
            return os << "FATAL";
        case DcpLogLevel::LVL_ERROR:
            return os << "ERROR";
        case DcpLogLevel::LVL_WARNING:
            return os << "WARNING";
        case DcpLogLevel::LVL_INFORMATION:
            return os << "INFO";
        case DcpLogLevel::LVL_DEBUG:
            return os << "DEBUG";
        default:
            return os << "UNKNOWN(" << (unsigned((uint8_t) level)) << ")";
    }
    return os << "UNKNOWN";
}

static std::string to_string(DcpLogLevel level) {
    std::ostringstream oss;
    oss << level;
    return oss.str();
}
#endif //DCPLIB_DCPLOGLEVEL_HPP