/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_FILTEREDOSTREAMLOG_H
#define DCPLIB_FILTEREDOSTREAMLOG_H

#include <dcp/log/OstreamLog.hpp>

class FilteredOstreamLog : public OstreamLog {
public:
    FilteredOstreamLog(std::ostream &_stream, DcpLogLevel level, logCategory_t category) :
            OstreamLog(_stream), level(level), category(category) {}

    ~FilteredOstreamLog() override {}

    void logOstream(const LogEntry &log) override {
        if(log.getCategory() == category && log.getLevel() == level){
            OstreamLog::logOstream(log);
        }
    }

private:
    DcpLogLevel level;
    logCategory_t category;
};
#endif //DCPLIB_FILTEREDOSTREAMLOG_H
