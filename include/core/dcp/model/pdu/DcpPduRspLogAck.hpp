/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUARSPLOGACK_HPP
#define DCPLIB_DCPPDUARSPLOGACK_HPP

#include <dcp/model/pdu/DcpPduRspAck.hpp>

class DcpPduRspLogAck : public DcpPduRspAck {
public:
    GET_FUN_PTR(getPayload, uint8_t, 4)

    DcpPduRspLogAck(unsigned char *stream, size_t stream_size) : DcpPduRspAck(stream, stream_size){}

    DcpPduRspLogAck(const uint8_t sender, const uint16_t resp_seq_id, uint8_t *payload, size_t payload_size) : DcpPduRspAck(
            4 + payload_size, DcpPduType::RSP_log_ack, sender, resp_seq_id) {
        memcpy(getPayload(), payload, payload_size);
    }

#if defined(DEBUG) || defined(LOGGING)
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduRspAck::operator<<(os);
        os << " payload=";
        for (size_t i = 4; i < stream_size; ++i)
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
        return this->stream_size >= 4;
    }

    /**
      * Returns the minimum stream_size as the in the standard defined fixed part of an RSP_log_ack PDU.
      * @return the minimum stream_size as the in the standard defined fixed part of an RSP_log_ack PDU.
      */
    virtual size_t getCorrectSize() {
        return 4;
    }

};
#endif //DCPLIB_DCPPDUARSPLOGACK_HPP
