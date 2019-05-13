/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDURSPACK_HPP
#define DCPLIB_DCPPDURSPACK_HPP

#include <dcp/model/pdu/DcpPdu.hpp>

/**
 * This class decscribes the structure for the Pdu "RSP_ack".
 */
class DcpPduRspAck : public DcpPdu {
public:

    /**
     * Get the sender.
     *@return the sender.
     */
    GET_FUN(getSender, uint8_t, 3)

    /**
     * Get the resp_seq_id.
     *@return the resp seq id.
     */
    GET_FUN(getRespSeqId, uint16_t, 1)

    /**
    /* Creates a AciPduAck from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduRspAck(unsigned char *stream, size_t stream_size) :DcpPdu(stream, stream_size) {}

    /**
     * Creates a new AciPduAck object.
     * @param sender the sender.
     * @param resp_seq_id the resp seq id.
     */
    DcpPduRspAck(const uint8_t sender, const uint16_t resp_seq_id) : DcpPdu(4, DcpPduType::RSP_ack) {
        getSender() = sender;
        getRespSeqId() = resp_seq_id;
    }

#if defined(DEBUG) || defined(LOGGING)
    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPdu::operator<<(os);
        os << " sender=" << unsigned(getSender());
        os << " resp_seq_id=" << getRespSeqId();
        return os;
    }
#endif

    /**
     * Check if the stream_size is equal to the in the standard defined size.
     * @return stream_size is equal to the in the standard defined size
     */
    SIZE_HANDLING(4)

protected:

    DcpPduRspAck() {}

    /**
     * Creates a new AciPduAck object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param sender the sender.
     * @param resp_seq_id the resp seq id.
     */
    DcpPduRspAck(const size_t stream_size, const DcpPduType type_id, const uint8_t sender, const uint16_t resp_seq_id)
            : DcpPdu(stream_size, type_id) {
        getSender() = sender;
        getRespSeqId() = resp_seq_id;
    }
};
#endif //DCPLIB_DCPPDURSPACK_HPP
