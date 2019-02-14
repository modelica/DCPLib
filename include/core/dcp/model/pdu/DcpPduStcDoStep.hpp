/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUDOSTEP_HPP
#define DCPLIB_DCPPDUDOSTEP_HPP

#include <dcp/model/pdu/DcpPduStc.hpp>

/**
 * This class decscribes the structure for the Pdu "STC_dostep".
 */
class DcpPduStcDoStep : public DcpPduStc {
public:

    /**
     * Get the steps.
     *@return the steps.
     */
    GET_FUN(getSteps, uint32_t, 5)

    /**
    /* Creates a AciPduDoStep from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduStcDoStep(unsigned char *stream, size_t stream_size) : DcpPduStc(stream, stream_size){}

    /**
     * Creates a new AciPduDoStep object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param state_id the state_id of the slaves state.
     * @param steps the steps.
     */
    DcpPduStcDoStep(const uint16_t pdu_seq_id, const uint8_t receiver, const DcpState state_id, const uint32_t steps) :
            DcpPduStc(9, DcpPduType::STC_do_step, pdu_seq_id, receiver, state_id) {
        getSteps() = steps;
    }
#if defined(DEBUG) || defined(LOGGING)

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " steps=" << getSteps();
        return os;
    }
#endif
    /**
   * Check if the stream_size is equal to the in the standard defined size.
   * @return stream_size is equal to the in the standard defined size
   */
    SIZE_HANDLING(9)

protected:
    /**
     * Creates a new AciPduDoStep object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param state_id the state_id of the slaves state.
     * @param steps the steps.
     */
    DcpPduStcDoStep(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id, const uint8_t receiver,
                 const DcpState state_id, const uint32_t steps) : DcpPduStc(stream_size, type_id,
                                                                                        pdu_seq_id, receiver,
                                                                                        state_id) {
        getSteps() = steps;
    }
};
#endif //DCPLIB_DCPPDUDOSTEP_HPP
