
#ifndef SLAVE_H_
#define SLAVE_H_

#include <dcp/helper/Helper.hpp>
#include <dcp/log/OstreamLog.hpp>
#include <dcp/logic/DcpManagerSlave.hpp>
#include <dcp/model/pdu/DcpPduFactory.hpp>
#include <dcp/driver/ethernet/udp/UdpDriver.hpp>
#include <dcp/xml/DcpSlaveDescriptionWriter.hpp>

#include <cstdint>
#include <cstdio>
#include <stdarg.h>
#include <thread>
#include <cmath>


class Slave {
public:
    Slave() : stdLog(std::cout) {
        udpDriver = new UdpDriver(HOST, PORT);
        manager = new DcpManagerSlave(getSlaveDescription(), udpDriver->getDcpDriver());
        manager->setInitializeCallback<SYNC>(
                std::bind(&Slave::initialize, this));
        manager->setConfigureCallback<SYNC>(
                std::bind(&Slave::configure, this));
        manager->setSynchronizingStepCallback<SYNC>(
                std::bind(&Slave::doStep, this, std::placeholders::_1));
        manager->setSynchronizedStepCallback<SYNC>(
                std::bind(&Slave::doStep, this, std::placeholders::_1));
        manager->setRunningStepCallback<SYNC>(
                std::bind(&Slave::doStep, this, std::placeholders::_1));
        manager->setTimeResListener<SYNC>(std::bind(&Slave::setTimeRes, this,
                                                    std::placeholders::_1,
                                                    std::placeholders::_2));

        //Display log messages on console
        manager->addLogListener(
                std::bind(&OstreamLog::logOstream, stdLog, std::placeholders::_1));
        manager->setGenerateLogString(true);

        writeDcpSlaveDescription(getSlaveDescription(), "MSD2-Slave-Description.xml");
        data_out_file.open("data_out_msd2.csv");

    }

    ~Slave() {
        delete manager;
        delete udpDriver;

		data_out_file.close();
    }


    void configure() {
        simulationTime = 0;
        currentStep = 0;

        Fc = manager->getOutput<float64_t *>(Fc_vr);
        x1 = manager->getInput<float64_t *>(x1_vr);
        v1 = manager->getInput<float64_t *>(v1_vr);

        x2 = manager->getOutput<float64_t *>(x2_vr);
        v2 = manager->getOutput<float64_t *>(v2_vr);
        data_out_file << "time,x2,v2,Fc" << std::endl;
    }

    void initialize() {
    }

    void doStep(uint64_t steps) {
        float64_t timeDiff =
                ((double) numerator) / ((double) denominator) * ((double) steps);

        double h = timeDiff / 10;
        for (int i = 0; i < 10; ++i) {
            *x2 = *x2 + (*v2)*h;
            *v2 = *v2 + (h / param_m2)*(-*Fc - param_c2 * (*x2));
            *Fc = param_cc * (*x2 - *x1) + param_dc * (*v2 - *v1);
            simulationTime += h;
        }

        data_out_file << simulationTime << ",";
        data_out_file << *x2 << ",";
        data_out_file << *v2 << ",";
        data_out_file << *Fc << std::endl;
        manager->Log(SIM_LOG, simulationTime, currentStep, *x2, *v2);
        //simulationTime += timeDiff;
        currentStep += steps;
    }

    void setTimeRes(const uint32_t numerator, const uint32_t denominator) {
        this->numerator = numerator;
        this->denominator = denominator;
    }

    void start() { manager->start(); }

    SlaveDescription_t getSlaveDescription(){
        SlaveDescription_t slaveDescription = make_SlaveDescription(1, 0, "dcpslave", "b3663a74-5e59-11ec-bf63-0242ac130003");
        slaveDescription.OpMode.SoftRealTime = make_SoftRealTime_ptr();
        Resolution_t resolution = make_Resolution();
        resolution.numerator = 1;
        resolution.denominator = 10;
        slaveDescription.TimeRes.resolutions.push_back(resolution);
        slaveDescription.TransportProtocols.UDP_IPv4 = make_UDP_ptr();
        slaveDescription.TransportProtocols.UDP_IPv4->Control =
                make_Control_ptr(HOST, PORT);
        ;
        slaveDescription.TransportProtocols.UDP_IPv4->DAT_input_output = make_DAT_ptr();
        slaveDescription.TransportProtocols.UDP_IPv4->DAT_input_output->availablePortRanges.push_back(
                make_AvailablePortRange(2048, 65535));
        slaveDescription.TransportProtocols.UDP_IPv4->DAT_parameter = make_DAT_ptr();
        slaveDescription.TransportProtocols.UDP_IPv4->DAT_parameter->availablePortRanges.push_back(
                make_AvailablePortRange(2048, 65535));
        slaveDescription.CapabilityFlags.canAcceptConfigPdus = true;
        slaveDescription.CapabilityFlags.canHandleReset = true;
        slaveDescription.CapabilityFlags.canHandleVariableSteps = true;
        slaveDescription.CapabilityFlags.canMonitorHeartbeat = false;
        slaveDescription.CapabilityFlags.canAcceptConfigPdus = true;
        slaveDescription.CapabilityFlags.canProvideLogOnRequest = true;
        slaveDescription.CapabilityFlags.canProvideLogOnNotification = true;

        std::shared_ptr<Output_t> caus_x2 = make_Output_ptr<float64_t>();
        caus_x2->Float64->start = std::make_shared<std::vector<float64_t>>();
        caus_x2->Float64->start->push_back(-1.0);
        slaveDescription.Variables.push_back(make_Variable_output("x2", x2_vr, caus_x2));
        std::shared_ptr<Output_t> caus_v2 = make_Output_ptr<float64_t>();
        caus_v2->Float64->start = std::make_shared<std::vector<float64_t>>();
        caus_v2->Float64->start->push_back(0.0);
        slaveDescription.Variables.push_back(make_Variable_output("v2", v2_vr, caus_v2));

        std::shared_ptr<CommonCausality_t> caus_x1 = make_CommonCausality_ptr<float64_t>();
        caus_x1->Float64->start = std::make_shared<std::vector<float64_t>>();
        caus_x1->Float64->start->push_back(0.0);
        slaveDescription.Variables.push_back(make_Variable_input("x1", x1_vr, caus_x1));
        std::shared_ptr<CommonCausality_t> caus_v1 = make_CommonCausality_ptr<float64_t>();
        caus_v1->Float64->start = std::make_shared<std::vector<float64_t>>();
        caus_v1->Float64->start->push_back(0.0);
        slaveDescription.Variables.push_back(make_Variable_input("v1", v1_vr, caus_v1));

        std::shared_ptr<Output_t> caus_Fc = make_Output_ptr<float64_t>();
        caus_Fc->Float64->start = std::make_shared<std::vector<float64_t>>();
        caus_Fc->Float64->start->push_back(0.0);
        slaveDescription.Variables.push_back(make_Variable_output("Fc", Fc_vr, caus_Fc));

        slaveDescription.Log = make_Log_ptr();
        slaveDescription.Log->categories.push_back(make_Category(1, "DCP_SLAVE"));
        slaveDescription.Log->templates.push_back(make_Template(
                1, 1, (uint8_t) DcpLogLevel::LVL_INFORMATION, "[Time = %float64][step = %uint64 ] : pos = %float64, vel = %float64"));

       return slaveDescription;
    }

private:
    DcpManagerSlave *manager;
    OstreamLog stdLog;

    UdpDriver* udpDriver;
    const char *const HOST = "127.0.0.1";
    const int PORT = 8082;

    uint32_t numerator;
    uint32_t denominator;

    double simulationTime;
    uint64_t currentStep;

    const LogTemplate SIM_LOG = LogTemplate(
            1, 1, DcpLogLevel::LVL_INFORMATION,
            "[Time = %float64][step = %uint64 ] : pos = %float64, vel = %float64",
            {DcpDataType::float64, DcpDataType::uint64, DcpDataType::float64, DcpDataType::float64});


    float64_t *x2;
    const uint32_t x2_vr = 1;
	float64_t *v2;
	const uint32_t v2_vr = 2;
	float64_t *x1;
	const uint32_t x1_vr = 3;
	float64_t *v1;
	const uint32_t v1_vr = 4;
	float64_t *Fc;
	const uint32_t Fc_vr = 5;
	float64_t param_c2 = 1.0;
	float64_t param_m2 = 1.0;
	float64_t param_cc = 1.0;
	float64_t param_dc = 1.0;

	std::ofstream data_out_file;

};

#endif /* SLAVE_H_ */
