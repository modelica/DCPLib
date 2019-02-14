/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_LOGMANAGER_H
#define DCPLIB_LOGMANAGER_H

#include <cstdint>
#include <dcp/model/LogTemplate.hpp>
#include <functional>


struct LogManager{
    std::function<void(const LogTemplate&, uint8_t*, size_t)> consume;
    std::function<uint8_t*(size_t)> alloc;
};
#endif //DCPLIB_LOGMANAGER_H
