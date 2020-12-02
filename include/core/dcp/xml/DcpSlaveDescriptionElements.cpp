#include "DcpSlaveDescriptionElements.hpp"

std::shared_ptr<NonRealTime_t> make_NonRealTime_ptr() {
    std::shared_ptr<NonRealTime_t> ptr = std::make_shared<NonRealTime_t>();
    ptr->defaultSteps = 1;
    ptr->fixedSteps = true;
    ptr->minSteps = 1;
    ptr->maxSteps = 1;
    return ptr;
}

std::shared_ptr<BaseUnit_t> make_BaseUnit_ptr(){
    std::shared_ptr<BaseUnit_t> ptr = std::make_shared<BaseUnit_t>();
    ptr->kg = 0;
    ptr->m = 0;
    ptr->s = 0;
    ptr->A = 0;
    ptr->K = 0;
    ptr->mol = 0;
    ptr->cd = 0;
    ptr->rad = 0;
    ptr->factor = 1.0;
    ptr->offset = 0;
    return ptr;
}

std::shared_ptr<Heartbeat_t> make_Heartbeat_ptr() {
    std::shared_ptr<Heartbeat_t> heartbeat(new Heartbeat_t());
    heartbeat->MaximumPeriodicInterval = make_MaximumPeriodicInterval();
    return heartbeat;
}

std::shared_ptr<Control_t> make_Control_ptr(std::string host, port_t port) {
    std::shared_ptr<Control_t> control(new Control_t());
    control->host = std::make_shared<std::string>(host);
    control->port = std::make_shared<port_t>(port);
    return control;
}

std::shared_ptr<DAT_t> make_DAT_ptr(std::string host) {
    std::shared_ptr<DAT_t> dat(new DAT_t());
    dat->host = std::make_shared<std::string>(host);
    dat->availablePorts = std::vector<AvailablePort_t>();
    dat->availablePortRanges = std::vector<AvailablePortRange_t>();
    return dat;
}

std::shared_ptr<DAT_t> make_DAT_ptr() {
    std::shared_ptr<DAT_t> dat(new DAT_t());
    dat->host = std::make_shared<std::string>("");
    dat->availablePorts = std::vector<AvailablePort_t>();
    dat->availablePortRanges = std::vector<AvailablePortRange_t>();
    return dat;
}

std::shared_ptr<Ethernet_t> make_UDP_ptr() {
    std::shared_ptr<Ethernet_t> udp(new Ethernet_t());
    udp->maxPduSize = 65507;
    udp->Control = std::shared_ptr<Control_t>(nullptr);
    udp->DAT_input_output = std::shared_ptr<DAT_t>(nullptr);
    udp->DAT_parameter = std::shared_ptr<DAT_t>(nullptr);
    return udp;
}

std::shared_ptr<Ethernet_t> make_TCP_ptr() {
    std::shared_ptr<Ethernet_t> udp(new Ethernet_t());
    udp->maxPduSize = 4294967267;
    udp->Control = std::shared_ptr<Control_t>(nullptr);
    udp->DAT_input_output = std::shared_ptr<DAT_t>(nullptr);
    udp->DAT_parameter = std::shared_ptr<DAT_t>(nullptr);
    return udp;
}

std::shared_ptr<USB_t> make_USB_ptr(){
    std::shared_ptr<USB_t> ptr = std::make_shared<USB_t>();
    ptr->maxPduSize = 1024;
    ptr->maxPower = std::shared_ptr<uint8_t>(nullptr);
    ptr->dataPipes = std::vector<DataPipe_t>();
    return ptr;
}

std::shared_ptr<Bluetooth_t> make_Bluetooth_ptr(){
    std::shared_ptr<Bluetooth_t> ptr = std::make_shared<Bluetooth_t>();
    ptr->maxPduSize = 1011;
    ptr->addresses = std::vector<Address_t>();
    return ptr;
}

std::shared_ptr<DependencyState_t> make_DependecyState_ptr(){
    std::shared_ptr<DependencyState_t> ptr = std::make_shared<DependencyState_t>();
    ptr->dependecies = std::vector<Dependency_t>();
    return ptr;
}

std::shared_ptr<Dependencies_t> make_Dependencies_ptr(){
    std::shared_ptr<Dependencies_t> ptr = std::make_shared<Dependencies_t>();
    ptr->Initialization = nullptr;
    ptr->Run = nullptr;
    return ptr;
}

SimpleType_t make_SimpleType_String(std::string name){
    SimpleType_t simpleType = {name, std::shared_ptr<std::string>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<uint8_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<uint16_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<uint32_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<uint64_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<int8_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<int16_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<int32_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<int64_t>>(nullptr),
                               std::shared_ptr<SimpleFloatDataType_t<float32_t>>(nullptr),
                               std::shared_ptr<SimpleFloatDataType_t<float64_t>>(nullptr),
                               std::shared_ptr<SimpleStringDataType_t>(nullptr),
                               std::shared_ptr<SimpleBinaryDataType_t>(nullptr),};

    simpleType.String = std::make_shared<SimpleStringDataType_t>();
    return simpleType;
}


SimpleType_t make_SimpleType_Binary(std::string name){
    SimpleType_t simpleType = {name, std::shared_ptr<std::string>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<uint8_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<uint16_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<uint32_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<uint64_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<int8_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<int16_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<int32_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<int64_t>>(nullptr),
                               std::shared_ptr<SimpleFloatDataType_t<float32_t>>(nullptr),
                               std::shared_ptr<SimpleFloatDataType_t<float64_t>>(nullptr),
                               std::shared_ptr<SimpleStringDataType_t>(nullptr),
                               std::shared_ptr<SimpleBinaryDataType_t>(nullptr),};

    simpleType.Binary = std::make_shared<SimpleBinaryDataType_t>();
    return simpleType;
}


std::shared_ptr<CommonCausality_t> make_CommonCausality_String_ptr() {
    return std::shared_ptr<CommonCausality_t>(new CommonCausality_t(std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
            std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
            std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
            std::shared_ptr<StringDataType_t>(new StringDataType_t()), std::shared_ptr<BinaryDataType_t>(nullptr),
            std::vector<Dimension_t>()));
}

std::shared_ptr<CommonCausality_t> make_CommonCausality_Binary_ptr() {
    return std::shared_ptr<CommonCausality_t>(new CommonCausality_t(std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
            std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
            std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
            std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(new BinaryDataType_t()),
            std::vector<Dimension_t>()));
}


std::shared_ptr<Output_t> make_Output_String_ptr() {
    std::shared_ptr<Output_t> o (new Output_t());
    o->String = std::shared_ptr<StringDataType_t>(new StringDataType_t());
    o->dimensions = std::vector<Dimension_t>();
    o->defaultSteps = 1;
    o->fixedSteps = true;
    o->minSteps = std::shared_ptr<steps_t>(nullptr);
    o->maxSteps = std::shared_ptr<steps_t>(nullptr);
    o->initialization = false;
    o->Dependencies = std::shared_ptr<Dependencies_t>(nullptr);
    return o;
}

std::shared_ptr<Output_t> make_Output_Binary_ptr() {
    std::shared_ptr<Output_t> o (new Output_t());
    o->Binary = std::shared_ptr<BinaryDataType_t>(new BinaryDataType_t());
    o->dimensions = std::vector<Dimension_t>();
    o->defaultSteps = 1;
    o->fixedSteps = true;
    o->minSteps = std::shared_ptr<steps_t>(nullptr);
    o->maxSteps = std::shared_ptr<steps_t>(nullptr);
    o->initialization = false;
    o->Dependencies = std::shared_ptr<Dependencies_t>(nullptr);
    return o;
}

Variable_t
make_Variable_input(std::string name, valueReference_t valueReference, std::shared_ptr<CommonCausality_t> Input) {
    return {name, valueReference, std::shared_ptr<std::string>(), Variability::CONTINUOUS,
            std::shared_ptr<double>(), std::shared_ptr<double>(),
            std::shared_ptr<uint32_t>(), std::shared_ptr<std::string>(), std::move(Input), std::shared_ptr<Output_t>(nullptr),
            std::shared_ptr<CommonCausality_t>(nullptr), std::shared_ptr<StructuralParameter_t>(nullptr)};
}

Variable_t
make_Variable_output(std::string name, valueReference_t valueReference, std::shared_ptr<Output_t> Output) {
    return {name, valueReference, std::shared_ptr<std::string>(), Variability::CONTINUOUS,
            std::shared_ptr<double>(), std::shared_ptr<double>(),
            std::shared_ptr<uint32_t>(), std::shared_ptr<std::string>(), std::shared_ptr<CommonCausality_t>(nullptr), std::move(Output),
            std::shared_ptr<CommonCausality_t>(nullptr), std::shared_ptr<StructuralParameter_t>(nullptr)};
}

Variable_t
make_Variable_parameter(std::string name, valueReference_t valueReference,
                        std::shared_ptr<CommonCausality_t> Parameter) {
    return {name, valueReference, std::shared_ptr<std::string>(), Variability::CONTINUOUS,
            std::shared_ptr<double>(), std::shared_ptr<double>(),
            std::shared_ptr<uint32_t>(), std::shared_ptr<std::string>(), std::shared_ptr<CommonCausality_t>(nullptr), std::shared_ptr<Output_t>(nullptr),
            std::move(Parameter), std::shared_ptr<StructuralParameter_t>(nullptr)};
}

Variable_t
make_Variable_structuralParameter(std::string name, valueReference_t valueReference,
                                  std::shared_ptr<StructuralParameter_t> StructuralParameter) {
    return {name, valueReference, std::shared_ptr<std::string>(), Variability::CONTINUOUS,
            std::shared_ptr<double>(), std::shared_ptr<double>(),
            std::shared_ptr<uint32_t>(), std::shared_ptr<std::string>(), std::shared_ptr<CommonCausality_t>(nullptr), std::shared_ptr<Output_t>(nullptr),
            std::shared_ptr<CommonCausality_t>(nullptr),
            std::move(StructuralParameter)};
}

SlaveDescription_t
make_SlaveDescription(uint8_t dcpMajorVersion, uint8_t dcpMinorVersion, const std::string dcpSlaveName,
                      const std::string uuid) {
    return {make_OpMode(), std::vector<Unit_t>(), std::vector<SimpleType_t>(), make_TimeRes(), std::shared_ptr<Heartbeat_t>(nullptr),
            make_TransportProtocols(),
            make_CapabilityFlags(), std::vector<Variable_t>(), std::shared_ptr<Log_t>(
                    nullptr), dcpMajorVersion, dcpMinorVersion, dcpSlaveName, uuid,
            std::shared_ptr<std::string>(nullptr), std::shared_ptr<std::string>(nullptr), std::shared_ptr<std::string>(
                    nullptr), std::shared_ptr<std::string>(nullptr), std::shared_ptr<std::string>(
                    nullptr), std::shared_ptr<std::string>(nullptr), std::shared_ptr<std::string>(nullptr),
            VariableNamingConvention::FLAT};
}

std::shared_ptr<SlaveDescription_t>
make_SlaveDescription_ptr(uint8_t dcpMajorVersion, uint8_t dcpMinorVersion, const std::string dcpSlaveName,
                      const std::string uuid) {
    std::shared_ptr<SlaveDescription_t> ptr = std::make_shared<SlaveDescription_t>();
    ptr->OpMode = make_OpMode();
    ptr->UnitDefinitions = std::vector<Unit_t>();
    ptr->TypeDefinitions = std::vector<SimpleType_t>();
    ptr->TimeRes = make_TimeRes();
    ptr->Heartbeat = std::shared_ptr<Heartbeat_t>(nullptr);
    ptr->TransportProtocols = make_TransportProtocols();
    ptr->CapabilityFlags = make_CapabilityFlags();
    ptr->Variables = std::vector<Variable_t>();
    ptr->Log = std::shared_ptr<Log_t>(nullptr);
    ptr->dcpMajorVersion = dcpMajorVersion;
    ptr->dcpMinorVersion = dcpMinorVersion;
    ptr->dcpSlaveName = dcpSlaveName;
    ptr->uuid = uuid;
    ptr->description = std::shared_ptr<std::string>(nullptr);
    ptr->author = std::shared_ptr<std::string>(nullptr);
    ptr->version = std::shared_ptr<std::string>(nullptr);
    ptr->copyright = std::shared_ptr<std::string>(nullptr);
    ptr->license = std::shared_ptr<std::string>(nullptr);
    ptr->generationTool = std::shared_ptr<std::string>(nullptr);
    ptr->generationDateAndTime = std::shared_ptr<std::string>(nullptr);
    ptr->variableNamingConvention = VariableNamingConvention::FLAT;
    return ptr;
}