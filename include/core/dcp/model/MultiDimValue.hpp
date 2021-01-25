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

class MultiDimValue{
public:
    MultiDimValue(DcpDataType dataType, size_t baseSize, const std::vector<size_t> dimensions);

    ~MultiDimValue();

    template<typename T>
    void updateValue(size_t index, DcpDataType dataType, T value){
        size_t baseSize = getDcpDataTypeSize(dataType);
        update((uint8_t*) &value, baseSize * index, dataType);
    }

    size_t update(const uint8_t* newPayload, size_t start, DcpDataType sourceDataType);

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