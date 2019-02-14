/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDURSPERRORACK_HPP
#define DCPLIB_DCPPDURSPERRORACK_HPP

#include <dcp/model/pdu/DcpPduRspAck.hpp>


/**
 * This class decscribes the structure for the Pdus "RSP_nack" & "RSP_error_ack".
 */
class DcpPduRspErrorAck : public DcpPduRspAck {
public:

    /**
     * Get the error_code.
     *@return the error code.
     */
    GET_FUN(getErrorCode, DcpError, 4)

    /**
    /* Creates a DcpPduRspNegative from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduRspErrorAck(unsigned char *stream, size_t stream_size) : DcpPduRspAck(stream, stream_size){}

    /**
     * Creates a new DcpPduRspNegative object.
     * @param type_id the type id.
     * @param sender the sender.
     * @param resp_seq_id the resp seq id.
     * @param error_code the error code.
     */
    DcpPduRspErrorAck(const DcpPduType type, const uint8_t sender, const uint16_t resp_seq_id, const DcpError error_code)
            : DcpPduRspAck(6, type, sender, resp_seq_id) {
        getErrorCode() = error_code;
    }

#if defined(DEBUG) || defined(LOGGING)
    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduRspAck::operator<<(os);
        os << " error_code=" << getErrorCode();
        return os;
    }
#endif

    /**
     * Check if the stream_size is equal to the in the standard defined size.
     * @return stream_size is equal to the in the standard defined size
     */
    SIZE_HANDLING(6)

protected:
    /**
     * Creates a new AciPduNack object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param sender the sender.
     * @param resp_seq_id the resp seq id.
     * @param error_code the error code.
     */
    DcpPduRspErrorAck(const size_t stream_size, const DcpPduType type_id, const uint8_t sender, const uint16_t resp_seq_id,
                      const DcpError error_code) : DcpPduRspAck(stream_size, type_id, sender, resp_seq_id) {
        getErrorCode() = error_code;
    }
};

#endif //DCPLIB_DCPPDURSPERRORACK_HPP
