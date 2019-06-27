/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef ACOSAR_DRIVERMANAGERSLAVE_H
#define ACOSAR_DRIVERMANAGERSLAVE_H
#include <thread>
#include <mutex>

#include "dcp/logic/AbstractDcpManagerSlave.hpp"
#include <dcp/model/DcpCallbackTypes.hpp>



/**
 * DCP mangement of an slave
 * @author Christian Kater <kater@sim.uni-hannover.de>
 */
class DcpManagerSlave : public AbstractDcpManagerSlave {

public:
    /**
     * Instanciate a DCP manager for an slave
     * @param dcpSlaveDescription Slave description of the slave
     * @param driver DCP driver of the slave
     */
    DcpManagerSlave(const SlaveDescription_t &dcpSlaveDescription, DcpDriver driver) : AbstractDcpManagerSlave(
            dcpSlaveDescription) {
        this->driver = driver;
    }

    ~DcpManagerSlave() {
        delete initializing;
        delete configuring;
        delete stopping;
        delete _doStep;
        delete running;
        delete heartbeat;
    }

    /**
     * Stops the slave if possible
     * @return True means slave switched to state STOPPING, false means no action done
     */
    virtual bool stop() override {
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

    DcpManager getDcpManager() override {
        return {[this](DcpPdu &msg) { receive(msg); },
                [this](const DcpError errorCode) { reportError(errorCode); }};
    }

    /**
    * Set the callback for action followed by a STC_prepare PDU
    * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param prepareCallback function which will be called after the event occurs
    *
    * @post IF ftype == ASYNC: prepareFinished needs to be called after finishing preparing action
    */
    template<FunctionType ftype>
    void setPrepareCallback(std::function<void()> prepareCallback) {
        this->prepareCallback = std::move(prepareCallback);
        asynchronousCallback[DcpCallbackTypes::PREPARE] = ftype == ASYNC;
    }

    /**
	 * Set the callback for action followed by a STC_configure PDU
	 * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
	 * @param prepareCallback function which will be called after the event occurs
	 *
	 * @post IF ftype == ASYNC: configureFinished needs to be called after finishing configuring action
	 */
    template<FunctionType ftype>
    void setConfigureCallback(std::function<void()> configureCallback) {
        this->configureCallback = std::move(configureCallback);
        asynchronousCallback[DcpCallbackTypes::CONFIGURE] = ftype == ASYNC;
    }

    /**
    * Set the callback for a step in state SYNCHRONIZING and operation mode NRT
    * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param prepareCallback function which will be called after the event occurs
    *
    * @post IF ftype == ASYNC: simulationStepFinished needs to be called after finishing configuring action
    */
    template<FunctionType ftype>
    void setSynchronizingNRTStepCallback(std::function<void(uint64_t steps)> synchronizingNRTStepCallback) {
        this->synchronizingNRTStepCallback = std::move(synchronizingNRTStepCallback);
        asynchronousCallback[DcpCallbackTypes::SYNCHRONIZING_NRT_STEP] = ftype == ASYNC;
    }

    /**
   * Set the callback for a step in state SYNCHRONIZED and operation mode NRT
   * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
   * @param prepareCallback function which will be called after the event occurs
   *
   * @post IF ftype == ASYNC: simulationStepFinished needs to be called after finishing configuring action
   */
    template<FunctionType ftype>
    void setSynchronizedNRTStepCallback(std::function<void(uint64_t steps)> synchronizedNRTStepCallback) {
        this->synchronizedNRTStepCallback = std::move(synchronizedNRTStepCallback);
        asynchronousCallback[DcpCallbackTypes::SYNCHRONIZED_NRT_STEP] = ftype == ASYNC;
    }

    /**
    * Set the callback for a step in state RUNNING and operation mode NRT
    * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param prepareCallback function which will be called after the event occurs
    *
    * @post IF ftype == ASYNC: simulationStepFinished needs to be called after finishing configuring action
    */
    template<FunctionType ftype>
    void setRunningNRTStepCallback(std::function<void(uint64_t steps)> runningNRTStepCallback) {
        this->runningNRTStepCallback = std::move(runningNRTStepCallback);
        asynchronousCallback[DcpCallbackTypes::RUNNING_NRT_STEP] = ftype == ASYNC;
    }

    /**
    * Set the callback for a realtime step in state SYNCHRONIZING and operation mode SRT or HRT
    * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param prepareCallback function which will be called after the event occurs
    *
    * @post IF ftype == ASYNC: simulationStepFinished needs to be called after finishing configuring action
    */
    template<FunctionType ftype>
    void setSynchronizingStepCallback(std::function<void(uint64_t steps)> synchronizingStepCallback) {
        this->synchronizingStepCallback = std::move(synchronizingStepCallback);
        asynchronousCallback[DcpCallbackTypes::SYNCHRONIZING_STEP] = ftype == ASYNC;
    }


    /**
    * Set the callback for a realtime step in state SYNCHRONIZED and operation mode SRT or HRT
    * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param prepareCallback function which will be called after the event occurs
    *
    * @post IF ftype == ASYNC: simulationStepFinished needs to be called after finishing configuring action
    */
    template<FunctionType ftype>
    void setSynchronizedStepCallback(std::function<void(uint64_t steps)> synchronizedStepCallback) {
        this->synchronizedStepCallback = std::move(synchronizedStepCallback);
        asynchronousCallback[DcpCallbackTypes::SYNCHRONIZED_STEP] = ftype == ASYNC;
    }

    /**
    * Set the callback for a realtime step in state RUNNING and operation mode SRT or HRT
    * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param prepareCallback function which will be called after the event occurs
    *
    * @post IF ftype == ASYNC: simulationStepFinished needs to be called after finishing configuring action
    */
    template<FunctionType ftype>
    void setRunningStepCallback(std::function<void(uint64_t steps)> runningStepCallback) {
        this->runningStepCallback = std::move(runningStepCallback);
        asynchronousCallback[DcpCallbackTypes::RUNNING_STEP] = ftype == ASYNC;
    }


    /**
     * Set the callback for action followed by a STC_stop PDU
     * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param prepareCallback function which will be called after the event occurs
     *
     * @post IF ftype == ASYNC: stopFinished needs to be called after finishing preparing action
     */
    template<FunctionType ftype>
    void setStopCallback(std::function<void()> stopCallback) {
        this->stopCallback = std::move(stopCallback);
        asynchronousCallback[DcpCallbackTypes::STOP] = ftype == ASYNC;
    }


    /**
     * Set the callback for action followed by a STC_initialize PDU
     * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param prepareCallback function which will be called after the event occurs
     *
     * @post IF ftype == ASYNC: initializeFinished needs to be called after finishing preparing action
     */
    template<FunctionType ftype>
    void setInitializeCallback(std::function<void()> initializeCallback) {
        this->initializeCallback = std::move(initializeCallback);
        asynchronousCallback[DcpCallbackTypes::INITIALIZE] = ftype == ASYNC;
    }

    /**
     * Set the callback for action followed by a STC_synchronize PDU
     * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param prepareCallback function which will be called after the event occurs
     *
     * @post IF ftype == ASYNC: synchronizeFinished needs to be called after finishing preparing action
     */
    template<FunctionType ftype>
    void setSynchronizeCallback(std::function<void()> synchronizeCallback) {
        this->synchronizeCallback = std::move(synchronizeCallback);
        asynchronousCallback[DcpCallbackTypes::synchronize] = ftype == ASYNC;
    }

    /**
     * A simulation step is finished.
     *
     * @return True if calling is allowed and action was performed, otherwise False
     *
     * @pre A ASYNC callback for set{Synchronizing, Synchronized, Running}{"", NRT}Callback was called
     *
     */
    bool simulationStepFinished() {
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

    /**
     * Preparing action is finished.
     * @return True if calling is allowed and action was performed, otherwise False
     *
     * @pre A ASYNC callback for setPrepareCallback was called
     */
    bool prepareFinished() {
        if (!asynchronousCallback[DcpCallbackTypes::PREPARE]) {
            return false;
        }
        preparingFinished();
        return true;
    }

    /**
    * Configure action is finished.
    * @return True if calling is allowed and action was performed, otherwise False
    *
    * @pre A ASYNC callback for setConfigureCallback was called
    */
    bool configureFinished() {
        if (!asynchronousCallback[DcpCallbackTypes::PREPARE]) {
            return false;
        }
        configuringFinished();
        return true;
    }

    /**
   * Stop action is finished.
   * @return True if calling is allowed and action was performed, otherwise False
   *
   * @pre A ASYNC callback for setStopCallback was called
   */
    bool stopFinished() {
        if (!asynchronousCallback[DcpCallbackTypes::STOP]) {
            return false;
        }
        stoppingFinished();
        return true;
    }

    /**
    * Initialize action is finished.
    * @return True if calling is allowed and action was performed, otherwise False
    *
    * @pre A ASYNC callback for setInitializeCallback was called
    */
    bool initializeFinished() {
        if (!asynchronousCallback[DcpCallbackTypes::INITIALIZE]) {
            return false;
        }
        initializingFinished();
        return true;
    }

    /**
    * Synchronize action is finished.
    * @return True if calling is allowed and action was performed, otherwise False
    *
    * @pre A ASYNC callback for setSynchronizeCallback was called
    */
    bool synchronizeFinished() {
        if (!asynchronousCallback[DcpCallbackTypes::synchronize]) {
            return false;
        }
        synchronizingFinished();
        return true;
    }

    /**
      * Set the listener for CFG_time_res PDUs
      * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
      * @param errorAckReceivedListener function which will be called after the event occurs
      */
    template<FunctionType ftype>
    void setTimeResListener(const std::function<void(uint32_t, uint32_t)> timeResListener) {
        this->timeResListener = std::move(timeResListener);
        asynchronousCallback[DcpCallbackTypes::TIME_RES] = ftype == ASYNC;
        if (timeResolutionSet) {
            setTimeRes(numerator, denominator);
        }
    }

    /**
      * Set the listener for CFG_steps PDUs
      * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
      * @param errorAckReceivedListener function which will be called after the event occurs
      */
    template<FunctionType ftype>
    void setStepsListener(const std::function<void(uint16_t, uint32_t)> stepsListener) {
        this->stepsListener = std::move(stepsListener);
        asynchronousCallback[DcpCallbackTypes::STEPS] = ftype == ASYNC;
    }

    /**
    * Set the listener for CFG_time_res PDUs
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setOperationInformationListener(const std::function<void(uint8_t, DcpOpMode)> operationInformationListener) {
        this->operationInformationListener = std::move(operationInformationListener);
        asynchronousCallback[DcpCallbackTypes::OPERATION_INFORMATION] = ftype == ASYNC;

    }

    /**
    * Set the listener for CFG_clear PDUs
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setConfigurationClearedListener(const std::function<void()> configurationClearedListener) {
        this->configurationClearedListener = std::move(configurationClearedListener);
        asynchronousCallback[DcpCallbackTypes::CONFIGURATION_CLEARED] = ftype == ASYNC;
    }

    /**
    * Set the listener for any error that may occur
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setErrorListener(const std::function<void(DcpError)> errorListener) {
        this->errorListener = errorListener;
        asynchronousCallback[DcpCallbackTypes::ERROR_LI] = ftype == ASYNC;
    }

    /**
    * Set the listener for missing control PDUs
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setMissingControlPduListener(const std::function<void()> missingControlPduListener) {
        this->missingControlPduListener = std::move(missingControlPduListener);
        asynchronousCallback[DcpCallbackTypes::CONTROL_MISSED] = ftype == ASYNC;
    }

    /**
    * Set the listener for missing DAT_input_output PDUs
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setMissingInputOutputPduListener(const std::function<void(uint16_t)> missingInputOutputPduListener) {
        this->missingInputOutputPduListener = std::move(missingInputOutputPduListener);
        asynchronousCallback[DcpCallbackTypes::IN_OUT_MISSED] = ftype == ASYNC;
    }

    /**
    * Set the listener for missing DAT_parameter PDUs
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setMissingParameterPduListener(const std::function<void(uint16_t)> missingParameterPduListener) {
        this->missingParameterPduListener = std::move(missingParameterPduListener);
        asynchronousCallback[DcpCallbackTypes::PARAM_MISSED] = ftype == ASYNC;
    }

    /**
    * Set the listener for STC_run PDUs
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setRuntimeListener(const std::function<void(int64_t)> runtimeListener) {
        this->runtimeListener = std::move(runtimeListener);
        asynchronousCallback[DcpCallbackTypes::RUNTIME] = ftype == ASYNC;
    }

    /**
    * Set the listener for state changes which was made by the dcp slave manager
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setStateChangedListener(const std::function<void(DcpState)> stateChangedListener) {
        this->stateChangedListener = std::move(stateChangedListener);
        asynchronousCallback[DcpCallbackTypes::STATE_CHANGED] = ftype == ASYNC;
    }

private:
    /* Callbacks */
    std::map<DcpCallbackTypes, bool> asynchronousCallback;
    std::function<void()> configureCallback = []() {};
    std::function<void()> prepareCallback = []() {};
    std::function<void()> initializeCallback = []() {};
    std::function<void()> synchronizeCallback = []() {};
    std::function<void(uint64_t steps)> synchronizingStepCallback = [](uint64_t steps) {};
    std::function<void(uint64_t steps)> synchronizedStepCallback = [](uint64_t steps) {};
    std::function<void(uint64_t steps)> runningStepCallback = [](uint64_t steps) {};
    std::function<void(uint64_t steps)> synchronizingNRTStepCallback = [](uint64_t steps) {};
    std::function<void(uint64_t steps)> synchronizedNRTStepCallback = [](uint64_t steps) {};
    std::function<void(uint64_t steps)> runningNRTStepCallback = [](uint64_t steps) {};

    std::function<void()> stopCallback = []() {};

    std::function<void(uint32_t nominator,
                       uint32_t denominator)> timeResListener = [](uint32_t nominator,
                                                                   uint32_t denominator) {};

    std::function<void(uint16_t dataId, uint32_t steps)> stepsListener = [](uint16_t dataId, uint32_t steps) {};
    std::function<void(uint8_t acuId, DcpOpMode opMode)> operationInformationListener = [](uint8_t acuId,
                                                                                           DcpOpMode opMode) {};
    std::function<void(DcpError error)> errorListener = [](DcpError error) {};
    std::function<void()> configurationClearedListener = []() {};
    std::function<void()> missingControlPduListener = []() {};
    std::function<void(uint16_t dataId)> missingInputOutputPduListener = [](uint16_t dataId) {};
    std::function<void(uint16_t dataId)> missingParameterPduListener = [](uint16_t paramId) {};
    std::function<void(int64_t unixTimeStamp)> runtimeListener = [](int64_t unixTimeStamp) {};
    std::function<void(DcpState state)> stateChangedListener = [](DcpState state) {};


protected:
    /**
     * Upcounting id for last execution. Is used to determine
     * if between an longer operation, e.g. initializing, an
     * state change has happened and the done calculations are
     * obsolete.
     */
    int lastExecution;
    std::thread *initializing = NULL;
    std::thread *synchronizing = NULL;
    std::thread *configuring = NULL;
    std::thread *preparing = NULL;
    std::thread *stopping = NULL;
    std::thread *_doStep = NULL;
    std::thread *running = NULL;
    std::thread *heartbeat = NULL;

    /* Mutex */
    std::mutex mtxInput;
    std::mutex mtxOutput;
    std::mutex mtxHeartbeat;
    std::mutex mtxParam;
    std::mutex mtxLog;

    /* Time Handling */
    std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> lastStateRequest;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> nextCommunication;



    /**************************
    *  General
    **************************/

    template<DcpState preconditon, DcpState postcondition>
    inline void routineFinished(const LogTemplate &finished, const LogTemplate &interrupted) {
#ifdef DEBUG
        Log(finished);
#endif
        if (state == preconditon) {
            state = postcondition;
            notifyStateChange();
        } else {
#ifdef DEBUG
            Log(interrupted);
#endif
        }
    }

    /**************************
    *  Prepare
    **************************/

    virtual void prepare() override {
        lastExecution++;
        if (preparing != NULL) {
            preparing->detach();
            delete preparing;

        }
        preparing = new std::thread(&DcpManagerSlave::startPreparing, this);
    }

    void startPreparing() {
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

    virtual void preparingFinished() {
        routineFinished<DcpState::PREPARING, DcpState::PREPARED>(PREPARING_FINISHED, PREPARING_INTERRUPTED);
    }

    /**************************
     *  Configure
     **************************/

    virtual void configure() override {
        lastExecution++;
        if (configuring != NULL) {
            configuring->detach();
            delete configuring;

        }
        configuring = new std::thread(&DcpManagerSlave::startConfiguring, this);
    }

    void startConfiguring() {
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

    virtual void configuringFinished() {
        routineFinished<DcpState::CONFIGURING, DcpState::CONFIGURED>(CONFIGURING_FINISHED, CONFIGURING_INTERRUPTED);
    }

    /**************************
    *  Initialize
    **************************/

    virtual void initialize() override {
        lastExecution++;
        if (initializing != NULL) {
            initializing->detach();
            delete initializing;
        }
        initializing = new std::thread(&DcpManagerSlave::startInitializing, this);
    }

    void startInitializing() {
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

    virtual void initializingFinished() {
        routineFinished<DcpState::INITIALIZING, DcpState::INITIALIZED>(INITIALIZING_FINISHED, INITIALIZING_INTERRUPTED);
    }

    /**************************
     *  Synchronize
     **************************/

    virtual void synchronize() override {
        lastExecution++;
        if (synchronizing != NULL) {
            synchronizing->detach();
            delete synchronizing;
        }
        synchronizing = new std::thread(&DcpManagerSlave::startSynchronizing, this);
    }

    void startSynchronizing() {
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

    virtual void synchronizingFinished() {
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

    virtual void doStep(const uint32_t steps) override {
        if (_doStep != NULL) {
            _doStep->detach();
            delete _doStep;
        }
        _doStep = new std::thread(&DcpManagerSlave::startComputing, this,
                                  steps);
    }

    void startComputing(uint32_t steps) {
#ifdef DEBUG
        Log(COMPUTING_STARTED);
#endif

        switch (runLastExitPoint) {
            case DcpState::RUNNING: {
                if (state == DcpState::RUNNING) {
                    if (asynchronousCallback[DcpCallbackTypes::RUNNING_NRT_STEP]) {
                        std::thread t(runningNRTStepCallback, steps);
                        t.detach();
                    } else {
                        runningNRTStepCallback(steps);
                        computingFinished();
                    }
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

    virtual void computingFinished() {
        routineFinished<DcpState::COMPUTING, DcpState::COMPUTED>(COMPUTING_FINISHED, COMPUTING_INTERRUPTED);
    }

    /**************************
    *  Realtime
    **************************/

    virtual void run(const int64_t startTime) override {
        if (running != NULL) {
            running->detach();
            delete running;
        }
        running = new std::thread(&DcpManagerSlave::startRealtime,
                                  this, startTime);
    }

    void startRealtime(int64_t startTime) {
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

    void startRealtimeStep() {
        using namespace std::chrono;

        uint32_t steps = 1;

        switch (realtimeState) {
            case DcpState::RUNNING: {
                if (state == DcpState::RUNNING) {
                    mtxInput.lock();
                    mtxOutput.lock();

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
                mtxInput.lock();
                mtxOutput.lock();

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
                mtxInput.lock();
                mtxOutput.lock();

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

    virtual void realtimeStepFinished() {
        mtxInput.unlock();
        uint32_t steps = 1;

        for (std::tuple<std::vector<uint16_t>, uint32_t, uint32_t> &el : outputCounter) {
            std::get<1>(el) -= steps;
            if (std::get<1>(el) == 0) {
                std::get<1>(el) = 0 + std::get<2>(el);
                sendOutputs(std::get<0>(el));
            }
        }
        mtxOutput.unlock();

        nextCommunication += std::chrono::microseconds((int64_t) ((((double) numerator) / ((double) denominator)
                                                                   * ((double) steps)) * 1000000.0));
        //steps = newSteps;
        std::this_thread::sleep_until(nextCommunication);
        startRealtimeStep();
    }


    virtual void sendOutputs(std::vector<dataId_t> dataIdsToSend) override {
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

    virtual void updateLastStateRequest() override {
        mtxHeartbeat.lock();
        lastStateRequest = std::chrono::time_point_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now());
        mtxHeartbeat.unlock();
    }

    /**************************
    *  Stop
    **************************/

    //In stopping there can not be another legal message. So no executionNr is needed
    void startStopping() {
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

    virtual void stoppingFinished() {
#ifdef DEBUG
        Log(STOPPING_FINISHED);
#endif
        state = DcpState::STOPPED;
        notifyStateChange();
    }

    /**************************
    *  Heartbeat
    **************************/

    void startHeartbeat() override {
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

    void heartBeat() {
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

    virtual void notifyStateChangedListener() override {
        if (asynchronousCallback[DcpCallbackTypes::STATE_CHANGED]) {
            std::thread t(stateChangedListener, state);
            t.detach();
        } else {
            stateChangedListener(state);
        }
    }

    virtual void notifyTimeResListener() override {
        if (asynchronousCallback[DcpCallbackTypes::TIME_RES]) {
            std::thread t(timeResListener, numerator, denominator);
            t.detach();
        } else {
            timeResListener(numerator, denominator);
        }
    }

    virtual void notifyStepsListener(uint16_t dataId, uint32_t steps) override {
        if (asynchronousCallback[DcpCallbackTypes::STEPS]) {
            std::thread t(stepsListener, dataId, steps);
            t.detach();
        } else {
            stepsListener(dataId, steps);
        }
    }

    virtual void notifyOperationInformationListener() override {
        if (asynchronousCallback[DcpCallbackTypes::OPERATION_INFORMATION]) {
            std::thread t(operationInformationListener, dcpId, opMode);
            t.detach();
        } else {
            operationInformationListener(dcpId, opMode);
        }
    }

    virtual void notifyRuntimeListener(int64_t unixTimeStamp) override {
        if (asynchronousCallback[DcpCallbackTypes::RUNTIME]) {
            std::thread t(runtimeListener, unixTimeStamp);
            t.detach();
        } else {
            runtimeListener(unixTimeStamp);
        }
    }

    virtual void notifyMissingControlPduListener() override {
        if (asynchronousCallback[DcpCallbackTypes::CONTROL_MISSED]) {
            std::thread t(missingControlPduListener);
            t.detach();
        } else {
            missingControlPduListener();
        }
    }

    virtual void notifyMissingInputOutputPduListener(uint16_t dataId) override {
        if (asynchronousCallback[DcpCallbackTypes::IN_OUT_MISSED]) {
            std::thread t(missingInputOutputPduListener, dataId);
            t.detach();
        } else {
            missingInputOutputPduListener(dataId);
        }
    }

    virtual void notifyMissingParameterPduListener(uint16_t paramId) override {
        if (asynchronousCallback[DcpCallbackTypes::CONTROL_MISSED]) {
            std::thread t(missingParameterPduListener, paramId);
            t.detach();
        } else {
            missingParameterPduListener(paramId);
        }
    }

    virtual void reportError(const DcpError errorCode) override {
        if (asynchronousCallback[DcpCallbackTypes::ERROR_LI]) {
            std::thread t(errorListener, errorCode);
            t.detach();
        } else {
            errorListener(errorCode);
        }
    }

};


#endif //ACOSAR_DRIVERMANAGERSLAVE_H
