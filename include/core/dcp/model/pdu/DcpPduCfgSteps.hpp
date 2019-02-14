/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUCFGSTEPS_HPP
#define DCPLIB_DCPPDUCFGSTEPS_HPP

#include <dcp/model/pdu/DcpPduBasic.hpp>

/**
 * This class decscribes the structure for the Pdu "CFG_steps".
 */
class DcpPduCfgSteps : public DcpPduBasic {
public:

    /**
     * Get the steps.
     *@return the steps.
     */
    GET_FUN(getSteps, uint32_t, 4)

    /**
     * Get the data_id.
     *@return the data id.
     */
    GET_FUN(getDataId, uint16_t, 8)

    /**
    /* Creates a AciPduSetCommStepSize from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduCfgSteps(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

    /**
     * Creates a new AciPduSetCommStepSize object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param steps the steps.
     * @param data_id the data id.
     */
    DcpPduCfgSteps(const uint16_t pdu_seq_id, const uint8_t receiver, const uint32_t steps, const uint16_t data_id) :
            DcpPduBasic(10, DcpPduType::CFG_steps, pdu_seq_id, receiver) {
        getSteps() = steps;
        getDataId() = data_id;
    }
#if defined(DEBUG) || defined(LOGGING)

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " steps=" << getSteps();
        os << " data_id=" << getDataId();
        return os;
    }
#endif
    /**
   * Check if the stream_size is equal to the in the standard defined size.
   * @return stream_size is equal to the in the standard defined size
   */
    SIZE_HANDLING(10)

protected:
    /**
     * Creates a new AciPduSetCommStepSize object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param steps the steps.
     * @param data_id the data id.
     */
    DcpPduCfgSteps(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                   const uint8_t receiver, const uint32_t steps, const uint16_t data_id) : DcpPduBasic(stream_size,
                                                                                                       type_id,
                                                                                                       pdu_seq_id,
                                                                                                       receiver) {
        getSteps() = steps;
        getDataId() = data_id;
    }
};
#endif //DCPLIB_DCPPDUCFGSTEPS_HPP
