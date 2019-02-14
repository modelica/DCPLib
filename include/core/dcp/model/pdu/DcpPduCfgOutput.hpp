/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUCFGOUTPUT_HPP
#define DCPLIB_DCPPDUCFGOUTPUT_HPP

#include <dcp/model/pdu/DcpPduBasic.hpp>

/**
 * This class decscribes the structure for the Pdu "CFG_output".
 */
class DcpPduCfgOutput : public DcpPduBasic {
public:

    /**
     * Get the data_id .
     *@return the data id .
     */
    GET_FUN(getDataId, uint16_t, 4)

    /**
     * Get the pos.
     *@return the pos.
     */
    GET_FUN(getPos, uint16_t, 6)

    /**
     * Get the source_vr.
     *@return the source vr.
     */
    GET_FUN(getSourceVr, uint64_t, 8)

    /**
    /* Creates a AciPduConfigOutput from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduCfgOutput(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

    /**
     * Creates a new AciPduConfigOutput object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param data_id  the data id .
     * @param pos the pos.
     * @param source_vr the source vr.
     */
    DcpPduCfgOutput(const uint16_t pdu_seq_id, const uint8_t receiver, const uint16_t data_id, const uint16_t pos,
                    const uint64_t source_vr) :
            DcpPduBasic(16, DcpPduType::CFG_output, pdu_seq_id, receiver) {
        getDataId() = data_id;
        getPos() = pos;
        getSourceVr() = source_vr;
    }

#if defined(DEBUG) || defined(LOGGING)
    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " data_id =" << getDataId();
        os << " pos=" << getPos();
        os << " source_vr=" << getSourceVr();
        return os;
    }
#endif

    /**
     * Check if the stream_size is equal to the in the standard defined size.
     * @return stream_size is equal to the in the standard defined size
     */
    SIZE_HANDLING(16)

protected:
    /**
     * Creates a new AciPduConfigOutput object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param data_id  the data id .
     * @param pos the pos.
     * @param source_vr the source vr.
     */
    DcpPduCfgOutput(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                    const uint8_t receiver, const uint16_t data_id, const uint16_t pos, const uint64_t source_vr)
            : DcpPduBasic(stream_size, type_id, pdu_seq_id, receiver) {
        getDataId() = data_id;
        getPos() = pos;
        getSourceVr() = source_vr;
    }
};
#endif //DCPLIB_DCPPDUCFGOUTPUT_HPP
