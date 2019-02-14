/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef LIBACOSAR_ACIDESCRIPTIONREADER_H
#define LIBACOSAR_ACIDESCRIPTIONREADER_H

#include <dcp/xml/DcpSlaveDescriptionElements.hpp>
#include <dcp/model/constant/DcpOpMode.hpp>
#include <dcp/model/constant/DcpTransportProtocol.hpp>
#include <dcp/helper/Helper.hpp>

namespace slavedescription {

    inline const Variable_t *getVariable(const SlaveDescription_t &slaveDescription, uint64_t vr) {
        for (auto &var: slaveDescription.Variables) {
            if (var.valueReference == vr) {
                return &var;
            }
        }
        return nullptr;
    }

    inline const CommonCausality_t *getInput(const SlaveDescription_t &slaveDescription, const uint64_t vr) {
        const Variable_t *variable = getVariable(slaveDescription, vr);
        if(variable != nullptr){
            return variable->Input.get();
        }
        return nullptr;
    }

    inline const bool inputExists(const SlaveDescription_t &slaveDescription, const uint64_t vr) {
        try {
            return getInput(slaveDescription, vr) != nullptr;
        } catch (...) {
            return false;
        }
        return false;
    }

    inline const Output_t *getOutput(const SlaveDescription_t &slaveDescription, const uint64_t vr) {
        const Variable_t *variable = getVariable(slaveDescription, vr);
        if(variable != nullptr){
            return variable->Output.get();
        }
        return nullptr;

    }

    inline const bool outputExists(const SlaveDescription_t &slaveDescription, const uint64_t vr) {
        try {
            return getOutput(slaveDescription, vr) != nullptr;
        } catch (...) {
            return false;
        }
        return false;
    }

    inline const CommonCausality_t *getParameter(const SlaveDescription_t &slaveDescription, const uint64_t vr) {
        const Variable_t *variable = getVariable(slaveDescription, vr);
        if(variable != nullptr){
            return variable->Parameter.get();
        }
        return nullptr;
    }

    inline const bool parameterExists(const SlaveDescription_t &slaveDescription, const uint64_t vr) {
        try {
            return getParameter(slaveDescription, vr) != nullptr;
        } catch (...) {
            return false;
        }
        return false;
    }

    inline const StructuralParameter_t *
    getStructuralParameter(const SlaveDescription_t &slaveDescription, const uint64_t vr) {
        const Variable_t *variable = getVariable(slaveDescription, vr);
        if(variable != nullptr){
            return variable->StructuralParameter.get();
        }
        return nullptr;
    }

    inline const bool structuralParameterExists(const SlaveDescription_t &slaveDescription, const uint64_t vr) {
        try {
            return getStructuralParameter(slaveDescription, vr) != nullptr;
        } catch (...) {
            return false;
        }
        return false;
    }

    inline const DcpDataType getDataType(const SlaveDescription_t &slaveDescription, const uint64_t vr) {
        const Variable_t *varP = getVariable(slaveDescription, vr);
        if(varP == nullptr){
            return DcpDataType::uint8;
        }

        const Variable_t &var = *varP;
        if (var.Output.get() != nullptr) {
            const Output_t &output = *var.Output.get();
            if (output.Uint8.get() != nullptr) {
                return DcpDataType::uint8;
            } else if (output.Uint16.get() != nullptr) {
                return DcpDataType::uint16;
            } else if (output.Uint32.get() != nullptr) {
                return DcpDataType::uint32;
            } else if (output.Uint64.get() != nullptr) {
                return DcpDataType::uint64;
            } else if (output.Int8.get() != nullptr) {
                return DcpDataType::int8;
            } else if (output.Int16.get() != nullptr) {
                return DcpDataType::int16;
            } else if (output.Int32.get() != nullptr) {
                return DcpDataType::int32;
            } else if (output.Int64.get() != nullptr) {
                return DcpDataType::int64;
            } else if (output.Float32.get() != nullptr) {
                return DcpDataType::float32;
            } else if (output.Float64.get() != nullptr) {
                return DcpDataType::float64;
            } else if (output.Binary.get() != nullptr) {
                return DcpDataType::binary;
            } else if (output.String.get() != nullptr) {
                return DcpDataType::string;
            }
        } else if (var.Input.get() != nullptr) {
            const CommonCausality_t &input = *var.Input.get();
            if (input.Uint8.get() != nullptr) {
                return DcpDataType::uint8;
            } else if (input.Uint16.get() != nullptr) {
                return DcpDataType::uint16;
            } else if (input.Uint32.get() != nullptr) {
                return DcpDataType::uint32;
            } else if (input.Uint64.get() != nullptr) {
                return DcpDataType::uint64;
            } else if (input.Int8.get() != nullptr) {
                return DcpDataType::int8;
            } else if (input.Int16.get() != nullptr) {
                return DcpDataType::int16;
            } else if (input.Int32.get() != nullptr) {
                return DcpDataType::int32;
            } else if (input.Int64.get() != nullptr) {
                return DcpDataType::int64;
            } else if (input.Float32.get() != nullptr) {
                return DcpDataType::float32;
            } else if (input.Float64.get() != nullptr) {
                return DcpDataType::float64;
            } else if (input.Binary.get() != nullptr) {
                return DcpDataType::binary;
            } else if (input.String.get() != nullptr) {
                return DcpDataType::string;
            }

        } else if (var.Parameter.get() != nullptr) {
            const CommonCausality_t &parameter = *var.Parameter.get();
            if (parameter.Uint8.get() != nullptr) {
                return DcpDataType::uint8;
            } else if (parameter.Uint16.get() != nullptr) {
                return DcpDataType::uint16;
            } else if (parameter.Uint32.get() != nullptr) {
                return DcpDataType::uint32;
            } else if (parameter.Uint64.get() != nullptr) {
                return DcpDataType::uint64;
            } else if (parameter.Int8.get() != nullptr) {
                return DcpDataType::int8;
            } else if (parameter.Int16.get() != nullptr) {
                return DcpDataType::int16;
            } else if (parameter.Int32.get() != nullptr) {
                return DcpDataType::int32;
            } else if (parameter.Int64.get() != nullptr) {
                return DcpDataType::int64;
            } else if (parameter.Float32.get() != nullptr) {
                return DcpDataType::float32;
            } else if (parameter.Float64.get() != nullptr) {
                return DcpDataType::float64;
            } else if (parameter.Binary.get() != nullptr) {
                return DcpDataType::binary;
            } else if (parameter.String.get() != nullptr) {
                return DcpDataType::string;
            }

        } else if (var.StructuralParameter.get() != nullptr) {
            const StructuralParameter_t &structuralParameter = *var.StructuralParameter.get();
            if (structuralParameter.Uint8.get() != nullptr) {
                return DcpDataType::uint8;
            } else if (structuralParameter.Uint16.get() != nullptr) {
                return DcpDataType::uint16;
            } else if (structuralParameter.Uint32.get() != nullptr) {
                return DcpDataType::uint32;
            } else if (structuralParameter.Uint64.get() != nullptr) {
                return DcpDataType::uint64;
            }
        }
        return DcpDataType::uint8;
    }

    inline const bool
    isTimeResolutionSupported(const SlaveDescription_t &slaveDescription, const uint32_t numerator,
                              const uint32_t denominator) {
        if (numerator == 0 && denominator == 0) {
            return false;
        }
        for (auto const &timeRes : slaveDescription.TimeRes.resolutions) {
            // timeRes a == timeRes b
            // <=> (a.numerator / a.denominator) == (b.numerator / b.denominator)
            // <=> (a.numerator * b.denominator) == (a.denominator * b.numerator)
            // note: only valid without further check, because numerator and denominator are natural numbers
            if ((timeRes.numerator * denominator) == (timeRes.denominator * numerator)) {
                return true;
            }
        }
        for (auto const &timeResRange : slaveDescription.TimeRes.resolutionRanges) {
            uint32_t cNumerator_low = timeResRange.numeratorFrom;
            uint32_t cNumerator_high = timeResRange.numeratorTo;
            uint32_t cDenominator = timeResRange.denominator;
            // timeRes a >= timeRes b && timeRes a <= timeRes c
            // <=> (a.numerator / a.denominator) >= (b.numerator / b.denominator) && (a.numerator / a.denominator) <= (c.numerator / c.denominator)
            // <=> (a.numerator * b.denominator) >= (a.denominator * b.numerator) && (a.numerator * c.denominator) <= (a.denominator * c.numerator)
            // note: only valid without further check, because numerator and denominator are natural number
            if ((numerator * cDenominator >= denominator * cNumerator_low) &&
                (numerator * cDenominator <= denominator * cNumerator_high)) {
                return true;
            }
        }
        return false;
    }

    inline const std::string supportedTimeResolutions(const SlaveDescription_t &slaveDescription) {
        std::string str = "(";
        for (auto const &tr : slaveDescription.TimeRes.resolutions) {
            str +=
                    std::to_string(tr.numerator) + "/" + std::to_string(tr.denominator) + ", ";
        }
        for (auto const &timeResRange : slaveDescription.TimeRes.resolutionRanges) {
            str +=
                    std::to_string(timeResRange.numeratorFrom) + "/" +
                    std::to_string(timeResRange.denominator) + " - " +
                    std::to_string(timeResRange.numeratorTo) + "/" +
                    std::to_string(timeResRange.denominator) + ", ";
        }
        if (str.size() > 2) str.resize(str.size() - 2);
        str += ")";
        return str;
    }

    inline const bool
    isStepsSupported(const SlaveDescription_t &slaveDescription, const Output_t &output, const uint32_t steps) {
        if (output.fixedSteps) {
            return steps == output.defaultSteps;
        }

        return steps == output.defaultSteps || (output.minSteps != nullptr && output.maxSteps != nullptr && steps >= *output.minSteps && steps <= *output.maxSteps);
    }


    inline const bool isStepsSupportedNRT(const SlaveDescription_t &slaveDescription, const uint32_t steps) {
        std::shared_ptr<NonRealTime_t> nonRealTime = slaveDescription.OpMode.NonRealTime;
        if (nonRealTime != nullptr) {
            if (nonRealTime->fixedSteps) {
                return steps == nonRealTime->defaultSteps;
            } else {
                return steps >= nonRealTime->minSteps && steps <= nonRealTime->maxSteps;
            }
        }
        return false;
    }

    inline const std::string supportedStepsNRT(const SlaveDescription_t &slaveDescription) {
        std::shared_ptr<NonRealTime_t> nonRealTime = slaveDescription.OpMode.NonRealTime;
        if (nonRealTime != nullptr) {
            if (nonRealTime->fixedSteps) {
                return "(" + std::to_string(nonRealTime->defaultSteps) + ")";
            } else {
                return "(" + std::to_string(nonRealTime->minSteps) + " - " +
                       std::to_string(nonRealTime->maxSteps) + ")";
            }
        }
        return "()";
    }


    inline const bool isOpModeSupported(const SlaveDescription_t &slaveDescription, const DcpOpMode opMode) {
        if (slaveDescription.OpMode.HardRealTime != nullptr && opMode == DcpOpMode::HRT) {
            return true;
        } else if (slaveDescription.OpMode.SoftRealTime != nullptr&& opMode == DcpOpMode::SRT) {
            return true;
        } else if (slaveDescription.OpMode.NonRealTime != nullptr && opMode == DcpOpMode::NRT) {
            return true;
        }
        return false;
    }

    inline const bool
    isTransportProtocolSupported(const SlaveDescription_t &slaveDescription, DcpTransportProtocol transportProtocol) {
        switch (transportProtocol) {
            case DcpTransportProtocol::UDP_IPv4:
                return slaveDescription.TransportProtocols.UDP_IPv4.get() != nullptr;
            case DcpTransportProtocol::rfcomm_Bluetooth:
                return slaveDescription.TransportProtocols.Bluetooth.get() != nullptr;
            case DcpTransportProtocol::CAN_BASED:
                return slaveDescription.TransportProtocols.CAN;
            case DcpTransportProtocol::USB:
                return slaveDescription.TransportProtocols.USB.get() != nullptr;
            case DcpTransportProtocol::TCP_IPv4:
                return slaveDescription.TransportProtocols.TCP_IPv4.get() != nullptr;
        }
    }


    inline const bool
    isEthernetPortSupportedForInputOutput(const std::shared_ptr<Ethernet_t> &protocol, std::shared_ptr<DAT_t> type,
                                          const SlaveDescription_t &slaveDescription, uint16_t port) {
        if (protocol.get() != nullptr) {
            if (type.get() != nullptr) {
                for (const auto &range: type->availablePortRanges) {
                    if (port >= range.from && port <= range.to) {
                        return true;
                    }
                }
                for (const auto &portEl: type->availablePorts) {
                    if (portEl.port == port) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    inline const std::string
    supportedEthernetPorts(const std::shared_ptr<Ethernet_t> &protocol, std::shared_ptr<DAT_t> type,
                           const SlaveDescription_t &slaveDescription) {
        std::string str = "(";

        if (protocol.get() != nullptr) {
            if (protocol->DAT_input_output.get() != nullptr) {
                for (const auto &range: protocol->DAT_input_output->availablePortRanges) {
                    str += std::to_string(range.from) + " - " + std::to_string(range.to) + ", ";
                }
                for (const auto &portEl: protocol->DAT_input_output->availablePorts) {
                    str += std::to_string(portEl.port) + ", ";
                }
            }
        }
        if (str.size() > 2) str.resize(str.size() - 2);
        str += ")";
        return str;
    }

    inline const bool isUDPPortSupportedForInputOutput(const SlaveDescription_t &slaveDescription, uint16_t port) {
        if (slaveDescription.TransportProtocols.UDP_IPv4.get() != nullptr) {
            return isEthernetPortSupportedForInputOutput(slaveDescription.TransportProtocols.UDP_IPv4,
                                                         slaveDescription.TransportProtocols.UDP_IPv4->DAT_input_output,
                                                         slaveDescription, port);
        }
        return false;
    }


    inline const bool isTCPPortSupportedForInputOutput(const SlaveDescription_t &slaveDescription, uint16_t port) {
        if (slaveDescription.TransportProtocols.TCP_IPv4.get() != nullptr) {
            return isEthernetPortSupportedForInputOutput(slaveDescription.TransportProtocols.TCP_IPv4,
                                                         slaveDescription.TransportProtocols.TCP_IPv4->DAT_input_output,
                                                         slaveDescription, port);
        }
        return false;
    }


    inline const std::string supportedUdpPorts(const SlaveDescription_t &slaveDescription) {
        if (slaveDescription.TransportProtocols.UDP_IPv4.get() != nullptr) {
            return supportedEthernetPorts(slaveDescription.TransportProtocols.UDP_IPv4,
                                          slaveDescription.TransportProtocols.UDP_IPv4->DAT_input_output,
                                          slaveDescription);
        }
        return "";
    }

    inline const std::string supportedTCPPorts(const SlaveDescription_t &slaveDescription) {
        if (slaveDescription.TransportProtocols.TCP_IPv4.get() != nullptr) {
            return supportedEthernetPorts(slaveDescription.TransportProtocols.TCP_IPv4,
                                          slaveDescription.TransportProtocols.TCP_IPv4->DAT_input_output,
                                          slaveDescription);
        }
        return "";
    }


    inline const bool isUDPPortSupportedForParameter(const SlaveDescription_t &slaveDescription, uint16_t port) {
        if (slaveDescription.TransportProtocols.UDP_IPv4.get() != nullptr) {
            return isEthernetPortSupportedForInputOutput(slaveDescription.TransportProtocols.UDP_IPv4,
                                                         slaveDescription.TransportProtocols.UDP_IPv4->DAT_parameter,
                                                         slaveDescription, port);
        }
        return false;
    }

    inline const bool isTCPPortSupportedForParameter(const SlaveDescription_t &slaveDescription, uint16_t port) {
        if (slaveDescription.TransportProtocols.TCP_IPv4.get() != nullptr) {
            return isEthernetPortSupportedForInputOutput(slaveDescription.TransportProtocols.TCP_IPv4,
                                                         slaveDescription.TransportProtocols.TCP_IPv4->DAT_parameter,
                                                         slaveDescription, port);
        }
        return false;
    }


    inline const std::string supportedUdpPortsParameter(const SlaveDescription_t &slaveDescription) {
        if (slaveDescription.TransportProtocols.UDP_IPv4.get() != nullptr) {
            return supportedEthernetPorts(slaveDescription.TransportProtocols.UDP_IPv4,
                                          slaveDescription.TransportProtocols.UDP_IPv4->DAT_parameter,
                                          slaveDescription);
        }
        return "";
    }

    inline const std::string supportedTCPPortsParameter(const SlaveDescription_t &slaveDescription) {
        if (slaveDescription.TransportProtocols.TCP_IPv4.get() != nullptr) {
            return supportedEthernetPorts(slaveDescription.TransportProtocols.TCP_IPv4,
                                          slaveDescription.TransportProtocols.TCP_IPv4->DAT_parameter,
                                          slaveDescription);
        }
        return "";
    }

    inline const bool logCategoryExists(const SlaveDescription_t &slaveDescription, uint8_t logCategory) {
        if (slaveDescription.Log.get() != nullptr) {
            for (const auto &logCat : slaveDescription.Log->categories) {
                if (logCat.id == logCategory) {
                    return true;
                }
            }
        }
        return false;
    }
}

#endif //LIBACOSAR_ACIDESCRIPTIONREADER_H
