/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUCFGPARAMETER_HPP
#define DCPLIB_DCPPDUCFGPARAMETER_HPP

#include <dcp/model/pdu/DcpPduBasic.hpp>

/**
 * This class decscribes the structure for the Pdu "CFG_parameter".
 */
class DcpPduCfgParameter : public DcpPduBasic {
public:

    /**
     * Get the data_id .
     *@return the data id .
     */
    GET_FUN(getParameterVr, uint64_t, 4)

    GET_FUN(getSourceDataType, DcpDataType, 12)

    /**
     * Get the payload.
     *@return the payload.
     */
    GET_FUN_PTR(getConfiguration, uint8_t, 13)


    /**
    /* Creates a AciPduSetParameter from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduCfgParameter(unsigned char *stream, size_t stream_size) :DcpPduBasic(stream, stream_size) {}

    /**
     * Creates a new AciPduSetParameter object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param parameter_vr  the parameters value reference.
     * @param value the value.
     * @param value_size the size of value.
     */
    DcpPduCfgParameter(const uint16_t pdu_seq_id, const uint8_t receiver, const uint64_t parameter_vr,
                       const DcpDataType sourceDataType, const uint8_t *configuration, const size_t configuration_size)
            :
            DcpPduBasic(13 + configuration_size, DcpPduType::CFG_parameter, pdu_seq_id, receiver) {
        getParameterVr() = parameter_vr;
        getSourceDataType() = sourceDataType;
        memcpy(getConfiguration(), configuration, configuration_size);
    }

#if defined(DEBUG) || defined(LOGGING)
    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " parameter_vr=" << getParameterVr();
        os << " source_data_type=" << getSourceDataType();
        os << " configuration=";
        for (size_t i = 13 + PDU_LENGTH_INDICATOR_SIZE; i < stream_size; ++i)
            os << std::hex << std::setw(2) << std::setfill('0')
               << (int) stream[i] << " ";

        return os;
    }
#endif
    /**
      * Check if the stream_size is at least as much as the in the standard defined fixed part of an PARAM PDU.
      * @return tream_size is at least as much as the in the standard defined fixed part of an PARAM PDU
      */
    virtual bool isSizeCorrect() {
        return this->stream_size - PDU_LENGTH_INDICATOR_SIZE >= 13;
    }

    /**
      * Returns the minimum stream_size as the in the standard defined fixed part of an PARAM PDU.
      * @return the minimum stream_size as the in the standard defined fixed part of an PARAM PDU.
      */
    virtual size_t getCorrectSize() {
        return 13;
    }

protected:

};
#endif //DCPLIB_DCPPDUCFGPARAMETER_HPP
