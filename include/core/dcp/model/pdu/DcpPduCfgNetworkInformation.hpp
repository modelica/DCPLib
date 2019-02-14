/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUCFGNETWORKINFORMATION_HPP
#define DCPLIB_DCPPDUCFGNETWORKINFORMATION_HPP

#include <dcp/model/pdu/DcpPduBasic.hpp>

/**
 * This class decscribes the structure for the Pdus "CFG_target_network_information" & "CFG_source_network_information".
 */
class DcpPduCfgNetworkInformation : public DcpPduBasic {
public:

    /**
     * Get the data_id .
     *@return the data id .
     */
    GET_FUN(getDataId, uint16_t, 4)

    /**
     * Get the transport_protocol.
     *@return the transport protocol.
     */
    GET_FUN(getTransportProtocol, DcpTransportProtocol, 6)


    GET_FUN_PTR(getNetworkInformation, uint8_t, 7)

    /**
    /* Creates a AciPduSetNetworkInformation from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduCfgNetworkInformation(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

#if defined(DEBUG) || defined(LOGGING)
    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " data_id =" << getDataId();
        os << " transport_protocol=" << getTransportProtocol();
        return os;
    }
#endif

protected:

    DcpPduCfgNetworkInformation() {}

    /**
     * Creates a new AciPduSetNetworkInformation object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param data_id  the data id .
     * @param transport_protocol the transport protocol.
     */
    DcpPduCfgNetworkInformation(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                                const uint8_t receiver, const uint16_t data_id,
                                const DcpTransportProtocol transport_protocol) : DcpPduBasic(stream_size, type_id,
                                                                                             pdu_seq_id, receiver) {
        getDataId() = data_id;
        getTransportProtocol() = transport_protocol;
    }

    SIZE_HANDLING(7)
};

/**
 * This class decscribes the structure for the Pdus "CFG_target_network_information" & "CFG_source_network_information"
 * for UDP/IPv4 and TCP/IPv4.
 */
class DcpPduCfgNetworkInformationIPv4: public DcpPduCfgNetworkInformation {
public:

    /**
     * Get the port.
     *@return the port.
     */
    GET_FUN(getPort, uint16_t, 7)

    /**
    * Get the ip_address .
    *@return the ip address .
    */
    GET_FUN(getIpAddress, uint32_t, 9)

    /**
    /* Creates a DcpPduSetNetworkInformationIPv4 from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduCfgNetworkInformationIPv4(unsigned char *stream, size_t stream_size) : DcpPduCfgNetworkInformation(stream, stream_size){}

    /**
     * Creates a new DcpPduSetNetworkInformationIPv4 object.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param data_id  the data id .
     * @param transport_protocol the transport protocol.
     * @param ip_address  the ip address .
     * @param port the port.
     */
    DcpPduCfgNetworkInformationIPv4(const DcpPduType type_id, const uint16_t pdu_seq_id, const uint8_t receiver,
                                        const uint16_t data_id, const uint16_t port, const uint32_t ip_address, DcpTransportProtocol transportProtocol) :
            DcpPduCfgNetworkInformation(13, type_id, pdu_seq_id, receiver, data_id, transportProtocol) {
        getIpAddress() = ip_address;
        getPort() = port;
    }

#if defined(DEBUG) || defined(LOGGING)
    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduCfgNetworkInformation::operator<<(os);
        os << " ip_address =" << getIpAddress();
        os << " port=" << getPort();
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
     * Creates a new DcpPduSetNetworkInformationIPv4 object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param data_id  the data id .
     * @param transport_protocol the transport protocol.
     * @param ip_address  the ip address .
     * @param port the port.
     */
    DcpPduCfgNetworkInformationIPv4(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                                        const uint8_t receiver, const uint16_t data_id,
                                        const DcpTransportProtocol transport_protocol, const uint32_t ip_address,
                                        const uint16_t port) :
            DcpPduCfgNetworkInformation(stream_size, type_id, pdu_seq_id, receiver, data_id, transport_protocol) {
        getIpAddress() = ip_address;
        getPort() = port;
    }
};

#endif //DCPLIB_DCPPDUCFGNETWORKINFORMATION_HPP
