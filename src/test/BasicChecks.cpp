//
// Created by kater on 18.06.18.
//
#include <dcp/logic/DcpManagerSlave.hpp>
#include <dcp/logic/DcpManagerMaster.hpp>
#include <dcp/xml/DcpSlaveDescriptionReader.hpp>
#include <dcp/xml/DcpSlaveDescriptionWriter.hpp>

#include <dcp/driver/ethernet/udp/UdpDriver.hpp>
#include <dcp/driver/bluetooth/rfcomm/RfCommDriver.hpp>
#include <dcp/driver/ethernet/tcp/TcpDriver.hpp>
#include <dcp/driver/bluetooth/rfcomm/RfCommDriver.hpp>
#include <dcp/driver/ethernet/tcp/TcpDriver.hpp>

#include <dcp/zip/DcpSlaveReader.hpp>
#include <dcp/zip/DcpSlaveWriter.hpp>


int main(){
    //std::shared_ptr<SlaveDescription_t> slaveDescription = readSlaveDescription("Example-Slave-Description.xml");
    std::shared_ptr<SlaveDescription_t> slaveDescription = getSlaveDescriptionFromDcpFile(1,0,"1.zip");
    writeDcpSlaveFile(slaveDescription, "test.zip");

}
