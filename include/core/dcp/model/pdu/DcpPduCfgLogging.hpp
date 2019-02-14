/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDULOGGING_HPP
#define DCPLIB_DCPPDULOGGING_HPP

#include <dcp/model/pdu/DcpPduBasic.hpp>

/**
 * This class decscribes the structure for the Pdus
 */
class DcpPduCfgLogging : public DcpPduBasic {
public:

    /**
     * Get the log_category
     *@return the log_category.
     */
    GET_FUN(getLogCategory, uint8_t, 4)

    GET_FUN(getLogLevel, DcpLogLevel, 5)

    GET_FUN(getLogMode, DcpLogMode, 6)

    /**
    /* Creates a DcpPduInfLog from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduCfgLogging(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

    /**
     * Creates a new DcpPduSetLogging object.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param log_category
     * @param log_max_num
     */
    DcpPduCfgLogging(const uint16_t pdu_seq_id, const uint8_t receiver, const uint8_t log_category,
                     const DcpLogLevel log_level, DcpLogMode log_mode) : DcpPduBasic(7, DcpPduType::CFG_logging,
                                                                                     pdu_seq_id, receiver) {
        getLogCategory() = log_category;
        getLogLevel() = log_level;
        getLogMode() = log_mode;
    }
#if defined(DEBUG) || defined(LOGGING)

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " log_category=" << unsigned(getLogCategory());
        os << " log_level=" << getLogLevel();
        os << " log_mode=" << getLogMode();


        return os;
    }
#endif
    /**
     * Check if the stream_size is equal to the in the standard defined size.
     * @return stream_size is equal to the in the standard defined size
     */
    SIZE_HANDLING(7)


};

#endif //DCPLIB_DCPPDULOGGING_HPP
