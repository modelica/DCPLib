/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef ACI_LOGIC_DRIVERMANAGERSLAVE_H_
#define ACI_LOGIC_DRIVERMANAGERSLAVE_H_


#include <map>
#include <vector>
#include <set>
#include <iterator>

#include <dcp/model/DcpTypes.hpp>
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
#include <dcp/model/MultiDimValue.hpp>

#include <dcp/helper/Helper.hpp>
#include <dcp/helper/DcpSlaveDescriptionHelper.hpp>

#include <dcp/logic/AbstractDcpManager.hpp>
#include <dcp/xml/DcpSlaveDescriptionElements.hpp>

#if defined(DEBUG) || defined(LOGGING)
#include <dcp/logic/DCPSlaveErrorCodes.hpp>
#include <dcp/model/pdu/DcpPduRspErrorAck.hpp>

#endif

#ifdef ERROR
/* windows has already an error define */
#undef ERROR_LI
#endif

struct Payload {
    uint8_t *payload;
    size_t size;
};


/**
 * Basic Logic for a DCP slave
 *
 * @author Christian Kater
 */
class AbstractDcpManagerSlave : public AbstractDcpManager {
public:


    ~AbstractDcpManagerSlave() {
        for (auto const &entry : values) {
            delete entry.second;
        }
        for (auto const &entry : updatedStructure) {
            delete entry.second;
        }
    }

    void receive(DcpPdu &msg) override {

        if (!checkForError(msg)) {
            return;
        }


        switch (msg.getTypeId()) {
            case DcpPduType::STC_prepare: {
                state = DcpState::PREPARING;
                notifyStateChange();
                driver.prepare();
                prepare();
                break;
            }
            case DcpPduType::STC_configure: {
                state = DcpState::CONFIGURING;
                notifyStateChange();
                for (auto const &ent : outputAssignment) {
                    uint16_t dataId = ent.first;
#ifdef DEBUG
                    Log(DATA_BUFFER_CREATED, ent.first, bufferSize);
#endif
                    DcpPduDatInputOutput *pdu = new DcpPduDatInputOutput(0, ent.first, bufferSize);
                    outputBuffer[ent.first] = pdu;


                }

                std::map<uint32_t, std::vector<uint16_t>> stepsToDataId;
                for (dataId_t dataId : runningScope) {
                    stepsToDataId[steps[dataId]].push_back(dataId);
                }

                outputCounter.clear();
                for (auto &entry : stepsToDataId) {
                    outputCounter.push_back(std::make_tuple(entry.second, entry.first, entry.first));
                }
                driver.configure();
                configure();
                break;
            }
            case DcpPduType::STC_initialize: {
                state = DcpState::INITIALIZING;
                notifyStateChange();
                initialize();
                break;
            }
            case DcpPduType::STC_run: {
                if (state == DcpState::CONFIGURED) {
                    state = DcpState::SYNCHRONIZING;
                } else if (state == DcpState::SYNCHRONIZED) {
                    state = DcpState::RUNNING;
                }
                notifyStateChange();
                DcpPduStcRun &_run = static_cast<DcpPduStcRun &>(msg);
                if (opMode == DcpOpMode::NRT) {
                    if (state == DcpState::SYNCHRONIZING) {
                        synchronize();
                    }
                } else {
                    notifyRuntimeListener(_run.getStartTime());
                    run(_run.getStartTime());
                }
                break;
            }
            case DcpPduType::STC_do_step: {
                runLastExitPoint = state;
                state = DcpState::COMPUTING;
                notifyStateChange();
                DcpPduStcDoStep &doStepPdu = static_cast<DcpPduStcDoStep &>(msg);
                doStep(doStepPdu.getSteps());
                break;
            }
            case DcpPduType::STC_stop: {
                driver.stop();
                stop();
                break;
            }
            case DcpPduType::STC_reset: {
                errorCode = DcpError::NONE;
                state = DcpState::CONFIGURATION;
                notifyStateChange();
                startHeartbeat();
                break;
            }
            case DcpPduType::INF_state: {
                updateLastStateRequest();
                DcpPduBasic &basic = static_cast<DcpPduBasic &>(msg);

                DcpPduRspStateAck stateAck = {state == DcpState::ALIVE ? basic.getReceiver() : dcpId, basic.getPduSeqId(),
                                           state};
                driver.send(stateAck);
                break;
            }
            case DcpPduType::INF_error: {
                DcpPduBasic &basic = static_cast<DcpPduBasic &>(msg);

                DcpPduRspErrorAck errorAck = {DcpPduType::RSP_error_ack,
                                       state == DcpState::ALIVE ? basic.getReceiver() : dcpId,
                                       basic.getPduSeqId(), errorCode};
                driver.send(errorAck);
                break;
            }
            case DcpPduType::CFG_time_res: {
                DcpPduCfgTimeRes timeRes = static_cast<DcpPduCfgTimeRes &>(msg);
                setTimeRes(timeRes.getNumerator(), timeRes.getDenominator());
                break;
            }
            case DcpPduType::CFG_steps: {
                DcpPduCfgSteps &stepsMSG = static_cast<DcpPduCfgSteps &>(msg);
                setSteps(stepsMSG.getDataId(), stepsMSG.getSteps());
                break;
            }
            case DcpPduType::STC_register: {
                DcpPduStcRegister registerPdu = static_cast<DcpPduStcRegister &>(msg);
                setOperationInformation(registerPdu.getReceiver(), registerPdu.getOpMode());
                seqAtRegister = registerPdu.getPduSeqId();
                segNumsIn[masterId] = seqAtRegister;
#ifdef DEBUG
                Log(NEXT_SEQUENCE_ID_FROM_MASTER, (uint16_t) (segNumsIn[masterId] + 1));
#endif
                driver.registerSuccessfull();
                state = DcpState::CONFIGURATION;
                notifyStateChange();
                startHeartbeat();
                break;
            }
            case DcpPduType::STC_deregister: {
                opMode = DcpOpMode::HRT;
                clearConfig();
                state = DcpState::ALIVE;
                notifyStateChange();
                dcpId = 0;
                driver.disconnect();
                break;
            }
            case DcpPduType::CFG_input: {
                DcpPduCfgInput &inputConfig = static_cast<DcpPduCfgInput &>(msg);
                configuredInPos[inputConfig.getDataId()].push_back(inputConfig.getPos());
                inputAssignment[inputConfig.getDataId()][inputConfig.getPos()] = std::make_pair(
                        inputConfig.getTargetVr(),
                        inputConfig.getSourceDataType());
                std::shared_ptr<uint32_t> maxConsecMissedPdus = slavedescription::getVariable(slaveDescription,
                        inputConfig.getTargetVr())->maxConsecMissedPdus;
                if(maxConsecMissedPdus != nullptr){
                    if(maxConsecMissedPduData[inputConfig.getDataId()] == 0 ||
                    *maxConsecMissedPdus < maxConsecMissedPduData[inputConfig.getDataId()]){
                        maxConsecMissedPduData[inputConfig.getDataId()] = *maxConsecMissedPdus;
                    }
                }


#ifdef DEBUG
                Log(NEW_INPUT_CONFIG, inputConfig.getTargetVr(), inputConfig.getSourceDataType(),
                    inputConfig.getDataId());
#endif
                break;
            }

            case DcpPduType::CFG_output: {
                DcpPduCfgOutput &outputConfig = static_cast<DcpPduCfgOutput &>(msg);
                if (outputAssignment.find(outputConfig.getDataId()) == outputAssignment.end()) {
                    outputAssignment.insert(
                            std::make_pair(outputConfig.getDataId(),
                                           std::map<uint16_t, uint64_t>()));
                }
                configuredOutPos[outputConfig.getDataId()].push_back(outputConfig.getPos());


                outputAssignment[outputConfig.getDataId()][outputConfig.getPos()] = outputConfig.getSourceVr();


#ifdef DEBUG
                Log(NEW_OUTPUT_CONFIG, outputConfig.getSourceVr(), outputConfig.getDataId());
#endif

                break;

            }
            case DcpPduType::CFG_clear: {
                clearConfig();
                segNumsIn[masterId] = seqAtRegister;
#ifdef DEBUG
                Log(NEXT_SEQUENCE_ID_FROM_MASTER, (uint16_t) (segNumsIn[masterId] + 1));
#endif
                break;
            }
            case DcpPduType::STC_send_outputs: {
                if (state == DcpState::INITIALIZED) {
                    state = DcpState::SENDING_I;
                    notifyStateChange();
                    sendOutputs(initializationScope);
                    state = DcpState::CONFIGURED;
                } else if (state == DcpState::COMPUTED) {
                    state = DcpState::SENDING_D;
                    notifyStateChange();
                    sendOutputs(runningScope);
                    state = runLastExitPoint;
                }
                notifyStateChange();
                break;
            }
            case DcpPduType::DAT_input_output: {
                DcpPduDatInputOutput &data = static_cast<DcpPduDatInputOutput &>(msg);
                int offset = 0;
                std::map<uint16_t, std::pair<uint64_t, DcpDataType>> vrsToReceive =
                        inputAssignment[data.getDataId()];
                for (uint16_t i = 0; i < vrsToReceive.size(); i++) {
                    std::pair<uint64_t, DcpDataType> p = vrsToReceive[i];
                    uint64_t valueReference = p.first;
                    DcpDataType sourceDataType = p.second;

                    offset += values[valueReference]->update(data.getPayload(), offset, sourceDataType);
#ifdef DEBUG
                    Log(ASSIGNED_INPUT, valueReference, sourceDataType,
                        slavedescription::getDataType(slaveDescription, valueReference));
#endif
                }
                break;
            }
            case DcpPduType::DAT_parameter: {
                DcpPduDatParameter &param = static_cast<DcpPduDatParameter &>(msg);

                int offset = 0;
                std::map<uint16_t, std::pair<uint64_t, DcpDataType>> vrsToReceive =
                        paramAssignment[param.getParamId()];
                for (uint16_t i = 0; i < vrsToReceive.size(); i++) {
                    std::pair<uint64_t, DcpDataType> p = vrsToReceive[i];
                    uint64_t valueReference = p.first;
                    DcpDataType sourceDataType = p.second;

                    if (slavedescription::structuralParameterExists(slaveDescription, valueReference)) {
                        offset += values[valueReference]->update(param.getConfiguration(), offset, sourceDataType);

                        size_t value;
                        switch (slavedescription::getDataType(slaveDescription, valueReference)) {
                            case DcpDataType::uint8:
                                value = *values[valueReference]->getValue<int8_t *>();
                                break;
                            case DcpDataType::uint16:
                                value = *values[valueReference]->getValue<int16_t *>();
                                break;
                            case DcpDataType::uint32:
                                value = *values[valueReference]->getValue<int32_t *>();
                                break;
                            case DcpDataType::uint64:
                                value = *values[valueReference]->getValue<int64_t *>();
                                break;
                            default:
                                //only uint datatypes are allowed for structual parameters
                                break;
                        }
                        updateStructualDependencies(valueReference, value);
                    } else {
                        checkForUpdatedStructure(valueReference);
                        offset += values[valueReference]->update(param.getConfiguration(), offset, sourceDataType);
                    }
                }
                break;
            }
            case DcpPduType::CFG_parameter: {
                DcpPduCfgParameter &parameter = static_cast<DcpPduCfgParameter &>(msg);
                uint64_t &valueReference = parameter.getParameterVr();
                if (slavedescription::structuralParameterExists(slaveDescription, valueReference)) {
                    values[valueReference]->update(parameter.getConfiguration(), 0,
                                                   slavedescription::getDataType(slaveDescription, valueReference));

                    size_t value;
                    switch (slavedescription::getDataType(slaveDescription, valueReference)) {
                        case DcpDataType::uint8:
                            value = *values[valueReference]->getValue<int8_t *>();
                            break;
                        case DcpDataType::uint16:
                            value = *values[valueReference]->getValue<int16_t *>();
                            break;
                        case DcpDataType::uint32:
                            value = *values[valueReference]->getValue<int32_t *>();
                            break;
                        case DcpDataType::uint64:
                            value = *values[valueReference]->getValue<int64_t *>();
                            break;
                        default:
                            //only uint datatypes are allowed for structual parameters
                            break;
                    }
                    updateStructualDependencies(valueReference, value);
                } else {
                    checkForUpdatedStructure(parameter.getParameterVr());
                    values[valueReference]->update(parameter.getConfiguration(), 0,
                                                   slavedescription::getDataType(slaveDescription, valueReference));
                }
                break;
            }
            case DcpPduType::CFG_tunable_parameter: {
                DcpPduCfgTunableParameter &paramConfig = static_cast<DcpPduCfgTunableParameter &>(msg);
                configuredParamPos[paramConfig.getParamId()].push_back(paramConfig.getPos());
                paramAssignment[paramConfig.getParamId()][paramConfig.getPos()] = std::make_pair(
                        paramConfig.getParameterVr(),
                        paramConfig.getSourceDataType());
                std::shared_ptr<uint32_t> maxConsecMissedPdus = slavedescription::getVariable(slaveDescription,
                        paramConfig.getParameterVr())->maxConsecMissedPdus;
                if(maxConsecMissedPdus != nullptr){
                    if(maxConsecMissedPduData[paramConfig.getParamId()] == 0 ||
                       *maxConsecMissedPdus <  maxConsecMissedPduData[paramConfig.getParamId()]){
                        maxConsecMissedPduData[paramConfig.getParamId()] = *maxConsecMissedPdus;
                    }
                }
#ifdef DEBUG
                Log(NEW_TUNABLE_CONFIG, paramConfig.getParameterVr(), paramConfig.getSourceDataType(),
                    paramConfig.getParamId());
#endif
                break;

            }
            case DcpPduType::CFG_scope: {
                DcpPduCfgScope &setScope = static_cast<DcpPduCfgScope &>(msg);
                const DcpScope scope = setScope.getScope();
                uint16_t dataId = setScope.getDataId();

                runningScope.erase(std::remove(runningScope.begin(), runningScope.end(), dataId), runningScope.end());
                initializationScope.erase(std::remove(initializationScope.begin(), initializationScope.end(), dataId),
                                          initializationScope.end());


                if (scope == DcpScope::Run_NonRealTime || scope == DcpScope::Initialization_Run_NonRealTime) {
                    runningScope.push_back(dataId);
                }
                if (scope == DcpScope::Initialization ||
                    scope == DcpScope::Initialization_Run_NonRealTime) {
                    initializationScope.push_back(dataId);
                }

                break;

            }
            case DcpPduType::CFG_source_network_information: {
                DcpPduCfgNetworkInformation &netInf = static_cast<DcpPduCfgNetworkInformation &>(msg);
                sourceNetworkConfigured.insert(netInf.getDataId());
                driver.setSourceNetworkInformation(netInf.getDataId(), netInf.getNetworkInformation());
                break;
            }
            case DcpPduType::CFG_target_network_information: {
                DcpPduCfgNetworkInformation &netInf = static_cast<DcpPduCfgNetworkInformation &>(msg);
                targetNetworkConfigured.insert(netInf.getDataId());
                driver.setTargetNetworkInformation(netInf.getDataId(), netInf.getNetworkInformation());
                break;
            }
            case DcpPduType::CFG_param_network_information: {
                DcpPduCfgParamNetworkInformation &netInf = static_cast<DcpPduCfgParamNetworkInformation &>(msg);
                paramNetworkConfigured.insert(netInf.getParamId());
                driver.setParamNetworkInformation(netInf.getParamId(), netInf.getNetworkInformation());
                break;
            }
            case DcpPduType::INF_log: {
#if defined(DEBUG) || defined(LOGGING)
                DcpPduInfLog &log = static_cast<DcpPduInfLog &>(msg);
                size_t currentSize = 0;
                uint8_t numLogs = 0;

                DcpPduRspLogAck logAck = {dcpId, log.getPduSeqId(), logRspBuffer, bufferSize};
                std::vector<Payload> &logs = logBuffer[log.getLogCategory()];
                while (numLogs < log.getLogMaxNum() && logs.size() > 0 &&
                       (logs.front().size + currentSize) < bufferSize) {
                    const Payload &current = logs.front();
                    memcpy(logAck.getPayload() + currentSize, current.payload, current.size);
                    delete[] current.payload;
                    logs.erase(logs.begin());
                    currentSize += current.size;
                    numLogs++;
                }
                logAck.setPduSize(currentSize);
                driver.send(logAck);
#endif
                break;
            }
            case DcpPduType::CFG_logging: {
#if defined(DEBUG) || defined(LOGGING)

                DcpPduCfgLogging &logging = static_cast<DcpPduCfgLogging &>(msg);
                uint8_t categoryStart = 1;
                uint8_t categoryEnd = 255;
                std::map<logCategory_t, std::map<DcpLogLevel, bool>> &map = logOnRequest;
                std::map<logCategory_t, std::map<DcpLogLevel, bool>> &notMap = logOnNotification;
                if (logging.getLogCategory() != 0) {
                    categoryStart = logging.getLogCategory();
                    categoryEnd = logging.getLogCategory();
                }
                if (logging.getLogMode() == DcpLogMode::LOG_ON_NOTIFICATION) {
                    map = logOnNotification;
                    notMap = logOnRequest;
                }
                for (int i = categoryStart; i <= categoryEnd; i++) {
                    map[i][logging.getLogLevel()] = true;
                    notMap[i][logging.getLogLevel()] = false;
                }
#endif
                break;
            }
        }
    }

    /**
     * Stops the slave if possible
     * @return True means slave switched to state STOPPING, false means no action done
     */
    virtual bool stop() = 0;

    /**
     * Go to state ERRORHANDLING immediatly.
     * The master will be informed by NTF_state_changed.
     */
    void gotoErrorHandling() {
        state = DcpState::ERROR_HANDLING;
        notifyStateChange();
    }

    /**
     * Set the error code
     * @param errorCode New error code
     */
    void setError(DcpError errorCode) {
        this->errorCode = errorCode;
    }

    /**
     * Go to state ERRORRESOLVED, if current state is ERRORHANDLING.
     * Indicate that the error is resolved.
     * The master will be informed by NTF_state_changed.
     */
    void gotoErrorResolved() {
        if (state == DcpState::ERROR_HANDLING) {
            state = DcpState::ERROR_RESOLVED;
            notifyStateChange();
        }
    }

    /**
     * Get a pointer to a input
     * @tparam T Data type of the input. Has to be a pointer. E. g. uint16_t* for uint16.
     * @param vr Value reference of the input
     * @return Pointer to value of the corresponding input.
     */
    template<typename T>
    T getInput(uint64_t vr) {
        //toDo check if T belongs to vr
        return values[vr]->getValue<T>();
    }

    /**
     * Get a pointer to an output
     * @tparam T Data type of the output. Has to be a pointer. E. g. uint16_t* for uint16.
     * @param vr Value reference of the output
     * @return Pointer to value of the corresponding output.
     */
    template<typename T>
    T getOutput(uint64_t vr) {
        //toDo check if T belongs to vr
        return values[vr]->getValue<T>();
    }

    /**
     * Get a pointer to a parameter
     * @tparam T Data type of the parameter. Has to be a pointer. E. g. uint16_t* for uint16.
     * @param vr Value reference of the parameter
     * @return Pointer to value of the corresponding parameter.
     */
    template<typename T>
    T getParameter(uint64_t vr) {
        //toDo check if T belongs to vr
        return values[vr]->getValue<T>();
    }


protected:

    const SlaveDescription_t slaveDescription;

    std::map<DcpState, std::map<DcpPduType, bool>> stateChangePossible;

    uint16_t seqAtRegister;

    DcpState state;
    DcpState realtimeState;
    DcpState runLastExitPoint;
    DcpError errorCode;

    // Acu-D constants
    DcpOpMode opMode;

    uint32_t numerator;
    uint32_t denominator;
    bool timeResolutionSet = false;
    bool timeResolutionFix = false;

    uint32_t fixNrtStep = 0;

    uint32_t bufferSize = 900;

#if defined(DEBUG) || defined(LOGGING)
    /*Logging*/
    std::map<logCategory_t, std::map<DcpLogLevel, bool>> logOnNotification;
    std::map<logCategory_t, std::map<DcpLogLevel, bool>> logOnRequest;

    std::map<logCategory_t, std::vector<Payload>> logBuffer;

    uint8_t *logRspBuffer = new uint8_t[bufferSize];
#endif
    /*Data Handling*/
    std::map<valueReference_t, MultiDimValue *> values;

    std::set<dataId_t> sourceNetworkConfigured;
    std::set<dataId_t> targetNetworkConfigured;
    std::set<paramId_t> paramNetworkConfigured;

    //Outputs
    std::map<dataId_t, DcpPduDatInputOutput *> outputBuffer;
    std::map<dataId_t, std::map<pos_t, valueReference_t>> outputAssignment;
    std::map<dataId_t, std::vector<pos_t>> configuredOutPos;

    std::vector<dataId_t> runningScope;
    std::vector<dataId_t> initializationScope;


    std::map<uint16_t, uint32_t> steps;
    //<List of data_ids, logBufferCurrent step counter, set steps>
    std::vector<std::tuple<std::vector<dataId_t>, steps_t, steps_t>> outputCounter;

    //Inputs
    std::map<dataId_t, std::map<pos_t, std::pair<valueReference_t, DcpDataType>>> inputAssignment;
    std::map<dataId_t, std::vector<pos_t>> configuredInPos;

    std::map<dataId_t, uint16_t> maxConsecMissedPduData;

    //Parameter
    std::map<paramId_t, std::vector<pos_t>> configuredParamPos;
    std::map<paramId_t, std::map<pos_t, std::pair<valueReference_t, DcpDataType>>> paramAssignment;

    std::map<dataId_t, uint16_t> maxConsecMissedPduParam;


    //which sttructual parameter (valueReference) change which inputs/outputs/parameters (valueReference)
    std::map<valueReference_t, std::vector<std::pair<valueReference_t, size_t>>> structualDependencies;
    std::map<valueReference_t, MultiDimValue *> updatedStructure;


#if defined(DEBUG) || defined(LOGGING)

    uint8_t id = 162;

#endif


    AbstractDcpManagerSlave(const SlaveDescription_t _slaveDescription) : slaveDescription(_slaveDescription) {
        this->errorCode = DcpError::NONE;
        this->state = DcpState::ALIVE;
        this->masterId = 0;
        this->opMode = DcpOpMode::HRT;

        this->seqAtRegister = 0;
        this->numerator = 0;
        this->denominator = 0;

        //define possible state changes
        stateChangePossible[DcpState::ALIVE][DcpPduType::STC_register] = true;
        stateChangePossible[DcpState::ALIVE][DcpPduType::INF_state] = true;

        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::STC_deregister] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::STC_prepare] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_steps] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_time_res] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_input] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_output] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_clear] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_target_network_information] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_source_network_information] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_tunable_parameter] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_parameter] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_param_network_information] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_logging] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_scope] = true;

        stateChangePossible[DcpState::PREPARING][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::PREPARING][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::PREPARING][DcpPduType::INF_log] = true;

        stateChangePossible[DcpState::PREPARED][DcpPduType::STC_configure] = true;
        stateChangePossible[DcpState::PREPARED][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::PREPARED][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::PREPARED][DcpPduType::INF_log] = true;

        stateChangePossible[DcpState::CONFIGURING][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::CONFIGURING][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::CONFIGURING][DcpPduType::INF_log] = true;

        stateChangePossible[DcpState::CONFIGURED][DcpPduType::STC_initialize] = true;
        stateChangePossible[DcpState::CONFIGURED][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::CONFIGURED][DcpPduType::STC_run] = true;
        stateChangePossible[DcpState::CONFIGURED][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::CONFIGURED][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::CONFIGURED][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::CONFIGURED][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::INITIALIZING][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::INITIALIZING][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::INITIALIZING][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::INITIALIZING][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::INITIALIZING][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::INITIALIZED][DcpPduType::STC_send_outputs] = true;
        stateChangePossible[DcpState::INITIALIZED][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::INITIALIZED][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::INITIALIZED][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::INITIALIZED][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::INITIALIZED][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::SENDING_I][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::SENDING_I][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::SENDING_I][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::SENDING_I][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::SENDING_I][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::SYNCHRONIZING][DcpPduType::STC_do_step] = true;
        stateChangePossible[DcpState::SYNCHRONIZING][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::SYNCHRONIZING][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::SYNCHRONIZING][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::SYNCHRONIZING][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::SYNCHRONIZING][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::SYNCHRONIZED][DcpPduType::STC_run] = true;
        stateChangePossible[DcpState::SYNCHRONIZED][DcpPduType::STC_do_step] = true;
        stateChangePossible[DcpState::SYNCHRONIZED][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::SYNCHRONIZED][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::SYNCHRONIZED][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::SYNCHRONIZED][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::SYNCHRONIZED][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::RUNNING][DcpPduType::STC_do_step] = true;
        stateChangePossible[DcpState::RUNNING][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::RUNNING][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::RUNNING][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::RUNNING][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::RUNNING][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::COMPUTING][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::COMPUTING][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::COMPUTING][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::COMPUTING][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::COMPUTING][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::COMPUTED][DcpPduType::STC_send_outputs] = true;
        stateChangePossible[DcpState::COMPUTED][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::COMPUTED][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::COMPUTED][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::COMPUTED][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::COMPUTED][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::SENDING_D][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::SENDING_D][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::SENDING_D][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::SENDING_D][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::STOPPING][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::STOPPING][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::STOPPING][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::STOPPING][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::STOPPED][DcpPduType::STC_deregister] = true;
        stateChangePossible[DcpState::STOPPED][DcpPduType::STC_reset] = true;
        stateChangePossible[DcpState::STOPPED][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::STOPPED][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::STOPPED][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::STOPPED][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::ERROR_HANDLING][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::ERROR_HANDLING][DcpPduType::INF_error] = true;
        stateChangePossible[DcpState::ERROR_HANDLING][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::ERROR_HANDLING][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::ERROR_HANDLING][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::ERROR_RESOLVED][DcpPduType::STC_reset] = true;
        stateChangePossible[DcpState::ERROR_RESOLVED][DcpPduType::STC_deregister] = true;
        stateChangePossible[DcpState::ERROR_RESOLVED][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::ERROR_RESOLVED][DcpPduType::INF_error] = true;
        stateChangePossible[DcpState::ERROR_RESOLVED][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::ERROR_RESOLVED][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::ERROR_RESOLVED][DcpPduType::DAT_parameter] = true;

        for (auto const &timeResolution : slaveDescription.TimeRes.resolutions) {
            if (timeResolution.fixed) {
                numerator = timeResolution.numerator;
                denominator = timeResolution.denominator;
                this->timeResolutionSet = true;
                //this->timeResListener(timeResolution.numerator(), timeResolution.denominator());
                this->timeResolutionFix = true;
                break;
            } else if (timeResolution.recommended) {
                numerator = timeResolution.numerator;
                denominator = timeResolution.denominator;
                this->timeResolutionSet = true;
                //this->timeResListener(timeResolution.numerator(), timeResolution.denominator());
                break;
            }
        }

        for (auto const &var: slaveDescription.Variables) {
            if (var.StructuralParameter.get() != nullptr) {
                const valueReference_t valueReference = var.valueReference;
                const DcpDataType dataType = slavedescription::getDataType(slaveDescription, valueReference);
                size_t baseSize = 0;
                switch (dataType) {
                    case DcpDataType::binary:
                    case DcpDataType::string:
                        baseSize = 1024;
                        break;
                    default:
                        baseSize = getDcpDataTypeSize(dataType);
                        break;
                }
                std::vector<size_t> dimensions;
                dimensions.push_back(1);
                values[valueReference] = new MultiDimValue(dataType, baseSize, dimensions);
                switch (dataType) {
                    case DcpDataType::uint8: {
                        if (var.StructuralParameter.get()->Uint8.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint8.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint16: {
                        if (var.StructuralParameter.get()->Uint16.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint16.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint32: {
                        if (var.StructuralParameter.get()->Uint32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint64: {
                        if (var.StructuralParameter.get()->Uint64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    default:
                        //only uint datatypes are allowed for structual parameters
                        break;
                }
            }
        }

        for (auto const &var: slaveDescription.Variables) {
            const valueReference_t &valueReference = var.valueReference;
            const DcpDataType dataType = slavedescription::getDataType(slaveDescription, valueReference);
            size_t baseSize = 0;


            if (var.Input != nullptr) {

                switch (dataType) {
                    case DcpDataType::binary:
                        if (var.Input.get()->Binary.get()->maxSize != nullptr) {
                            baseSize = *var.Input.get()->Binary.get()->maxSize;
                        } else {
                            baseSize = 4294967295 + 4;
                        }
                        break;
                    case DcpDataType::string:
                        if (var.Input.get()->String.get()->maxSize != nullptr) {
                            baseSize = *var.Input.get()->String.get()->maxSize;
                        } else {
                            baseSize = 4294967295 + 4;
                        }
                        break;
                    default:
                        baseSize = getDcpDataTypeSize(dataType);
                        break;
                }

                std::vector<size_t> dimensions;
                size_t i = 0;
                if (var.Input->dimensions.size() > 0) {
                    for (auto const &dim: var.Input->dimensions) {
                        size_t dimVal;
                        if (dim.type == DimensionType::CONSTANT) {
                            dimVal = dim.value;
                        } else if (dim.type == DimensionType::LINKED_VR) {
                            size_t linkedVr = dim.value;
                            MultiDimValue *mdv = values[linkedVr];
                            DcpDataType dataType = mdv->getDataType();
                            switch (dataType) {
                                case DcpDataType::uint8:
                                    dimVal = *mdv->getValue<uint8_t *>();
                                    break;
                                case DcpDataType::uint16:
                                    dimVal = *mdv->getValue<uint16_t *>();
                                    break;
                                case DcpDataType::uint32:
                                    dimVal = *mdv->getValue<uint32_t *>();
                                    break;
                                case DcpDataType::uint64:
                                    dimVal = *mdv->getValue<uint64_t *>();
                                    break;
                                default:
                                    //only uint datatypes are allowed for dimensions
                                    continue;
                                    break;
                            }
                            structualDependencies[linkedVr].push_back(
                                    std::pair<uint64_t, size_t>((uint64_t) valueReference, i));
                        }
                        dimensions.push_back(dimVal);
                        i++;
                    }
                } else {
                    dimensions.push_back(1);
                }

                values[valueReference] = new MultiDimValue(dataType, baseSize, dimensions);

                switch (dataType) {
                    case DcpDataType::int8: {
                        if (var.Input.get()->Int8.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Int8.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int16: {
                        if (var.Input.get()->Int16.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Int16.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int32: {
                        if (var.Input.get()->Int32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Int32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int64: {
                        if (var.Input.get()->Int64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Int64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint8: {
                        if (var.Input.get()->Uint8.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Uint8.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint16: {
                        if (var.Input.get()->Uint16.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Uint16.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint32: {
                        if (var.Input.get()->Uint32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Uint32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint64: {
                        if (var.Input.get()->Uint64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Uint64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::float32: {
                        if (var.Input.get()->Float32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Float32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::float64: {
                        if (var.Input.get()->Float64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Float64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::string: {
                        if (var.Input.get()->String.get()->start.get() != nullptr) {
                            std::shared_ptr<std::string> startValue = var.Input.get()->String.get()->start;
                            DcpString startString(baseSize - 4);
                            startString.setString(*startValue);
                            values[valueReference]->update((uint8_t *) startString.getPayload(), 0, DcpDataType::string);
                        }
                        break;
                    }
                    case DcpDataType::binary: {
                        if (var.Input.get()->Binary.get()->start.get() != nullptr) {
                            std::shared_ptr<BinaryStartValue> startValue = var.Input.get()->Binary.get()->start;
                            DcpBinary startBinary(startValue->length, startValue->value, baseSize - 4);
                            values[valueReference]->update(startBinary.getBinary(), 0, DcpDataType::binary);
                        }
                        break;
                    }
                }
            }

            if (var.Output != nullptr) {

                switch (dataType) {
                    case DcpDataType::binary:
                        if (var.Output.get()->Binary.get()->maxSize != nullptr) {
                            baseSize = *var.Output.get()->Binary.get()->maxSize;
                        } else {
                            baseSize = 4294967295 + 4;
                        }
                        break;
                    case DcpDataType::string:
                        if (var.Output.get()->String.get()->maxSize != nullptr) {
                            baseSize = *var.Output.get()->String.get()->maxSize;
                        } else {
                            baseSize = 4294967295 + 4;
                        }
                        break;
                    default:
                        baseSize = getDcpDataTypeSize(dataType);
                        break;
                }

                std::vector<size_t> dimensions;
                size_t i = 0;
                if (var.Output->dimensions.size() > 0) {
                    for (auto const &dim: var.Output->dimensions) {
                        size_t dimVal;
                        if (dim.type == DimensionType::CONSTANT) {
                            dimVal = dim.value;
                        } else if (dim.type == DimensionType::LINKED_VR) {
                            size_t linkedVr = dim.value;
                            MultiDimValue *mdv = values[linkedVr];
                            DcpDataType dataType = mdv->getDataType();
                            switch (dataType) {
                                case DcpDataType::uint8:
                                    dimVal = *mdv->getValue<uint8_t *>();
                                    break;
                                case DcpDataType::uint16:
                                    dimVal = *mdv->getValue<uint16_t *>();
                                    break;
                                case DcpDataType::uint32:
                                    dimVal = *mdv->getValue<uint32_t *>();
                                    break;
                                case DcpDataType::uint64:
                                    dimVal = *mdv->getValue<uint64_t *>();
                                    break;
                                default:
                                    //only uint datatypes are allowed for dimensions
                                    continue;
                                    break;
                            }
                            structualDependencies[linkedVr].push_back(
                                    std::pair<uint64_t, size_t>((uint64_t) valueReference, i));
                        }
                        dimensions.push_back(dimVal);
                        i++;
                    }
                } else {
                    dimensions.push_back(1);
                }
                values[valueReference] = new MultiDimValue(dataType, baseSize, dimensions);
                switch (dataType) {
                    case DcpDataType::int8: {
                        if (var.Output.get()->Int8.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Int8.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int16: {
                        if (var.Output.get()->Int16.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Int16.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int32: {
                        if (var.Output.get()->Int32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Int32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int64: {
                        if (var.Output.get()->Int64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Int64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint8: {
                        if (var.Output.get()->Uint8.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint8.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }

                        }
                        break;
                    }
                    case DcpDataType::uint16: {
                        if (var.Output.get()->Uint16.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint16.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint32: {
                        if (var.Output.get()->Uint32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint64: {
                        if (var.Output.get()->Uint64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::float32: {
                        if (var.Output.get()->Float32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Float32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::float64: {
                        if (var.Output.get()->Float64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Float64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::string: {
                        if (var.Output.get()->String.get()->start.get() != nullptr) {
                            std::shared_ptr<std::string> startValue = var.Output.get()->String.get()->start;
                            DcpString startString(baseSize - 4);
                            startString.setString(*startValue);
                            values[valueReference]->update((uint8_t *) startString.getPayload(), 0, DcpDataType::string);
                        }
                        break;
                    }
                    case DcpDataType::binary: {
                        if (var.Output.get()->Binary.get()->start.get() != nullptr) {
                            std::shared_ptr<BinaryStartValue> startValue = var.Output.get()->Binary.get()->start;
                            DcpBinary startBinary(startValue->length, startValue->value, baseSize - 4);
                            values[valueReference]->update(startBinary.getBinary(), 0, DcpDataType::binary);
                        }
                        break;
                    }
                }
            }

            if (var.Parameter != nullptr) {

                switch (dataType) {
                    case DcpDataType::binary:
                        if (var.Parameter.get()->Binary.get()->maxSize != nullptr) {
                            baseSize = *var.Parameter.get()->Binary.get()->maxSize;
                        } else {
                            baseSize = 4294967295 + 4;
                        }

                    case DcpDataType::string:
                        if (var.Parameter.get()->String.get()->maxSize != nullptr) {
                            baseSize = *var.Parameter.get()->String.get()->maxSize;
                        } else {
                            baseSize = 4294967295 + 4;
                        }
                        break;
                    default:
                        baseSize = getDcpDataTypeSize(dataType);
                        break;
                }

                std::vector<size_t> dimensions;
                size_t i = 0;
                if (var.Parameter->dimensions.size() > 0) {
                    for (auto const &dim: var.Parameter->dimensions) {
                        size_t dimVal;
                        if (dim.type == DimensionType::CONSTANT) {
                            dimVal = dim.value;
                        } else if (dim.type == DimensionType::LINKED_VR) {
                            size_t linkedVr = dim.value;
                            MultiDimValue *mdv = values[linkedVr];
                            DcpDataType dataType = mdv->getDataType();
                            switch (dataType) {
                                case DcpDataType::uint8:
                                    dimVal = *mdv->getValue<uint8_t *>();
                                    break;
                                case DcpDataType::uint16:
                                    dimVal = *mdv->getValue<uint16_t *>();
                                    break;
                                case DcpDataType::uint32:
                                    dimVal = *mdv->getValue<uint32_t *>();
                                    break;
                                case DcpDataType::uint64:
                                    dimVal = *mdv->getValue<uint64_t *>();
                                    break;
                                default:
                                    //only uint datatypes are allowed for dimensions
                                    continue;
                                    break;
                            }
                            structualDependencies[linkedVr].push_back(
                                    std::pair<uint64_t, size_t>((uint64_t) valueReference, i));
                        }
                        dimensions.push_back(dimVal);
                        i++;
                    }
                } else {
                    dimensions.push_back(1);
                }
                values[valueReference] = new MultiDimValue(dataType, baseSize, dimensions);
                switch (dataType) {
                    case DcpDataType::int8: {
                        if (var.Parameter.get()->Int8.get()->start.get() != nullptr) {
                            auto &startValues = *var.Parameter.get()->Int8.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int16: {
                        if (var.Parameter.get()->Int16.get()->start.get() != nullptr) {
                            auto &startValues = *var.Parameter.get()->Int16.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int32: {
                        if (var.Parameter.get()->Int32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Parameter.get()->Int32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int64: {
                        if (var.Parameter.get()->Int64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Parameter.get()->Int64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint8: {
                        if (var.Parameter.get()->Uint8.get()->start.get() != nullptr) {
                            auto &startValues = *var.Parameter.get()->Uint8.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint16: {
                        if (var.Parameter.get()->Uint16.get()->start.get() != nullptr) {
                            auto &startValues = *var.Parameter.get()->Uint16.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint32: {
                        if (var.Parameter.get()->Uint32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Parameter.get()->Uint32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint64: {
                        if (var.Parameter.get()->Uint64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Parameter.get()->Uint64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::float32: {
                        if (var.Parameter.get()->Float32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Parameter.get()->Float32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::float64: {
                        if (var.Parameter.get()->Float64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Parameter.get()->Float64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, dataType, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::string: {
                        if (var.Parameter.get()->String.get()->start.get() != nullptr) {
                            std::shared_ptr<std::string> startValue = var.Parameter.get()->String.get()->start;
                            DcpString startString(baseSize - 4);
                            startString.setString(*startValue);
                            values[valueReference]->update((uint8_t *) startString.getPayload(), 0, DcpDataType::string);
                        }
                        break;
                    }
                    case DcpDataType::binary: {
                        if (var.Parameter.get()->Binary.get()->start.get() != nullptr) {
                            std::shared_ptr<BinaryStartValue> startValue = var.Parameter.get()->Binary.get()->start;
                            DcpBinary startBinary(startValue->length, startValue->value, baseSize - 4);
                            values[valueReference]->update(startBinary.getBinary(), 0, DcpDataType::binary);
                        }
                        break;
                    }
                }
            }

        }
    }

    void notifyStateChange() {
        DcpPduNtfStateChanged notification = {dcpId, state};
        driver.send(notification);
#ifdef DEBUG
        Log(STATE_CHANGED, state);
#endif
        notifyStateChangedListener();
    }

    void setTimeRes(uint32_t numerator,
                    uint32_t denominator) {
        this->numerator = numerator;
        this->denominator = denominator;
#ifdef DEBUG
        Log(TIME_RES_SET, numerator, denominator);
#endif
        timeResolutionSet = true;
        notifyTimeResListener();
    }

    void setSteps(uint16_t dataId, uint32_t steps) {
        this->steps[dataId] = steps;
#ifdef DEBUG
        Log(STEP_SET, dataId, steps);
#endif
        notifyStepsListener(dataId, steps);
    }

    void setOperationInformation(uint8_t dcpId, DcpOpMode opMode) {
        this->dcpId = dcpId;
#if defined(DEBUG)
        Log(DCP_ID_SET, dcpId);
#endif
        this->opMode = opMode;
#if defined(DEBUG)
        Log(OP_MODE_SET, opMode);
#endif
        notifyOperationInformationListener();
    }

    void ack(uint16_t respSeqId) {
        DcpPduRspAck ack = {dcpId, respSeqId};
        driver.send(ack);
    }

    void nack(uint16_t respSeqId, DcpError errorCode) {
        const uint16_t expSeqId = segNumsIn[masterId] + 1;
        DcpPduRspNack nack = {DcpPduType::RSP_nack, dcpId, respSeqId, expSeqId, errorCode};
        driver.send(nack);
    }

    bool checkForError(DcpPdu &msg) {
        bool valid = true;
        DcpError error = DcpError::NONE;
        uint8_t type = (uint8_t) msg.getTypeId();
        if (!(
                // STC_*
                (type >= 0x01 && type <= 0x0A) ||
                // CFG_*
                (type >= 0x20 && type <= 0x2B) ||
                //INF_*
                (type >= 0x80 && type <= 0x82) ||
                //DAT_*
                (type >= 0xF0 && type <= 0xF1)
        )) {
#if defined(DEBUG) || defined(LOGGING)
            Log(INVALID_TYPE_ID, type);
#endif
            return false;
        }

        //check Receiver
        switch (msg.getTypeId()) {
            case DcpPduType::DAT_input_output: {
                //toDo distinguish scopes
                DcpPduDatInputOutput &aciPduData = static_cast<DcpPduDatInputOutput &>(msg);
                if (inputAssignment.count(aciPduData.getDataId()) == 0) {
#if defined(DEBUG) || defined(LOGGING)
                    Log(UNKNOWN_DATA_ID, aciPduData.getDataId());
#endif
                    return false;
                }
                break;
            }
            case DcpPduType::DAT_parameter: {
                //toDo distinguish scopes
                DcpPduDatParameter &paramPdu = static_cast<DcpPduDatParameter &>(msg);
                if (paramAssignment.count(paramPdu.getParamId()) == 0) {
#if defined(DEBUG) || defined(LOGGING)
                    Log(UNKNOWN_PARAM_ID, paramPdu.getParamId());
#endif
                    return false;
                }
                break;
            }
            default: {
                DcpPduBasic &basicPdu = static_cast<DcpPduBasic &>(msg);
                if (state != DcpState::ALIVE && dcpId != basicPdu.getReceiver()) {
#if defined(DEBUG) || defined(LOGGING)
                    Log(INVALID_RECEIVER, basicPdu.getReceiver());
#endif
                    return false;
                }
                break;
            }
        }

        //check sequence id
        switch (msg.getTypeId()) {
            case DcpPduType::DAT_input_output: {
                DcpPduDatInputOutput &dcpPduDatInputOutput = static_cast<DcpPduDatInputOutput &>(msg);
                uint16_t diff = checkSeqIdInOut(dcpPduDatInputOutput.getDataId(), dcpPduDatInputOutput.getPduSeqId());
                if (diff != 1) {
                    notifyMissingInputOutputPduListener(dcpPduDatInputOutput.getDataId());
                    Log(IN_OUT_PDU_MISSED);
                }
                if(maxConsecMissedPduData[dcpPduDatInputOutput.getDataId()] > 0 && maxConsecMissedPduData[dcpPduDatInputOutput.getDataId()] < diff){
                    gotoErrorHandling();
                    gotoErrorResolved();
                    return false;
                }
                break;

            }
            case DcpPduType::DAT_parameter: {
                DcpPduDatParameter dcpPduDatParameter = static_cast<DcpPduDatParameter &>(msg);
                uint16_t diff = checkSeqIdParam(dcpPduDatParameter.getParamId(), dcpPduDatParameter.getPduSeqId());
                if (diff != 1) {
                    notifyMissingParameterPduListener(dcpPduDatParameter.getParamId());
                    Log(PARAM_PDU_MISSED);
                }
                if(maxConsecMissedPduParam[dcpPduDatParameter.getParamId()] > 0 && maxConsecMissedPduParam[dcpPduDatParameter.getParamId()] < diff){
                    gotoErrorHandling();
                    gotoErrorResolved();
                    return false;
                }
                break;
            }
            default: {
                if (state != DcpState::ALIVE) {
                    DcpPduBasic &basicPdu = static_cast<DcpPduBasic &>(msg);
                    uint16_t diff = checkSeqId(masterId, basicPdu.getPduSeqId());
                    if (diff != 1) {
                        error = DcpError::INVALID_SEQUENCE_ID;
                        notifyMissingControlPduListener();
                        Log(CTRL_PDU_MISSED);
                    }
                }
                break;
            }
        }

        if(error == DcpError::NONE){
            //check support
            if (opMode != DcpOpMode::NRT) {
                switch (msg.getTypeId()) {
                    case DcpPduType::STC_do_step:
#if defined(DEBUG) || defined(LOGGING)
                        Log(ONLY_NRT, opMode);
#endif
                        error = DcpError::NOT_SUPPORTED_PDU;
                }
            }
        }

        if (error == DcpError::NONE) {
            //check logging
            switch (msg.getTypeId()) {
                case DcpPduType::CFG_logging:
                case DcpPduType::INF_log:
                    if (slaveDescription.Log == nullptr ||
                        (!slaveDescription.CapabilityFlags.canProvideLogOnNotification &&
                         !slaveDescription.CapabilityFlags.canProvideLogOnRequest)) {
                        error = DcpError::NOT_SUPPORTED_PDU;
                    }
            }

        }


        if (error == DcpError::NONE) {
            //check length
            switch (msg.getTypeId()) {
                case DcpPduType::DAT_input_output: {
                    DcpPduDatInputOutput &aciPduData = static_cast<DcpPduDatInputOutput &>(msg);
                    size_t correctLength = 0;
                    for (auto &pos: inputAssignment[aciPduData.getDataId()]) {
                        switch (pos.second.second) {
                            case DcpDataType::binary:
                            case DcpDataType::string:
                                correctLength += *((uint16_t *) (aciPduData.getPayload() + correctLength)) + 4;
                                break;
                            default:
                                correctLength += getDcpDataTypeSize(pos.second.second);
                        }
                    }
                    if (aciPduData.getPduSize() != (correctLength + 5)) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_LENGTH, (uint16_t) aciPduData.getPduSize(), (uint16_t) (correctLength + 5));
#endif
                        return false;

                    }
                    break;
                }
                case DcpPduType::DAT_parameter: {
                    DcpPduDatParameter &aciPduParam = static_cast<DcpPduDatParameter &>(msg);
                    size_t correctLength = 0;
                    for (auto &pos: paramAssignment[aciPduParam.getParamId()]) {
                        switch (pos.second.second) {
                            case DcpDataType::binary:
                            case DcpDataType::string:
                                correctLength += *((uint16_t *) (aciPduParam.getConfiguration()));
                                break;
                            default:
                                correctLength += getDcpDataTypeSize(pos.second.second);
                                break;
                        }
                    }
                    if (aciPduParam.getPduSize() != (correctLength + 5)) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_LENGTH, (uint16_t) aciPduParam.getPduSize(),
                            (uint16_t) (correctLength + 5));
#endif
                        return false;
                    }
                    break;
                }
                case DcpPduType::CFG_parameter: {
                    DcpPduCfgParameter setParameter = static_cast<DcpPduCfgParameter &>(msg);
                    size_t correctLength = 0;
                    switch (setParameter.getSourceDataType()) {
                        case DcpDataType::binary:
                        case DcpDataType::string:
                            correctLength = *((uint16_t *) (setParameter.getConfiguration() + correctLength));
                            break;
                        default:
                            correctLength = getDcpDataTypeSize(setParameter.getSourceDataType());
                    }
                    if (setParameter.getPduSize() != (correctLength + 13)) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_LENGTH, (uint16_t) setParameter.getPduSize(),
                            (uint16_t) (correctLength + 13));
#endif
                        error = DcpError::INVALID_LENGTH;
                    }
                    break;
                }
                default: {
                    if (!msg.isSizeCorrect()) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_LENGTH, (uint16_t) msg.getPduSize(),
                            (uint16_t) msg.getCorrectSize());
#endif
                        error = DcpError::INVALID_LENGTH;
                    }
                    break;
                }
            }
        }

        //check state
        if (error == DcpError::NONE) {
            switch (msg.getTypeId()) {
                case DcpPduType::DAT_input_output:
                case DcpPduType::DAT_parameter: {
                    if (!stateChangePossible[state][msg.getTypeId()]) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(DATA_NOT_ALLOWED, state);
#endif
                        return false;
                    } else {
                        return true;
                    }
                    break;
                }
                default:
                    if (!stateChangePossible[state][msg.getTypeId()]) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(MSG_NOT_ALLOWED, msg.getTypeId(), state);
#endif
                        error = DcpError::PROTOCOL_ERROR_PDU_NOT_ALLOWED_IN_THIS_STATE;
                    }
                    break;
            }

        }

        //check state_id
        if (error == DcpError::NONE) {

            switch (msg.getTypeId()) {
                case DcpPduType::STC_register: {
                    DcpPduStcRegister registerPdu = static_cast<DcpPduStcRegister &>(msg);
                    if (registerPdu.getStateId() != state) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_STATE_ID, registerPdu.getStateId(), state);
#endif
                        const uint16_t exp_seq_id = registerPdu.getPduSeqId() + 1;
                        DcpPduRspNack nack = {DcpPduType::RSP_nack, registerPdu.getReceiver(), registerPdu.getPduSeqId(),
                                           exp_seq_id, DcpError::INVALID_STATE_ID};
                        driver.send(nack);
                        return false;
                    }
                    break;
                }
                case DcpPduType::STC_deregister:
                case DcpPduType::STC_prepare:
                case DcpPduType::STC_configure:
                case DcpPduType::STC_initialize:
                case DcpPduType::STC_run:
                case DcpPduType::STC_do_step:
                case DcpPduType::STC_send_outputs:
                case DcpPduType::STC_stop:
                case DcpPduType::STC_reset: {
                    DcpPduStc &bst = static_cast<DcpPduStc &>(msg);
                    if (bst.getStateId() != state) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_STATE_ID, bst.getStateId(), state);
#endif
                        error = DcpError::INVALID_STATE_ID;
                    }
                    break;
                }
            }
        }

        if (error == DcpError::NONE) {
            switch (msg.getTypeId()) {
                case DcpPduType::INF_state:
                case DcpPduType::INF_error:
                    return true;
            }

        }

        //check other pdu semantics
        if (error == DcpError::NONE) {
            switch (msg.getTypeId()) {
                case DcpPduType::STC_register: {
                    DcpPduStcRegister registerPdu = static_cast<DcpPduStcRegister &>(msg);

                    uint128_t uuidFromACUD = convertToUUID(slaveDescription.uuid);
                    uint128_t &uuidFromPDU = registerPdu.getSlaveUuid();
                    if (uuidFromACUD != uuidFromPDU) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_UUID, std::string(slaveDescription.uuid),
                            convertToUUIDStr(uuidFromPDU));
#endif
                        error = DcpError::INVALID_UUID;
                    } else if (!slavedescription::isOpModeSupported(slaveDescription, registerPdu.getOpMode())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_OP_MODE, registerPdu.getOpMode());
#endif
                        error = DcpError::INVALID_OP_MODE;
                    } else if (registerPdu.getMajorVersion() != slaveDescription.dcpMajorVersion) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_MAJOR_VERSION, registerPdu.getMajorVersion(),
                            slaveDescription.dcpMajorVersion, slaveDescription.dcpMinorVersion);
#endif
                        error = DcpError::INVALID_MAJOR_VERSION;
                    } else if (registerPdu.getMinorVersion() != slaveDescription.dcpMinorVersion) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_MINOR_VERSION, registerPdu.getMinorVersion(),
                            slaveDescription.dcpMajorVersion, slaveDescription.dcpMinorVersion);
#endif
                        error = DcpError::INVALID_MINOR_VERSION;
                    }

                    if (error == DcpError::NONE) {
                        DcpPduRspAck ack = {registerPdu.getReceiver(), registerPdu.getPduSeqId()};
                        driver.send(ack);
                        return true;
                    } else {
                        std::cout << registerPdu.getReceiver() << std::endl;
                        const uint16_t exp_seq_id = registerPdu.getPduSeqId() + 1;
                        DcpPduRspNack nack = {DcpPduType::RSP_nack, registerPdu.getReceiver(), registerPdu.getPduSeqId(),
                                              exp_seq_id, error};
                        driver.send(nack);
                        return false;
                    }
                    break;
                }
                case DcpPduType::STC_prepare: {
                    for (auto &in : configuredInPos) {
                        uint16_t maxInPos = *std::max_element(std::begin(in.second), std::end(in.second));
                        if (maxInPos >= in.second.size()) {

                            std::string missingInPosition = "";
                            for (int i = 0; i < maxInPos; ++i) {
                                if (std::find(in.second.begin(), in.second.end(), i) == in.second.end()) {
                                    missingInPosition += std::to_string(i) + ", ";
                                }
                            }
                            missingInPosition = missingInPosition.substr(0, missingInPosition.size() - 2);
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIGURATION_GAP_INPUT_POS, missingInPosition, in.first,
                                maxInPos);
#endif
                            error = DcpError::INCOMPLETE_CONFIG_GAP_INPUT_POS;
                        }
                    }
                    for (auto &out : configuredOutPos) {
                        uint16_t maxOutPos = *std::max_element(std::begin(out.second), std::end(out.second));
                        if (maxOutPos >= out.second.size()) {

                            std::string missingOutPosition = "";
                            for (int i = 0; i < maxOutPos; ++i) {
                                if (std::find(out.second.begin(), out.second.end(), i) == out.second.end()) {
                                    missingOutPosition += std::to_string(i) + ", ";
                                }
                            }
                            missingOutPosition = missingOutPosition.substr(0, missingOutPosition.size() - 2);
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIGURATION_GAP_OUTPUT_POS, missingOutPosition, out.first,
                                maxOutPos);
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INCOMPLETE_CONFIG_GAP_OUTPUT_POS;
                            }
                        }
                    }
                    for (auto &param : configuredParamPos) {
                        uint16_t maxParamPos = *std::max_element(std::begin(param.second), std::end(param.second));
                        if (maxParamPos >= param.second.size()) {

                            std::string missingParamPosition = "";
                            for (int i = 0; i < maxParamPos; ++i) {
                                if (std::find(param.second.begin(), param.second.end(), i) == param.second.end()) {
                                    missingParamPosition += std::to_string(i) + ", ";
                                }
                            }
                            missingParamPosition = missingParamPosition.substr(0, missingParamPosition.size() - 2);
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIGURATION_GAP_PARAM_POS, missingParamPosition, param.first,
                                maxParamPos);
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INCOMPLETE_CONFIG_GAP_TUNABLE_POS;
                            }
                        }
                    }

                    for (const auto &inputAss: inputAssignment) {
                        if (sourceNetworkConfigured.count(inputAss.first) == 0) {
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIG_NW_INFO_INPUT, inputAss.first);
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INCOMPLETE_CONFIG_NW_INFO_INPUT;
                            }
                        }
                    }

                    for (const auto &outputAss: outputAssignment) {
                        if (targetNetworkConfigured.count(outputAss.first) == 0) {
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIG_NW_INFO_OUTPUT, outputAss.first);
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INCOMPLETE_CONFIG_NW_INFO_OUTPUT;
                            }
                        }
                    }

                    for (const auto &paramAss: paramAssignment) {
                        if (paramNetworkConfigured.count(paramAss.first) == 0) {
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIG_NW_INFO_TUNABLE, paramAss.first);
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INCOMPLETE_CONFIG_NW_INFO_TUNABLE;
                            }
                        }
                    }

                    for (auto &entr : outputAssignment) {
                        if (steps.count(entr.first) == 0) {
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIGURATION_STEPS, entr.first);
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INCOMPLETE_CONFIG_STEPS;
                            }
                        }
                    }

                    if (!timeResolutionSet) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INCOMPLETE_CONFIGURATION_TIME_RESOLUTION);
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::INCOMPLETE_CONFIG_TIME_RESOLUTION;
                        }
                    }

                    for (const auto &inputAss: inputAssignment) {

                        if (std::find(runningScope.begin(), runningScope.end(), inputAss.first) == runningScope.end() &&
                            std::find(initializationScope.begin(), initializationScope.end(), inputAss.first) ==
                            initializationScope.end()) {
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIG_SCOPE, inputAss.first);
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INCOMPLETE_CONFIG_SCOPE;
                            }
                        }
                    }

                    for (const auto &outputAss: outputAssignment) {

                        if (std::find(runningScope.begin(), runningScope.end(), outputAss.first) ==
                            runningScope.end() &&
                            std::find(initializationScope.begin(), initializationScope.end(), outputAss.first) ==
                            initializationScope.end()) {
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIG_SCOPE, outputAss.first);
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INCOMPLETE_CONFIG_SCOPE;
                            }
                        }
                    }

                    break;
                }
                case DcpPduType::STC_run: {
                    DcpPduStcRun &runPDU = static_cast<DcpPduStcRun &>(msg);
                    int64_t time_since_epoch = std::chrono::seconds(std::time(NULL)).count();
                    if (opMode != DcpOpMode::NRT
                        && (runPDU.getStartTime() > 0
                            && runPDU.getStartTime()
                               < time_since_epoch)) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_START_TIME, convertUnixTimestamp(runPDU.getStartTime()), currentTime());
#endif
                        error = DcpError::INVALID_START_TIME;
                        break;
                    }
#if defined(DEBUG) || defined(LOGGING)
                    else {
                        Log(START_TIME, convertUnixTimestamp(runPDU.getStartTime()));
                    }
#endif
                    break;
                }
                case DcpPduType::STC_do_step: {
                    DcpPduStcDoStep &doStepPDU = static_cast<DcpPduStcDoStep &>(msg);
                    if (!slavedescription::isStepsSupportedNRT(slaveDescription, doStepPDU.getSteps())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_STEPS, doStepPDU.getSteps(),
                            slavedescription::supportedStepsNRT(slaveDescription));
#endif
                        error = DcpError::INVALID_STEPS;
                    }
                    if (!slaveDescription.CapabilityFlags.canHandleVariableSteps) {
                        if (fixNrtStep > 0) {
                            if (fixNrtStep != doStepPDU.getSteps()) {
#if defined(DEBUG) || defined(LOGGING)
                                Log(NOT_SUPPORTED_VARIABLE_STEPS, doStepPDU.getSteps(), fixNrtStep);
#endif
                                if (error == DcpError::NONE) {
                                    error = DcpError::NOT_SUPPORTED_VARIABLE_STEPS;
                                }
                            }
                        } else {
                            fixNrtStep = doStepPDU.getSteps();
                        }
                    }
                    break;
                }
                case DcpPduType::INF_log: {
                    DcpPduInfLog &infLog = static_cast<DcpPduInfLog &>(msg);
                    if (!slavedescription::logCategoryExists(slaveDescription, infLog.getLogCategory())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_LOG_CATEGORY, infLog.getLogCategory());
#endif
                        error = DcpError::INVALID_LOG_CATEGORY;
                        break;
                    } else {
                        return true;
                    }
                    break;
                }
                case DcpPduType::CFG_time_res: {
                    DcpPduCfgTimeRes &setTimeRes = static_cast<DcpPduCfgTimeRes &>(msg);
                    if (timeResolutionFix &&
                        !(numerator * setTimeRes.getDenominator() == setTimeRes.getNumerator() * denominator)) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(FIX_TIME_RESOLUTION);
#endif
                        error = DcpError::INVALID_TIME_RESOLUTION;
                    }

                    if (!slavedescription::isTimeResolutionSupported(slaveDescription, setTimeRes.getNumerator(),
                                                                     setTimeRes.getDenominator())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_TIME_RESOLUTION, setTimeRes.getNumerator(), setTimeRes.getNumerator(),
                            slavedescription::supportedTimeResolutions(slaveDescription));
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::INVALID_TIME_RESOLUTION;
                        }
                    }
                    break;
                }
                case DcpPduType::CFG_steps: {
                    DcpPduCfgSteps &setSteps = static_cast<DcpPduCfgSteps &>(msg);
                    uint16_t dataId = setSteps.getDataId();

                    if (outputAssignment.find(dataId) != outputAssignment.end()) {

                        for (auto &e : outputAssignment[dataId]) {

                            const uint64_t vr = e.second;

                            const Output_t &output = *slavedescription::getOutput(slaveDescription, vr);

                            if (!slavedescription::isStepsSupported(slaveDescription, output, setSteps.getSteps())) {
#if defined(DEBUG) || defined(LOGGING)
                                Log(INVALID_STEPS, setSteps.getSteps(), vr, output.fixedSteps ?
                                                                            std::to_string(
                                                                                    output.defaultSteps)
                                                                                              :
                                                                            "between " +
                                                                            std::to_string(
                                                                                    *output.minSteps) +
                                                                            " and " +
                                                                            std::to_string(
                                                                                    *output.maxSteps));
#endif
                                error = DcpError::INVALID_STEPS;

                                break;
                            }

                        }
                    }

                    break;
                }
                case DcpPduType::CFG_input: {
                    DcpPduCfgInput &configInput = static_cast<DcpPduCfgInput &>(msg);
                    if (!slavedescription::inputExists(slaveDescription, configInput.getTargetVr())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_VALUE_REFERENCE_INPUT, configInput.getTargetVr());
#endif
                        error = DcpError::INVALID_VALUE_REFERENCE;
                        break;
                    }
                    if (!castAllowed(slavedescription::getDataType(slaveDescription, configInput.getTargetVr()),
                                     configInput.getSourceDataType())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_SOURCE_DATA_TYPE, configInput.getSourceDataType(),
                            slavedescription::getDataType(slaveDescription,
                                                          configInput.getTargetVr()));
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::INVALID_SOURCE_DATA_TYPE;
                        }

                    }
                    break;
                }
                case DcpPduType::CFG_output: {
                    DcpPduCfgOutput &outputConfig = static_cast<DcpPduCfgOutput &>(msg);
                    if (!slavedescription::outputExists(slaveDescription, outputConfig.getSourceVr())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_VALUE_REFERENCE_OUTPUT, outputConfig.getSourceVr());
#endif
                        error = DcpError::INVALID_VALUE_REFERENCE;
                        break;
                    }
                    const Output_t &output = *slavedescription::getOutput(slaveDescription, outputConfig.getSourceVr());
                    if (steps.count(outputConfig.getDataId()) >= 1) {

                        if (!slavedescription::isStepsSupported(slaveDescription, output,
                                                                steps[outputConfig.getDataId()])) {
#if defined(DEBUG) || defined(LOGGING)
                            Log(INVALID_STEPS, steps[outputConfig.getDataId()],
                                outputConfig.getSourceVr(), output.fixedSteps ?
                                                            std::to_string(output.defaultSteps) :
                                                            "between " + std::to_string(*output.minSteps) + " and " +
                                                            std::to_string(*output.maxSteps));
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INVALID_STEPS;
                            }
                        }
                    }
                    break;
                }
                case DcpPduType::CFG_target_network_information: {
                    DcpPduCfgNetworkInformation &networkInfo = static_cast<DcpPduCfgNetworkInformation &>(msg);

                    if (!slavedescription::isTransportProtocolSupported(slaveDescription,
                                                                        networkInfo.getTransportProtocol())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_TRANSPORT_PROTOCOL, networkInfo.getTransportProtocol());
#endif
                        error = DcpError::INVALID_TRANSPORT_PROTOCOL;
                        break;
                    }
                    break;
                }
                case DcpPduType::CFG_source_network_information: {
                    DcpPduCfgNetworkInformation &networkInfo = static_cast<DcpPduCfgNetworkInformation &>(msg);

                    if (!slavedescription::isTransportProtocolSupported(slaveDescription,
                                                                        networkInfo.getTransportProtocol())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_TRANSPORT_PROTOCOL, networkInfo.getTransportProtocol());
#endif
                        error = DcpError::INVALID_TRANSPORT_PROTOCOL;
                    }


                    switch (networkInfo.getTransportProtocol()) {
                        case DcpTransportProtocol::UDP_IPv4: {
                            DcpPduCfgNetworkInformationIPv4 networkInfoUdp =
                                    static_cast<DcpPduCfgNetworkInformationIPv4 &>(networkInfo);

                            if (!slavedescription::isUDPPortSupportedForInputOutput(slaveDescription,
                                                                                    networkInfoUdp.getPort())) {
#if defined(DEBUG) || defined(LOGGING)
                                Log(INVALID_PORT, networkInfoUdp.getPort(),
                                    slavedescription::supportedUdpPorts(slaveDescription));
#endif
                                if (error == DcpError::NONE) {
                                    error = DcpError::INVALID_NETWORK_INFORMATION;
                                }
                            }
                            //toDo check ip address
                            break;
                        }
                        case DcpTransportProtocol::TCP_IPv4: {
                            DcpPduCfgNetworkInformationIPv4 networkInfoTcp =
                                    static_cast<DcpPduCfgNetworkInformationIPv4 &>(networkInfo);

                            if (!slavedescription::isTCPPortSupportedForInputOutput(slaveDescription,
                                                                                    networkInfoTcp.getPort())) {
#if defined(DEBUG) || defined(LOGGING)
                                Log(INVALID_PORT, networkInfoTcp.getPort(),
                                    slavedescription::supportedTCPPorts(slaveDescription));
#endif
                                if (error == DcpError::NONE) {
                                    error = DcpError::INVALID_NETWORK_INFORMATION;
                                }
                            }
                            break;
                        }
                    }
                    break;
                }
                case DcpPduType::CFG_parameter: {
                    DcpPduCfgParameter &setParameter = static_cast<DcpPduCfgParameter &>(msg);
                    if (!slavedescription::parameterExists(slaveDescription, setParameter.getParameterVr())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_VALUE_REFERENCE_PARAMETER, setParameter.getParameterVr());
#endif
                        error = DcpError::INVALID_VALUE_REFERENCE;
                        break;
                    }
                    if (!castAllowed(slavedescription::getDataType(slaveDescription, setParameter.getParameterVr()),
                                     setParameter.getSourceDataType())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_SOURCE_DATA_TYPE, setParameter.getSourceDataType(),
                            slavedescription::getDataType(slaveDescription, setParameter.getParameterVr()));
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::INVALID_SOURCE_DATA_TYPE;
                        }
                    }
                    break;
                }
                case DcpPduType::CFG_tunable_parameter: {
                    DcpPduCfgTunableParameter configTunableParameter = static_cast<DcpPduCfgTunableParameter &>(msg);

                    if (!slavedescription::parameterExists(slaveDescription, configTunableParameter.getParameterVr())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_VALUE_REFERENCE_PARAMETER, configTunableParameter.getParameterVr());
#endif
                        error = DcpError::INVALID_VALUE_REFERENCE;
                        break;
                    }
                    if (!castAllowed(
                            slavedescription::getDataType(slaveDescription, configTunableParameter.getParameterVr()),
                            configTunableParameter.getSourceDataType())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_SOURCE_DATA_TYPE, configTunableParameter.getSourceDataType(),
                            slavedescription::getDataType(slaveDescription, configTunableParameter.getParameterVr()));
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::INVALID_SOURCE_DATA_TYPE;
                        }
                    }
                    break;
                }
                case DcpPduType::CFG_param_network_information: {
                    DcpPduCfgParamNetworkInformation &paramNetworkInfo = static_cast<DcpPduCfgParamNetworkInformation &>(msg);

                    if (!slavedescription::isTransportProtocolSupported(slaveDescription,
                                                                        paramNetworkInfo.getTransportProtocol())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_TRANSPORT_PROTOCOL, paramNetworkInfo.getTransportProtocol());
#endif
                        error = DcpError::INVALID_TRANSPORT_PROTOCOL;
                    }

                    switch (paramNetworkInfo.getTransportProtocol()) {
                        case DcpTransportProtocol::UDP_IPv4: {
                            DcpPduCfgParamNetworkInformationIPv4 paramNetworkInfoUDP =
                                    static_cast<DcpPduCfgParamNetworkInformationIPv4 &>(paramNetworkInfo);
                            if (!slavedescription::isUDPPortSupportedForParameter(slaveDescription,
                                                                                  paramNetworkInfoUDP.getPort())) {
#if defined(DEBUG) || defined(LOGGING)
                                Log(INVALID_PORT, paramNetworkInfoUDP.getPort(),
                                    slavedescription::supportedUdpPortsParameter(slaveDescription));
#endif
                                if (error == DcpError::NONE) {
                                    error = DcpError::INVALID_NETWORK_INFORMATION;
                                }

                            }
                            //toDo check ip address

                            break;
                        }
                        case DcpTransportProtocol::TCP_IPv4: {
                            DcpPduCfgParamNetworkInformationIPv4 paramNetworkInfTCP =
                                    static_cast<DcpPduCfgParamNetworkInformationIPv4 &>(paramNetworkInfo);
                            if (!slavedescription::isTCPPortSupportedForParameter(slaveDescription,
                                                                                  paramNetworkInfTCP.getPort())) {
#if defined(DEBUG) || defined(LOGGING)
                                Log(INVALID_PORT, paramNetworkInfTCP.getPort(),
                                    slavedescription::supportedTCPPortsParameter(slaveDescription));
#endif
                                if (error == DcpError::NONE) {
                                    error = DcpError::INVALID_NETWORK_INFORMATION;
                                }

                            }
                            //toDo check ip address

                            break;
                        }
                    }
                    break;
                }
                case DcpPduType::CFG_logging: {

                    DcpPduCfgLogging &setLogging = static_cast<DcpPduCfgLogging &>(msg);
                    if (setLogging.getLogMode() == DcpLogMode::LOG_ON_REQUEST &&
                        !slaveDescription.CapabilityFlags.canProvideLogOnRequest) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(NOT_SUPPORTED_LOG_ON_REQUEST);
#endif
                        error = DcpError::NOT_SUPPORTED_LOG_ON_REQUEST;
                    }
                    if (setLogging.getLogMode() == DcpLogMode::LOG_ON_NOTIFICATION &&
                        !slaveDescription.CapabilityFlags.canProvideLogOnNotification) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(NOT_SUPPORTED_LOG_ON_NOTIFICATION);
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::NOT_SUPPORTED_LOG_ON_NOTIFICATION;
                        }
                    }
                    //toDo find better solution
                    bool categoryFound = false;
                    for (const auto &category: slaveDescription.Log->categories) {
                        if (category.id == setLogging.getLogCategory()) {
                            categoryFound = true;
                            break;
                        }
                    }
                    if (!categoryFound) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_LOG_CATEGORY, setLogging.getLogCategory());
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::INVALID_LOG_CATEGORY;
                        }
                    }

                    if ((uint8_t) setLogging.getLogLevel() > 4) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_LOG_LEVEL, setLogging.getLogLevel());
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::INVALID_LOG_LEVEL;
                        }
                    }

                    if ((uint8_t) setLogging.getLogMode() > 1) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_LOG_MODE, setLogging.getLogMode());
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::INVALID_LOG_MODE;
                        }
                    }
                    break;
                }
                case DcpPduType::CFG_scope: {
                    DcpPduCfgScope &setScope = static_cast<DcpPduCfgScope &>(msg);
                    if ((uint8_t) setScope.getScope() > 2) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_SCOPE, setScope.getScope());
#endif
                        error = DcpError::INVALID_SCOPE;
                        break;
                    }
                    break;
                }
            }
        }

        DcpPduBasic &basic = static_cast<DcpPduBasic &>(msg);
        if (error == DcpError::NONE) {
            ack(basic.getPduSeqId());
            return true;
        } else {
            nack(basic.getPduSeqId(), error);
        }
        return false;
    }


#define ALLOWED_INPUT_OUTPUT(input, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11) \
case DcpDataType::input: \
    switch(output){ \
    case DcpDataType::uint8: \
        return b0;\
    case DcpDataType::uint16:\
        return b1;\
    case DcpDataType::uint32:\
        return b2;\
    case DcpDataType::uint64:\
        return b3;\
    case DcpDataType::int8:\
        return b4;\
    case DcpDataType::int16:\
        return b5;\
    case DcpDataType::int32:\
        return b6;\
    case DcpDataType::int64:\
        return b7;\
    case DcpDataType::float32:\
        return b8;\
    case DcpDataType::float64:\
        return b9;\
    case DcpDataType::string:\
        return b10;\
    case DcpDataType::binary:\
        return b11;\
    default: \
        return false;\
    }

    bool castAllowed(DcpDataType input, DcpDataType output) {
        switch (input) {
            //ALLOWED_INPUT_OUTPUT(INPUT_TYPE, uint8, uint16, uint32, uint64,
            //int8, int16, int32, int64, float32, double64, string, binary)
            ALLOWED_INPUT_OUTPUT(uint8, true, false, false, false,
                                 false, false, false, false, false, false, false, false)
            ALLOWED_INPUT_OUTPUT(uint16, true, true, false, false,
                                 false, false, false, false, false, false, false, false)
            ALLOWED_INPUT_OUTPUT(uint32, true, true, true, false,
                                 false, false, false, false, false, false, false, false)
            ALLOWED_INPUT_OUTPUT(uint64, true, true, true, true, false,
                                 false, false, false, false, false, false, false)
            ALLOWED_INPUT_OUTPUT(int8, false, false, false, false,
                                 true, false, false, false, false, false, false, false)
            ALLOWED_INPUT_OUTPUT(int16, true, false, false, false,
                                 true, true, false, false, false, false, false, false)
            ALLOWED_INPUT_OUTPUT(int32, true, true, false, false, true,
                                 true, true, false, false, false, false, false)
            ALLOWED_INPUT_OUTPUT(int64, true, true, true, false, true,
                                 true, true, true, false, false, false, false)
            ALLOWED_INPUT_OUTPUT(float32, true, true, false, false,
                                 true, true, false, false, true, false, false, false)
            ALLOWED_INPUT_OUTPUT(float64, true, true, true, false,
                                 true, true, true, false, true, true, false, false)
            ALLOWED_INPUT_OUTPUT(string, false, false, false, false,
                                 false, false, false, false, false, false, true, false)
            ALLOWED_INPUT_OUTPUT(binary, false, false, false, false,
                                 false, false, false, false, false, false, false, true)
        }
        return false;
    }

    void clearOutputBuffer() {
        for (auto const &ent : outputBuffer) {
            delete ent.second;
        }
        outputBuffer.clear();
    }

    void clearConfig() {
        driver.stop();

        inputAssignment.clear();
        configuredInPos.clear();

        outputAssignment.clear();
        configuredOutPos.clear();
        clearOutputBuffer();

        configuredParamPos.clear();
        paramAssignment.clear();

        segNumsOut.clear();
        segNumsIn.clear();
        dataSegNumsOut.clear();
        dataSegNumsIn.clear();

        steps.clear();
        runningScope.clear();
        initializationScope.clear();

        sourceNetworkConfigured.clear();
        targetNetworkConfigured.clear();
        paramNetworkConfigured.clear();
        timeResolutionSet = false;
        timeResolutionFix = false;
        for (auto const &timeResolution : slaveDescription.TimeRes.resolutions) {
            if (timeResolution.fixed) {
                numerator = timeResolution.numerator;
                denominator = timeResolution.denominator;
                this->timeResolutionSet = true;
                //this->timeResListener(timeResolution.numerator(), timeResolution.denominator());
                this->timeResolutionFix = true;
                break;
            } else if (timeResolution.recommended) {
                numerator = timeResolution.numerator;
                denominator = timeResolution.denominator;
                this->timeResolutionSet = true;
                //this->timeResListener(timeResolution.numerator(), timeResolution.denominator());
                break;
            }
        }
#if defined(DEBUG)
        Log(CONFIGURATION_CLEARED);
#endif
    }

    void updateStructualDependencies(uint64_t valueReference, size_t value) {
        for (auto const &dependency: structualDependencies[valueReference]) {
            uint64_t vrToUpdate = dependency.first;
            size_t pos = dependency.second;
            std::vector<size_t> newDimensions(values[vrToUpdate]->getDimensions());
            newDimensions[pos] = value;
            if (slavedescription::inputExists(slaveDescription, valueReference) ||
                slavedescription::outputExists(slaveDescription, valueReference)) {
                values[vrToUpdate] = new MultiDimValue(slavedescription::getDataType(slaveDescription, vrToUpdate),
                                                       values[vrToUpdate]->getBaseSize(), newDimensions);
            } else {
                updatedStructure[vrToUpdate] = new MultiDimValue(
                        slavedescription::getDataType(slaveDescription, vrToUpdate),
                        values[vrToUpdate]->getBaseSize(), newDimensions);
            }
        }
    }

    void checkForUpdatedStructure(uint64_t valueReference) {
        if (updatedStructure.count(valueReference)) {
            delete values[valueReference];
            values[valueReference] = updatedStructure[valueReference];
            updatedStructure.erase(valueReference);
        }
    }

#if defined(DEBUG) || defined(LOGGING)

    virtual void consume(const LogTemplate &logTemplate, uint8_t *payload, size_t size) override {
        LogEntry logEntry(logTemplate, payload, size);
        if (generateLogString) {
            logEntry.applyPayloadToString();
        }
        for (const std::function<void(const LogEntry &)> &logListener: logListeners) {
            logListener(logEntry);
        }

        if (logOnNotification[logEntry.getCategory()][logEntry.getLevel()]) {
            DcpPduNtfLog ntfLog = {dcpId, logEntry.getId(), logEntry.getTime(), logEntry.serialize(),
                                   logEntry.serializedSize()};
            driver.send(ntfLog);
        };
        if (logOnRequest[logEntry.getCategory()][logEntry.getLevel()]) {
            logBuffer[logEntry.getCategory()].push_back({logEntry.serialize(), logEntry.serializedSize()});
        } else {
            delete[] payload;
        }
    }

#endif

    virtual void notifyStateChangedListener() = 0;

    virtual void notifyTimeResListener() = 0;

    virtual void notifyStepsListener(uint16_t dataId, uint32_t steps) = 0;

    virtual void notifyOperationInformationListener() = 0;

    virtual void notifyRuntimeListener(int64_t unixTimeStamp) = 0;

    virtual void notifyMissingControlPduListener() = 0;

    virtual void notifyMissingInputOutputPduListener(uint16_t dataId) = 0;

    virtual void notifyMissingParameterPduListener(uint16_t paramId) = 0;

    virtual void prepare() = 0;

    virtual void configure() = 0;

    virtual void initialize() = 0;

    virtual void synchronize() = 0;

    virtual void run(const int64_t startTime) = 0;

    virtual void doStep(const uint32_t steps) = 0;

    virtual void startHeartbeat() = 0;

    virtual void updateLastStateRequest() = 0;

    virtual void sendOutputs(std::vector<uint16_t> dataIdsToSend) = 0;

};

#endif /* ACI_LOGIC_DRIVERMANAGERSLAVE_H_ */
