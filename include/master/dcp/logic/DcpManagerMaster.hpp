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
#include <thread>

#include <dcp/model/pdu/DcpPdu.hpp>
#include <dcp/model/DcpCallbackTypes.hpp>

#include "dcp/xml/DcpSlaveDescriptionElements.hpp"
#include "dcp/logic/AbstractDcpManager.hpp"
#include "dcp/model/LogEntry.hpp"


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
    DcpManagerMaster(DcpDriver driver);

    virtual ~DcpManagerMaster() {}

    virtual void receive(DcpPdu &msg) override;

    virtual void reportError(const DcpError errorCode) override;

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
                      const DcpOpMode opMode, const uint8_t majorversion, const uint8_t minorVersion);

    /**
     * Send a STC_deregister PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_deregister(const uint8_t dcpId, const DcpState stateId);

    /**
     * Send a STC_prepare PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_prepare(const uint8_t dcpId, const DcpState stateId);

    /**
     * Send a STC_configure PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_configure(const uint8_t dcpId, const DcpState stateId);

    /**
     * Send a STC_initialize PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_initialize(const uint8_t dcpId, const DcpState stateId);

    /**
     * Send a STC_run PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     * @param startTime At which unix time stamp action will be active (soft realtime data transfer in SYNCHRONIZINNG or RUNNING)
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_run(const uint8_t dcpId, const DcpState stateId, const int64_t startTime);

    /**
     * Send STC_do_step PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     * @param steps Number of steps to simulate
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_do_step(const uint8_t dcpId, const DcpState stateId, const uint32_t steps);

    /**
     * Send a STC_send_outputs PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_send_outputs(const uint8_t dcpId, const DcpState stateId);

    /**
     * Send a STC_stop PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_stop(const uint8_t dcpId, const DcpState stateId);

    /**
     * Send a STC_reset PDU
     * @param dcpId Receiver of the PDU
     * @param stateId Current DCP state of the receiving slave
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void STC_reset(const uint8_t dcpId, const DcpState stateId);

    /**
     * Send a INF_state PDU
     * @param dcpId Receiver of the PDU
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void INF_state(const uint8_t dcpId);

    /**
     * Send a INF_error PDU
     * @param dcpId Receiver of the PDU
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void INF_error(const uint8_t dcpId);

    /**
     * Send a INF_log PDU.
     * @param dcpId Receiver of the PDU
     * @param logCategory Log category of the log entries in responding RPS_log_ack
     * @param logMaxNum Max. number of log entries in responding RSP_log_ack
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void INF_log(const uint8_t dcpId, const uint8_t logCategory, const uint8_t logMaxNum);

    /**
    * Send a CFG_time_res PDU
    * @param dcpId Receiver of the PDU
    * @param numerator numerator of the time resolution
    * @param denominator denominator of the time resolution
    *
    * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
    */
    void CFG_time_res(const uint8_t dcpId, const uint32_t numerator,
                          const uint32_t denominator);

    /**
     * Send a CFG_steps
     * @param dcpId Receiver of the PDU
     * @param dataId Data id for which the steps value is valid
     * @param steps Steps for DAT_input_output PDU of given dataId
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void CFG_steps(const uint8_t dcpId, uint16_t dataId, const uint32_t steps);

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
                          const DcpDataType sourceDataType);

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
                           const uint16_t pos, const uint64_t sourceVr);

    /**
     * Send a CFG_clear PDU
     * @param dcpId Receiver of the PDU
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void CFG_clear(const uint8_t dcpId);

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
                                                const uint16_t port);

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
                                            const uint16_t port);

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
                                                const uint16_t port);

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
                                            const uint16_t port);

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
                           uint8_t *configuration, size_t configurationLength);

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
                                      const DcpDataType sourceDataType);

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
                                               const uint16_t port);

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
                                           const uint16_t port);

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
                         const DcpLogMode logMode);

    /**
     * Send a CFG_scope PDU
     * @param dcpId Receiver of the PDU
     * @param dataId Data id for which this configuration is valid
     * @param scope Dcp scope of the given data id
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void CFG_scope(const uint8_t dcpId, const uint16_t dataId, const DcpScope scope);

    /**
     * Send a DAT_parameter PDU
     * @param paramId Parameter id of the PDU
     * @param configuration Values of the parameters, configured with this paramId
     * @param configurationLength Number of bytes of configuration
     *
     * @pre setTargetParamNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void DAT_parameter(const uint16_t paramId, uint8_t *configuration, size_t configurationLength);

    /**
    * Send a DAT_input_output PDU
    * @param dataId Data id of the PDU
    * @param configuration Values of the outputs, configured with this dataId
    * @param configurationLength Number of bytes of configuration
    *
    * @pre setTargetNetworkInformation of the given DcpDriver was called for dcpId before
    */
    void DAT_input_output(const uint16_t dataId, uint8_t *configuration, size_t configurationLength);

    /**
     * Enables to send periodically INF_state to a DCP slave
     * @param dcpId Receiver of the INF_state PDU
     * @param numerator Numerator of the sending period
     * @param denominator Denominator of the sending period
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void enableHeartbeat(const uint8_t dcpId, const uint32_t numerator, const uint32_t denominator);

    /**
     * Disable to send periodically INF_state to a DCP slave
     * @param dcpId Receiver of the INF_state PDU
     *
     * @pre setSlaveNetworkInformation of the given DcpDriver was called for dcpId before
     */
    void disableHeartbeat(const uint8_t dcpId);

    /**
     * Set the slave description for a DCP slave. This is necessary to receive Log messages of the slave.
     * @param dcpId DCP id of the slaves connected to the given slave description
     * @param slaveDescription Slave description of the slave
     */
    void setDcpSlaveDescription(const uint8_t dcpId, const SlaveDescription_t &slaveDescription);

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

    void setPDUListener(const std::function<void(DcpPdu &)> pduListener);

    DcpManager getDcpManager() override;

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

    void heartBeatRoutine(const uint8_t dcpId, const uint32_t numerator, const uint32_t denominator);
};

#endif /* ACI_LOGIC_DRIVERMANAGERMASTER_H_ */
