/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef PROJECT_HELPER_H_H
#define PROJECT_HELPER_H_H

#include <dcp/model/constant/DcpDataType.hpp>
#include <dcp/model/DcpTypes.hpp>

static inline int char2int(char input) {
    if(input >= '0' && input <= '9')
        return input - '0';
    if(input >= 'A' && input <= 'F')
        return input - 'A' + 10;
    if(input >= 'a' && input <= 'f')
        return input - 'a' + 10;
    return 0;
}

static inline uint128_t convertToUUID(std::string uuidStr){
    uint128_t uuid;
    int j = 0;
    int i = 0;
    //toDo: Find better solution which is also usable on arduino
    while(j < 16){
        if(uuidStr.at(i) == '-'){
            i++;
        } else {
            char first = uuidStr.at(i);
            char second = uuidStr.at(i+1);
            uint8_t b = char2int(first) * 16 + char2int(second);
            uuid.data[j] = b;
            j++;
            i = i + 2;
        }
    }

    return uuid;
}

static inline uint8_t getDcpDataTypeSize(DcpDataType type) {
    switch (type) {
        case DcpDataType::uint8:
            return 1;
        case DcpDataType::uint16:
            return 2;
        case DcpDataType::uint32:
            return 4;
        case DcpDataType::uint64:
            return 8;
        case DcpDataType::int8:
            return 1;
        case DcpDataType::int16:
            return 2;
        case DcpDataType::int32:
            return 4;
        case DcpDataType::int64:
            return 8;
        case DcpDataType::float32:
            return 4;
        case DcpDataType::float64:
            return 8;
        default:
            return 0;
    }
    return 0;
}

/**
* Transform an string in a human readable format to an DcpDataType.
* @param str
* @param type
*/
static DcpDataType from_string_DcpDataType(std::string str) {
    if (str == "uint8") {
        return DcpDataType::uint8;
    } else if (str == "uint16") {
        return DcpDataType::uint16;
    } else if (str == "uint32") {
        return DcpDataType::uint32;
    } else if (str == "uint64") {
        return DcpDataType::uint64;
    } else if (str == "int8") {
        return DcpDataType::int8;
    } else if (str == "int16") {
        return DcpDataType::int16;
    } else if (str == "int32") {
        return DcpDataType::int32;
    } else if (str == "int64") {
        return DcpDataType::int64;
    } else if (str == "float32") {
        return DcpDataType::float32;
    } else if (str == "float64") {
        return DcpDataType::float64;
    } else if (str == "string") {
        return DcpDataType::string;
    } else if (str == "binary") {
        return DcpDataType::binary;
    }
    return DcpDataType::uint8;
}


#endif //PROJECT_HELPER_H_H
