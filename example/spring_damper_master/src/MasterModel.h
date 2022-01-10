/*
 * Master.h
 *
 *  Created on: 17.12.2021
 *      Author: cschiffer
 */

#ifndef ACI_EXAMPLE_MASTER_H_
#define ACI_EXAMPLE_MASTER_H_

#include <iostream>
#include <cstdint>
#include <fstream>
#include <memory>

#include <dcp/helper/LogHelper.hpp>
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

        slaveDescription1 = readSlaveDescription("MSD1-Slave-Description.xml");
		slaveDescription2 = readSlaveDescription("MSD2-Slave-Description.xml");
        manager = new DcpManagerMaster(driver->getDcpDriver());
        uint8_t netInfo1[6];
        *((uint16_t *) netInfo1) = *slaveDescription1->TransportProtocols.UDP_IPv4->Control->port;
        *((uint32_t *) (netInfo1 + 2)) = asio::ip::address_v4::from_string(*slaveDescription1->TransportProtocols.UDP_IPv4->Control->host).to_ulong();
        driver->getDcpDriver().setSlaveNetworkInformation(1, netInfo1);

		uint8_t netInfo2[6];
		*((uint16_t *)netInfo2) = *slaveDescription2->TransportProtocols.UDP_IPv4->Control->port;
		*((uint32_t *)(netInfo2 + 2)) = asio::ip::address_v4::from_string(*slaveDescription2->TransportProtocols.UDP_IPv4->Control->host).to_ulong();
		driver->getDcpDriver().setSlaveNetworkInformation(2, netInfo2);

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
        manager->STC_register(1, DcpState::ALIVE, convertToUUID(slaveDescription1->uuid), DcpOpMode::SRT, 1, 0);
		manager->STC_register(2, DcpState::ALIVE, convertToUUID(slaveDescription2->uuid), DcpOpMode::SRT, 1, 0);
        b.join();
    }

private:
    void initialize(uint8_t sender) {
        SlavesReady[sender - 1] = true;
        if (std::all_of(SlavesReady, SlavesReady + 2, [](bool i) { return i; })) {
            std::cout << "Initialize Slaves" << std::endl;
            manager->STC_initialize(1, DcpState::CONFIGURED);
            manager->STC_initialize(2, DcpState::CONFIGURED);
            intializationRuns++;
        }
    }

    void configuration(uint8_t sender) {
        std::cout << "Configure Slaves" << std::endl;

        const uint16_t port1 = 60001;
        const uint16_t port2 = 60002;
        if (1 == sender)
        {
            receivedAcks[1] = 0;
            manager->CFG_scope(1, 1, DcpScope::Initialization_Run_NonRealTime);
            manager->CFG_scope(1, 2, DcpScope::Initialization_Run_NonRealTime);

            // output dataId = 1, input dataId = 2
            manager->CFG_output(1, 1, 0, slaveDescription1->Variables.at(0).valueReference);
            manager->CFG_output(1, 1, 1, slaveDescription1->Variables.at(1).valueReference);
            manager->CFG_input(1, 2, 0, slaveDescription1->Variables.at(2).valueReference, DcpDataType::float64);

            manager->CFG_steps(1, 1, 1);
            manager->CFG_time_res(1, slaveDescription1->TimeRes.resolutions.front().numerator,
                slaveDescription1->TimeRes.resolutions.front().denominator);
            manager->CFG_source_network_information_UDP(1, 2, asio::ip::address_v4::from_string(
                *slaveDescription1->TransportProtocols.UDP_IPv4->Control->host).to_ulong(), port1);
            manager->CFG_target_network_information_UDP(1, 1, asio::ip::address_v4::from_string(
                *slaveDescription2->TransportProtocols.UDP_IPv4->Control->host).to_ulong(), port2);
            numOfCmd[1] = 9;
        }
        if (2 == sender)
        {

            receivedAcks[2] = 0;
            manager->CFG_scope(2, 1, DcpScope::Initialization_Run_NonRealTime);
            manager->CFG_scope(2, 2, DcpScope::Initialization_Run_NonRealTime);

            // inuput dataId = 1, output dataId = 2
            manager->CFG_output(2, 2, 0, slaveDescription2->Variables.at(4).valueReference);
            manager->CFG_input(2, 1, 0, slaveDescription2->Variables.at(2).valueReference, DcpDataType::float64);
            manager->CFG_input(2, 1, 1, slaveDescription2->Variables.at(3).valueReference, DcpDataType::float64);

            manager->CFG_steps(2, 2, 1);
            manager->CFG_time_res(2, slaveDescription1->TimeRes.resolutions.front().numerator,
                slaveDescription1->TimeRes.resolutions.front().denominator);
            manager->CFG_source_network_information_UDP(2, 1, asio::ip::address_v4::from_string(
                *slaveDescription2->TransportProtocols.UDP_IPv4->Control->host).to_ulong(), port2);
            manager->CFG_target_network_information_UDP(2, 2, asio::ip::address_v4::from_string(
                *slaveDescription1->TransportProtocols.UDP_IPv4->Control->host).to_ulong(), port1);
            numOfCmd[2] = 9;
        }
    }

    void configure(uint8_t sender) {
        manager->STC_configure(sender, DcpState::PREPARED);
    }

    void run(DcpState currentState, uint8_t sender) {
        SlavesReady[sender - 1] = true;
        if (std::all_of(SlavesReady, SlavesReady + 2, [](bool i) { return i; })) {
            std::cout << "Run Simulation" << std::endl;
            std::time_t now = std::time(0);
            manager->STC_run(1, currentState, now + 2);
            manager->STC_run(2, currentState, now + 2);
            std::fill(SlavesReady, SlavesReady + 2, false);
        }
    }

    void stop(uint8_t sender) {
        SlavesReady[sender - 1] = true;
        if (std::all_of(SlavesReady, SlavesReady + 2, [](bool i) { return i; })) {
            std::chrono::seconds dura(secondsToSimulate + 2);
            std::this_thread::sleep_for(dura);
            std::cout << "Stop Simulation" << std::endl;

            manager->STC_stop(1, DcpState::RUNNING);
            manager->STC_stop(2, DcpState::RUNNING);
            std::fill(SlavesReady, SlavesReady + 2, false);
        }
    }

    void deregister(uint8_t sender) {
        std::cout << "Deregister Slaves" << std::endl;
		std::chrono::seconds dura(1);
		std::this_thread::sleep_for(dura);
        manager->STC_deregister(sender, DcpState::STOPPED);
    }

    void sendOutputs(uint8_t sender) {
        std::cout << "Send Outputs" << std::endl;
        manager->STC_send_outputs(sender, DcpState::INITIALIZED);
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

	void shutdown(uint8_t sender) {
		SlavesReady[sender - 1] = true;
		if (std::all_of(SlavesReady, SlavesReady + 2, [](bool i) { return i; })) {
			std::exit(0);
		}
	}

    void receiveStateChangedNotification(uint8_t sender,
                                         DcpState state) {
        std::chrono::milliseconds dura(250);
        //std::this_thread::sleep_for(dura);
        switch (state) {
            case DcpState::CONFIGURATION:
                configuration(sender);
                break;
            case DcpState::CONFIGURED:
                if (intializationRuns < maxInitRuns) {
                    initialize(sender);

                } else {
                    run(DcpState::CONFIGURED, sender);
                }
                break;
            case DcpState::SYNCHRONIZED:
                run(DcpState::SYNCHRONIZED, sender);
                break;
            case DcpState::PREPARED:
                configure(sender);
                break;

            case DcpState::INITIALIZED:
                sendOutputs(sender);
                break;

            case DcpState::RUNNING:
                stop(sender);
                break;
            case DcpState::STOPPED:
                deregister(sender);
                break;
            case DcpState::ALIVE:
				shutdown(sender);
                break;
        }
    }

    void logAck(uint8_t sender, uint16_t pduSeqId, std::shared_ptr<std::vector<LogEntry>> entries);

    OstreamLog stdLog;

    uint8_t maxInitRuns = 0;
    uint8_t intializationRuns = 1;

    std::map<dcpId_t, DcpState> curState;

	bool SlavesReady[2] = { false, false };

    UdpDriver *driver;
    const char *const HOST = "127.0.0.1";
    const uint16_t PORT = 8080;

    DcpManagerMaster *manager;

    uint64_t secondsToSimulate = 10;
    std::map<dcpId_t, uint8_t> numOfCmd;
    std::map<dcpId_t, uint64_t> receivedAcks;


    std::shared_ptr<SlaveDescription_t> slaveDescription1;
	std::shared_ptr<SlaveDescription_t> slaveDescription2;


};

#endif /* ACI_EXAMPLE_MASTER_H_ */
