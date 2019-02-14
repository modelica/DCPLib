/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUCFGSCOPE_HPP
#define DCPLIB_DCPPDUCFGSCOPE_HPP

#include <dcp/model/pdu/DcpPduBasic.hpp>

/**
* This class decscribes the structure for the Pdu "CFG_data_id".
*/
class DcpPduCfgScope : public DcpPduBasic {
public:


    /**
     * Get the data_id.
     *@return the data id.
     */
    GET_FUN(getDataId, uint16_t, 4)

    GET_FUN(getScope, DcpScope, 6);

    /**
    /* Creates a AciPduSetCommStepSize from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduCfgScope(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

    /**
     * Creates a new AciPduSetCommStepSize object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param steps the steps.
     * @param data_id the data id.
     */
    DcpPduCfgScope(const uint16_t pdu_seq_id, const uint8_t receiver, const uint16_t data_id, const DcpScope scope) :
            DcpPduBasic(7, DcpPduType::CFG_scope, pdu_seq_id, receiver) {
        getDataId() = data_id;
        getScope() = scope;
    }
#if defined(DEBUG) || defined(LOGGING)
    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " data_id=" << getDataId();
        os << " scope=" << getScope();

        return os;
    }
#endif
    /**
   * Check if the stream_size is equal to the in the standard defined size.
   * @return stream_size is equal to the in the standard defined size
   */
    SIZE_HANDLING(7)
};

#endif //DCPLIB_DCPPDUCFGSCOPE_HPP
