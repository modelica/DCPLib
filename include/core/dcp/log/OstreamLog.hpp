/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef ACOSAR_OSTREAMLOG_H
#define ACOSAR_OSTREAMLOG_H

#include <iostream>
#include <string>
#include <regex>
#include <dcp/helper/Helper.hpp>
#include <dcp/helper/LogHelper.hpp>
#include <dcp/model/LogEntry.hpp>
#include <iostream>

class OstreamLog {
public:

    OstreamLog(std::ostream& _stream) : stream(_stream) {
    }

    virtual ~OstreamLog() {}

    virtual inline void logOstream(const LogEntry & log){
        stream << convertUnixTimestamp(log.getTime()) << " \t "
        << to_string(log.getLevel()) << " \t\t "
        << log.getMsg() << std::endl;
    }

private:

    std::ostream& stream;
};




#endif //ACOSAR_OSTREAMLOG_H
