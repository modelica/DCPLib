/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDU_HPP
#define DCPLIB_DCPPDU_HPP

#define PDU_LENGTH_INDICATOR_SIZE 4

//Define macros
#define GET_FUN(function, type, offset) inline type& function(){return *((type*)(stream + (offset + PDU_LENGTH_INDICATOR_SIZE)));}
#define GET_FUN_PTR(function, type, offset) inline type* function(){return ((type*)(stream + (offset + PDU_LENGTH_INDICATOR_SIZE)));}
#define SIZE_HANDLING(size) virtual bool isSizeCorrect(){return this->stream_size - PDU_LENGTH_INDICATOR_SIZE == size;} \
    virtual size_t getCorrectSize(){return size;}

#define CHECK_TYPE_ID

//Include files

#include "dcp/helper/Helper.hpp"
#include "dcp/model/constant/DcpPduType.hpp"

#include <cstdint>
#include <cstring>
#include <string>
#if defined(DEBUG) || defined(LOGGING)
#include <sstream>
#include <iostream>
#include <iomanip>
#include <dcp/helper/LogHelper.hpp>
#endif

/**
 * This class describes the a general DCP PDU.
 * @author Christian Kater
 */
class DcpPdu {
public:

    /**
     * Creates a DcpPdu from existing byte array.
     * @param stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
     * @param stream_size number of bytes in stream
     */
    DcpPdu(unsigned char *stream, size_t pduSize) {
        this->stream = stream;
        setPduSize(pduSize);
        this->deleteStream = false;
    }

    /**
     * Get the type id.
     * @return the type id.
     */
    GET_FUN(getTypeId, DcpPduType, 0)

    ~DcpPdu() {
        if (deleteStream) {
            delete[] stream;
        }
    }

    /**
     * Serialize the DcpPdu
     * @return byte array containing all data with respect to Aci specification.
     */
    unsigned char *serialize() {
        return stream;
    }

    /**
    * Serialize the DcpPdu
    * @return byte array containing all data with respect to Aci specification.
    */
    unsigned char *serializePdu() {
        return stream + PDU_LENGTH_INDICATOR_SIZE;
    }

    /**
     * Number of bytes in serialized DcpPdu
     * @return size of serialized DcpPdu in bytes
     */
    size_t getSerializedSize() {
        return stream_size;
    }

    /**
     * Number of bytes in serialized DcpPdu
     * @return size of serialized DcpPdu in bytes
     */
    size_t getPduSize() {
        return stream_size - PDU_LENGTH_INDICATOR_SIZE;
    }

    /**
     * Check if the stream_size is equal to the in the standard defined size.
     * Special case type_id == DATA: This only checks if size greate than fixed part.
     * @return stream_size is equal to the in the standard defined size
     */
    virtual bool isSizeCorrect() { return false; };

    virtual size_t getCorrectSize() { return 0; };
#if defined(DEBUG) || defined(LOGGING)
    /**
     * Returns a string with the hexadecimal representation of each byte of the DcpPdu
     * @return DcpPdu in hex
     */
    std::string getAsHex() {
        std::stringstream hs;
        for (size_t i = 4; i < stream_size; ++i)
            hs << std::hex << std::setw(2) << std::setfill('0')
               << (int) stream[i] << " ";
        return hs.str();
    }

    std::string to_string() {
        std::ostringstream oss;
        operator<<(oss);
        return oss.str();
    }

    /**
      * Writes the Pdu in a human readable format to the given stream.
      * @param os stream to write on.
      */
    virtual std::ostream &operator<<(std::ostream &os) {
        os << "type_id=" << getTypeId();
        return os;
    }
#endif

    void setPduSize(size_t pduSize) {
        this->stream_size = pduSize + PDU_LENGTH_INDICATOR_SIZE;
        *((uint32_t*) this->stream) = pduSize;
    }

protected:
    /**
     * byte array containg pdu data
     */
    unsigned char *stream;
    /**
     * size of pdu byte stream
     */
    size_t stream_size;
    /**
     * indicates weather stream should be deleted on distruction or not
     */
    bool deleteStream;

    DcpPdu() {}

    /**
     * creates an new DcpPdu with all fields are 0
     * @param stream_size how much bytes are needed for the DcpPdu
     * @param type type of DcpPdu
     */
    DcpPdu(size_t pduSize, DcpPduType type) {
        stream = new unsigned char[pduSize + PDU_LENGTH_INDICATOR_SIZE];
        getTypeId() = type;
        setPduSize(pduSize);
        this->deleteStream = true;
    }
};

#endif //DCPLIB_DCPPDU_HPP