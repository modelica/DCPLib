/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPSLAVEDESCRIPTIONWRITER_HPP
#define DCPLIB_DCPSLAVEDESCRIPTIONWRITER_HPP

#include <dcp/xml/DcpSlaveDescriptionElements.hpp>
#include <string>

std::string to_string(SlaveDescription_t slaveDescription);


bool writeDcpSlaveDescription(SlaveDescription_t slaveDescription, const char *dcpxFile);

#endif //DCPLIB_DCPSLAVEDESCRIPTIONWRITER_HPP
