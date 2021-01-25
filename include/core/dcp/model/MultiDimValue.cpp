#include "MultiDimValue.hpp"

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

MultiDimValue::MultiDimValue(DcpDataType dataType, size_t baseSize, const std::vector<size_t> dimensions) : 
dataType(dataType), baseSize(baseSize), dimensions(dimensions) {
    numberOfAssignments = 1;
    for(size_t dim : dimensions){
        numberOfAssignments *= dim;
    }
    payload = new uint8_t[numberOfAssignments * baseSize];
}

MultiDimValue::~MultiDimValue(){
    delete[] payload;
}

size_t MultiDimValue::update(const uint8_t* newPayload, size_t start, DcpDataType sourceDataType){
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
            bool invalidPayload = false;
            for (int i = 0; i < numberOfAssignments; i++) {
                    uint32_t& length = *((uint32_t*)(payload + offset));
                    uint32_t& newLength = *((uint32_t*)(newPayload + (start + otherOffset)));
                    if (newLength <= baseSize) {
                        length = newLength;
                    }
                    else {
                        length = baseSize;
                        invalidPayload = true;
                    }
                    std::memcpy(payload + (offset + 4), newPayload + (start + otherOffset + 4), length);
                    offset += baseSize;
                    otherOffset += (newLength + 4);
            }
            if(invalidPayload) {
                throw std::range_error("maxSize exceeded");
            }
            break;
    }
    return otherOffset;
}