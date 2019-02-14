/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUCFGPARAMNETWORKINFORMATION_HPP
#define DCPLIB_DCPPDUCFGPARAMNETWORKINFORMATION_HPP

#include <dcp/model/pdu/DcpPduBasic.hpp>

/**
 * This class decscribes the structure for the Pdus "CFG_target_network_information" & "MSG_source_network_information".
 */
class DcpPduCfgParamNetworkInformation : public DcpPduBasic {
public:

    /**
     * Get the data_id .
     *@return the data id .
     */
    GET_FUN(getParamId, uint16_t, 4)

    /**
     * Get the transport_protocol.
     *@return the transport protocol.
     */
    GET_FUN(getTransportProtocol, DcpTransportProtocol, 6)

    GET_FUN_PTR(getNetworkInformation, uint8_t, 7)

    /**
    /* Creates a AciPduSetParamNetworkInformation from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduCfgParamNetworkInformation(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

#if defined(DEBUG) || defined(LOGGING)
    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " param_id=" << getParamId();
        os << " transport_protocol=" << getTransportProtocol();
        return os;
    }
#endif

protected:

    DcpPduCfgParamNetworkInformation() {}

    /**
     * Creates a new AciPduSetParamNetworkInformation object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param param_id  the param_id .
     * @param transport_protocol the transport protocol.
     */
    DcpPduCfgParamNetworkInformation(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                                     const uint8_t receiver, const uint16_t param_id,
                                     const DcpTransportProtocol transport_protocol) : DcpPduBasic(stream_size, type_id,
                                                                                                  pdu_seq_id,
                                                                                                  receiver) {
        getParamId() = param_id;
        getTransportProtocol() = transport_protocol;
    }
};

/**
 * This class decscribes the structure for the Pdus "CFG_source_network_information" for UDP.
 */
class DcpPduCfgParamNetworkInformationIPv4 : public DcpPduCfgParamNetworkInformation {
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
    /* Creates a AciPduSetSourceNetworkInformationUdp from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduCfgParamNetworkInformationIPv4(unsigned char *stream, size_t stream_size) : DcpPduCfgParamNetworkInformation(stream, stream_size){}

    /**
     * Creates a new AciPduSetSourceNetworkInformationUdp object.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param param_id  the param_id .
     * @param transport_protocol the transport protocol.
     * @param ip_address  the ip address .
     * @param port the port.
     */
    DcpPduCfgParamNetworkInformationIPv4(const uint16_t pdu_seq_id, const uint8_t receiver, const uint16_t param_id,
                                         const uint16_t port, const uint32_t ipAddress, DcpTransportProtocol transportProtocol) :
            DcpPduCfgParamNetworkInformation(13, DcpPduType::CFG_param_network_information, pdu_seq_id, receiver,
                                             param_id, transportProtocol) {
        getPort() = port;
        getIpAddress() = ipAddress;
    }

#if defined(DEBUG) || defined(LOGGING)
    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduCfgParamNetworkInformation::operator<<(os);
        os << " port=" << getPort();
        os << " ip_address=" << getIpAddress();


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
     * Creates a new AciPduSetNetworkInformationUdp object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param data_id  the data id .
     * @param transport_protocol the transport protocol.
     * @param ip_address  the ip address .
     * @param port the port.
     */
    DcpPduCfgParamNetworkInformationIPv4(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                                         const uint8_t receiver, const uint16_t param_id,
                                         const DcpTransportProtocol transport_protocol, const uint16_t port) :
            DcpPduCfgParamNetworkInformation(stream_size, type_id, pdu_seq_id, receiver, param_id, transport_protocol) {
        getPort() = port;
    }
};

#endif //DCPLIB_DCPPDUCFGPARAMNETWORKINFORMATION_HPP
