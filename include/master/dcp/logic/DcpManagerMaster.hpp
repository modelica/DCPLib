/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef ACI_LOGIC_DRIVERMANAGERMASTER_H_
#define ACI_LOGIC_DRIVERMANAGERMASTER_H_

#include <cstdint>
#include <condition_variable>

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
#include <dcp/model/pdu/DcpPduRspLogAck.hpp>
#include <dcp/model/pdu/DcpPduRspNack.hpp>
#include <dcp/model/pdu/DcpPduRspStateAck.hpp>
#include <dcp/model/pdu/DcpPduStc.hpp>
#include <dcp/model/pdu/DcpPduStcDoStep.hpp>
#include <dcp/model/pdu/DcpPduStcRegister.hpp>
#include <dcp/model/pdu/DcpPduStcRun.hpp>
#include <dcp/model/DcpCallbackTypes.hpp>

#include "dcp/logic/AbstractDcpManager.hpp"
#include "dcp/model/LogEntry.hpp"

#include "dcp/model/DcpTypes.hpp"
#include "dcp/xml/DcpSlaveDescriptionElements.hpp"

#include <dcp/helper/Helper.hpp>

#include <thread>
#include <iostream>

/**
 * DCP management of a master.
 *
 * @author Christian Kater <kater@sim.uni-hannover.de>
 *
 */
class DcpManagerMaster : public AbstractDcpManager {
public:

    /**
     * Intanciates a DCP manager of a master
     * @param driver Driver object of the DCP master.
     */
    DcpManagerMaster(DcpDriver driver) {
        this->driver = driver;
        this->masterId = 0;
    }

    virtual ~DcpManagerMaster() {}

    virtual void receive(DcpPdu &msg) override {
        //check sequence id
        switch(msg.getTypeId()){
            case DcpPduType::RSP_ack:
            case DcpPduType::RSP_nack:
            case DcpPduType::RSP_state_ack:
            case DcpPduType::RSP_error_ack:
            case DcpPduType::RSP_log_ack:{
                DcpPduRspAck &pdu = static_cast<DcpPduRspAck &>(msg);
                if(lastRegisterSuccessfullSeq.count(pdu.getSender()) > 0 && checkSeqId(pdu.getSender(), pdu.getRespSeqId()) != 1){
                    if (synchronousCallback[DcpCallbackTypes::PDU_MISSED]) {
                        pduMissedListener(pdu.getSender());
                    } else {
                        std::thread t(pduMissedListener, pdu.getSender());
                        t.detach();
                    }
                }
                break;
            }
            case DcpPduType::DAT_input_output:{
                DcpPduDatInputOutput &data = static_cast<DcpPduDatInputOutput &>(msg);
                if(checkSeqIdInOut(data.getDataId(), data.getPduSeqId()) != 1){
                    if (synchronousCallback[DcpCallbackTypes::PDU_MISSED]) {
                        inputOutputPduMissedListener(data.getDataId());
                    } else {
                        std::thread t(inputOutputPduMissedListener, data.getDataId());
                        t.detach();
                    }
                }
                break;
            }
        }

        switch (msg.getTypeId()) {
            case DcpPduType::RSP_ack: {
                DcpPduRspAck &ack = static_cast<DcpPduRspAck &>(msg);
                if(lastRegisterSeq[ack.getSender()] == ack.getRespSeqId()){
                    lastRegisterSuccessfullSeq[ack.getSender()] = ack.getRespSeqId();
                    segNumsIn[ack.getSender()] = ack.getRespSeqId();
                } else if(lastClearSeq[ack.getSender()] == ack.getRespSeqId()){
                    segNumsOut[ack.getSender()] = lastRegisterSuccessfullSeq[ack.getSender()] + 1;
                    segNumsIn[ack.getSender()] = lastRegisterSuccessfullSeq[ack.getSender()];
                    dataSegNumsOut[ack.getSender()] = 0;
                    dataSegNumsIn[ack.getSender()] = 0;
                    lastRegisterSeq[ack.getSender()] = 0;
                    lastClearSeq[ack.getSender()] = 0;
                }
                if (synchronousCallback[DcpCallbackTypes::ACK]) {
                    ackReceivedListener(ack.getSender(), ack.getRespSeqId());
                } else {
                    std::thread t(ackReceivedListener, ack.getSender(), ack.getRespSeqId());
                    t.detach();
                }
                break;
            }
            case DcpPduType::RSP_nack: {
                DcpPduRspNack &nack = static_cast<DcpPduRspNack &>(msg);
                if (synchronousCallback[DcpCallbackTypes::NACK]) {
                    nAckReceivedListener(nack.getSender(), nack.getRespSeqId(),
                                         nack.getErrorCode());
                } else {
                    std::thread t(nAckReceivedListener, nack.getSender(), nack.getRespSeqId(),
                                  nack.getErrorCode());
                    t.detach();
                }
                break;
            }
            case DcpPduType::RSP_state_ack: {
                DcpPduRspStateAck &stateAck = static_cast<DcpPduRspStateAck &>(msg);
                if (synchronousCallback[DcpCallbackTypes::NACK]) {
                    stateAckReceivedListener(stateAck.getSender(),
                                             stateAck.getRespSeqId(), stateAck.getStateId());
                } else {
                    std::thread t(stateAckReceivedListener, stateAck.getSender(),
                                  stateAck.getRespSeqId(), stateAck.getStateId());
                    t.detach();
                }
                break;
            }
            case DcpPduType::RSP_error_ack: {
                DcpPduRspNack &errorAck = static_cast<DcpPduRspNack &>(msg);
                DcpPduRspStateAck &stateAck = static_cast<DcpPduRspStateAck &>(msg);
                if (synchronousCallback[DcpCallbackTypes::NACK]) {
                    errorAckReceivedListener(errorAck.getSender(),
                                             errorAck.getRespSeqId(), errorAck.getErrorCode());
                } else {
                    std::thread t(errorAckReceivedListener, errorAck.getSender(),
                                  errorAck.getRespSeqId(), errorAck.getErrorCode());
                    t.detach();
                }
                break;
            }
            case DcpPduType::RSP_log_ack: {
                DcpPduRspLogAck &logAck = static_cast<DcpPduRspLogAck &>(msg);
                if (logTemplates.count(logAck.getSender()) > 0) {
                    std::vector<std::shared_ptr<LogEntry>> entries;
                    size_t offset = 0;
                    while (offset < logAck.getSerializedSize() - 4) {
                        uint8_t *curLog = logAck.getPayload() + offset;
                        uint8_t templateId = *((uint8_t *) curLog + 8);
                        if (logTemplates[logAck.getSender()].count(templateId) > 0) {
                            LogTemplate &logTemplate = logTemplates[logAck.getSender()].find(templateId)->second;
                            LogEntry *logEntry = new LogEntry(logTemplate, curLog, 0);
                            logEntry->applyPayloadToString();
                            uint8_t *payload = new uint8_t[logEntry->serializedSize()];
                            memcpy(payload, curLog, logEntry->serializedSize());
                            logEntry->setPayload(payload);

                            entries.push_back(std::shared_ptr<LogEntry>(logEntry));
                            offset += logEntry->serializedSize();
                        } else {
                            break;
                            //toDo log unknown id
                        }
                    }
                    if (synchronousCallback[DcpCallbackTypes::RSP_log_ack]) {
                        logAckListener(logAck.getSender(), logAck.getRespSeqId(), entries);
                    } else {
                        std::thread t(logAckListener, logAck.getSender(), logAck.getRespSeqId(), entries);
                        t.detach();
                    }
                }
                break;
            }
            case DcpPduType::NTF_state_changed: {
                DcpPduNtfStateChanged &stateChanged =
                        static_cast<DcpPduNtfStateChanged &>(msg);
                if (synchronousCallback[DcpCallbackTypes::NACK]) {
                    stateChangedNotificationReceivedListener(stateChanged.getSender(),
                                                             stateChanged.getStateId());
                } else {
                    std::thread t(stateChangedNotificationReceivedListener, stateChanged.getSender(),
                                  stateChanged.getStateId());
                    t.detach();
                }
                break;
            }
            case DcpPduType::NTF_log: {
                DcpPduNtfLog &log = static_cast<DcpPduNtfLog &>(msg);
                if (logTemplates.count(log.getSender()) > 0 &&
                    logTemplates[log.getSender()].count(log.getTemplateId()) > 0) {
                    uint8_t templateId = *((uint8_t *) log.getPayload() + 10);
                    if (logTemplates[log.getSender()].count(templateId) > 0) {
                        size_t size = log.getSerializedSize() - 2;
                        uint8_t *payload = new uint8_t[size];
                        memcpy(payload, log.getLogEntryPayload(), size);
                        LogTemplate &logTemplate = logTemplates[log.getSender()].find(templateId)->second;
                        const std::shared_ptr<LogEntry> sharedPtr =
                                std::shared_ptr<LogEntry>(new LogEntry(logTemplate, payload,
                                                                       size));

                        if (synchronousCallback[DcpCallbackTypes::NTF_LOG]) {
                            logNotificationListener(log.getSender(), sharedPtr);
                        } else {
                            std::thread t(logNotificationListener, log.getSender(), sharedPtr);
                            t.detach();
                        }
                    } else {
                        break;
                        //toDo log unknown id
                    }
                }
                break;
            }
            case DcpPduType::DAT_input_output: {
                DcpPduDatInputOutput &data = static_cast<DcpPduDatInputOutput &>(msg);
                if (synchronousCallback[DcpCallbackTypes::NACK]) {
                    dataReceivedListener(data.getDataId(), data.getPduSize() - data.getCorrectSize(), data.getPayload());
                } else {
                    std::thread t(dataReceivedListener, data.getDataId(), data.getPduSize() - data.getCorrectSize(), data.getPayload());
                    t.detach();
                }
                break;
            }
        }
        pduListener(msg);
    }

    virtual void reportError(const DcpError errorCode) override {
        if (synchronousCallback[DcpCallbackTypes::ERROR_LI]) {
            errorListener(errorCode);
        } else {
            std::thread t(errorListener, errorCode);
            t.detach();
        }
    }

    /**
   * Send a STC_register PDU
   * @param dcpId Receiver of the PDU
   * @param stateId Current DCP state of the receiving slave
   * @param slaveUuid UUID of the receiving slave
   * @param opMode Operation mode in which the receiving slave will operate
   *
   * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
   */
    void STC_register(const uint8_t dcpId, const DcpState stateId, const uint128_t slaveUuid,
                      const DcpOpMode opMode, const uint8_t majorversion, const uint8_t minorVersion) {
        DcpPduStcRegister pdu = {getNextSeqNum(dcpId), dcpId, stateId, slaveUuid, opMode, majorversion, minorVersion};
        lastRegisterSeq[dcpId] = pdu.getPduSeqId();
        driver.send(pdu);
    }

    /**
     * Send a STC_deregister PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_deregister(const uint8_t dcpId, const DcpState stateId) {
        DcpPduStc pdu = {DcpPduType::STC_deregister, getNextSeqNum(
                dcpId), dcpId, stateId};
        driver.send(pdu);
    }

    /**
     * Send a STC_prepare PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_prepare(const uint8_t dcpId, const DcpState stateId) {
        DcpPduStc pdu = {DcpPduType::STC_prepare, getNextSeqNum(
                dcpId), dcpId, stateId};
        driver.send(pdu);
    }

    /**
     * Send a STC_configure PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_configure(const uint8_t dcpId, const DcpState stateId) {
        DcpPduStc pdu = {DcpPduType::STC_configure, getNextSeqNum(
                dcpId), dcpId, stateId};
        driver.send(pdu);
    }

    /**
     * Send a STC_initialize PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_initialize(const uint8_t dcpId, const DcpState stateId) {
        DcpPduStc pdu = {DcpPduType::STC_initialize, getNextSeqNum(
                dcpId), dcpId, stateId};
        driver.send(pdu);
    }

    /**
     * Send a STC_run PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     * @param startTime At which unix time stamp action will be active (soft realtime data transfer in SYNCHRONIZINNG or RUNNING)
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_run(const uint8_t dcpId, const DcpState stateId, const int64_t startTime) {
        DcpPduStcRun pdu = {getNextSeqNum(dcpId),
                         dcpId, stateId, startTime};
        driver.send(pdu);
    }

    /**
     * Send STC_do_step PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     * @param steps Number of steps to simulate
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_do_step(const uint8_t dcpId, const DcpState stateId, const uint32_t steps) {
        DcpPduStcDoStep pdu = {getNextSeqNum(dcpId),
                            dcpId, stateId, steps};
        driver.send(pdu);
    }

    /**
     * Send a STC_send_outputs PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_send_outputs(const uint8_t dcpId, const DcpState stateId) {
        DcpPduStc pdu = {DcpPduType::STC_send_outputs, getNextSeqNum(
                dcpId), dcpId, stateId};
        driver.send(pdu);
    }

    /**
     * Send a STC_stop PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_stop(const uint8_t dcpId, const DcpState stateId) {
        DcpPduStc pdu = {DcpPduType::STC_stop, getNextSeqNum(dcpId),
                                          dcpId, stateId};
        driver.send(pdu);
    }

    /**
     * Send a STC_reset PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_reset(const uint8_t dcpId, const DcpState stateId) {
        DcpPduStc pdu = {DcpPduType::STC_reset, getNextSeqNum(dcpId),
                                          dcpId, stateId};
        driver.send(pdu);
    }

    /**
     * Send a INF_state PDU
     * @param dcpId Receiver of the PDU
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void INF_state(const uint8_t dcpId) {
        DcpPduBasic pdu = {DcpPduType::INF_state, getNextSeqNum(
                dcpId), dcpId};
        driver.send(pdu);
    }

    /**
     * Send a INF_error PDU
     * @param dcpId Receiver of the PDU
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void INF_error(const uint8_t dcpId) {
        DcpPduBasic pdu = {DcpPduType::INF_error, getNextSeqNum(
                dcpId), dcpId};
        driver.send(pdu);
    }

    /**
     * Send a INF_log PDU.
     * @param dcpId Receiver of the PDU
     * @param logCategory Log category of the log entries in responding RPS_log_ack
     * @param logMaxNum Max. number of log entries in responding RSP_log_ack
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void INF_log(const uint8_t dcpId, const uint8_t logCategory, const uint8_t logMaxNum) {
        DcpPduInfLog pdu = {getNextSeqNum(
                dcpId), dcpId, logCategory, logMaxNum};
        driver.send(pdu);
    }

    /**
    * Send a CFG_time_res PDU
    * @param dcpId Receiver of the PDU
    * @param numerator numerator of the time resolution
    * @param denominator denominator of the time resolution
    *
    * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
    */
    void CFG_time_res(const uint8_t dcpId, const uint32_t numerator,
                          const uint32_t denominator) {
        DcpPduCfgTimeRes pdu = {getNextSeqNum(dcpId), dcpId,
                                numerator, denominator};
        driver.send(pdu);
    }

    /**
     * Send a CFG_steps
     * @param dcpId Receiver of the PDU
     * @param dataId Data id for which the steps value is valid
     * @param steps Steps for DAT_input_output PDU of given dataId
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void CFG_steps(const uint8_t dcpId, uint16_t dataId, const uint32_t steps) {
        DcpPduCfgSteps pdu = {getNextSeqNum(dcpId), dcpId, steps, dataId};
        driver.send(pdu);
    }

    /**
     * Send a CFG_input
     * @param dcpId Receiver of the PDU
     * @param dataId Data id from which the input will read its value
     * @param pos Position of value in the sequence of variables in DAT_input_output PDU (This can be at any byte position of the PDU)
     * @param targetVr Value reference of the input
     * @param sourceDataType Data type of the value in the DCP_input_output PDU
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void CFG_input(const uint8_t dcpId,
                          const uint16_t dataId, uint16_t pos, const uint64_t targetVr,
                          const DcpDataType sourceDataType) {
        assert((uint8_t) sourceDataType <= 11);
        DcpPduCfgInput pdu = {getNextSeqNum(dcpId), dcpId, dataId, pos, targetVr, sourceDataType};
        driver.send(pdu);
    }

    /**
     * Send a CFG_output
     * @param dcpId Receiver of the PDU
     * @param dataId Data id of the DAT_input_output PDU to which the output (defined by sourceVR) writes its value
     * @param pos Position of value in the sequence of variables in DAT_input_output PDU (This can be at any byte position of the PDU)
     * @param sourceVr Value reference of the output
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void CFG_output(const uint8_t dcpId, const uint16_t dataId,
                           const uint16_t pos, const uint64_t sourceVr) {
        DcpPduCfgOutput pdu = {getNextSeqNum(dcpId), dcpId, dataId, pos, sourceVr};
        driver.send(pdu);
    }

    /**
     * Send a CFG_clear PDU
     * @param dcpId Receiver of the PDU
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void CFG_clear(const uint8_t dcpId) {
        DcpPduBasic pdu = {DcpPduType::CFG_clear, getNextSeqNum(dcpId),
                           dcpId};
        lastClearSeq[dcpId] = pdu.getPduSeqId();
        driver.send(pdu);
    }

    /**
     * Send a CFG_target_network_information PDU for the UDP_IPv4 transport protocol
     *
     * @param dcpId Receiver of the PDU
     * @param dataId Data id for which this configuration is valid
     * @param ipAddress Destination IP address of the DAT_input_output PDU
     * @param port Destination IP port of the DAT_input_output PDU
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void CFG_target_network_information_UDP(const uint8_t dcpId,
                                                const uint16_t dataId, const uint32_t ipAddress,
                                                const uint16_t port) {
        DcpPduCfgNetworkInformationIPv4 pdu = {DcpPduType::CFG_target_network_information, getNextSeqNum(dcpId),
                                                   dcpId, dataId, port, ipAddress,  DcpTransportProtocol::UDP_IPv4};
        driver.send(pdu);
    }

    /**
     * Send a CFG_target_network_information PDU for the TCP_IPv4 transport protocol
     *
     * @param dcpId Receiver of the PDU
     * @param dataId Data id for which this configuration is valid
     * @param ipAddress Destination IP address of the DAT_input_output PDU
     * @param port Destination IP port of the DAT_input_output PDU
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void CFG_target_network_information_TCP(const uint8_t dcpId,
                                            const uint16_t dataId, const uint32_t ipAddress,
                                            const uint16_t port) {
        DcpPduCfgNetworkInformationIPv4 pdu = {DcpPduType::CFG_target_network_information, getNextSeqNum(dcpId),
                                                   dcpId, dataId, port, ipAddress,  DcpTransportProtocol::TCP_IPv4};
        driver.send(pdu);
    }

    /**
     * Send a CFG_source_network_information PDU for the UDP_IPv4 transport protocol
     * @param dcpId Receiver of the PDU
     * @param dataId Data id for which this configuration is valid
     * @param ipAddress IP address on which the slave has to listen for DAT_input_output PDU
     * @param port Port on which the slave has to listen for DAT_input_output PDU
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void CFG_source_network_information_UDP(const uint8_t dcpId,
                                                const uint16_t dataId, const uint32_t ipAddress,
                                                const uint16_t port) {
        DcpPduCfgNetworkInformationIPv4 pdu = {DcpPduType::CFG_source_network_information, getNextSeqNum(dcpId),
                                                   dcpId, dataId, port, ipAddress, DcpTransportProtocol::UDP_IPv4};
        driver.send(pdu);
    }

    /**
    * Send a CFG_source_network_information PDU for the TCP_IPv4 transport protocol
    * @param dcpId Receiver of the PDU
    * @param dataId Data id for which this configuration is valid
    * @param ipAddress IP address on which the slave has to listen for DAT_input_output PDU
    * @param port Port on which the slave has to listen for DAT_input_output PDU
    *
    * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
    */
    void CFG_source_network_information_TCP(const uint8_t dcpId,
                                            const uint16_t dataId, const uint32_t ipAddress,
                                            const uint16_t port) {
        DcpPduCfgNetworkInformationIPv4 pdu = {DcpPduType::CFG_source_network_information, getNextSeqNum(dcpId),
                                                   dcpId, dataId, port, ipAddress, DcpTransportProtocol::TCP_IPv4};
        driver.send(pdu);
    }

    /**
     * Send a CFG_parameter PDU
     * @param dcpId Receiver of the PDU
     * @param parameterVr Value reference of the parameter
     * @param sourceDataType Data Type of the value in configuration
     * @param configuration Value of the parameter
     * @param configurationLength NUmber of bytes used by configuration
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void CFG_parameter(const uint8_t dcpId, const uint64_t parameterVr, const DcpDataType sourceDataType,
                           uint8_t *configuration, size_t configurationLength) {
        assert((uint8_t) sourceDataType <= 11);
        DcpPduCfgParameter setParameter = {getNextSeqNum(dcpId), dcpId, parameterVr, sourceDataType, configuration,
                                           configurationLength};
        driver.send(setParameter);
    }

    /**
     * Send a CFG_tunable_parameter PDU
     * @param dcpId Receiver of the PDU
     * @param paramId Parameter id of DAT parameter PDUs from which the parameter will read its value
     * @param pos  Position of value in the sequence of variables in DAT_input_output PDU (This can be at any byte position of the PDU)
     * @param parameterVr Value reference of the parameter
     * @param sourceDataType Data type of the value in the DCP_parameter PDU
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void CFG_tunable_parameter(const uint8_t dcpId,
                                      const uint16_t paramId, uint16_t pos, const uint64_t parameterVr,
                                      const DcpDataType sourceDataType) {
        assert((uint8_t) sourceDataType <= 11);
        DcpPduCfgTunableParameter configTunableParameter = {getNextSeqNum(dcpId), dcpId, paramId, pos, parameterVr,
                                                               sourceDataType};
        driver.send(configTunableParameter);
    }

    /**
     * Send a CFG_param_network_information PDU for the UDP_IPv4 transport protocol
     * @param dcpId Receiver of the PDU
     * @param paramId Parameter id for which this configuration is valid
     * @param ipAddress IP address on which the slave has to listen for DAT_input_output PDU
     * @param port Port on which the slave has to listen for DAT_input_output PDU
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void CFG_param_network_information_UDP(const uint8_t dcpId,
                                               const uint16_t paramId, const uint32_t ipAddress,
                                               const uint16_t port) {
        DcpPduCfgParamNetworkInformationIPv4 aciPduSetParamNetworkInformationUdp = {getNextSeqNum(dcpId), dcpId, paramId,
                                                                                   port, ipAddress, DcpTransportProtocol::UDP_IPv4};
        driver.send(aciPduSetParamNetworkInformationUdp);
    }

    /**
    * Send a CFG_param_network_information PDU for the TCP_IPv4 transport protocol
    * @param dcpId Receiver of the PDU
    * @param paramId Parameter id for which this configuration is valid
    * @param ipAddress IP address on which the slave has to listen for DAT_input_output PDU
    * @param port Port on which the slave has to listen for DAT_input_output PDU
    *
    * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
    */
    void CFG_param_network_information_TCP(const uint8_t dcpId,
                                           const uint16_t paramId, const uint32_t ipAddress,
                                           const uint16_t port) {
        DcpPduCfgParamNetworkInformationIPv4 aciPduSetParamNetworkInformationUdp = {getNextSeqNum(dcpId), dcpId, paramId,
                                                                                        port, ipAddress, DcpTransportProtocol::TCP_IPv4};
        driver.send(aciPduSetParamNetworkInformationUdp);
    }

    /**
     * Send a CFG_logging PDU
     * @param dcpId Receiver of the PDU
     * @param logCategory Log category for which this configuration is valid
     * @param logLevel Log Level for which this configuration is valid
     * @param logMode Log mode of the log entries with the corresponding category and level
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void CFG_logging(const uint8_t dcpId, const uint8_t logCategory, const DcpLogLevel logLevel,
                         const DcpLogMode logMode) {
        DcpPduCfgLogging setLogging = {getNextSeqNum(dcpId), dcpId, logCategory, logLevel, logMode};
        driver.send(setLogging);
    }

    /**
     * Send a CFG_scope PDU
     * @param dcpId Receiver of the PDU
     * @param dataId Data id for which this configuration is valid
     * @param scope Dcp scope of the given data id
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void CFG_scope(const uint8_t dcpId, const uint16_t dataId, const DcpScope scope) {
        DcpPduCfgScope setScope = {getNextSeqNum(dcpId), dcpId, dataId, scope};
        driver.send(setScope);
    }

    /**
     * Send a DAT_parameter PDU
     * @param paramId Parameter id of the PDU
     * @param configuration Values of the parameters, configured with this paramId
     * @param configurationLength Number of bytes of configuration
     *
     * @pre setTargetParamNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void DAT_parameter(const uint16_t paramId, uint8_t *configuration, size_t configurationLength) {
        DcpPduDatParameter parameter = {getNextParameterSeqNum(paramId), paramId, configuration, configurationLength};
        driver.send(parameter);
    }

    /**
    * Send a DAT_input_output PDU
    * @param dataId Data id of the PDU
    * @param configuration Values of the outputs, configured with this dataId
    * @param configurationLength Number of bytes of configuration
    *
    * @pre setTargetNetworkInformation of the given DcpDriver was called for dcpId before
    */
    void DAT_input_output(const uint16_t dataId, uint8_t *configuration, size_t configurationLength) {
        DcpPduDatInputOutput data = {getNextParameterSeqNum(dataId), dataId, configuration, configurationLength};
        driver.send(data);
    }

    /**
     * Enables to send periodically INF_state to a DCP slave
     * @param dcpId Receiver of the INF_state PDU
     * @param numerator Numerator of the sending period
     * @param denominator Denominator of the sending period
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void enableHeartbeat(const uint8_t dcpId, const uint32_t numerator, const uint32_t denominator) {
        //close existing heartbeat thread if exist
        disableHeartbeat(dcpId);
        runningHeartbeats[dcpId] = true;
        heartbeatThreads[dcpId] = std::unique_ptr<std::thread>(
                new std::thread(&DcpManagerMaster::heartBeatRoutine, this, dcpId, numerator, denominator));
    }

    /**
     * Disable to send periodically INF_state to a DCP slave
     * @param dcpId Receiver of the INF_state PDU
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void disableHeartbeat(const uint8_t dcpId) {
        if (heartbeatThreads.count(dcpId)) {
            runningHeartbeats[dcpId] = false;
            heartbeatCV[dcpId].notify_all();
            heartbeatThreads[dcpId]->join();
            heartbeatThreads.erase(dcpId);
            runningHeartbeats.erase(dcpId);
        }
    }

    /**
     * Set the slave description for a DCP slave. This is necessary to receive Log messages of the slave.
     * @param dcpId DCP id of the slaves connected to the given slave description
     * @param slaveDescription Slave description of the slave
     */
    void setDcpSlaveDescription(const uint8_t dcpId, const SlaveDescription_t &slaveDescription) {
        if (slaveDescription.Log.get() != nullptr) {
            for (const auto &templateObj : slaveDescription.Log->templates) {
                std::vector<DcpDataType> cDataTypes;
                std::vector<std::string> strs;
                std::string buf;                 // Have a buffer string
                std::stringstream ss(templateObj.msg);
                while (ss >> buf)
                    strs.push_back(buf);
                for (const std::string &str: strs) {
                    if (str[0] == '%') {
                        cDataTypes.push_back(from_string_DcpDataType(str.substr(1)));
                    }
                }
                logTemplates[dcpId].insert(std::make_pair(templateObj.id,
                                                          LogTemplate(templateObj.id, templateObj.category,
                                                                      (DcpLogLevel) templateObj.level, templateObj.msg,
                                                                      cDataTypes)));
            }
        }
    }

    /*
     * Set the listener for RPS_ack PDUs
     * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param ackReceivedListener function which will be called after the event occurs
     */
    template<FunctionType ftype>
    void setAckReceivedListener(const std::function<void(uint8_t, uint16_t)> ackReceivedListener) {
        this->ackReceivedListener = std::move(ackReceivedListener);
        synchronousCallback[DcpCallbackTypes::ACK] = ftype == SYNC;
    }

    /**
     * Set the listener for RSP_nack PDUs
     * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param nAckReceivedListener function which will be called after the event occurs
     */
    template<FunctionType ftype>
    void setNAckReceivedListener(const std::function<void(uint8_t, uint16_t, DcpError)> nAckReceivedListener) {
        this->nAckReceivedListener = std::move(nAckReceivedListener);
        synchronousCallback[DcpCallbackTypes::NACK] = ftype == SYNC;
    }

    /**
     * Set the listener for RSP_state_ack PDUs
     * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param stateAckReceivedListener function which will be called after the event occurs
     */
    template<FunctionType ftype>
    void setStateAckReceivedListener(const std::function<void(uint8_t, uint16_t, DcpState)> stateAckReceivedListener) {
        this->stateAckReceivedListener = std::move(stateAckReceivedListener);
        synchronousCallback[DcpCallbackTypes::STATE_ACK] = ftype == SYNC;
    }

    /**
     * Set the listener for RSP_error_ack PDUs
     * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param errorAckReceivedListener function which will be called after the event occurs
     */
    template<FunctionType ftype>
    void setErrorAckReceivedListener(const std::function<void(uint8_t, uint16_t, DcpError)> errorAckReceivedListener) {
        this->errorAckReceivedListener = std::move(errorAckReceivedListener);
        synchronousCallback[DcpCallbackTypes::ERROR_ACK] = ftype == SYNC;
    }

    /**
     * Set the listener for NTF_state PDUs
     * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param stateChangedNotificationReceivedListener function which will be called after the event occurs
     */
    template<FunctionType ftype>
    void setStateChangedNotificationReceivedListener(
            const std::function<void(uint8_t, DcpState)> stateChangedNotificationReceivedListener) {
        this->stateChangedNotificationReceivedListener = std::move(stateChangedNotificationReceivedListener);
        synchronousCallback[DcpCallbackTypes::STATE_CHANGED] = ftype == SYNC;
    }

    template<FunctionType ftype>
    void setErrorListener(const std::function<void(DcpError errorCode)> errorListener) {
        this->errorListener = std::move(errorListener);
        synchronousCallback[DcpCallbackTypes::ERROR_LI] = ftype == SYNC;
    }

    /**
     * Set the listener for missed control PDUs
     * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param pduMissedListener function which will be called after the event occurs
     */
    template<FunctionType ftype>
    void setPduMissedListener(const std::function<void(uint8_t)> pduMissedListener) {
        this->pduMissedListener = std::move(pduMissedListener);
        synchronousCallback[DcpCallbackTypes::PDU_MISSED] = ftype == SYNC;
    }

    /**
     * Set the listener for missed DAT_input_output PDUs
     * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param inputOutputPduMissedListener function which will be called after the event occurs
     */
    template<FunctionType ftype>
    void setInputOutputPduMissedListener(const std::function<void(uint16_t)> inputOutputPduMissedListener) {
        this->inputOutputPduMissedListener = std::move(inputOutputPduMissedListener);
        synchronousCallback[DcpCallbackTypes::IN_OUT_MISSED] = ftype == SYNC;
    }

    /**
     * Set the listener for DAT_input_output PDUs
     * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param dataReceivedListener function which will be called after the event occurs
     */
    template<FunctionType ftype>
    void setDataReceivedListener(const std::function<void(uint16_t, size_t, uint8_t *)> dataReceivedListener) {
        this->dataReceivedListener = std::move(dataReceivedListener);
        synchronousCallback[DcpCallbackTypes::DATA] = ftype == SYNC;
    }

    /**
     * Set the listener for RSP_log PDUs
     * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param logAckListener function which will be called after the event occurs
     */
    template<FunctionType ftype>
    void setLogAckListener(
            const std::function<void(uint8_t, uint16_t, std::vector<std::shared_ptr<LogEntry>>)> logAckListener) {
        DcpManagerMaster::logAckListener = std::move(logAckListener);
        synchronousCallback[DcpCallbackTypes::RSP_log_ack] = ftype == SYNC;
    }

    /**
     * Set the listener for NTF_log PDUs
     * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param logNotificationListener function which will be called after the event occurs
     */
    template<FunctionType ftype>
    void
    setLogNotificationListener(const std::function<void(uint8_t, std::shared_ptr<LogEntry>)> logNotificationListener) {
        DcpManagerMaster::logNotificationListener = std::move(logNotificationListener);
        synchronousCallback[DcpCallbackTypes::NTF_LOG] = ftype == SYNC;
    }

    void setPDUListener(const std::function<void(DcpPdu &)> pduListener) {
        DcpManagerMaster::pduListener = std::move(pduListener);
    }

    DcpManager getDcpManager() override {
        return {[this](DcpPdu &msg) { receive(msg); },
                [this](const DcpError errorCode) { reportError(errorCode); }};
    }

private:
    std::map<uint8_t, bool> runningHeartbeats;
    std::map<uint8_t, std::unique_ptr<std::thread>> heartbeatThreads;
    std::map<uint8_t, std::condition_variable> heartbeatCV;
    std::map<uint8_t, std::mutex> heartbeatMutex;

    std::map<uint8_t, uint16_t> lastRegisterSeq;
    std::map<uint8_t, uint16_t> lastRegisterSuccessfullSeq;
    std::map<uint8_t, uint16_t> lastClearSeq;

    std::map<DcpCallbackTypes, bool> synchronousCallback;
    std::function<void(uint8_t sender, uint16_t pduSeqId)> ackReceivedListener = [](uint8_t sender,
                                                                                    uint16_t pduSeqId) {};
    std::function<void(uint8_t sender, uint16_t pduSeqId,
                       DcpError errorCode)> nAckReceivedListener = [](uint8_t sender, uint16_t pduSeqId,
                                                                      DcpError errorCode) {};
    std::function<void(uint8_t sender, uint16_t pduSeqId,
                       DcpState state)> stateAckReceivedListener = [](uint8_t sender, uint16_t pduSeqId,
                                                                      DcpState state) {};
    std::function<void(uint8_t sender, uint16_t pduSeqId,
                       DcpError errorCode)> errorAckReceivedListener = [](uint8_t sender, uint16_t pduSeqId,
                                                                          DcpError errorCode) {};
    std::function<void(uint8_t sender,
                       DcpState state)> stateChangedNotificationReceivedListener = [](uint8_t sender,
                                                                                      DcpState state) {};
    std::function<void(DcpError errorCode)> errorListener = [](DcpError errorCode) {};
    std::function<void(uint8_t slaveId)> pduMissedListener = [](uint8_t slaveId) {};
    std::function<void(uint16_t dataId)> inputOutputPduMissedListener = [](uint16_t dataId) {};
    std::function<void(uint16_t dataId, size_t length, uint8_t payload[])> dataReceivedListener = [](uint16_t dataId,
                                                                                                     size_t length,
                                                                                                     uint8_t payload[]) {};
    std::function<void(uint8_t sender, uint16_t pduSeqId,
                       std::vector<std::shared_ptr<LogEntry>> entries)> logAckListener = [](uint8_t sender,
                                                                                            uint16_t pduSeqId,
                                                                                            std::vector<std::shared_ptr<LogEntry>> entries) {};
    std::function<void(uint8_t sender, std::shared_ptr<LogEntry> entry)> logNotificationListener = [](uint8_t sender,
                                                                                                      std::shared_ptr<LogEntry> entry) {};
    std::function<void(DcpPdu &pdu)> pduListener = [](DcpPdu &pdu) {};

    std::map<dcpId_t, std::map<logTemplateId_t, LogTemplate>> logTemplates;

    const LogTemplate SENDING_HEARTBEAT_STARTED = LogTemplate(160, LogCategory::DCP_LIB_MASTER,
                                                              DcpLogLevel::LVL_INFORMATION,
                                                              "Start sending heartbeat to slave id %uint8 every %uint32 / %uint32s.",
                                                              {DcpDataType::uint8, DcpDataType::uint32,
                                                               DcpDataType::uint32});
    const LogTemplate SENDING_HEARTBEAT_STOPPED = LogTemplate(161, LogCategory::DCP_LIB_MASTER,
                                                              DcpLogLevel::LVL_INFORMATION,
                                                              "Stop sending heartbeat to slave id %uint8.",
                                                              {DcpDataType::uint8});

    void heartBeatRoutine(const uint8_t dcpId, const uint32_t numerator, const uint32_t denominator) {
        using namespace std::chrono;
#ifdef DEBUG
        Log(SENDING_HEARTBEAT_STARTED, dcpId, numerator, denominator);
#endif
        int64_t between = (int64_t) (1000000 * ((double) numerator) / ((double) denominator));
        std::unique_lock<std::mutex> lk(heartbeatMutex[dcpId]);

        while (runningHeartbeats[dcpId]) {
            time_point<system_clock> now = time_point_cast<microseconds>(system_clock::now());
            INF_state(dcpId);
            time_point<system_clock> nextCheck = now + microseconds(between);
            heartbeatCV[dcpId].wait_until(lk, nextCheck);
        }
#ifdef DEBUG
        Log(SENDING_HEARTBEAT_STOPPED, dcpId);
#endif
    }
};

#endif /* ACI_LOGIC_DRIVERMANAGERMASTER_H_ */