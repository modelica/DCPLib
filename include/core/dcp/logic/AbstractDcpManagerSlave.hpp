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

#include <dcp/model/DcpTypes.hpp>
#include <dcp/model/constant/DcpState.hpp>
#include <dcp/logic/AbstractDcpManager.hpp>

#include <dcp/xml/DcpSlaveDescriptionElements.hpp>
#include <dcp/model/MultiDimValue.hpp>

#include <dcp/logic/DCPSlaveErrorCodes.hpp>

#include <set>


#ifdef ERROR
/* windows has already an error define */
#undef ERROR_LI
#endif

struct Payload {
    uint8_t *payload;
    size_t size;
};

//Forward declare DcpPdu
class DcpPdu;
class MultiDimValue;
class DcpPduDatInputOutput;

/**
 * Basic Logic for a DCP slave
 *
 * @author Christian Kater
 */
class AbstractDcpManagerSlave : public AbstractDcpManager {
public:


    ~AbstractDcpManagerSlave();

    void receive(DcpPdu &msg) override;

    /**
     * Stops the slave if possible
     * @return True means slave switched to state STOPPING, false means no action done
     */
    virtual bool stop() = 0;

    /**
     * Go to state ERRORHANDLING immediatly.
     * The master will be informed by NTF_state_changed.
     */
    void gotoErrorHandling();

    /**
     * Set the error code
     * @param errorCode New error code
     */
    void setError(DcpError errorCode);

    /**
     * Go to state ERRORRESOLVED, if current state is ERRORHANDLING.
     * Indicate that the error is resolved.
     * The master will be informed by NTF_state_changed.
     */
    void gotoErrorResolved();

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

    virtual void stopRunning() = 0;

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


    AbstractDcpManagerSlave(const SlaveDescription_t _slaveDescription);

    void notifyStateChange();

    void setTimeRes(uint32_t numerator,
                    uint32_t denominator);

    void setSteps(uint16_t dataId, uint32_t steps);

    void setOperationInformation(uint8_t dcpId, DcpOpMode opMode);

    void ack(uint16_t respSeqId);

    void nack(uint16_t respSeqId, DcpError errorCode);

    bool checkForError(DcpPdu &msg);


    bool castAllowed(DcpDataType input, DcpDataType output);

    void clearOutputBuffer();

    void clearConfig();

    void updateStructualDependencies(uint64_t valueReference, size_t value);

    void checkForUpdatedStructure(uint64_t valueReference);

#if defined(DEBUG) || defined(LOGGING)

    virtual void consume(const LogTemplate &logTemplate, uint8_t *payload, size_t size) override;

#endif

    virtual void notifyStateChangedListener() = 0;

    virtual void notifyTimeResListener() = 0;

    virtual void notifyStepsListener(uint16_t dataId, uint32_t steps) = 0;

    virtual void notifyOperationInformationListener() = 0;

    virtual void notifyRuntimeListener(int64_t unixTimeStamp) = 0;

    virtual void notifyMissingControlPduListener() = 0;

    virtual void notifyMissingInputOutputPduListener(uint16_t dataId) = 0;

    virtual void notifyMissingParameterPduListener(uint16_t paramId) = 0;

    virtual void notifyInputOutputUpdateListener(uint64_t valueReference) = 0;

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
