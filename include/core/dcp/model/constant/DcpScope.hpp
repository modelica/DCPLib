/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPSCOPE_HPP
#define DCPLIB_DCPSCOPE_HPP

#include <string>
#if defined(DEBUG) || defined(LOGGING)
#include <sstream>
#endif

enum class DcpScope : uint8_t {
    /**
     * Configuration PDUs for data ids with scope Initialization_Run_NonRealTime are valid in the superstates
     * Initialization, Run & NonRealTime.
     */
    Initialization_Run_NonRealTime = 0x00,
    /**
     * Configuration PDUs for data ids with scope Initialization_Run_NonRealTime are valid in the superstates
     * Initialization.
     */
    Initialization = 0x01,
    /**
     * Configuration PDUs for data ids with scope Initialization_Run_NonRealTime are valid in the superstates
     * Run & NonRealTime.
     */
    Run_NonRealTime = 0x02,
};

#if defined(DEBUG) || defined(LOGGING)
static std::ostream &operator<<(std::ostream &os, DcpScope scope) {
    switch (scope) {
        case DcpScope::Initialization_Run_NonRealTime:
            return os << "Initialization/Run/NonRealTime";
        case DcpScope::Initialization:
            return os << "Initialization";
        case DcpScope::Run_NonRealTime:
            return os << "Run/NonRealTime";
        default:
            return os << "UNKNOWN(" << (unsigned((uint8_t) scope)) << ")";
    }
    return os;
}


static std::string to_string(DcpScope scope) {
    std::ostringstream oss;
    oss << scope;
    return oss.str();
}
#endif
#endif //DCPLIB_DCPSCOPE_HPP
