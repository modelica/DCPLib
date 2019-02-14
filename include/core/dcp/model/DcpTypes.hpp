/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef ACOSAR_DCPTYPES_H
#define ACOSAR_DCPTYPES_H

#include <cstdint>
#include <cstring>

//Define types
#pragma pack(push)
#pragma pack(1)
typedef struct __uint128_t__ {
    uint8_t data[16];

    inline bool operator!=(__uint128_t__ a) {
        return (0 != memcmp(data, a.data, sizeof(data)));
    }
} uint128_t;
#pragma pack(pop)
//must be binary32 as defined in IEEE 754-2008
typedef float float32_t;
//must be binary64 as defined in IEEE 754-2008
typedef double float64_t;


typedef uint16_t dataId_t;
typedef uint16_t paramId_t;
typedef uint64_t valueReference_t;
typedef uint32_t steps_t;
typedef uint16_t pos_t;
typedef uint8_t logCategory_t;
typedef uint8_t logTemplateId_t;
typedef uint32_t numerator_t;
typedef uint32_t denominator_t;

typedef uint16_t port_t;
typedef uint32_t ip_address_t;

typedef uint8_t dcpId_t;




#endif //ACOSAR_DCPTYPES_H
