/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_LOGHELPER_HPP
#define DCPLIB_LOGHELPER_HPP

#include <typeinfo>
#include <iomanip>
#include <chrono>
#include <algorithm>

#include <dcp/model/constant/DcpDataType.hpp>
#include <dcp/model/constant/DcpPduType.hpp>
#include <dcp/model/constant/DcpState.hpp>
#include <dcp/model/constant/DcpOpMode.hpp>
#include <dcp/model/constant/DcpError.hpp>
#include <dcp/model/constant/DcpScope.hpp>
#include <dcp/model/constant/DcpLogMode.hpp>
#include <dcp/model/DcpTypes.hpp>
#include <dcp/model/constant/DcpTransportProtocol.hpp>
#include <dcp/model/LogTemplate.hpp>

#ifdef __GNUG__ // gnu C++ compiler

#include <cxxabi.h>

inline std::string demangle(const char* mangled_name) {
	std::size_t len = 0;
	int status = 0;
	std::unique_ptr< char, decltype(&std::free) > ptr(
		__cxxabiv1::__cxa_demangle(mangled_name, nullptr, &len, &status), &std::free);
	return ptr.get();
}

#else

inline std::string demangle(const char* name) { return name; }

#endif // _GNUG_

template<typename T>
std::string type_name()
{
    std::string tname = typeid(T).name();

    return demangle(tname.c_str());
}

template<typename T>
static DcpDataType getFixedSizeDcpDataType() {
    if (std::is_same<uint8_t, T>::value) {
        return DcpDataType::uint8;
    } else if (std::is_same<uint16_t, T>::value) {
        return DcpDataType::uint16;
    } else if (std::is_same<uint32_t, T>::value) {
        return DcpDataType::uint32;
    } else if (std::is_same<uint64_t, T>::value) {
        return DcpDataType::uint64;
    } else if (std::is_same<int8_t, T>::value) {
        return DcpDataType::int8;
    } else if (std::is_same<int16_t, T>::value) {
        return DcpDataType::int16;
    } else if (std::is_same<int32_t, T>::value) {
        return DcpDataType::int32;
    } else if (std::is_same<int64_t, T>::value) {
        return DcpDataType::int64;
    } else if (std::is_same<float32_t, T>::value) {
        return DcpDataType::float32;
    } else if (std::is_same<float64_t, T>::value) {
        return DcpDataType::float64;
    } else if (std::is_same<DcpLogLevel , T>::value) {
        return DcpDataType::uint8;
    } else if (std::is_same<DcpPduType , T>::value) {
        return DcpDataType::pduType;
    } else if (std::is_same<DcpState , T>::value) {
        return DcpDataType::state;
    }else if (std::is_same<DcpOpMode , T>::value) {
        return DcpDataType::opMode;
    }else if (std::is_same<DcpDataType , T>::value) {
        return DcpDataType::dataType;
    }else if (std::is_same<DcpError , T>::value) {
        return DcpDataType::error;
    }else if (std::is_same<DcpScope, T>::value) {
        return DcpDataType::scope;
    }else if (std::is_same<DcpTransportProtocol , T>::value) {
        return DcpDataType::transportProtocol;
    }else if (std::is_same<DcpLogMode, T>::value) {
        return DcpDataType::logMode;
    } else if (std::is_same<DcpLogLevel , T>::value) {
        return DcpDataType::logLevel;
    }
    throw std::invalid_argument(type_name<T>() + " is not an fixed size DcpDataType");
    return DcpDataType::uint8;
}

namespace DcpLogHelper {
    template<typename T>
    inline size_t calcsize(const T val) {
        return sizeof(T);
    };

    template<>
    inline size_t calcsize(const std::string val) {
        return val.length() + 2;
    };

    template<>
    inline size_t calcsize(const char *const val) {
        return calcsize(std::string(val));
    };

    inline size_t size() {
        return 0;
    };

    template<typename T, typename ... Args>
    inline size_t size(const T *val, const Args... args) {
        return calcsize<T>(*val) + size(args...);
    };

    template<typename T>
    inline size_t applyField(uint8_t *payload, const T val) {
        *((T *) payload) = val;
        return sizeof(T);
    };

    template<>
    inline size_t applyField(uint8_t *payload, const std::string val) {
        *((uint16_t *) payload) = val.length();
        std::memcpy(payload + 2, val.c_str(), val.length());
        return val.length() + 2;
    };

    template<>
    inline size_t applyField(uint8_t *payload, const char *const val) {
        return applyField(payload, std::string(val));
    };

    inline void applyFields(uint8_t *payload) {
        //exit recursion
    };

    template<typename T>
    inline void applyFields(uint8_t *payload, const T val) {
        applyField<T>(payload, val);
    };

    template<typename T, typename ... Args>
    inline void applyFields(uint8_t *payload, const T val, const Args... args) {
        applyFields(payload + applyField<T>(payload, val), args...);
    };

    template<typename T>
    inline void _checkDataTypes(const LogTemplate &logTemplate, const size_t index, const T val) {
        if(index >= logTemplate.dataTypes.size()){
            throw std::invalid_argument("To many arguments given for template id " + std::to_string(logTemplate.id));
        }
        try {
            if (logTemplate.dataTypes.at(index) != getFixedSizeDcpDataType<T>()) {
                throw std::invalid_argument("Wrong data type in for template_id " + std::to_string(logTemplate.id) + " (index = " +
                                            std::to_string(index) + " ). Is " + type_name<T>()+ ". Is expected to be " +
                                            to_string(logTemplate.dataTypes.at(index)) + " on " +
                                            std::to_string(index) + ". index.");
            }
        } catch(std::exception e){
            std::cout << e.what() << std::endl;
            throw std::invalid_argument("Error for template_id " + std::to_string(logTemplate.id) + "index=" + std::to_string(index) + " type=" + type_name<T>());
        }
    }

    template<>
    inline void _checkDataTypes(const LogTemplate &logTemplate, const size_t index, const std::string val) {
        if(index >= logTemplate.dataTypes.size()){
            throw std::invalid_argument("To many arguments given for template id " + std::to_string(logTemplate.id));
        }
        try {
            if (logTemplate.dataTypes.at(index) != DcpDataType::string) {
                throw std::invalid_argument("Wrong data type in for template_id " + std::to_string(logTemplate.id) + " (index = " +
                                            std::to_string(index) + " ). Is string. Is expected to be " +
                                            to_string(logTemplate.dataTypes.at(index)) + " on " +
                                            std::to_string(index) + ". index.");
            }
        } catch(std::exception e){
            throw std::invalid_argument("Error for template_id " + std::to_string(logTemplate.id));
        }
    }

    template<>
    inline void
    _checkDataTypes(const LogTemplate &logTemplate, const size_t index, const char *const val) {
        _checkDataTypes(logTemplate, index, std::string(val));
    }

    inline void checkDataTypes(const LogTemplate &logTemplate, const size_t index) {
        //exit recursion
    }

    template<typename T, typename ... Args>
    inline void checkDataTypes(const LogTemplate &logTemplate, const size_t index, const T val, const Args... args) {
        _checkDataTypes<T>(logTemplate, index, val);
        checkDataTypes(logTemplate, index + 1, args...);
    }
}
static std::string to_string(std::chrono::system_clock::time_point tp){
    // convert to std::time_t in order to convert to std::tm (broken time)
    auto timer = std::chrono::system_clock::to_time_t(tp);

    // convert to broken time
    std::tm bt = *std::localtime(&timer);

    std::ostringstream oss;
    oss << std::put_time(&bt, "%F"); // %Y-%m-%d
    oss << " ";
    oss << std::put_time(&bt, "%T"); // HH:MM:SS
    return oss.str();
}

static inline std::string convertUnixTimestamp(int64_t unixTimestamp){
    return to_string(std::chrono::system_clock::time_point(std::chrono::seconds(unixTimestamp)));
}
static inline std::string currentTime(){
    return to_string(std::chrono::system_clock::now());
}

static inline std::string convertToUUIDStr(uint128_t uuid){
    std::stringstream sstream;
    for(int i = 0; i < 16; i++){
        sstream << std::hex << std::setw(2) << std::setfill('0')
                << unsigned(uuid.data[i]);
        if(i == 3 || i == 5 || i == 7 || i == 9){
            sstream << "-";
        }
    }
    return sstream.str();
}

#endif //DCPLIB_LOGHELPER_HPP