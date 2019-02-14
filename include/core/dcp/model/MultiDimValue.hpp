/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef ACOSAR_MULTIDIMVALUE_H
#define ACOSAR_MULTIDIMVALUE_H


#include <vector>
#include <dcp/helper/Helper.hpp>
#include <dcp/model/constant/DcpDataType.hpp>
#include <dcp/model/DcpString.hpp>
#include <dcp/model/DcpBinary.hpp>


#define CAST(T1, T2)\
    for(int i = 0; i < numberOfAssignments; i++){\
        *((T1*)(payload + offset)) = *((T2*)(newPayload + (start + otherOffset)));\
        offset += baseSize;\
        otherOffset += otherBaseSize;\
    }

#define INNER_CASE_CAST(val, T1, T2) \
		case DcpDataType::val : \
		 	 CAST(T1, T2) \
		 	 break;

#define INNER_SWITCH_START switch(sourceDataType){
#define INNER_SWITCH_END \
        default: \
        break;\
    } \
    break;

class MultiDimValue{
public:
    MultiDimValue(DcpDataType dataType, size_t baseSize, const std::vector<size_t> dimensions) : dataType(dataType),
                                                                                                  baseSize(baseSize),
                                                                                                  dimensions(
                                                                                                          dimensions) {
        numberOfAssignments = 1;
        for(size_t dim : dimensions){
            numberOfAssignments *= dim;
        }
        payload = new uint8_t[numberOfAssignments * baseSize];
    }

    ~MultiDimValue(){
        delete[] payload;
    }

    template<typename T>
    void updateValue(size_t index, DcpDataType dataType, T value){
        size_t baseSize = getDcpDataTypeSize(dataType);
        update((uint8_t*) &value, baseSize * index, dataType);
    }

    size_t update(const uint8_t* newPayload, size_t start, DcpDataType sourceDataType){
        size_t offset = 0;
        size_t otherOffset = 0;
        size_t otherBaseSize = getDcpDataTypeSize(sourceDataType);
        switch (dataType) {
            case DcpDataType::uint8:
                INNER_SWITCH_START //
                    INNER_CASE_CAST(uint8, uint8_t, uint8_t)
            INNER_SWITCH_END

            case DcpDataType::uint16:
                INNER_SWITCH_START //
                    INNER_CASE_CAST(uint8, uint16_t, uint8_t)
                    INNER_CASE_CAST(uint16, uint16_t, uint16_t)
            INNER_SWITCH_END

            case DcpDataType::uint32:
                INNER_SWITCH_START //
                    INNER_CASE_CAST(uint8, uint32_t, uint8_t)
                    INNER_CASE_CAST(uint16, uint32_t, uint16_t)
                    INNER_CASE_CAST(uint32, uint32_t, uint32_t)
            INNER_SWITCH_END

            case DcpDataType::uint64:
                INNER_SWITCH_START //
                    INNER_CASE_CAST(uint8, uint64_t, uint8_t)
                    INNER_CASE_CAST(uint16, uint64_t, uint16_t)
                    INNER_CASE_CAST(uint32, uint64_t, uint32_t)
                    INNER_CASE_CAST(uint64, uint64_t, uint64_t)
            INNER_SWITCH_END

            case DcpDataType::int8:
                INNER_SWITCH_START //
                    INNER_CASE_CAST(int8, int8_t, int8_t)
            INNER_SWITCH_END

            case DcpDataType::int16:
                INNER_SWITCH_START //
                    INNER_CASE_CAST(int8, int16_t, int8_t)
                    INNER_CASE_CAST(int16, int16_t, int16_t)
                    INNER_CASE_CAST(uint8, int16_t, uint8_t)
            INNER_SWITCH_END

            case DcpDataType::int32:
                INNER_SWITCH_START //
                    INNER_CASE_CAST(int8, int32_t, int8_t)
                    INNER_CASE_CAST(int16, int32_t, int16_t)
                    INNER_CASE_CAST(int32, int32_t, int32_t)
                    INNER_CASE_CAST(uint8, int32_t, uint8_t)
                    INNER_CASE_CAST(uint16, int32_t, uint16_t)
            INNER_SWITCH_END

            case DcpDataType::int64:
                INNER_SWITCH_START //
                    INNER_CASE_CAST(int8, int64_t, int8_t)
                    INNER_CASE_CAST(int16, int64_t, int16_t)
                    INNER_CASE_CAST(int32, int64_t, int32_t)
                    INNER_CASE_CAST(int64, int64_t, int64_t)
                    INNER_CASE_CAST(uint8, int64_t, uint8_t)
                    INNER_CASE_CAST(uint16, int64_t, uint16_t)
                    INNER_CASE_CAST(uint32, int64_t, uint32_t)
            INNER_SWITCH_END

            case DcpDataType::float32:
                INNER_SWITCH_START //
                    INNER_CASE_CAST(int8, float32_t, int8_t)
                    INNER_CASE_CAST(int16, float32_t, int16_t)
                    INNER_CASE_CAST(uint8, float32_t, uint8_t)
                    INNER_CASE_CAST(uint16, float32_t, uint16_t)
                    INNER_CASE_CAST(float32, float32_t, float32_t)
            INNER_SWITCH_END

            case DcpDataType::float64:
                INNER_SWITCH_START //
                    INNER_CASE_CAST(int8, float64_t, int8_t)
                    INNER_CASE_CAST(int16, float64_t, int16_t)
                    INNER_CASE_CAST(int32, float64_t, int32_t)
                    INNER_CASE_CAST(uint8, float64_t, uint8_t)
                    INNER_CASE_CAST(uint16, float64_t, uint16_t)
                    INNER_CASE_CAST(uint32, float64_t, uint32_t)
                    INNER_CASE_CAST(float32, float64_t, float32_t)
                    INNER_CASE_CAST(float64, float64_t, float64_t)
            INNER_SWITCH_END
            case DcpDataType::binary:
            case DcpDataType::string:
                for (int i = 0; i < numberOfAssignments; i++) {
                        uint32_t& length = *((uint32_t*)(payload + offset));
                        uint32_t& newLength = *((uint32_t*)(newPayload + (start + otherOffset)));
                        length = newLength;
                        std::memcpy(payload + (offset + 4), newPayload + (start + otherOffset + 4), newLength);
                        offset += baseSize;
                        otherOffset += (newLength + 4);
                }
                break;
        }
        return otherOffset;
    }

    inline size_t serialize(uint8_t* output, size_t start){
        size_t offset = 0;
        size_t otherOffset = 0;
        switch(dataType){
            case DcpDataType::binary:
            case DcpDataType::string:
                for (int i = 0; i < numberOfAssignments; i++) {
                    uint32_t& length = *((uint32_t*)(payload + offset));
                    uint32_t& outLength = *((uint32_t*)(output + (start + otherOffset)));
                    outLength = length;
                    std::memcpy(output + (start + otherOffset + 4), payload + (offset + 4), outLength);
                    offset += baseSize;
                    otherOffset += (outLength + 4);
                }
                break;
            default:
                for (int i = 0; i < numberOfAssignments; i++) {
                    std::memcpy(output + (start + otherOffset), payload + otherOffset, baseSize);
                    otherOffset += baseSize;
                }
                break;
        }
        return otherOffset;
    }

    inline DcpDataType getDataType(){
        return dataType;
    }

    inline  std::vector<size_t> getDimensions(){
        return dimensions;
    }

    inline size_t getBaseSize(){
        return baseSize;
    }

    template<typename T>
    inline T getValue() {
        static_assert(std::is_pointer<T>::value, "Expected a pointer");
        return ((T)(payload));
    }






private:
    DcpDataType dataType;
    size_t baseSize;
    size_t numberOfAssignments;
    uint8_t* payload;

    std::vector<size_t> dimensions;

};


#endif //ACOSAR_MULTIDIMVALUE_H
