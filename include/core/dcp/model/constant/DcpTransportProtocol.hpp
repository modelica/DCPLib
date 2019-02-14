/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPTRANSPORTPROTOCOL_HPP
#define DCPLIB_DCPTRANSPORTPROTOCOL_HPP

#include <string>
#if defined(DEBUG) || defined(LOGGING)
#include <sstream>
#endif

/**
 * Transport protocols which can be used as transport protocol for DCP
 */
enum class DcpTransportProtocol : uint8_t {
    /**
     * UDP over IPv4
     */
    UDP_IPv4 = 0,
    /**
    * rfcomm over Bluetooth
    */
    rfcomm_Bluetooth = 1,
    /**
     * CAN based communication
     */
    CAN_BASED = 2,
    /**
     * USB 2.0
     */
    USB = 3,
    /**
     * TCP over Ipv4
     */
    TCP_IPv4 = 4,

};

#if defined(DEBUG) || defined(LOGGING)
/**
 * Adds an DcpTransportProtocol to an osstream in a human readable format.
 * @param str
 * @param transportProtocol
 */
static std::ostream &operator<<(std::ostream &os, DcpTransportProtocol transportProtocol) {
    switch (transportProtocol) {
        case DcpTransportProtocol::UDP_IPv4:
            return os << "UDP_IPv4";
        case DcpTransportProtocol::CAN_BASED:
            return os << "CAN_BASED";
        case DcpTransportProtocol::rfcomm_Bluetooth:
            return os << "rfcomm_Bluetooth";
        case DcpTransportProtocol::USB:
            return os << "USB";
        case DcpTransportProtocol::TCP_IPv4:
            return os << "TCP_IPv4";
        default:
            return os << "UNKNOWN(" << (unsigned((uint8_t) transportProtocol)) << ")";
    }
    return os;
}


/**
 * Concatenate an DcpTransportProtocol to an string in a human readable format.
 * @param str
 * @param type
 */
static std::string to_string(DcpTransportProtocol type) {
    std::ostringstream oss;
    oss << type;
    return oss.str();
}
#endif

#endif //DCPLIB_DCPTRANSPORTPROTOCOL_HPP
