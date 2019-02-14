/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUBASIC_HPP
#define DCPLIB_DCPPDUBASIC_HPP

#include <dcp/model/pdu/DcpPdu.hpp>

/**
 * This class decscribes the structure for the Pdus "INF_state", "INF_error", CFG_clear_config.
 */

class DcpPduBasic : public DcpPdu {
public:

    /**
     * Get the pdu_seq_id.
     *@return the pdu seq id.
     */
    GET_FUN(getPduSeqId, uint16_t, 1)

    /**
     * Get the receiver.
     *@return the receiver.
     */
    GET_FUN(getReceiver, uint8_t, 3)

    /**
    /* Creates a AciPduBasic from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduBasic(unsigned char *stream, size_t stream_size) : DcpPdu(stream, stream_size){}

    /**
     * Creates a new DcpPduBasic object.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     */
    DcpPduBasic(const DcpPduType type_id, const uint16_t pdu_seq_id, const uint8_t receiver) : DcpPdu(4, type_id) {
        getPduSeqId() = pdu_seq_id;
        getReceiver() = receiver;
    }
#if defined(DEBUG) || defined(LOGGING)

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPdu::operator<<(os);
        os << " pdu_seq_id=" << getPduSeqId();
        os << " receiver=" << unsigned(getReceiver());
        return os;
    }
#endif
    /**
	 * Check if the stream_size is equal to the in the standard defined size.
	 * @return stream_size is equal to the in the standard defined size
	 */
    SIZE_HANDLING(4)

protected:

    DcpPduBasic() {}

    /**
     * Creates a new DcpPduBasic object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     */
    DcpPduBasic(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id, const uint8_t receiver)
            : DcpPdu(stream_size, type_id) {
        getPduSeqId() = pdu_seq_id;
        getReceiver() = receiver;
    }
};
#endif //DCPLIB_DCPPDUBASIC_HPP
