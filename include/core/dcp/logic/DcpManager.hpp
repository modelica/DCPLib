/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPMANAGERCALLBACKS_H
#define DCPLIB_DCPMANAGERCALLBACKS_H

#include <dcp/model/pdu/DcpPdu.hpp>
#include <dcp/model/constant/DcpError.hpp>
#include <functional>
struct DcpManager{
    std::function<void(DcpPdu&)> receive;
    std::function<void(const DcpError)> reportError;
};

#endif //DCPLIB_DCPMANAGERCALLBACKS_H
