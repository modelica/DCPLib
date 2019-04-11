/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_LOGABLE_H
#define DCPLIB_LOGABLE_H

#include <dcp/helper/LogHelper.hpp>
#include <dcp/helper/Helper.hpp>
#include <dcp/model/LogEntry.hpp>
#include <dcp/logic/LogManager.hpp>

class Logable {
public:
    void setLogManager(const LogManager &logManager) {
        Logable::logManager = logManager;
    }

private:

protected:
    LogManager logManager;

public:
    template<typename ... Args>
    inline void Log(const LogTemplate &logTemplate, const Args... args) {
        using namespace std::chrono;

        DcpLogHelper::checkDataTypes(logTemplate, 0, args...);
        size_t size = DcpLogHelper::size(&args...);
        const auto logTime = time_point_cast<microseconds>(system_clock::now());

        uint8_t* payload = logManager.alloc(size + 9);
        *((int64_t *) payload) = (int64_t) duration_cast<seconds>(logTime.time_since_epoch()).count();
        *((uint8_t *) payload + 8) = logTemplate.id;

        DcpLogHelper::applyFields(payload + 9, args...);
        logManager.consume(logTemplate, payload, size + 9);
    }
};
#endif //DCPLIB_LOGABLE_H