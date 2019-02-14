/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUCFGTUNABLEPARAMETER_HPP
#define DCPLIB_DCPPDUCFGTUNABLEPARAMETER_HPP

#include <dcp/model/pdu/DcpPduBasic.hpp>

/**
 * This class decscribes the structure for the Pdu "CFG_config_tunable_parameter".
 */
class DcpPduCfgTunableParameter : public DcpPduBasic {
public:

    /**
     * Get the data_id .
     *@return the data id .
     */
    GET_FUN(getParamId, uint16_t, 4)

    /**
     * Get the pos.
     *@return the pos.
     */
    GET_FUN(getPos, uint16_t, 6)

    /**
     * Get the target_vr.
     *@return the target vr.
     */
    GET_FUN(getParameterVr, uint64_t, 8)

    /**
     * Get the source_data_type.
     *@return the source data type.
     */
    GET_FUN(getSourceDataType, DcpDataType, 16)

    /**
    /* Creates a AciPduConfigTunableParameter from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduCfgTunableParameter(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

    /**
     * Creates a new AciPduConfigTunableParameter object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param param_id  the param_id .
     * @param pos the pos.
     * @param parameter_vr the parameter_vr.
     * @param source_data_type the source data type.
     */
    DcpPduCfgTunableParameter(const uint16_t pdu_seq_id, const uint8_t receiver, const uint16_t param_id,
                              const uint16_t pos, const uint64_t parameter_vr, const DcpDataType source_data_type) :
            DcpPduBasic(17, DcpPduType::CFG_tunable_parameter, pdu_seq_id, receiver) {
        getParamId() = param_id;
        getPos() = pos;
        getParameterVr() = parameter_vr;
        getSourceDataType() = source_data_type;
    }

#if defined(DEBUG) || defined(LOGGING)
    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " param_id=" << getParamId();
        os << " pos=" << getPos();
        os << " parameter_vr=" << getParameterVr();
        os << " source_data_type=" << getSourceDataType();
        return os;
    }
#endif

    /**
   * Check if the stream_size is equal to the in the standard defined size.
   * @return stream_size is equal to the in the standard defined size
   */
    SIZE_HANDLING(17)

protected:
    /**
     * Creates a new AciPduConfigTunableParameter object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param param_id  the param_id.
     * @param pos the pos.
     * @param parameter_vr the parameter_vr.
     * @param source_data_type the source data type.
     */
    DcpPduCfgTunableParameter(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                              const uint8_t receiver, const uint16_t param_id, const uint16_t pos,
                              const uint64_t parameter_vr, const DcpDataType source_data_type) : DcpPduBasic(
            stream_size, type_id, pdu_seq_id, receiver) {
        getParamId() = param_id;
        getPos() = pos;
        getParameterVr() = parameter_vr;
        getSourceDataType() = source_data_type;
    }
};

#endif //DCPLIB_DCPPDUCFGTUNABLEPARAMETER_HPP
