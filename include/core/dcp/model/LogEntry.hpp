/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef ACOSAR_DCPLOGENTRY_H
#define ACOSAR_DCPLOGENTRY_H

#include <cstdint>
#include <regex>
#include <string>
#include <iomanip>
#include <sstream>
#include <dcp/model/LogTemplate.hpp>
#include <dcp/model/DcpTypes.hpp>
#include <dcp/model/constant/DcpState.hpp>
#include <dcp/model/constant/DcpOpMode.hpp>

class LogEntry {
public:

    LogEntry(const LogTemplate &_logTemplate, uint8_t *oPayload, const size_t oPayloadSize) : logTemplate(
            _logTemplate) {
        this->payload = oPayload;
        this->size = oPayloadSize;
    }

    ~LogEntry() {
        if(ownsPayload){
            delete[] payload;
        }
    }

    size_t applyPayloadToString() {
        size_t offset = 9;
        msg = std::string(logTemplate.msg);
        for (DcpDataType type : logTemplate.dataTypes) {
            std::string value;
            switch (type) {
                case DcpDataType::int8: {
                    value = std::to_string((int) *((int8_t *) (payload + offset)));
                    offset += 1;
                    break;
                }
                case DcpDataType::int16: {
                    value = std::to_string(*((int16_t *) (payload + offset)));
                    offset += 2;
                    break;

                }
                case DcpDataType::int32: {
                    value = std::to_string(*((int32_t *) (payload + offset)));
                    offset += 4;
                    break;

                }
                case DcpDataType::int64: {
                    value = std::to_string(*((int64_t *) (payload + offset)));
                    offset += 8;
                    break;

                }
                case DcpDataType::uint8: {
                    value = std::to_string((int) *((uint8_t *) (payload + offset)));
                    offset += 1;
                    break;

                }
                case DcpDataType::uint16: {
                    value = std::to_string(*((uint16_t *) (payload + offset)));
                    offset += 2;
                    break;

                }
                case DcpDataType::uint32: {
                    value = std::to_string(*((uint32_t *) (payload + offset)));
                    offset += 4;
                    break;

                }
                case DcpDataType::uint64: {
                    value = std::to_string(*((uint64_t *) (payload + offset)));
                    offset += 8;
                    break;

                }
                case DcpDataType::float32: {
                    std::stringstream stream;
                    stream << *((float32_t *) (payload + offset));
                    value = stream.str();
                    offset += 4;
                    break;

                }
                case DcpDataType::float64: {
                    std::stringstream stream;
                    stream << *((float64_t *) (payload + offset));
                    value = stream.str();
                    offset += 8;
                    break;

                }
                case DcpDataType::binary: {
                    std::stringstream hs;
                    size_t length = *((int16_t *) (payload + offset));
                    for (size_t i = 0; i < length; ++i)
                        hs << std::hex << std::setw(2) << std::setfill('0')
                           << (int) *(payload + (offset + 2 + i)) << " ";
                    value = hs.str();
                    offset += (length + 2);
                    break;

                }
                case DcpDataType::string: {
                    size_t length = *((int16_t *) (payload + offset));
                    value = std::string((char *) (payload + (offset + 2)), length);
                    offset += (length + 2);
                    break;

                }
                case DcpDataType::state: {
                    value = to_string(*((DcpState*) (payload + offset)));
                    offset += 1;
                    break;
                }
                case DcpDataType::opMode: {
                    value = to_string(*((DcpOpMode*) (payload + offset)));
                    offset += 1;
                    break;
                }
                case DcpDataType::pduType: {
                    value = to_string(*((DcpPduType*)(payload + offset)));
                    offset += 1;
                    break;
                }
                case DcpDataType::dataType: {
                    value = to_string(*((DcpDataType*)(payload + offset)));
                    offset += 1;
                    break;
                }
            }


            std::string typeStr = "%" + to_string(type);
            const size_t pos = msg.find(typeStr);
            msg.replace(pos, typeStr.length(), value);

        }
        std::stringstream hs;

        this->size = offset;
        this->msgGenerated = true;
        return offset;
    }

    int64_t getTime() const {
        return *((int64_t *) payload);
    }

    uint8_t getId() const {
        return logTemplate.id;
    }

    const std::string &getRawMsg() const {
        return logTemplate.msg;
    }

    const std::string &getMsg() const {

        return msg;
    }

    const std::string &getGeneratedMsg() {
        if (!msgGenerated) {
            applyPayloadToString();
        }
        return msg;
    }

    uint8_t getCategory() const {
        return logTemplate.category;
    }

    DcpLogLevel getLevel() const {
        return logTemplate.level;
    }

    uint8_t *serialize() const {
        return payload;
    }

    const size_t serializedSize() const {
        return size;
    }


    void setPayload(uint8_t* payload) {
        ownsPayload = true;
        this->payload = payload;
    }



private:
    const LogTemplate &logTemplate;
    uint8_t *payload;
    size_t size;
    std::string msg;
    bool msgGenerated = false;
    bool ownsPayload = false;
};

#endif //ACOSAR_DCPLOGENTRY_H
