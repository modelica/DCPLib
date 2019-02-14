/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUDATINPUTOUTPUT_HPP
#define DCPLIB_DCPPDUDATINPUTOUTPUT_HPP

#include <dcp/model/pdu/DcpPdu.hpp>

/**
 * This class decscribes the structure for the Pdu "DAT_input_output".
 */
class DcpPduDatInputOutput : public DcpPdu {
public:

    /**
     * Get the pdu_seq_id.
     *@return the pdu seq id.
     */
    GET_FUN(getPduSeqId, uint16_t, 1)

    /**
     * Get the data_id.
     *@return the data id.
     */
    GET_FUN(getDataId, uint16_t, 3)

    /**
     * Get the payload.
     *@return the payload.
     */
    GET_FUN_PTR(getPayload, uint8_t, 5)



    /**
    /* Creates a AciPduData from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduDatInputOutput(unsigned char *stream, size_t stream_size) : DcpPdu(stream, stream_size){}

    /**
     * Creates a new AciPduData object.
     * @param pdu_seq_id the pdu seq id.
     * @param data_id the data id.
     * @param payload the payload from which the data will be copied into this pdu.
     * @param payload_size the length in bytes of the payload.

     */
    DcpPduDatInputOutput(const uint16_t pdu_seq_id, const uint16_t data_id, const uint8_t *payload,
                         const size_t payload_size) :
            DcpPdu(5 + payload_size, DcpPduType::DAT_input_output) {
        getPduSeqId() = pdu_seq_id;
        getDataId() = data_id;
        memcpy(getPayload(), payload, payload_size);
    }

    /**
	 * Creates a new AciPduData object.
	 * @param pdu_seq_id the pdu seq id.
	 * @param data_id the data id.
	 * @param payload the payload.
	 */
    DcpPduDatInputOutput(const uint16_t pdu_seq_id, uint16_t data_id, uint16_t payloadSize) :
            DcpPdu(5 + payloadSize, DcpPduType::DAT_input_output) {
        getPduSeqId() = pdu_seq_id;
        getDataId() = data_id;
    }

#if defined(DEBUG) || defined(LOGGING)
    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPdu::operator<<(os);
        os << " pdu_seq_id=" << getPduSeqId();
        os << " data_id=" << getDataId();

        os << " payload=";
        for (size_t i = 5; i < stream_size; ++i)
            os << std::hex << std::setw(2) << std::setfill('0')
               << (int) stream[i] << " ";

        return os;
    }
#endif

    /**
   * Check if the stream_size is at least as much as the in the standard defined fixed part of an DATA PDU.
   * @return tream_size is at least as much as the in the standard defined fixed part of an DATA PDU
   */
    virtual bool isSizeCorrect() {
        return this->stream_size >= 5;
    }

    /**
      * Returns the minimum stream_size as the in the standard defined fixed part of an DAT_input_output PDU.
      * @return the minimum stream_size as the in the standard defined fixed part of an DAT_input_output PDU.
      */
    virtual size_t getCorrectSize() {
        return 5;
    }
};
#endif //DCPLIB_DCPPDUDATINPUTOUTPUT_HPP
