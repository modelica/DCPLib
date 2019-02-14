/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef ACOSAR_LOGTEMPLATE_H
#define ACOSAR_LOGTEMPLATE_H

#include <cstdint>
#include <stdexcept>
#include <vector>
#include <map>
#include <string>
#include <dcp/model/constant/DcpLogLevel.hpp>
#include <dcp/model/constant/DcpDataType.hpp>


enum LogCategory : uint8_t {
    DCP_LIB_ETHERNET = 240,
    DCP_LIB_SLAVE = 251,
    DCP_LIB_MASTER = 252,


};

class LogTemplate {


public:
    LogTemplate(uint8_t id, uint8_t category, DcpLogLevel level, const std::string &msg,
                const std::vector<DcpDataType> &dataTypes) : id(id), category(category), level(level), msg(msg),
                                                             dataTypes(dataTypes) {}
    ~LogTemplate(){}
    const uint8_t id;
    const uint8_t category;
    const DcpLogLevel level;
    const std::string msg;
    const std::vector<DcpDataType> dataTypes;
};

static uint8_t logId = 1;


#endif //ACOSAR_LOGTEMPLATE_H