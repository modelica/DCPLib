/*
 * Master.h
 *
 *  Created on: 25.02.2017
 *      Author: kater
 */

#ifndef ACI_EXAMPLE_MASTER_H_
#define ACI_EXAMPLE_MASTER_H_

#include <iostream>
#include <cstdint>
#include <fstream>

#include <dcp/model/pdu/DcpPduFactory.hpp>
#include <dcp/xml/DcpSlaveDescriptionReader.hpp>
#include <dcp/driver/ethernet/udp/UdpDriver.hpp>
#include <dcp/logic/DcpManagerMaster.hpp>
#include <dcp/log/OstreamLog.hpp>


/**
 * Note: This example is just for demonstration purpose of the API.
 * It uses very simple mechanisms and assumptions about the scenario to simulate,
 * which will not work out in a general master tool.
 */
class MasterModel {
public:
    MasterModel() : stdLog(std::cout) {
        driver = new UdpDriver(HOST, PORT);

        slaveDescription = readSlaveDescription("Example-Slave-Description.xml");
        manager = new DcpManagerMaster(driver->getDcpDriver());
        uint8_t *netInfo = new uint8_t[6];
        *((uint16_t *) netInfo) = *slaveDescription->TransportProtocols.UDP_IPv4->Control->port;
        *((uint32_t *) (netInfo + 2)) = asio::ip::address_v4::from_string(*slaveDescription->TransportProtocols.UDP_IPv4->Control->host).to_ulong();
        driver->getDcpDriver().setSlaveNetworkInformation(1, netInfo);
        delete[] netInfo;
        manager->setAckReceivedListener<SYNC>(
                std::bind(&MasterModel::receiveAck, this, std::placeholders::_1, std::placeholders::_2));
        manager->setNAckReceivedListener<SYNC>(
                std::bind(&MasterModel::receiveNAck, this, std::placeholders::_1, std::placeholders::_2,
                          std::placeholders::_3));
        manager->setStateChangedNotificationReceivedListener<SYNC>(
                std::bind(&MasterModel::receiveStateChangedNotification, this, std::placeholders::_1,
                          std::placeholders::_2));
        manager->addLogListener(std::bind(&OstreamLog::logOstream, stdLog, std::placeholders::_1));
        manager->setGenerateLogString(true);
    }

    ~MasterModel() {
        delete driver;
        delete manager;
    }

    void start() {
        std::thread b(&DcpManagerMaster::start, manager);
        std::chrono::seconds dura(1);
        std::this_thread::sleep_for(dura);
        //driver->getDcpDriver().connectToSlave(1);
        std::cout << "Register Slaves" << std::endl;
        manager->STC_register(1, DcpState::ALIVE, convertToUUID(slaveDescription->uuid), DcpOpMode::SRT, 1, 0);
        b.join();
    }

private:
    void initialize() {
        std::cout << "Initialize Slaves" << std::endl;
        manager->STC_initialize(1, DcpState::CONFIGURED);
        intializationRuns++;
    }

    void configuration() {
        std::cout << "Configure Slaves" << std::endl;
        receivedAcks[1] = 0;

        manager->CFG_scope(1, 1, DcpScope::Initialization_Run_NonRealTime);

        manager->CFG_input(1, 1, 0, slaveDescription->Variables.at(1).valueReference, DcpDataType::float64);
        manager->CFG_output(1, 1, 0, slaveDescription->Variables.at(0).valueReference);

        manager->CFG_steps(1, 1, 1);
        manager->CFG_time_res(1, slaveDescription->TimeRes.resolutions.front().numerator,
                                 slaveDescription->TimeRes.resolutions.front().denominator);
        manager->CFG_source_network_information_UDP(1, 1, asio::ip::address_v4::from_string(
                        *slaveDescription->TransportProtocols.UDP_IPv4->Control->host).to_ulong(), *slaveDescription->TransportProtocols.UDP_IPv4->Control->port);
        manager->CFG_target_network_information_UDP(1, 1,  asio::ip::address_v4::from_string(
                *slaveDescription->TransportProtocols.UDP_IPv4->Control->host).to_ulong(), *slaveDescription->TransportProtocols.UDP_IPv4->Control->port);
        numOfCmd[1] = 7;
    }

    void configure() {
        manager->STC_configure(1, DcpState::PREPARED);
    }

    void run(DcpState currentState) {
        std::cout << "Run Simulation" << std::endl;
        std::time_t now = std::time(0);
        manager->STC_run(1, currentState, now + 2);
    }

    void stop() {
        std::chrono::seconds dura(secondsToSimulate + 2);
        std::this_thread::sleep_for(dura);
        std::cout << "Stop Simulation" << std::endl;

        manager->STC_stop(1, DcpState::RUNNING);
    }

    void deregister() {
        std::cout << "Deregister Slaves" << std::endl;
        manager->STC_deregister(1, DcpState::STOPPED);
    }

    void sendOutputs() {
        std::cout << "Send Outputs" << std::endl;
        manager->STC_send_outputs(1, DcpState::INITIALIZED);
    }

    void receiveAck(uint8_t sender, uint16_t pduSeqId) {
        receivedAcks[sender]++;
        if (receivedAcks[sender] == numOfCmd[sender]) {
            manager->STC_prepare(sender, DcpState::CONFIGURATION);
        }
    }

    void receiveNAck(uint8_t sender, uint16_t pduSeqId,
                     DcpError errorCode) {
        std::cerr << "Error in slave configuration." << std::endl;
        std::exit(1);
    }

    void receiveStateChangedNotification(uint8_t sender,
                                         DcpState state) {
        std::chrono::milliseconds dura(250);
        std::this_thread::sleep_for(dura);
        switch (state) {
            case DcpState::CONFIGURATION:
                configuration();
                break;
            case DcpState::CONFIGURED:
                if (intializationRuns < maxInitRuns) {
                    initialize();

                } else {
                    run(DcpState::CONFIGURED);
                }
                break;
            case DcpState::SYNCHRONIZED:
                run(DcpState::SYNCHRONIZED);
                break;

            case DcpState::PREPARED:
                configure();
                break;

            case DcpState::INITIALIZED:
                sendOutputs();
                break;

            case DcpState::RUNNING: {
                stop();
                break;
            }
            case DcpState::STOPPED:
                deregister();
                break;
            case DcpState::ALIVE:
                //simulation finished
                std::exit(0);
                break;
        }
    }

    void logAck(uint8_t sender, uint16_t pduSeqId, std::shared_ptr<std::vector<LogEntry>> entries);

    OstreamLog stdLog;

    uint8_t maxInitRuns = 0;
    uint8_t intializationRuns = 1;

    std::map<dcpId_t, DcpState> curState;

    UdpDriver *driver;
    const char *const HOST = "127.0.0.1";
    const uint16_t PORT = 8081;

    DcpManagerMaster *manager;

    uint64_t secondsToSimulate = 5;
    std::map<dcpId_t, uint8_t> numOfCmd;
    std::map<dcpId_t, uint64_t> receivedAcks;


    std::shared_ptr<SlaveDescription_t> slaveDescription;


};

#endif /* ACI_EXAMPLE_MASTER_H_ */
