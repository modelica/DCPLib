/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUREGISTER_HPP
#define DCPLIB_DCPPDUREGISTER_HPP

#include <dcp/model/pdu/DcpPduStc.hpp>

/**
 * This class decscribes the structure for the Pdu "STC_register".
 */
class DcpPduStcRegister : public DcpPduStc {
public:

    /**
     * Get the slave_uuid .
     *@return the slave uuid .
     */
    GET_FUN(getSlaveUuid, uint128_t, 5)

    /**
     * Get the op_mode.
     *@return the op mode.
     */
    GET_FUN(getOpMode, DcpOpMode, 21)


    GET_FUN(getMajorVersion, uint8_t, 22)

    GET_FUN(getMinorVersion, uint8_t, 23)


    /**
    /* Creates a AciPduRegister from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduStcRegister(unsigned char *stream, size_t stream_size) : DcpPduStc(stream, stream_size){}

    /**
     * Creates a new AciPduRegister object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param state_id the state_id of the slaves state.
     * @param slave_uuid  the slave uuid .
     * @param op_mode the op mode.
     */
    DcpPduStcRegister(const uint16_t pdu_seq_id, const uint8_t receiver, const DcpState state_id,
                   const uint128_t slave_uuid, const DcpOpMode op_mode, const uint8_t majorversion,
                   const uint8_t minorVersion) :
            DcpPduStc(24, DcpPduType::STC_register, pdu_seq_id, receiver, state_id) {
        getSlaveUuid() = slave_uuid;
        getOpMode() = op_mode;
        getMajorVersion() = majorversion;
        getMinorVersion() = minorVersion;
    }

#if defined(DEBUG) || defined(LOGGING)
    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " slave_uuid=" << convertToUUIDStr(getSlaveUuid());
        os << " state_id=" << getStateId();
        os << " op_mode=" << getOpMode();
        os << " major_version=" << unsigned(getMajorVersion());
        os << " minor_version=" << unsigned(getMinorVersion());
        return os;
    }
#endif

    /**
   * Check if the stream_size is equal to the in the standard defined size.
   * @return stream_size is equal to the in the standard defined size
   */
    SIZE_HANDLING(24)

protected:
    /**
     * Creates a new AciPduRegister object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param slave_uuid  the slave uuid .
     * @param op_mode the op mode.
     */
    DcpPduStcRegister(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                   const uint8_t receiver, const DcpState state_id, const uint128_t slave_uuid, const DcpOpMode op_mode)
            : DcpPduStc(stream_size, type_id, pdu_seq_id, receiver, state_id) {
        getSlaveUuid() = slave_uuid;
        getOpMode() = op_mode;
    }
};


#endif //DCPLIB_DCPPDUREGISTER_HPP
