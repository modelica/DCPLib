/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUTIMERES_HPP
#define DCPLIB_DCPPDUTIMERES_HPP

#include <dcp/model/pdu/DcpPduBasic.hpp>

/**
 * This class decscribes the structure for the Pdu "CFG_time_res".
 */
class DcpPduCfgTimeRes : public DcpPduBasic {
public:

    /**
     * Get the numerator.
     *@return the numerator.
     */
    GET_FUN(getNumerator, uint32_t, 4)

    /**
     * Get the denominator.
     *@return the denominator.
     */
    GET_FUN(getDenominator, uint32_t, 8)

    /**
    /* Creates a AciPduSetTimeRes from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduCfgTimeRes(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

    /**
     * Creates a new AciPduSetTimeRes object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param numerator the numerator.
     * @param denominator the denominator.
     */
    DcpPduCfgTimeRes(const uint16_t pdu_seq_id, const uint8_t receiver, const uint32_t numerator,
                  const uint32_t denominator) :
            DcpPduBasic(12, DcpPduType::CFG_time_res, pdu_seq_id, receiver) {
        getNumerator() = numerator;
        getDenominator() = denominator;
    }
#if defined(DEBUG) || defined(LOGGING)
    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " numerator=" << getNumerator();
        os << " denominator=" << getDenominator();
        return os;
    }
#endif
    /**
   * Check if the stream_size is equal to the in the standard defined size.
   * @return stream_size is equal to the in the standard defined size
   */
    SIZE_HANDLING(12)

protected:
    /**
     * Creates a new AciPduSetTimeRes object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param numerator the numerator.
     * @param denominator the denominator.
     */
    DcpPduCfgTimeRes(const int stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id, const uint8_t receiver,
                  const uint32_t numerator, const uint32_t denominator) : DcpPduBasic(stream_size, type_id,
                                                                                      pdu_seq_id, receiver) {
        getNumerator() = numerator;
        getDenominator() = denominator;
    }
};
#endif //DCPLIB_DCPPDUTIMERES_HPP
