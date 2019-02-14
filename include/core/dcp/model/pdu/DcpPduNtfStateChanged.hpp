/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUNTFSTATECHANGED_HPP
#define DCPLIB_DCPPDUNTFSTATECHANGED_HPP

#include <dcp/model/pdu/DcpPdu.hpp>

/**
 * This class decscribes the structure for the Pdu "NTF_state_changed".
 */
class DcpPduNtfStateChanged : public DcpPdu {
public:

    /**
    * Get the sender.
    *@return the sender.
    */
    GET_FUN(getSender, uint8_t, 1)

    /**
    * Get the sender.
    *@return the sender.
    */
    GET_FUN(getStateId, DcpState, 2)

    /**
    /* Creates a DcpStateChangedNotification from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduNtfStateChanged(unsigned char *stream, size_t stream_size) : DcpPdu(stream, stream_size){}

    /**
     * Creates a new DcpStateChangedNotification object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param numerator the numerator.
     * @param denominator the denominator.
     */
    DcpPduNtfStateChanged(const uint8_t sender, const DcpState stateId) :
            DcpPdu(3, DcpPduType::NTF_state_changed) {
        getSender() = sender;
        getStateId() = stateId;
    }
#if defined(DEBUG) || defined(LOGGING)
    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPdu::operator<<(os);
        os << " sender=" << unsigned(getSender());
        os << " state_id=" << getStateId();
        return os;
    }
#endif
    /**
       * Check if the stream_size is equal to the in the standard defined size.
       * @return stream_size is equal to the in the standard defined size
       */
    SIZE_HANDLING(3)
};
#endif //DCPLIB_DCPPDUNTFSTATECHANGED_HPP
