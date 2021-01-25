/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUFACTORY_HPP
#define DCPLIB_DCPPDUFACTORY_HPP

#include <dcp/model/pdu/DcpPdu.hpp>

DcpPdu *makeDcpPdu(unsigned char *stream, size_t stream_size);

#endif //DCPLIB_DCPPDUFACTORY_HPP
