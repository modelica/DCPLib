/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUNTFLOG_HPP
#define DCPLIB_DCPPDUNTFLOG_HPP

#include <dcp/model/pdu/DcpPdu.hpp>

class DcpPduNtfLog : public DcpPdu {
public:
    GET_FUN(getSender, uint8_t, 1)

    GET_FUN_PTR(getLogEntryPayload, uint8_t, 2)

    GET_FUN(getTime, uint64_t, 2)

    GET_FUN(getTemplateId, uint8_t, 10)

    GET_FUN_PTR(getPayload, uint8_t, 11)


    DcpPduNtfLog(unsigned char *stream, size_t stream_size) : DcpPdu(stream, stream_size){}

    DcpPduNtfLog(const uint8_t sender, const uint8_t templateId, const int64_t time, const uint8_t *payload,
                 const size_t payload_size) : DcpPdu(11 + payload_size, DcpPduType::NTF_log) {
        getSender() = sender;
        getTemplateId() = templateId;
        getTime() = time;
        memcpy(getPayload(), payload, payload_size);
    }

    DcpPduNtfLog(const uint8_t sender, const uint8_t templateId, const int64_t time,
                 size_t payload_size) : DcpPdu(11 + payload_size, DcpPduType::NTF_log) {
        getSender() = sender;
        getTemplateId() = templateId;
        getTime() = time;
    }

#if defined(DEBUG) || defined(LOGGING)
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPdu::operator<<(os);
        os << " payload=";
        for (size_t i = 11; i < stream_size; ++i)
            os << std::hex << std::setw(2) << std::setfill('0')
               << (int) stream[i] << " ";
        return os;
    }
#endif

    /**
  * Check if the stream_size is at least as much as the in the standard defined fixed part of an RSP_log_ack PDU.
  * @return stream_size is at least as much as the in the standard defined fixed part of an RSP_log_ack PDU
  */
    virtual bool isSizeCorrect() {
        return this->stream_size >= 11;
    }

    /**
      * Returns the minimum stream_size as the in the standard defined fixed part of an RSP_log_ack PDU.
      * @return the minimum stream_size as the in the standard defined fixed part of an RSP_log_ack PDU.
      */
    virtual size_t getCorrectSize() {
        return 11;
    }

};
#endif //DCPLIB_DCPPDUNTFLOG_HPP
