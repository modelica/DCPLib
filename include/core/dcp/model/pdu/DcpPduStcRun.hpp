/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDURUN_HPP
#define DCPLIB_DCPPDURUN_HPP

#include <dcp/model/pdu/DcpPduStc.hpp>

/**
 * This class decscribes the structure for the Pdu "STC_run".
 */
class DcpPduStcRun : public DcpPduStc {
public:

    /**
     * Get the start_time.
     *@return the start time.
     */
    GET_FUN(getStartTime, int64_t, 5)

    /**
    /* Creates a AciPduRun from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduStcRun(unsigned char *stream, size_t stream_size) : DcpPduStc(stream, stream_size){}

    /**
     * Creates a new AciPduRun object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param start_time the start time.
     * @param state_id the state_id of the slaves state.
     */
    DcpPduStcRun(const uint16_t pdu_seq_id, const uint8_t receiver, const DcpState state_id, const int64_t start_time) :
            DcpPduStc(13, DcpPduType::STC_run, pdu_seq_id, receiver, state_id) {
        getStartTime() = start_time;
    }
#if defined(DEBUG) || defined(LOGGING)

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " start_time=" << getStartTime();
        return os;
    }
#endif
    /**
	 * Check if the stream_size is equal to the in the standard defined size.
	 * @return stream_size is equal to the in the standard defined size
	 */
    SIZE_HANDLING(13)

protected:
    /**
     * Creates a new AciPduRun object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param start_time the start time.
     * @param state_id the state_id of the slaves state.

     */
    DcpPduStcRun(const int stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id, const uint8_t receiver,
              const DcpState state_id, const int64_t start_time) : DcpPduStc(stream_size, type_id,
                                                                                         pdu_seq_id, receiver,
                                                                                         state_id) {
        getStartTime() = start_time;
    }
};

#endif //DCPLIB_DCPPDURUN_HPP
