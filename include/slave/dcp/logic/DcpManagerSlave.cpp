#include "DcpManagerSlave.hpp"

#include "dcp/logic/AbstractDcpManagerSlave.hpp"
#include <dcp/model/DcpCallbackTypes.hpp>

#include <dcp/model/pdu/DcpPduDatInputOutput.hpp>

DcpManagerSlave::DcpManagerSlave(const SlaveDescription_t &dcpSlaveDescription, DcpDriver driver) : AbstractDcpManagerSlave(
        dcpSlaveDescription),
        mtxInput(1),
        mtxOutput(1) {
    this->driver = driver;
}

DcpManagerSlave::~DcpManagerSlave() {
    delete initializing;
    delete configuring;
    delete stopping;
    delete _doStep;
    delete running;
    delete heartbeat;
}

bool DcpManagerSlave::stop() {
    if (!(state == DcpState::ALIVE || state == DcpState::CONFIGURATION || state == DcpState::STOPPING ||
            state == DcpState::STOPPED || state == DcpState::ERROR_HANDLING || state == DcpState::ERROR_RESOLVED)) {
        lastExecution++;
        std::thread *toStop = NULL;

        if (state == DcpState::CONFIGURING) {
            toStop = configuring;
        } else if (state == DcpState::INITIALIZING) {
            toStop = initializing;
        }
        state = DcpState::STOPPING;
        notifyStateChange();
        if (stopping != NULL) {
            stopping->detach();
            delete stopping;
        }
        stopping = new std::thread(&DcpManagerSlave::startStopping, this);
        return true;
    }
    return false;
}

DcpManager DcpManagerSlave::getDcpManager() {
    return {[this](DcpPdu &msg) { receive(msg); },
            [this](const DcpError errorCode) { reportError(errorCode); }};
}

bool DcpManagerSlave::simulationStepFinished() {
    if (!asynchronousCallback[DcpCallbackTypes::SYNCHRONIZING_STEP] ||
        !asynchronousCallback[DcpCallbackTypes::SYNCHRONIZED_STEP] ||
        !asynchronousCallback[DcpCallbackTypes::RUNNING_STEP] ||
        !asynchronousCallback[DcpCallbackTypes::SYNCHRONIZING_NRT_STEP] ||
        !asynchronousCallback[DcpCallbackTypes::SYNCHRONIZED_NRT_STEP] ||
        !asynchronousCallback[DcpCallbackTypes::RUNNING_NRT_STEP]) {
        return false;
    }

    if (opMode == DcpOpMode::NRT) {
        computingFinished();
    } else {
        realtimeStepFinished();
    }
    return true;
}

bool DcpManagerSlave::prepareFinished() {
    if (!asynchronousCallback[DcpCallbackTypes::PREPARE]) {
        return false;
    }
    preparingFinished();
    return true;
}

bool DcpManagerSlave::configureFinished() {
    if (!asynchronousCallback[DcpCallbackTypes::PREPARE]) {
        return false;
    }
    configuringFinished();
    return true;
}

bool DcpManagerSlave::stopFinished() {
    if (!asynchronousCallback[DcpCallbackTypes::STOP]) {
        return false;
    }
    stoppingFinished();
    return true;
}

bool DcpManagerSlave::initializeFinished() {
    if (!asynchronousCallback[DcpCallbackTypes::INITIALIZE]) {
        return false;
    }
    initializingFinished();
    return true;
}

bool DcpManagerSlave::synchronizeFinished() {
    if (!asynchronousCallback[DcpCallbackTypes::synchronize]) {
        return false;
    }
    synchronizingFinished();
    return true;
}

void DcpManagerSlave::prepare() {
    lastExecution++;
    if (preparing != NULL) {
        preparing->detach();
        delete preparing;

    }
    preparing = new std::thread(&DcpManagerSlave::startPreparing, this);
}

void DcpManagerSlave::startPreparing() {
#ifdef DEBUG
    Log(PREPARING_STARTED);
#endif
    if (asynchronousCallback[DcpCallbackTypes::PREPARE]) {
        std::thread t(prepareCallback);
        t.detach();
    } else {
        prepareCallback();
        preparingFinished();

    }
}

void DcpManagerSlave::preparingFinished() {
    routineFinished<DcpState::PREPARING, DcpState::PREPARED>(PREPARING_FINISHED, PREPARING_INTERRUPTED);
}

/**************************
 *  Configure
 **************************/

void DcpManagerSlave::configure() {
    lastExecution++;
    if (configuring != NULL) {
        configuring->detach();
        delete configuring;

    }
    configuring = new std::thread(&DcpManagerSlave::startConfiguring, this);
}

void DcpManagerSlave::startConfiguring() {
#ifdef DEBUG
    Log(CONFIGURING_STARTED);
#endif
    if (asynchronousCallback[DcpCallbackTypes::CONFIGURE]) {
        std::thread t(configureCallback);
        t.detach();
    } else {
        configureCallback();
        configuringFinished();

    }
}

void DcpManagerSlave::configuringFinished() {
    routineFinished<DcpState::CONFIGURING, DcpState::CONFIGURED>(CONFIGURING_FINISHED, CONFIGURING_INTERRUPTED);
}

/**************************
*  Initialize
**************************/
void DcpManagerSlave::initialize() {
    lastExecution++;
    if (initializing != NULL) {
        initializing->detach();
        delete initializing;
    }
    initializing = new std::thread(&DcpManagerSlave::startInitializing, this);
}

void DcpManagerSlave::startInitializing() {
#ifdef DEBUG
    Log(INITIALIZING_STARTED);
#endif

    if (asynchronousCallback[DcpCallbackTypes::INITIALIZE]) {
        std::thread t(initializeCallback);
        t.detach();
    } else {
        initializeCallback();
        initializingFinished();
    }
}

void DcpManagerSlave::initializingFinished() {
    routineFinished<DcpState::INITIALIZING, DcpState::INITIALIZED>(INITIALIZING_FINISHED, INITIALIZING_INTERRUPTED);
}

/**************************
 *  Synchronize
 **************************/

void DcpManagerSlave::synchronize() {
    lastExecution++;
    if (synchronizing != NULL) {
        synchronizing->detach();
        delete synchronizing;
    }
    synchronizing = new std::thread(&DcpManagerSlave::startSynchronizing, this);
}

void DcpManagerSlave::startSynchronizing() {
#ifdef DEBUG
    Log(SYNCHRONIZING_STARTED);
#endif

    if (asynchronousCallback[DcpCallbackTypes::synchronize]) {
        std::thread t(synchronizeCallback);
        t.detach();
    } else {
        synchronizeCallback();
        synchronizingFinished();
    }
}

void DcpManagerSlave::synchronizingFinished() {
#ifdef DEBUG
    Log(SYNCHRONIZING_FINISHED);
#endif
    if (state == DcpState::SYNCHRONIZING) {
        state = DcpState::SYNCHRONIZED;
        realtimeState = DcpState::SYNCHRONIZED;
        notifyStateChange();
    } else {
#ifdef DEBUG
        Log(SYNCHRONIZING_INTERRUPTED);
#endif
    }
}


/**************************
*  Compute
**************************/

void DcpManagerSlave::doStep(const uint32_t steps) {
    if (_doStep != NULL) {
        _doStep->detach();
        delete _doStep;
    }
    _doStep = new std::thread(&DcpManagerSlave::startComputing, this,
                                steps);
}

void DcpManagerSlave::startComputing(uint32_t steps) {
#ifdef DEBUG
    Log(COMPUTING_STARTED);
#endif

    switch (runLastExitPoint) {
        case DcpState::RUNNING: {
        if (asynchronousCallback[DcpCallbackTypes::RUNNING_NRT_STEP]) {
        std::thread t(runningNRTStepCallback, steps);
        t.detach();
        } else {
        runningNRTStepCallback(steps);
        computingFinished();
        }
            break;
        }
        case DcpState::SYNCHRONIZING: {
            if (asynchronousCallback[DcpCallbackTypes::SYNCHRONIZING_NRT_STEP]) {
                std::thread t(synchronizingNRTStepCallback, steps);
                t.detach();
            } else {
                synchronizingNRTStepCallback(steps);
                computingFinished();
            }
            break;
        }
        case DcpState::SYNCHRONIZED: {
            if (asynchronousCallback[DcpCallbackTypes::SYNCHRONIZED_NRT_STEP]) {
                std::thread t(synchronizedNRTStepCallback, steps);
                t.detach();
            } else {
                synchronizedNRTStepCallback(steps);
                computingFinished();
            }
            break;
        }
        default: {
            //computing only from RUNNING, SYNCHRONIZING, SYNCHRONIZED possible.
            break;
        }
    }
}

void DcpManagerSlave::computingFinished() {
    routineFinished<DcpState::COMPUTING, DcpState::COMPUTED>(COMPUTING_FINISHED, COMPUTING_INTERRUPTED);
}

/**************************
*  Realtime
**************************/

void DcpManagerSlave::run(const int64_t startTime) {
    if (running != NULL) {
        running->detach();
        delete running;
    }
    running = new std::thread(&DcpManagerSlave::startRealtime,
                                this, startTime);
}

void DcpManagerSlave::startRealtime(int64_t startTime) {
    using namespace std::chrono;
    if (startTime == 0) {
        nextCommunication = time_point_cast<microseconds>(system_clock::now());
    } else {
        nextCommunication = time_point<system_clock, microseconds>(
                seconds(startTime));
        std::this_thread::sleep_until(nextCommunication);
    }

    if(state == DcpState::SYNCHRONIZING){
        realtimeState = state;
        startRealtimeStep();
        synchronize();
    } else if(state == DcpState::RUNNING){
        realtimeState = state;
    }
}

void DcpManagerSlave::startRealtimeStep() {
    using namespace std::chrono;

    uint32_t steps = 1;

    switch (realtimeState) {
        case DcpState::RUNNING: {
            if (state == DcpState::RUNNING) {
                mtxInput.wait();
                mtxOutput.wait();

                if (asynchronousCallback[DcpCallbackTypes::RUNNING_STEP]) {
                    std::thread t(runningStepCallback, steps);
                    t.detach();
                } else {
                    runningStepCallback(steps);
                    std::thread t(&DcpManagerSlave::realtimeStepFinished, this);
                    t.detach();
                }
            }
            break;
        }
        case DcpState::SYNCHRONIZING: {
            mtxInput.wait();
            mtxOutput.wait();

            if (asynchronousCallback[DcpCallbackTypes::SYNCHRONIZING_STEP]) {
                std::thread t(synchronizingStepCallback, steps);
                t.detach();
            } else {
                synchronizingStepCallback(steps);
                std::thread t(&DcpManagerSlave::realtimeStepFinished, this);
                t.detach();
            }
            break;
        }
        case DcpState::SYNCHRONIZED: {
            mtxInput.wait();
            mtxOutput.wait();

            if (asynchronousCallback[DcpCallbackTypes::SYNCHRONIZED_STEP]) {
                std::thread t(synchronizedStepCallback, steps);
                t.detach();
            } else {
                synchronizedStepCallback(steps);
                std::thread t(&DcpManagerSlave::realtimeStepFinished, this);
                t.detach();
            }
            break;
        }
        default: {
            //realtime routine is only in RUNNING, SYNCHRONIZING, SYNCHRONIZED active.
            break;
        }
    }
}

void DcpManagerSlave::realtimeStepFinished() {
    mtxInput.post();
    uint32_t steps = 1;

    for (std::tuple<std::vector<uint16_t>, uint32_t, uint32_t> &el : outputCounter) {
        std::get<1>(el) -= steps;
        if (std::get<1>(el) == 0) {
            std::get<1>(el) = 0 + std::get<2>(el);
            sendOutputs(std::get<0>(el));
        }
    }
    mtxOutput.post();

    nextCommunication += std::chrono::microseconds((int64_t) ((((double) numerator) / ((double) denominator)
                                                                * ((double) steps)) * 1000000.0));
    //steps = newSteps;
    std::this_thread::sleep_until(nextCommunication);
    startRealtimeStep();
}


void DcpManagerSlave::sendOutputs(std::vector<dataId_t> dataIdsToSend) {
    for (dataId_t dataId  : dataIdsToSend) {
        if (outputAssignment.find(dataId) == outputAssignment.end()) {
            continue;
        }
        DcpPduDatInputOutput *pdu = outputBuffer[dataId];
        size_t offset = 0;

        std::map<uint16_t, uint64_t> vrsToSend = outputAssignment[dataId];
        for (uint16_t i = 0; i < vrsToSend.size(); i++) {
            uint64_t vr = vrsToSend[i];

            offset += values[vr]->serialize(pdu->getPayload(), offset);

        }
        pdu->getPduSeqId() = getNextDataSeqNum(pdu->getDataId());
        pdu->setPduSize(offset + 5);

        driver.send(*pdu);
    }
}

void DcpManagerSlave::updateLastStateRequest() {
    mtxHeartbeat.lock();
    lastStateRequest = std::chrono::time_point_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now());
    mtxHeartbeat.unlock();
}

/**************************
*  Stop
**************************/

//In stopping there can not be another legal message. So no executionNr is needed
void DcpManagerSlave::startStopping() {
#ifdef DEBUG
    Log(STOPPING_STARTED);
#endif
    if (asynchronousCallback[DcpCallbackTypes::STOP]) {
        std::thread t(stopCallback);
        t.detach();
    } else {
        stopCallback();
        stoppingFinished();
    }
}

void DcpManagerSlave::stoppingFinished() {
#ifdef DEBUG
    Log(STOPPING_FINISHED);
#endif
    state = DcpState::STOPPED;
    notifyStateChange();
}

/**************************
*  Heartbeat
**************************/

void DcpManagerSlave::startHeartbeat() {
    if (slaveDescription.CapabilityFlags.canMonitorHeartbeat) {
        if (slaveDescription.Heartbeat == nullptr) {
#ifdef DEBUG
            Log(HEARTBEAT_IGNORED);
#endif
            return;
        }
        if (heartbeat != NULL) {
            heartbeat->detach();
            delete heartbeat;
        }
        heartbeat = new std::thread(&DcpManagerSlave::heartBeat, this);
    }
}

void DcpManagerSlave::heartBeat() {
    using namespace std::chrono;
#ifdef DEBUG
    Log(HEARTBEAT_STARTED);
#endif
    MaximumPeriodicInterval_t &interval = slaveDescription.Heartbeat->MaximumPeriodicInterval;
    uint32_t numerator = interval.numerator;
    uint32_t denominator = interval.denominator;
    time_point<system_clock, microseconds> now;
    mtxHeartbeat.lock();
    lastStateRequest = time_point_cast<microseconds>(system_clock::now());
    mtxHeartbeat.unlock();
    while (!(state == DcpState::ALIVE || state == DcpState::ERROR_HANDLING || state == DcpState::ERROR_RESOLVED)) {
        now = time_point_cast<microseconds>(system_clock::now());

        mtxHeartbeat.lock();
        auto between = duration_cast<microseconds>(now - lastStateRequest).count();
        mtxHeartbeat.unlock();
        if (between * denominator >= numerator * 1000000) {
#ifdef DEBUG
            Log(HEARTBEAT_MISSED, to_string(now), to_string(lastStateRequest));
#endif
            errorCode = DcpError::PROTOCOL_ERROR_HEARTBEAT_MISSED;
            gotoErrorHandling();
            gotoErrorResolved();
        } else {
            mtxHeartbeat.lock();
            time_point<system_clock, microseconds> nextCheck = lastStateRequest + microseconds(
                    (int64_t) (1000000 * ((double) numerator) / ((double) denominator)));
            mtxHeartbeat.unlock();
            std::this_thread::sleep_until(nextCheck);
        }
    }
#ifdef DEBUG
    Log(HEARTBEAT_STOPPED);
#endif
}

void DcpManagerSlave::notifyStateChangedListener() {
    if (asynchronousCallback[DcpCallbackTypes::STATE_CHANGED]) {
        std::thread t(stateChangedListener, state);
        t.detach();
    } else {
        stateChangedListener(state);
    }
}

void DcpManagerSlave::notifyTimeResListener() {
    if (asynchronousCallback[DcpCallbackTypes::TIME_RES]) {
        std::thread t(timeResListener, numerator, denominator);
        t.detach();
    } else {
        timeResListener(numerator, denominator);
    }
}

void DcpManagerSlave::notifyStepsListener(uint16_t dataId, uint32_t steps) {
    if (asynchronousCallback[DcpCallbackTypes::STEPS]) {
        std::thread t(stepsListener, dataId, steps);
        t.detach();
    } else {
        stepsListener(dataId, steps);
    }
}

void DcpManagerSlave::notifyOperationInformationListener() {
    if (asynchronousCallback[DcpCallbackTypes::OPERATION_INFORMATION]) {
        std::thread t(operationInformationListener, dcpId, opMode);
        t.detach();
    } else {
        operationInformationListener(dcpId, opMode);
    }
}

void DcpManagerSlave::notifyRuntimeListener(int64_t unixTimeStamp) {
    if (asynchronousCallback[DcpCallbackTypes::RUNTIME]) {
        std::thread t(runtimeListener, unixTimeStamp);
        t.detach();
    } else {
        runtimeListener(unixTimeStamp);
    }
}

void DcpManagerSlave::notifyMissingControlPduListener() {
    if (asynchronousCallback[DcpCallbackTypes::CONTROL_MISSED]) {
        std::thread t(missingControlPduListener);
        t.detach();
    } else {
        missingControlPduListener();
    }
}

void DcpManagerSlave::notifyMissingInputOutputPduListener(uint16_t dataId) {
    if (asynchronousCallback[DcpCallbackTypes::IN_OUT_MISSED]) {
        std::thread t(missingInputOutputPduListener, dataId);
        t.detach();
    } else {
        missingInputOutputPduListener(dataId);
    }
}

void DcpManagerSlave::notifyMissingParameterPduListener(uint16_t paramId) {
    if (asynchronousCallback[DcpCallbackTypes::CONTROL_MISSED]) {
        std::thread t(missingParameterPduListener, paramId);
        t.detach();
    } else {
        missingParameterPduListener(paramId);
    }
}

void DcpManagerSlave::notifyInputOutputUpdateListener(uint64_t valueReference) {
    if (asynchronousCallback[DcpCallbackTypes::IN_OUT_UPDATE]) {
        std::thread t(inputOutputUpdateListener, valueReference);
        t.detach();
    } else {
        inputOutputUpdateListener(valueReference);
    }
}

void DcpManagerSlave::reportError(const DcpError errorCode) {
    if (asynchronousCallback[DcpCallbackTypes::ERROR_LI]) {
        std::thread t(errorListener, errorCode);
        t.detach();
    } else {
        errorListener(errorCode);
    }
}