/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUSTATETRANSITION_HPP
#define DCPLIB_DCPPDUSTATETRANSITION_HPP

#include <dcp/model/pdu/DcpPduBasic.hpp>
#include <dcp/model/constant/DcpState.hpp>

/**
 * This class decscribes the structure for the Pdus "STC_initialize", "STC_synchronize",
 * "STC_unregister", "STC_resynchronize", "STC_stop", "STC_reset" & "STC_sendoutputs".
 */
class DcpPduStc : public DcpPduBasic {
public:

    /**
     * Get thet state_id of the slaves state.
     *@return the state_id.
     */
    GET_FUN(getStateId, DcpState, 4)

    /**
    /* Creates a AciPduBasicStateTransition from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduStc(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size) {}

    /**
     * Creates a new DcpPduBasic object.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param state_id the state_id of the slaves state.
     */
    DcpPduStc(const DcpPduType type_id, const uint16_t pdu_seq_id, const uint8_t receiver,
                          const DcpState state_id) : DcpPduBasic(5, type_id, pdu_seq_id, receiver) {
        getStateId() = state_id;
    }
#if defined(DEBUG) || defined(LOGGING)

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
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

    DcpPduStc() {}

    /**
     * Creates a new DcpPduBasic object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param state_id the state_id of the slaves state.

     */
    DcpPduStc(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                          const uint8_t receiver, const DcpState state_id) : DcpPduBasic(stream_size, type_id,
                                                                                         pdu_seq_id, receiver) {
        getStateId() = state_id;
    }
};

#endif //DCPLIB_DCPPDUSTATETRANSITION_HPP
