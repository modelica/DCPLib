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
#include <condition_variable>

#include "dcp/logic/AbstractDcpManagerSlave.hpp"
#include <dcp/model/DcpCallbackTypes.hpp>

namespace internal {
    struct Semaphore {
        explicit Semaphore(unsigned int value): value_(value)
        {}

        void wait() {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [&]{
                return value_ > 0;
            });
            value_--;
        }

        void post() {
            std::unique_lock<std::mutex> lock(mtx_);
            value_++;
            lock.unlock();
            cv_.notify_one();
        }

    private:
        unsigned int value_;
        std::mutex mtx_;
        std::condition_variable cv_;
    };
}

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
    DcpManagerSlave(const SlaveDescription_t &dcpSlaveDescription, DcpDriver driver);

    ~DcpManagerSlave();

    /**
     * Stops the slave if possible
     * @return True means slave switched to state STOPPING, false means no action done
     */
    virtual bool stop() override;

    DcpManager getDcpManager() override;

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
    bool simulationStepFinished();

    /**
     * Preparing action is finished.
     * @return True if calling is allowed and action was performed, otherwise False
     *
     * @pre A ASYNC callback for setPrepareCallback was called
     */
    bool prepareFinished();

    /**
    * Configure action is finished.
    * @return True if calling is allowed and action was performed, otherwise False
    *
    * @pre A ASYNC callback for setConfigureCallback was called
    */
    bool configureFinished();

    /**
   * Stop action is finished.
   * @return True if calling is allowed and action was performed, otherwise False
   *
   * @pre A ASYNC callback for setStopCallback was called
   */
    bool stopFinished();

    /**
    * Initialize action is finished.
    * @return True if calling is allowed and action was performed, otherwise False
    *
    * @pre A ASYNC callback for setInitializeCallback was called
    */
    bool initializeFinished();

    /**
    * Synchronize action is finished.
    * @return True if calling is allowed and action was performed, otherwise False
    *
    * @pre A ASYNC callback for setSynchronizeCallback was called
    */
    bool synchronizeFinished();

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
    * Set the listener for DAT_input_output PDUs
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setInputOutputUpdateListener(const std::function<void(uint64_t)> inputOutputUpdateListener) {
        this->inputOutputUpdateListener = std::move(inputOutputUpdateListener);
        asynchronousCallback[DcpCallbackTypes::IN_OUT_UPDATE] = ftype == ASYNC;
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
    std::function<void(uint64_t valueReference)> inputOutputUpdateListener = [](uint64_t valueReference) {};
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
    internal::Semaphore mtxInput;
    internal::Semaphore mtxOutput;
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

    virtual void prepare() override;

    void startPreparing();

    virtual void preparingFinished();

    /**************************
     *  Configure
     **************************/

    virtual void configure() override;

    void startConfiguring();

    virtual void configuringFinished();

    /**************************
    *  Initialize
    **************************/

    virtual void initialize() override;

    void startInitializing();

    virtual void initializingFinished();

    /**************************
     *  Synchronize
     **************************/

    virtual void synchronize() override;

    void startSynchronizing();

    virtual void synchronizingFinished();


    /**************************
    *  Compute
    **************************/

    virtual void doStep(const uint32_t steps) override;

    void startComputing(uint32_t steps);

    virtual void computingFinished();

    /**************************
    *  Realtime
    **************************/

    virtual void run(const int64_t startTime) override;

    void startRealtime(int64_t startTime);

    void startRealtimeStep();

    virtual void realtimeStepFinished();


    virtual void sendOutputs(std::vector<dataId_t> dataIdsToSend) override;

    virtual void updateLastStateRequest() override;

    /**************************
    *  Stop
    **************************/

    //In stopping there can not be another legal message. So no executionNr is needed
    void startStopping();

    virtual void stoppingFinished();

    /**************************
    *  Heartbeat
    **************************/

    void startHeartbeat() override;

    void heartBeat();

    virtual void notifyStateChangedListener() override;

    virtual void notifyTimeResListener() override;

    virtual void notifyStepsListener(uint16_t dataId, uint32_t steps) override;

    virtual void notifyOperationInformationListener() override;

    virtual void notifyRuntimeListener(int64_t unixTimeStamp) override;

    virtual void notifyMissingControlPduListener() override;

    virtual void notifyMissingInputOutputPduListener(uint16_t dataId) override;

    virtual void notifyMissingParameterPduListener(uint16_t paramId) override;

    virtual void notifyInputOutputUpdateListener(uint64_t valueReference) override;

    virtual void reportError(const DcpError errorCode) override;

};


#endif //ACOSAR_DRIVERMANAGERSLAVE_H
