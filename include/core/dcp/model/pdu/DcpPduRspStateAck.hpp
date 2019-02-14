/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDURSPSTATEACK_HPP
#define DCPLIB_DCPPDURSPSTATEACK_HPP

#include <dcp/model/pdu/DcpPduRspAck.hpp>

/**
 * This class decscribes the structure for the Pdu "RSP_state_ack".
 */
class DcpPduRspStateAck : public DcpPduRspAck {
public:

    /**
     * Get the state_id.
     *@return the state id.
     */
    GET_FUN(getStateId, DcpState, 4)

    /**
    /* Creates a DcpPduRspStateAck from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduRspStateAck(unsigned char *stream, size_t stream_size) : DcpPduRspAck(stream, stream_size){}

    /**
     * Creates a new DcpPduRspStateAck object.
     * @param sender the sender.
     * @param resp_seq_id the resp seq id.
     * @param state_id the state id.
     */
    DcpPduRspStateAck(const uint8_t sender, const uint16_t resp_seq_id, const DcpState state_id) :
            DcpPduRspAck(5, DcpPduType::RSP_state_ack, sender, resp_seq_id) {
        getStateId() = state_id;
    }

#if defined(DEBUG) || defined(LOGGING)
    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduRspAck::operator<<(os);
        os << " state_id=" << getStateId();
        return os;
    }
#endif

    /**
     * Check if the stream_size is equal to the in the standard defined size.
     * @return stream_size is equal to the in the standard defined size
     */
    SIZE_HANDLING(5)

protected:
    /**
     * Creates a new DcpPduRspStateAck object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param sender the sender.
     * @param resp_seq_id the resp seq id.
     * @param state_id the state id.
     */
    DcpPduRspStateAck(const size_t stream_size, const DcpPduType type_id, const uint8_t sender, const uint16_t resp_seq_id,
                      const DcpState state_id) : DcpPduRspAck(stream_size, type_id, sender, resp_seq_id) {
        getStateId() = state_id;
    }
};

#endif //DCPLIB_DCPPDURSPSTATEACK_HPP
