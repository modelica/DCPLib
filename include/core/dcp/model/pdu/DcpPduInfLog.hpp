/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUINFLOG_HPP
#define DCPLIB_DCPPDUINFLOG_HPP

#include <dcp/model/pdu/DcpPduBasic.hpp>

/**
 * This class decscribes the structure for the Pdu INF_log
 */
class DcpPduInfLog : public DcpPduBasic {
public:

    /**
     * Get the log_category
     *@return the log_category.
     */
    GET_FUN(getLogCategory, uint8_t, 4)


    GET_FUN(getLogMaxNum, uint8_t, 5)


    /**
    /* Creates a DcpPduInfLog from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduInfLog(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

    /**
     * Creates a new DcpPduInfLog object.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param log_category
     * @param log_max_num
     */
    DcpPduInfLog(const uint16_t pdu_seq_id, const uint8_t receiver, const uint8_t log_category,
                 const uint8_t log_max_num) : DcpPduBasic(6, DcpPduType::INF_log, pdu_seq_id, receiver) {
        getLogCategory() = log_category;
        getLogMaxNum() = log_max_num;
    }
#if defined(DEBUG) || defined(LOGGING)

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " log_category=" << unsigned(getLogCategory());
        os << " log_max_num=" << unsigned(getLogMaxNum());

        return os;
    }
#endif
    /**
     * Check if the stream_size is equal to the in the standard defined size.
     * @return stream_size is equal to the in the standard defined size
     */
    SIZE_HANDLING(6)

protected:

    DcpPduInfLog() {}

    /**
     * Creates a new DcpPduInfLog object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param log_category
     * @param log_max_num
     */
    DcpPduInfLog(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id, const uint8_t receiver,
                 const uint8_t log_category, const uint8_t log_max_num) : DcpPduBasic(stream_size, type_id, pdu_seq_id,
                                                                                      receiver) {
        getLogCategory() = log_category;
        getLogMaxNum() = log_max_num;
    }
};

#endif //DCPLIB_DCPPDUINFLOG_HPP
