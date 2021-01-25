/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universit�t Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPPDUFACTORY_HPP
#define DCPLIB_DCPPDUFACTORY_HPP

#include <dcp/model/pdu/DcpPdu.hpp>
#include <dcp/model/pdu/DcpPduBasic.hpp>
#include <dcp/model/pdu/DcpPduCfgInput.hpp>
#include <dcp/model/pdu/DcpPduCfgLogging.hpp>
#include <dcp/model/pdu/DcpPduCfgNetworkInformation.hpp>
#include <dcp/model/pdu/DcpPduCfgOutput.hpp>
#include <dcp/model/pdu/DcpPduCfgParameter.hpp>
#include <dcp/model/pdu/DcpPduCfgParamNetworkInformation.hpp>
#include <dcp/model/pdu/DcpPduCfgScope.hpp>
#include <dcp/model/pdu/DcpPduCfgSteps.hpp>
#include <dcp/model/pdu/DcpPduCfgTimeRes.hpp>
#include <dcp/model/pdu/DcpPduCfgTunableParameter.hpp>
#include <dcp/model/pdu/DcpPduDatInputOutput.hpp>
#include <dcp/model/pdu/DcpPduDatParameter.hpp>
#include <dcp/model/pdu/DcpPduInfLog.hpp>
#include <dcp/model/pdu/DcpPduNtfLog.hpp>
#include <dcp/model/pdu/DcpPduNtfStateChanged.hpp>
#include <dcp/model/pdu/DcpPduRspAck.hpp>
#include <dcp/model/pdu/DcpPduRspErrorAck.hpp>
#include <dcp/model/pdu/DcpPduRspLogAck.hpp>
#include <dcp/model/pdu/DcpPduRspNack.hpp>
#include <dcp/model/pdu/DcpPduRspStateAck.hpp>
#include <dcp/model/pdu/DcpPduStc.hpp>
#include <dcp/model/pdu/DcpPduStcDoStep.hpp>
#include <dcp/model/pdu/DcpPduStcRegister.hpp>
#include <dcp/model/pdu/DcpPduStcRun.hpp>

static DcpPdu *makeDcpPdu(unsigned char *stream, size_t stream_size) {
    DcpPduType &type_id = *((DcpPduType * )(stream + PDU_LENGTH_INDICATOR_SIZE));
    switch (type_id) {
        case DcpPduType::STC_configure:
        case DcpPduType::STC_initialize:
        case DcpPduType::STC_stop:
        case DcpPduType::STC_reset:
        case DcpPduType::STC_deregister:
        case DcpPduType::STC_send_outputs:
        case DcpPduType::STC_prepare:
            return new DcpPduStc(stream, stream_size);
        case DcpPduType::STC_run:
            return new DcpPduStcRun(stream, stream_size);
        case DcpPduType::INF_state:
            return new DcpPduBasic(stream, stream_size);
        case DcpPduType::INF_error:
            return new DcpPduBasic(stream, stream_size);
        case DcpPduType::INF_log:
            return new DcpPduInfLog(stream, stream_size);
        case DcpPduType::NTF_state_changed:
            return new DcpPduNtfStateChanged(stream, stream_size);
        case DcpPduType::NTF_log:
            return new DcpPduNtfLog(stream, stream_size);
        case DcpPduType::STC_do_step:
            return new DcpPduStcDoStep(stream, stream_size);
        case DcpPduType::CFG_time_res:
            return new DcpPduCfgTimeRes(stream, stream_size);
        case DcpPduType::CFG_steps:
            return new DcpPduCfgSteps(stream, stream_size);
        case DcpPduType::CFG_scope:
            return new DcpPduCfgScope(stream, stream_size);
        case DcpPduType::STC_register:
            return new DcpPduStcRegister(stream, stream_size);
        case DcpPduType::CFG_input:
            return new DcpPduCfgInput(stream, stream_size);
        case DcpPduType::CFG_output:
            return new DcpPduCfgOutput(stream, stream_size);
        case DcpPduType::CFG_clear:
            return new DcpPduBasic(stream, stream_size);
        case DcpPduType::CFG_tunable_parameter:
            return new DcpPduCfgTunableParameter(stream, stream_size);
        case DcpPduType::CFG_parameter:
            return new DcpPduCfgParameter(stream, stream_size);
        case DcpPduType::CFG_logging:
            return new DcpPduCfgLogging(stream, stream_size);
        case DcpPduType::DAT_input_output:
            return new DcpPduDatInputOutput(stream, stream_size);
        case DcpPduType::DAT_parameter:
            return new DcpPduDatParameter(stream, stream_size);
        case DcpPduType::RSP_ack:
            return new DcpPduRspAck(stream, stream_size);
        case DcpPduType::RSP_nack:
            return new DcpPduRspNack(stream, stream_size);
        case DcpPduType::RSP_error_ack:
            return new DcpPduRspErrorAck(stream, stream_size);
        case DcpPduType::RSP_state_ack:
            return new DcpPduRspStateAck(stream, stream_size);
        case DcpPduType::RSP_log_ack:
            return new DcpPduRspLogAck(stream, stream_size);
        case DcpPduType::CFG_target_network_information: {
            DcpTransportProtocol &tp = *((DcpTransportProtocol * )(stream + 10));
            switch (tp) {
                case DcpTransportProtocol::TCP_IPv4:
                case DcpTransportProtocol::UDP_IPv4:
                    return new DcpPduCfgNetworkInformationIPv4(stream, stream_size);
                default:
                    return new DcpPduCfgNetworkInformation(stream, stream_size);
            }

        }
        case DcpPduType::CFG_source_network_information: {
            DcpTransportProtocol &tp = *((DcpTransportProtocol * )(stream + 10));
            switch (tp) {
                case DcpTransportProtocol::TCP_IPv4:
                case DcpTransportProtocol::UDP_IPv4:
                    return new DcpPduCfgNetworkInformationIPv4(stream, stream_size);
                default:
                    return new DcpPduCfgNetworkInformation(stream, stream_size);
            }

        }
        case DcpPduType::CFG_param_network_information: {
            DcpTransportProtocol &tp = *((DcpTransportProtocol * )(stream + 10));
            switch (tp) {
                case DcpTransportProtocol::TCP_IPv4:
                case DcpTransportProtocol::UDP_IPv4:
                    return new DcpPduCfgParamNetworkInformationIPv4(stream, stream_size);
                default:
                    return new DcpPduCfgNetworkInformation(stream, stream_size);
            }

        }
    }
    return new DcpPdu(stream, stream_size);
}

#endif //DCPLIB_DCPPDUFACTORY_HPP
