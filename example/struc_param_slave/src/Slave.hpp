
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
        manager->setSynchronizedNRTStepCallback<SYNC>(
            std::bind(&Slave::doStep, this, std::placeholders::_1));
        manager->setRunningNRTStepCallback<SYNC>(
            std::bind(&Slave::doStep, this, std::placeholders::_1));
        manager->setTimeResListener<SYNC>(std::bind(&Slave::setTimeRes, this,
                                                    std::placeholders::_1,
                                                    std::placeholders::_2));

        //Display log messages on console
        manager->addLogListener(
                std::bind(&OstreamLog::logOstream, stdLog, std::placeholders::_1));
        manager->setGenerateLogString(true);

        writeDcpSlaveDescription(getSlaveDescription(), "StrucParam-Slave-Description.xml");
    }

    ~Slave() {
        delete manager;
        delete udpDriver;
    }


    void configure() {
        simulationTime = 0;
        currentStep = 0;

        a = manager->getInput<float64_t *>(a_vr);
        y = manager->getOutput<float64_t *>(y_vr);
        dim = manager->getParameter<uint16_t *>(dim_vr);
        for (size_t i = 0; i < *dim; ++i) {
            // Set first inputs manually, as length of start value is not known before runtime
            a[i] = (float)i - 1;
        }
        for (uint16_t i = 0; i < *dim; ++i) {
            manager->Log(SIM_LOG, simulationTime, i, a[i]);
        }

    }

    void initialize() {
        for (size_t i = 0; i < *dim; ++i) {
            y[i] = i + 1;
            std::cout << "Initialize " << std::endl;
        }
    }

    void doStep(uint64_t steps) {
        float64_t timeDiff =
            ((double)numerator) / ((double)denominator) * ((double)steps);

        for (uint16_t i = 0; i < *dim; ++i) {
            y[i] = a[i] + 1;
        }
        for (uint16_t i = 0; i < *dim; ++i) {
            manager->Log(SIM_LOG, simulationTime, i, y[i]);
        }

        simulationTime += timeDiff;
        currentStep += steps;
    }

    void setTimeRes(const uint32_t numerator, const uint32_t denominator) {
        this->numerator = numerator;
        this->denominator = denominator;
    }

    void start() { manager->start(); }

    SlaveDescription_t getSlaveDescription(){
        SlaveDescription_t slaveDescription = make_SlaveDescription(1, 0, "dcpslave", "2fcef2a4-51d0-11ec-bf63-0242ac130002");
        slaveDescription.OpMode.SoftRealTime = make_SoftRealTime_ptr();
        slaveDescription.OpMode.NonRealTime = make_NonRealTime_ptr();
        Resolution_t resolution = make_Resolution();
        resolution.numerator = 1;
        resolution.denominator = 100;
        slaveDescription.TimeRes.resolutions.push_back(resolution);
        slaveDescription.TransportProtocols.UDP_IPv4 = make_UDP_ptr();
        slaveDescription.TransportProtocols.UDP_IPv4->Control =
                make_Control_ptr(HOST, 8080);
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

        std::shared_ptr<Output_t> caus_y = make_Output_ptr<float64_t>();
        Dimension_t dim_y = make_Dimension(DimensionType::LINKED_VR, dim_vr);
        caus_y->dimensions.push_back(dim_y);
        slaveDescription.Variables.push_back(make_Variable_output("y", y_vr, caus_y));
        std::shared_ptr<CommonCausality_t> caus_a =
            make_CommonCausality_ptr<float64_t>();
        caus_a->Float64->start = std::make_shared<std::vector<float64_t>>();
        caus_a->Float64->start->push_back(10.0);
        caus_a->dimensions.push_back(dim_y);
        slaveDescription.Variables.push_back(make_Variable_input("a", a_vr, caus_a));
        std::shared_ptr<StructuralParameter_t> caus_dim =
            make_StructuralParameter_ptr<uint16_t>();
        caus_dim->Uint16->start = std::make_shared<std::vector<uint16_t>>();
        caus_dim->Uint16->start->push_back(2);
        slaveDescription.Variables.push_back(make_Variable_structuralParameter("dim", dim_vr, caus_dim));
        slaveDescription.Log = make_Log_ptr();
        slaveDescription.Log->categories.push_back(make_Category(1, "DCP_SLAVE"));
        slaveDescription.Log->templates.push_back(make_Template(
            1, 1, (uint8_t)DcpLogLevel::LVL_INFORMATION, "[Time = %float64]: vector component %uint16 has value: %float64"));

        return slaveDescription;
    }

private:
    DcpManagerSlave *manager;
    OstreamLog stdLog;

    UdpDriver* udpDriver;
    const char *const HOST = "127.0.0.1";
    const int PORT = 8080;

    uint32_t numerator;
    uint32_t denominator;

    double simulationTime;
    uint64_t currentStep;

    const LogTemplate SIM_LOG = LogTemplate(
            1, 1, DcpLogLevel::LVL_INFORMATION,
            "[Time = %float64]: vector component %uint16 has value: %float64",
            {DcpDataType::float64, DcpDataType::uint16, DcpDataType::float64});

    float64_t *a;
    const uint32_t a_vr = 2;
    float64_t *y;
    const uint32_t y_vr = 1;
	uint16_t *dim;
	const uint32_t dim_vr = 3;

};

#endif /* SLAVE_H_ */
