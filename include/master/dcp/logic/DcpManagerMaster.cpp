#include "DcpManagerMaster.hpp"

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

#include <dcp/helper/Helper.hpp>

#include <iostream>
#include <cassert>


DcpManagerMaster::DcpManagerMaster(DcpDriver driver) {
    this->driver = driver;
    this->masterId = 0;
}

void DcpManagerMaster::receive(DcpPdu &msg) {
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
                if (synchronousCallback[DcpCallbackTypes::IN_OUT_MISSED]) {
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
                // TODO dataSegNums maps dataId to seq not slave! rm these 2 lines
                dataSegNumsOut[ack.getSender()] = 0;
                dataSegNumsIn[ack.getSender()] = 0;
                // what should be done instead?
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
            if (synchronousCallback[DcpCallbackTypes::STATE_ACK]) {
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
            if (synchronousCallback[DcpCallbackTypes::ERROR_ACK]) {
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
            if (synchronousCallback[DcpCallbackTypes::STATE_CHANGED]) {
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
            if (synchronousCallback[DcpCallbackTypes::DATA]) {
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

void DcpManagerMaster::reportError(const DcpError errorCode) {
    if (synchronousCallback[DcpCallbackTypes::ERROR_LI]) {
        errorListener(errorCode);
    } else {
        std::thread t(errorListener, errorCode);
        t.detach();
    }
}


void DcpManagerMaster::STC_register(const uint8_t dcpId, const DcpState stateId, const uint128_t slaveUuid,
                    const DcpOpMode opMode, const uint8_t majorversion, const uint8_t minorVersion) {
    DcpPduStcRegister pdu = {getNextSeqNum(dcpId), dcpId, stateId, slaveUuid, opMode, majorversion, minorVersion};
    lastRegisterSeq[dcpId] = pdu.getPduSeqId();
    driver.send(pdu);
}


void DcpManagerMaster::STC_deregister(const uint8_t dcpId, const DcpState stateId) {
    DcpPduStc pdu = {DcpPduType::STC_deregister, getNextSeqNum(
            dcpId), dcpId, stateId};
    driver.send(pdu);
}


void DcpManagerMaster::STC_prepare(const uint8_t dcpId, const DcpState stateId) {
    DcpPduStc pdu = {DcpPduType::STC_prepare, getNextSeqNum(
            dcpId), dcpId, stateId};
    driver.send(pdu);
}


void DcpManagerMaster::STC_configure(const uint8_t dcpId, const DcpState stateId) {
    DcpPduStc pdu = {DcpPduType::STC_configure, getNextSeqNum(
            dcpId), dcpId, stateId};
    driver.send(pdu);
}


void DcpManagerMaster::STC_initialize(const uint8_t dcpId, const DcpState stateId) {
    DcpPduStc pdu = {DcpPduType::STC_initialize, getNextSeqNum(
            dcpId), dcpId, stateId};
    driver.send(pdu);
}


void DcpManagerMaster::STC_run(const uint8_t dcpId, const DcpState stateId, const int64_t startTime) {
    DcpPduStcRun pdu = {getNextSeqNum(dcpId),
                        dcpId, stateId, startTime};
    driver.send(pdu);
}


void DcpManagerMaster::STC_do_step(const uint8_t dcpId, const DcpState stateId, const uint32_t steps) {
    DcpPduStcDoStep pdu = {getNextSeqNum(dcpId),
                        dcpId, stateId, steps};
    driver.send(pdu);
}


void DcpManagerMaster::STC_send_outputs(const uint8_t dcpId, const DcpState stateId) {
    DcpPduStc pdu = {DcpPduType::STC_send_outputs, getNextSeqNum(
            dcpId), dcpId, stateId};
    driver.send(pdu);
}


void DcpManagerMaster::STC_stop(const uint8_t dcpId, const DcpState stateId) {
    DcpPduStc pdu = {DcpPduType::STC_stop, getNextSeqNum(dcpId),
                                        dcpId, stateId};
    driver.send(pdu);
}


void DcpManagerMaster::STC_reset(const uint8_t dcpId, const DcpState stateId) {
    DcpPduStc pdu = {DcpPduType::STC_reset, getNextSeqNum(dcpId),
                                        dcpId, stateId};
    driver.send(pdu);
}


void DcpManagerMaster::INF_state(const uint8_t dcpId) {
    DcpPduBasic pdu = {DcpPduType::INF_state, getNextSeqNum(
            dcpId), dcpId};
    driver.send(pdu);
}


void DcpManagerMaster::INF_error(const uint8_t dcpId) {
    DcpPduBasic pdu = {DcpPduType::INF_error, getNextSeqNum(
            dcpId), dcpId};
    driver.send(pdu);
}


void DcpManagerMaster::INF_log(const uint8_t dcpId, const uint8_t logCategory, const uint8_t logMaxNum) {
    DcpPduInfLog pdu = {getNextSeqNum(
            dcpId), dcpId, logCategory, logMaxNum};
    driver.send(pdu);
}


void DcpManagerMaster::CFG_time_res(const uint8_t dcpId, const uint32_t numerator,
                        const uint32_t denominator) {
    DcpPduCfgTimeRes pdu = {getNextSeqNum(dcpId), dcpId,
                            numerator, denominator};
    driver.send(pdu);
}


void DcpManagerMaster::CFG_steps(const uint8_t dcpId, uint16_t dataId, const uint32_t steps) {
    DcpPduCfgSteps pdu = {getNextSeqNum(dcpId), dcpId, steps, dataId};
    driver.send(pdu);
}


void DcpManagerMaster::CFG_input(const uint8_t dcpId,
                        const uint16_t dataId, uint16_t pos, const uint64_t targetVr,
                        const DcpDataType sourceDataType) {
    assert((uint8_t) sourceDataType <= 11);
    DcpPduCfgInput pdu = {getNextSeqNum(dcpId), dcpId, dataId, pos, targetVr, sourceDataType};
    driver.send(pdu);
}


void DcpManagerMaster::CFG_output(const uint8_t dcpId, const uint16_t dataId,
                        const uint16_t pos, const uint64_t sourceVr) {
    DcpPduCfgOutput pdu = {getNextSeqNum(dcpId), dcpId, dataId, pos, sourceVr};
    driver.send(pdu);
}


void DcpManagerMaster::CFG_clear(const uint8_t dcpId) {
    DcpPduBasic pdu = {DcpPduType::CFG_clear, getNextSeqNum(dcpId),
                        dcpId};
    lastClearSeq[dcpId] = pdu.getPduSeqId();
    driver.send(pdu);
}


void DcpManagerMaster::CFG_target_network_information_UDP(const uint8_t dcpId,
                                            const uint16_t dataId, const uint32_t ipAddress,
                                            const uint16_t port) {
    DcpPduCfgNetworkInformationIPv4 pdu = {DcpPduType::CFG_target_network_information, getNextSeqNum(dcpId),
                                                dcpId, dataId, port, ipAddress,  DcpTransportProtocol::UDP_IPv4};
    driver.send(pdu);
}


void DcpManagerMaster::CFG_target_network_information_TCP(const uint8_t dcpId,
                                        const uint16_t dataId, const uint32_t ipAddress,
                                        const uint16_t port) {
    DcpPduCfgNetworkInformationIPv4 pdu = {DcpPduType::CFG_target_network_information, getNextSeqNum(dcpId),
                                                dcpId, dataId, port, ipAddress,  DcpTransportProtocol::TCP_IPv4};
    driver.send(pdu);
}


void DcpManagerMaster::CFG_source_network_information_UDP(const uint8_t dcpId,
                                            const uint16_t dataId, const uint32_t ipAddress,
                                            const uint16_t port) {
    DcpPduCfgNetworkInformationIPv4 pdu = {DcpPduType::CFG_source_network_information, getNextSeqNum(dcpId),
                                                dcpId, dataId, port, ipAddress, DcpTransportProtocol::UDP_IPv4};
    driver.send(pdu);
}


void DcpManagerMaster::CFG_source_network_information_TCP(const uint8_t dcpId,
                                        const uint16_t dataId, const uint32_t ipAddress,
                                        const uint16_t port) {
    DcpPduCfgNetworkInformationIPv4 pdu = {DcpPduType::CFG_source_network_information, getNextSeqNum(dcpId),
                                                dcpId, dataId, port, ipAddress, DcpTransportProtocol::TCP_IPv4};
    driver.send(pdu);
}


void DcpManagerMaster::CFG_parameter(const uint8_t dcpId, const uint64_t parameterVr, const DcpDataType sourceDataType,
                        uint8_t *configuration, size_t configurationLength) {
    assert((uint8_t) sourceDataType <= 11);
    DcpPduCfgParameter setParameter = {getNextSeqNum(dcpId), dcpId, parameterVr, sourceDataType, configuration,
                                        configurationLength};
    driver.send(setParameter);
}


void DcpManagerMaster::CFG_tunable_parameter(const uint8_t dcpId,
                                    const uint16_t paramId, uint16_t pos, const uint64_t parameterVr,
                                    const DcpDataType sourceDataType) {
    assert((uint8_t) sourceDataType <= 11);
    DcpPduCfgTunableParameter configTunableParameter = {getNextSeqNum(dcpId), dcpId, paramId, pos, parameterVr,
                                                            sourceDataType};
    driver.send(configTunableParameter);
}


void DcpManagerMaster::CFG_param_network_information_UDP(const uint8_t dcpId,
                                            const uint16_t paramId, const uint32_t ipAddress,
                                            const uint16_t port) {
    DcpPduCfgParamNetworkInformationIPv4 aciPduSetParamNetworkInformationUdp = {getNextSeqNum(dcpId), dcpId, paramId,
                                                                                port, ipAddress, DcpTransportProtocol::UDP_IPv4};
    driver.send(aciPduSetParamNetworkInformationUdp);
}


void DcpManagerMaster::CFG_param_network_information_TCP(const uint8_t dcpId,
                                        const uint16_t paramId, const uint32_t ipAddress,
                                        const uint16_t port) {
    DcpPduCfgParamNetworkInformationIPv4 aciPduSetParamNetworkInformationUdp = {getNextSeqNum(dcpId), dcpId, paramId,
                                                                                    port, ipAddress, DcpTransportProtocol::TCP_IPv4};
    driver.send(aciPduSetParamNetworkInformationUdp);
}


void DcpManagerMaster::CFG_logging(const uint8_t dcpId, const uint8_t logCategory, const DcpLogLevel logLevel,
                        const DcpLogMode logMode) {
    DcpPduCfgLogging setLogging = {getNextSeqNum(dcpId), dcpId, logCategory, logLevel, logMode};
    driver.send(setLogging);
}


void DcpManagerMaster::CFG_scope(const uint8_t dcpId, const uint16_t dataId, const DcpScope scope) {
    DcpPduCfgScope setScope = {getNextSeqNum(dcpId), dcpId, dataId, scope};
    driver.send(setScope);
}


void DcpManagerMaster::DAT_parameter(const uint16_t paramId, uint8_t *configuration, size_t configurationLength) {
    DcpPduDatParameter parameter = {getNextParameterSeqNum(paramId), paramId, configuration, configurationLength};
    driver.send(parameter);
}


void DcpManagerMaster::DAT_input_output(const uint16_t dataId, uint8_t *configuration, size_t configurationLength) {
    DcpPduDatInputOutput data = {getNextParameterSeqNum(dataId), dataId, configuration, configurationLength};
    driver.send(data);
}


void DcpManagerMaster::enableHeartbeat(const uint8_t dcpId, const uint32_t numerator, const uint32_t denominator) {
    //close existing heartbeat thread if exist
    disableHeartbeat(dcpId);
    runningHeartbeats[dcpId] = true;
    heartbeatThreads[dcpId] = std::unique_ptr<std::thread>(
            new std::thread(&DcpManagerMaster::heartBeatRoutine, this, dcpId, numerator, denominator));
}


void DcpManagerMaster::disableHeartbeat(const uint8_t dcpId) {
    if (heartbeatThreads.count(dcpId)) {
        runningHeartbeats[dcpId] = false;
        heartbeatCV[dcpId].notify_all();
        heartbeatThreads[dcpId]->join();
        heartbeatThreads.erase(dcpId);
        runningHeartbeats.erase(dcpId);
    }
}


void DcpManagerMaster::setDcpSlaveDescription(const uint8_t dcpId, const SlaveDescription_t &slaveDescription) {
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

void DcpManagerMaster::setPDUListener(const std::function<void(DcpPdu &)> pduListener) {
    DcpManagerMaster::pduListener = std::move(pduListener);
}

DcpManager DcpManagerMaster::getDcpManager() {
    return {[this](DcpPdu &msg) { receive(msg); },
            [this](const DcpError errorCode) { reportError(errorCode); }};
}

void DcpManagerMaster::heartBeatRoutine(const uint8_t dcpId, const uint32_t numerator, const uint32_t denominator) {
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