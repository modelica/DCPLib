/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPSLAVEDESCRIPTIONWRITER_HPP
#define DCPLIB_DCPSLAVEDESCRIPTIONWRITER_HPP

#include <dcp/model/DcpTypes.hpp>
#include <dcp/xml/DcpSlaveDescriptionElements.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <iostream>
#include <sstream>
#include <fstream>

template<typename T>
std::string to_string(std::vector<T>& vector) {
    std::ostringstream oss;
    for(auto& value: vector){
        oss << value << " ";
    }
    return oss.str();
}

template<>
std::string to_string<uint8_t>(std::vector<uint8_t>& vector) {
    std::ostringstream oss;
    for(auto& value: vector){
        oss << unsigned(value) << " ";
    }
    return oss.str();
}

#define TO_STRING_ATTR(str, parent, attribute) str += #attribute "=\"" + to_string(parent.attribute) + "\" ";
#define TO_STRING_ATTR_BOOL(str, parent, attribute) str += #attribute "=\"" + (parent.attribute ? std::string("true") : std::string("false")) + "\" ";

#define TO_STRING_ATTR_PTR(str, parent, attribute) str += #attribute "=\"" + to_string(parent->attribute) + "\" ";
#define TO_STRING_OPTIONAL_ATTR(str, parent, attribute) if(parent.attribute != nullptr) {str += #attribute "=\"" + to_string(*parent.attribute) + "\" ";}

#define TO_STRING_OPTIONAL_ATTR_PTR(str, parent, attribute) if(parent->attribute != nullptr) {str += #attribute "=\"" + to_string(*parent->attribute) + "\" ";}

#define TO_STRING_ATTR_WITH_DEFAULT(str, parent, attribute, default) if(parent.attribute != default){ str += #attribute "=\"" + to_string(parent.attribute) + "\" ";}
#define TO_STRING_ATTR_WITH_DEFAULT_PTR(str, parent, attribute, default) if(parent != nullptr && parent->attribute != default){ str += #attribute "=\"" + to_string(parent->attribute) + "\" ";}

#define TO_STRING_ATTR_WITH_DEFAULT_BOOL(str, parent, attribute, default) if(parent.attribute != default){ str += #attribute "=\"" + (parent.attribute ? std::string("true") : std::string("false")) + "\" ";}
#define TO_STRING_ATTR_WITH_DEFAULT_BOOL_PTR(str, parent, attribute, default) if(parent != nullptr && parent->attribute != default){ str += #attribute "=\"" + (parent->attribute ? std::string("true") : std::string("false")) + "\" ";}

#define TO_STRING_OPTIONAL_ATTR_BOOL(str, parent, attribute) if(parent.attribute != nullptr){ str += #attribute "=\"" + (*parent.attribute ? std::string("true") : std::string("false")) + "\" ";}

#define ADD_ATTR(str, parent, attribute) str += #attribute "=\"" + parent.attribute + "\" ";
#define ADD_OPTIONAL_ATTR(str, parent, attribute) if(parent.attribute != nullptr) {str += #attribute "=\"" + *parent.attribute + "\" ";}
#define ADD_OPTIONAL_ATTR_PTR(str, parent, attribute) if(parent->attribute != nullptr) {str += #attribute "=\"" + *parent->attribute + "\" ";}


#define NEWLINE(str) str += "\n";

static std::string to_string(std::shared_ptr<Log_t> log) {
    using namespace std;

    if (log == nullptr) {
        return "";
    }
    string str = "\t<Log>";
    NEWLINE(str)
    str += "\t\t<Categories>";
    NEWLINE(str)
    for (auto &category: log->categories) {
        str += "\t\t\t<Category ";
        TO_STRING_ATTR(str, category, id)
        ADD_ATTR(str, category, name)
        str += "/>";
        NEWLINE(str)
    }
    str += "\t\t</Categories>";
    NEWLINE(str)
    str += "\t\t<Templates>";
    NEWLINE(str)
    for (auto &templ: log->templates) {
        str += "\t\t\t<Template ";
        TO_STRING_ATTR(str, templ, id)
        TO_STRING_ATTR(str, templ, category)
        TO_STRING_ATTR(str, templ, level)
        ADD_ATTR(str, templ, msg)
        str += "/>";
        NEWLINE(str)
    }
    str += "\t\t</Templates>";
    NEWLINE(str)
    return str + "\t</Log> ";
}


static std::string to_string(std::vector<Variable_t> variables) {
    using namespace std;

    string str = "\t<Variables>";
    NEWLINE(str)
    for (auto &variable : variables) {
        str += "\t\t<Variable ";
        ADD_ATTR(str, variable, name)
        TO_STRING_ATTR(str, variable, valueReference)
        ADD_OPTIONAL_ATTR(str, variable, description)
        TO_STRING_ATTR_WITH_DEFAULT(str, variable, variability, Variability::CONTINUOUS)
        TO_STRING_OPTIONAL_ATTR(str, variable, preEdge)
        TO_STRING_OPTIONAL_ATTR(str, variable, postEdge)
        TO_STRING_OPTIONAL_ATTR(str, variable, maxConsecMissedPdus)
        ADD_OPTIONAL_ATTR(str, variable, declaredType)
        str += ">";
        NEWLINE(str)
        if (variable.Input != nullptr) {
            str += "\t\t\t<Input>";
            NEWLINE(str)
            if(variable.Input->Int8 != nullptr) {
                str += "\t\t\t\t<Int8 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Int8, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Int8, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Int8, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Int8, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Input->Int16 != nullptr) {
                str += "\t\t\t\t<Int16 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Int16, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Int16, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Int16, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Int16, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Input->Int32 != nullptr) {
                str += "\t\t\t\t<Int32 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Int32, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Int32, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Int32, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Int32, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Input->Int64 != nullptr) {
                str += "\t\t\t\t<Int64 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Int64, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Int64, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Int64, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Int64, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Input->Uint8 != nullptr) {
                str += "\t\t\t\t<Uint8 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Uint8, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Uint8, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Uint8, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Uint8, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Input->Uint16 != nullptr) {
                str += "\t\t\t\t<Uint16 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Uint16, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Uint16, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Uint16, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Uint16, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Input->Uint32 != nullptr) {
                str += "\t\t\t\t<Uint32 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Uint32, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Uint32, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Uint32, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Uint32, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Input->Uint64 != nullptr) {
                str += "\t\t\t\t<Uint64 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Uint64, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Uint64, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Uint64, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Uint64, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Input->Float32 != nullptr) {
                str += "\t\t\t\t<Float32 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Float32, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Float32, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Float32, nominal)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Float32, gradient)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Input->Float32, quantity)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Input->Float32, unit)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Input->Float32, displayUnit)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Float32, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Input->Float64 != nullptr) {
                str += "\t\t\t\t<Float64 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Float64, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Float64, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Float64, nominal)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Float64, gradient)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Input->Float64, quantity)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Input->Float64, unit)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Input->Float64, displayUnit)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Float64, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Input->String != nullptr) {
                str += "\t\t\t\t<String ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->String, maxSize)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Input->String, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Input->Binary != nullptr) {
                str += "\t\t\t\t<Binary ";
                ADD_OPTIONAL_ATTR_PTR(str, variable.Input->Binary, mimeType)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Binary, maxSize)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Input->Binary, start)
                str += "/>";
                NEWLINE(str)
            }
            if(variable.Input->dimensions.size() > 0){
                str += "\t\t\t\t<Dimensions>";
                NEWLINE(str)
                for(auto& dimension : variable.Input->dimensions){
                    str += "\t\t\t\t<Dimension ";
                    if(dimension.type == DimensionType::CONSTANT){
                        str += "constant=\"" + to_string(dimension.value) + "\" ";
                    } else {
                        str += "linkedVR=\"" + to_string(dimension.value) + "\" ";
                    }
                    str += "/>";
                    NEWLINE(str)
                }
                str += "\t\t\t</Dimensions>";
                NEWLINE(str)
            }
            str += "\t\t\t</Input>";
            NEWLINE(str)
        } else if (variable.Output != nullptr) {
            str += "\t\t\t<Output>";
            NEWLINE(str)
            if(variable.Output->Int8 != nullptr) {
                str += "\t\t\t\t<Int8 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Int8, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Int8, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Int8, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Int8, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Output->Int16 != nullptr) {
                str += "\t\t\t\t<Int16 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Int16, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Int16, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Int16, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Int16, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Output->Int32 != nullptr) {
                str += "\t\t\t\t<Int32 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Int32, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Int32, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Int32, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Int32, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Output->Int64 != nullptr) {
                str += "\t\t\t\t<Int64 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Int64, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Int64, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Int64, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Int64, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Output->Uint8 != nullptr) {
                str += "\t\t\t\t<Uint8 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Uint8, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Uint8, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Uint8, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Uint8, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Output->Uint16 != nullptr) {
                str += "\t\t\t\t<Uint16 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Uint16, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Uint16, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Uint16, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Uint16, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Output->Uint32 != nullptr) {
                str += "\t\t\t\t<Uint32 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Uint32, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Uint32, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Uint32, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Uint32, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Output->Uint64 != nullptr) {
                str += "\t\t\t\t<Uint64 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Uint64, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Uint64, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Uint64, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Uint64, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Output->Float32 != nullptr) {
                str += "\t\t\t\t<Float32 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Float32, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Float32, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Float32, nominal)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Float32, gradient)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Output->Float32, quantity)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Output->Float32, unit)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Output->Float32, displayUnit)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Float32, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Output->Float64 != nullptr) {
                str += "\t\t\t\t<Float64 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Float64, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Float64, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Float64, nominal)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Float64, gradient)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Output->Float64, quantity)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Output->Float64, unit)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Output->Float64, displayUnit)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Float64, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Output->String != nullptr) {
                str += "\t\t\t\t<String ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->String, maxSize)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Output->String, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Output->Binary != nullptr) {
                str += "\t\t\t\t<Binary ";
                ADD_OPTIONAL_ATTR_PTR(str, variable.Output->Binary, mimeType)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Binary, maxSize)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Output->Binary, start)
                str += "/>";
                NEWLINE(str)
            }
            if(variable.Output->dimensions.size() > 0){
                str += "\t\t\t\t<Dimensions>";
                NEWLINE(str)
                for(auto& dimension : variable.Output->dimensions){
                    str += "\t\t\t\t<Dimension ";
                    if(dimension.type == DimensionType::CONSTANT){
                        str += "constant=\"" + to_string(dimension.value) + "\" ";
                    } else {
                        str += "linkedVR=\"" + to_string(dimension.value) + "\" ";
                    }
                    str += "/>";
                    NEWLINE(str)
                }
                str += "\t\t\t\t</Dimensions>";
                NEWLINE(str)
            }
            if(variable.Output->Dependencies != nullptr){
                str += "\t\t\t\t<Dependencies>";
                NEWLINE(str)
                if(variable.Output->Dependencies->Initialization != nullptr){
                    str += "\t\t\t\t\t<Initialization ";
                    if(variable.Output->Dependencies->Initialization->dependecies.size() == 0){
                        str += "/>";
                        NEWLINE(str)
                    } else {
                        str += ">";
                        NEWLINE(str)
                        for(auto& dependency : variable.Output->Dependencies->Initialization->dependecies){
                            str += "\t\t\t\t\t\t<Dependency ";
                            TO_STRING_ATTR(str, dependency, vr)
                            TO_STRING_ATTR(str, dependency, dependencyKind)
                            str += "/>";
                            NEWLINE(str)
                        }
                        str += "\t\t\t\t\t</Initialization>";
                        NEWLINE(str)
                    }
                }
                if(variable.Output->Dependencies->Run != nullptr){
                    str += "\t\t\t\t\t<Run ";
                    if(variable.Output->Dependencies->Run->dependecies.size() == 0){
                        str += "/>";
                        NEWLINE(str)
                    } else {
                        str += ">";
                        NEWLINE(str)
                        for(auto& dependency : variable.Output->Dependencies->Run->dependecies){
                            str += "\t\t\t\t\t\t<Dependency ";
                            TO_STRING_ATTR(str, dependency, vr)
                            TO_STRING_ATTR(str, dependency, dependencyKind)
                            str += "/>";
                            NEWLINE(str)
                        }
                        str += "\t\t\t\t\t</Run>";
                        NEWLINE(str)
                    }
                }
                str += "\t\t\t\t</Dependencies>";
                NEWLINE(str)
            }
            str += "\t\t\t</Output>";
            NEWLINE(str)
        } else if (variable.Parameter != nullptr) {
            str += "\t\t\t<Parameter>";
            NEWLINE(str)
            if(variable.Parameter->Int8 != nullptr) {
                str += "\t\t\t\t<Int8 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Int8, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Int8, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Int8, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Int8, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Parameter->Int16 != nullptr) {
                str += "\t\t\t\t<Int16 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Int16, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Int16, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Int16, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Int16, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Parameter->Int32 != nullptr) {
                str += "\t\t\t\t<Int32 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Int32, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Int32, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Int32, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Int32, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Parameter->Int64 != nullptr) {
                str += "\t\t\t\t<Int64 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Int64, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Int64, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Int64, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Int64, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Parameter->Uint8 != nullptr) {
                str += "\t\t\t\t<Uint8 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Uint8, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Uint8, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Uint8, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Uint8, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Parameter->Uint16 != nullptr) {
                str += "\t\t\t\t<Uint16 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Uint16, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Uint16, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Uint16, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Uint16, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Parameter->Uint32 != nullptr) {
                str += "\t\t\t\t<Uint32 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Uint32, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Uint32, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Uint32, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Uint32, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Parameter->Uint64 != nullptr) {
                str += "\t\t\t\t<Uint64 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Uint64, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Uint64, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Uint64, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Uint64, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Parameter->Float32 != nullptr) {
                str += "\t\t\t\t<Float32 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Float32, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Float32, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Float32, nominal)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Float32, gradient)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Parameter->Float32, quantity)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Parameter->Float32, unit)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Parameter->Float32, displayUnit)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Float32, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Parameter->Float64 != nullptr) {
                str += "\t\t\t\t<Float64 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Float64, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Float64, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Float64, nominal)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Float64, gradient)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Parameter->Float64, quantity)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Parameter->Float64, unit)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Parameter->Float64, displayUnit)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Float64, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Parameter->String != nullptr) {
                str += "\t\t\t\t<String ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->String, maxSize)
                ADD_OPTIONAL_ATTR_PTR(str, variable.Parameter->String, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.Parameter->Binary != nullptr) {
                str += "\t\t\t\t<Binary ";
                ADD_OPTIONAL_ATTR_PTR(str, variable.Parameter->Binary, mimeType)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Binary, maxSize)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.Parameter->Binary, start)
                str += "/>";
                NEWLINE(str)
            }
            if(variable.Parameter->dimensions.size() > 0){
                str += "\t\t\t\t<Dimensions>";
                NEWLINE(str)
                for(auto& dimension : variable.Parameter->dimensions){
                    str += "\t\t\t\t<Dimension ";
                    if(dimension.type == DimensionType::CONSTANT){
                        str += "constant=\"" + to_string(dimension.value) + "\" ";
                    } else {
                        str += "linkedVR=\"" + to_string(dimension.value) + "\" ";
                    }
                    str += "/>";
                    NEWLINE(str)
                }
                str += "\t\t\t</Dimensions>";
                NEWLINE(str)
            }
            str += "\t\t\t</Parameter>";
            NEWLINE(str)
        } else if (variable.StructuralParameter != nullptr) {
            str += "\t\t\t<StructuralParameter>";
            NEWLINE(str)
            if(variable.Parameter->Uint8 != nullptr) {
                str += "\t\t\t\t<Uint8 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.StructuralParameter->Uint8, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.StructuralParameter->Uint8, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.StructuralParameter->Uint8, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.StructuralParameter->Uint8, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.StructuralParameter->Uint16 != nullptr) {
                str += "\t\t\t\t<Uint16 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.StructuralParameter->Uint16, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.StructuralParameter->Uint16, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.StructuralParameter->Uint16, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.StructuralParameter->Uint16, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.StructuralParameter->Uint32 != nullptr) {
                str += "\t\t\t\t<Uint32 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.StructuralParameter->Uint32, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.StructuralParameter->Uint32, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.StructuralParameter->Uint32, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.StructuralParameter->Uint32, start)
                str += "/>";
                NEWLINE(str)
            } else if(variable.StructuralParameter->Uint64 != nullptr) {
                str += "\t\t\t\t<Uint64 ";
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.StructuralParameter->Uint64, min)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.StructuralParameter->Uint64, max)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.StructuralParameter->Uint64, gradient)
                TO_STRING_OPTIONAL_ATTR_PTR(str, variable.StructuralParameter->Uint64, start)
                str += "/>";
                NEWLINE(str)
            }
            str += "\t\t\t</StructuralParameter>";
            NEWLINE(str)
        }
        str += "\t\t</Variable>";
        NEWLINE(str)
    }
    return str + "\t</Variables> ";
    return "";
}

static std::string to_string(CapabilityFlags_t capabilityFlags) {
    using namespace std;
    string str = "\t<CapabilityFlags ";
    TO_STRING_ATTR_WITH_DEFAULT_BOOL(str, capabilityFlags, canAcceptConfigPdus, false)
    TO_STRING_ATTR_WITH_DEFAULT_BOOL(str, capabilityFlags, canHandleReset, false)
    TO_STRING_ATTR_WITH_DEFAULT_BOOL(str, capabilityFlags, canHandleVariableSteps, false)
    TO_STRING_ATTR_WITH_DEFAULT_BOOL(str, capabilityFlags, canMonitorHeartbeat, false)
    TO_STRING_ATTR_WITH_DEFAULT_BOOL(str, capabilityFlags, canProvideLogOnRequest, false)
    TO_STRING_ATTR_WITH_DEFAULT_BOOL(str, capabilityFlags, canProvideLogOnNotification, false)
    str += "/>";
    return str;
}

static std::string to_string(TransportProtocols_t transportProtocols) {
    using namespace std;
    string str = "\t<TransportProtocols>";
    NEWLINE(str)
    if (transportProtocols.UDP_IPv4 != nullptr) {
        str += "\t\t<UDP_IPv4 ";
        TO_STRING_ATTR_PTR(str, transportProtocols.UDP_IPv4, maxPduSize)
        str += ">";
        NEWLINE(str)
        if (transportProtocols.UDP_IPv4->Control != nullptr) {
            str += "\t\t\t<Control ";
            ADD_OPTIONAL_ATTR_PTR(str, transportProtocols.UDP_IPv4->Control, host)
            TO_STRING_OPTIONAL_ATTR_PTR(str, transportProtocols.UDP_IPv4->Control, port)
            str += "/>";
            NEWLINE(str)

        }
        if (transportProtocols.UDP_IPv4->DAT_input_output != nullptr) {
            str += "\t\t\t<DAT_input_output ";
            ADD_OPTIONAL_ATTR_PTR(str, transportProtocols.UDP_IPv4->DAT_input_output, host)
            str += ">";
            NEWLINE(str)
            for (AvailablePort_t &avPort : transportProtocols.UDP_IPv4->DAT_input_output->availablePorts) {
                str += "\t\t\t\t<AvailablePort ";
                TO_STRING_ATTR(str, avPort, port)
                str += "/>";
                NEWLINE(str)
            }
            for (AvailablePortRange_t &avPortRange : transportProtocols.UDP_IPv4->DAT_input_output->availablePortRanges) {
                str += "\t\t\t\t<AvailablePortRange ";
                TO_STRING_ATTR(str, avPortRange, from)
                TO_STRING_ATTR(str, avPortRange, to)
                str += "/>";
                NEWLINE(str)
            }
            str += "\t\t\t</DAT_input_output>";
            NEWLINE(str)
        }
        if (transportProtocols.UDP_IPv4->DAT_parameter != nullptr) {
            str += "\t\t\t<DAT_parameter ";
            ADD_OPTIONAL_ATTR_PTR(str, transportProtocols.UDP_IPv4->DAT_parameter, host)
            str += ">";
            NEWLINE(str)
            for (AvailablePort_t &avPort : transportProtocols.UDP_IPv4->DAT_parameter->availablePorts) {
                str += "\t\t\t\t<AvailablePort ";
                TO_STRING_ATTR(str, avPort, port)
                str += "/>";
                NEWLINE(str)
            }
            for (AvailablePortRange_t &avPortRange : transportProtocols.UDP_IPv4->DAT_parameter->availablePortRanges) {
                str += "\t\t\t\t<AvailablePortRange ";
                TO_STRING_ATTR(str, avPortRange, from)
                TO_STRING_ATTR(str, avPortRange, to)
                str += "/>";
                NEWLINE(str)
            }
            str += "\t\t\t</DAT_parameter>";
            NEWLINE(str)
        }
        str += "\t\t</UDP_IPv4>";
        NEWLINE(str)
    }
    if (transportProtocols.CAN) {
        str += "\t\t<CAN/>";
        NEWLINE(str)
    }

    if (transportProtocols.USB != nullptr) {
        str += "\t\t<USB2 ";
        TO_STRING_OPTIONAL_ATTR_PTR(str, transportProtocols.USB, maxPower)
        TO_STRING_ATTR_PTR(str, transportProtocols.USB, maxPduSize)
        str += ">";
        NEWLINE(str)
        for (auto &dataPipe: transportProtocols.USB->dataPipes) {
            str += "\t\t\t<DataPipe ";
            TO_STRING_ATTR(str, dataPipe, direction)
            TO_STRING_ATTR(str, dataPipe, endpointAddress)
            TO_STRING_ATTR(str, dataPipe, intervall)
            str += "/>";
            NEWLINE(str)
        }
        str += "\t\t</USB2>";
        NEWLINE(str)
    }

    if (transportProtocols.Bluetooth != nullptr) {
        str += "\t\t<Bluetooth ";
        TO_STRING_ATTR_PTR(str, transportProtocols.Bluetooth, maxPduSize)
        str += ">";
        NEWLINE(str)
        for (auto &address : transportProtocols.Bluetooth->addresses) {
            str += "\t\t\t<Address ";
            ADD_ATTR(str, address, bd_addr)
            TO_STRING_ATTR(str, address, port)
            ADD_OPTIONAL_ATTR(str, address, alias)
            str += "/>";
            NEWLINE(str)
        }
        str += "\t\t</Bluetooth>";
        NEWLINE(str)
    }

    if (transportProtocols.TCP_IPv4 != nullptr) {
        str += "\t\t<TCP_IPv4 ";
        TO_STRING_ATTR_PTR(str, transportProtocols.TCP_IPv4, maxPduSize)
        str += ">";
        NEWLINE(str)
        if (transportProtocols.TCP_IPv4->Control != nullptr) {
            str += "\t\t\t<Control ";
            ADD_OPTIONAL_ATTR_PTR(str, transportProtocols.TCP_IPv4->Control, host)
            TO_STRING_OPTIONAL_ATTR_PTR(str, transportProtocols.TCP_IPv4->Control, port)
            str += "/>";
            NEWLINE(str)

        }
        if (transportProtocols.TCP_IPv4->DAT_input_output != nullptr) {
            str += "\t\t\t<DAT_input_output ";
            ADD_OPTIONAL_ATTR_PTR(str, transportProtocols.TCP_IPv4->DAT_input_output, host)
            str += ">";
            NEWLINE(str)
            for (AvailablePort_t &avPort : transportProtocols.TCP_IPv4->DAT_input_output->availablePorts) {
                str += "\t\t\t\t<AvailablePort ";
                TO_STRING_ATTR(str, avPort, port)
                str += "/>";
                NEWLINE(str)
            }
            for (AvailablePortRange_t &avPortRange : transportProtocols.TCP_IPv4->DAT_input_output->availablePortRanges) {
                str += "\t\t\t\t<AvailablePortRange ";
                TO_STRING_ATTR(str, avPortRange, from)
                TO_STRING_ATTR(str, avPortRange, to)
                str += "/>";
                NEWLINE(str)
            }
            str += "\t\t\t</DAT_input_output>";
            NEWLINE(str)
        }
        if (transportProtocols.TCP_IPv4->DAT_parameter != nullptr) {
            str += "\t\t\t<DAT_parameter ";
            ADD_OPTIONAL_ATTR_PTR(str, transportProtocols.TCP_IPv4->DAT_parameter, host)
            str += ">";
            NEWLINE(str)
            for (AvailablePort_t &avPort : transportProtocols.TCP_IPv4->DAT_parameter->availablePorts) {
                str += "\t\t\t\t<AvailablePort ";
                TO_STRING_ATTR(str, avPort, port)
                str += "/>";
                NEWLINE(str)
            }
            for (AvailablePortRange_t &avPortRange : transportProtocols.TCP_IPv4->DAT_parameter->availablePortRanges) {
                str += "\t\t\t\t<AvailablePortRange ";
                TO_STRING_ATTR(str, avPortRange, from)
                TO_STRING_ATTR(str, avPortRange, to)
                str += "/>";
                NEWLINE(str)
            }
            str += "\t\t\t</DAT_parameter>";
            NEWLINE(str)
        }
        str += "\t\t</TCP_IPv4>";
    }
    NEWLINE(str)
    return str + "\t</TransportProtocols>";
}

static std::string to_string(std::shared_ptr<Heartbeat_t> heartbeat) {
    using namespace std;
    if (heartbeat == nullptr) {
        return "";
    }
    string str = "\t<Heartbeat>";
    NEWLINE(str)
    str += "\t\t<MaximumPeriodicInterval ";
    TO_STRING_ATTR_WITH_DEFAULT(str, heartbeat->MaximumPeriodicInterval, numerator, 1)
    TO_STRING_ATTR_WITH_DEFAULT(str, heartbeat->MaximumPeriodicInterval, denominator, 1)
    str += "/>";
    NEWLINE(str)
    return str + "\t</Heartbeat>";
}

static std::string to_string(TimeRes_t timeRes) {
    if (timeRes.resolutions.size() == 0 && timeRes.resolutionRanges.size() == 0) {
        return "\t<TimeRes/>";
    }
    using namespace std;
    string str = "\t<TimeRes>";
    NEWLINE(str)
    for (auto &resolution : timeRes.resolutions) {
        str += "\t\t<Resolution ";
        TO_STRING_ATTR_WITH_DEFAULT(str, resolution, numerator, 1)
        TO_STRING_ATTR_WITH_DEFAULT(str, resolution, denominator, 1000)
        TO_STRING_ATTR_WITH_DEFAULT_BOOL(str, resolution, fixed, true)
        TO_STRING_OPTIONAL_ATTR_BOOL(str, resolution, recommended)
        str += "/>";
        NEWLINE(str)
    }
    for (auto &resolution : timeRes.resolutionRanges) {
        str += "\t\t<ResolutionRange ";
        TO_STRING_ATTR(str, resolution, numeratorFrom)
        TO_STRING_ATTR(str, resolution, numeratorTo)
        TO_STRING_ATTR(str, resolution, denominator)
        str += "/>";
        NEWLINE(str)
    }
    return str + "\t</TimeRes>";
}

static std::string to_string(std::vector<SimpleType_t> typeDefinitions) {
    using namespace std;
    if (typeDefinitions.size() == 0) {
        return "";
    }
    string str = "\t<TypeDefinitions>";
    NEWLINE(str)
    for (auto &simpleType: typeDefinitions) {
        str += "\t\t<SimpleType ";
        ADD_ATTR(str, simpleType, name)
        ADD_OPTIONAL_ATTR(str, simpleType, description)
        str += ">";
        NEWLINE(str)
        if (simpleType.Int8 != nullptr) {
            str += "\t\t\t<Int8 ";
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Int8, min)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Int8, max)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Int8, gradient)
            str += "/>";
            NEWLINE(str)
        } else if (simpleType.Int16 != nullptr) {
            str += "\t\t\t<Int16 ";
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Int16, min)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Int16, max)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Int16, gradient)
            str += "/>";
            NEWLINE(str)
        } else if (simpleType.Int32 != nullptr) {
            str += "\t\t\t<Int32 ";
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Int32, min)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Int32, max)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Int32, gradient)
            str += "/>";
            NEWLINE(str)
        } else if (simpleType.Int64 != nullptr) {
            str += "\t\t\t<Int64 ";
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Int64, min)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Int64, max)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Int64, gradient)
            str += "/>";
            NEWLINE(str)
        } else if (simpleType.Uint8 != nullptr) {
            str += "\t\t\t<Uint8 ";
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Uint8, min)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Uint8, max)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Uint8, gradient)
            str += "/>";
            NEWLINE(str)
        } else if (simpleType.Uint16 != nullptr) {
            str += "\t\t\t<Uint16 ";
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Uint16, min)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Uint16, max)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Uint16, gradient)
            str += "/>";
            NEWLINE(str)
        } else if (simpleType.Uint32 != nullptr) {
            str += "\t\t\t<Uint32 ";
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Uint32, min)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Uint32, max)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Uint32, gradient)
            str += "/>";
            NEWLINE(str)
        } else if (simpleType.Uint64 != nullptr) {
            str += "\t\t\t<Uint64 ";
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Uint64, min)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Uint64, max)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Uint64, gradient)
            str += "/>";
            NEWLINE(str)
        } else if (simpleType.Float32 != nullptr) {
            str += "\t\t\t<Float32 ";
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Float32, min)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Float32, max)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Float32, gradient)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Float32, nominal)
            ADD_OPTIONAL_ATTR_PTR(str, simpleType.Float32, quantity)
            ADD_OPTIONAL_ATTR_PTR(str, simpleType.Float32, unit)
            ADD_OPTIONAL_ATTR_PTR(str, simpleType.Float32, displayUnit)

            str += "/>";
            NEWLINE(str)
        } else if (simpleType.Float64 != nullptr) {
            str += "\t\t\t<Float64 ";
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Float64, min)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Float64, max)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Float64, gradient)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Float64, nominal)
            ADD_OPTIONAL_ATTR_PTR(str, simpleType.Float64, quantity)
            ADD_OPTIONAL_ATTR_PTR(str, simpleType.Float64, unit)
            ADD_OPTIONAL_ATTR_PTR(str, simpleType.Float64, displayUnit)
            str += "/>";
            NEWLINE(str)
        } else if (simpleType.String != nullptr) {
            str += "\t\t\t<String ";
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.String, maxSize)
            str += "/>";
            NEWLINE(str)
        } else if (simpleType.Binary != nullptr) {
            str += "\t\t\t<Binary ";
            ADD_OPTIONAL_ATTR_PTR(str, simpleType.Binary, mimeType)
            TO_STRING_OPTIONAL_ATTR_PTR(str, simpleType.Binary, maxSize)
            str += "/>";
            NEWLINE(str)
        }
        str += "\t\t</SimpleType> ";
        NEWLINE(str)
    }
    return str + "\t</TypeDefinitions>";
}

static std::string to_string(std::vector<Unit_t> unitDefinitions) {
    using namespace std;
    if (unitDefinitions.size() == 0) {
        return "";
    }
    string str = "\t<UnitDefinitions>";
    NEWLINE(str)
    for (auto &unit: unitDefinitions) {
        str += "\t\t<Unit ";
        ADD_ATTR(str, unit, name)
        str += ">";
        NEWLINE(str)
        if (unit.BaseUnit != nullptr) {
            str += "\t\t\t<BaseUnit ";
            TO_STRING_ATTR_WITH_DEFAULT_PTR(str, unit.BaseUnit, kg, 0)
            TO_STRING_ATTR_WITH_DEFAULT_PTR(str, unit.BaseUnit, m, 0)
            TO_STRING_ATTR_WITH_DEFAULT_PTR(str, unit.BaseUnit, s, 0)
            TO_STRING_ATTR_WITH_DEFAULT_PTR(str, unit.BaseUnit, A, 0)
            TO_STRING_ATTR_WITH_DEFAULT_PTR(str, unit.BaseUnit, K, 0)
            TO_STRING_ATTR_WITH_DEFAULT_PTR(str, unit.BaseUnit, mol, 0)
            TO_STRING_ATTR_WITH_DEFAULT_PTR(str, unit.BaseUnit, cd, 0)
            TO_STRING_ATTR_WITH_DEFAULT_PTR(str, unit.BaseUnit, rad, 0)
            TO_STRING_ATTR_WITH_DEFAULT_PTR(str, unit.BaseUnit, factor, 1.0)
            TO_STRING_ATTR_WITH_DEFAULT_PTR(str, unit.BaseUnit, offset, 0)
            str += "/>";
        }
        NEWLINE(str)
        for (auto &displayUnit : unit.DisplayUnit) {
            str += "\t\t\t<DisplayUnit ";
            ADD_ATTR(str, displayUnit, name)
            TO_STRING_ATTR_WITH_DEFAULT(str, displayUnit, factor, 1.0)
            TO_STRING_ATTR_WITH_DEFAULT(str, displayUnit, offset, 0)
            str += "/>";
            NEWLINE(str)
        }
        str += "\t\t</Unit>";
    }
    NEWLINE(str)
    return str + "\t</UnitDefinitions>";
}

static std::string to_string(OpMode_t opMode) {
    using namespace std;
    string str = "\t<OpMode>";
    NEWLINE(str)

    if(opMode.HardRealTime != nullptr){
        str += "\t\t<HardRealTime/>";
    }
    NEWLINE(str)
    if(opMode.SoftRealTime != nullptr){
        str += "\t\t<SoftRealTime/>";
    }
    NEWLINE(str)
    if(opMode.NonRealTime != nullptr){
        str += "\t\t<NonRealTime ";
        TO_STRING_ATTR_WITH_DEFAULT_PTR(str, opMode.NonRealTime, defaultSteps, 1)
        TO_STRING_ATTR_WITH_DEFAULT_BOOL_PTR(str, opMode.NonRealTime, fixedSteps, true)
        TO_STRING_ATTR_WITH_DEFAULT_PTR(str, opMode.NonRealTime, minSteps, 1)
        TO_STRING_ATTR_WITH_DEFAULT_PTR(str, opMode.NonRealTime, maxSteps, 1)

        str += "/>";
        NEWLINE(str)
    }


    return str + "\t</OpMode>";
}

static std::string to_string(SlaveDescription_t slaveDescription) {
    using namespace std;
    string str = "<dcpSlaveDescription ";


    TO_STRING_ATTR(str, slaveDescription, dcpMajorVersion)
    TO_STRING_ATTR(str, slaveDescription, dcpMinorVersion)
    ADD_ATTR(str, slaveDescription, dcpSlaveName)
    ADD_ATTR(str, slaveDescription, uuid)
    ADD_OPTIONAL_ATTR(str, slaveDescription, description)
    ADD_OPTIONAL_ATTR(str, slaveDescription, author)
    ADD_OPTIONAL_ATTR(str, slaveDescription, version)
    ADD_OPTIONAL_ATTR(str, slaveDescription, copyright)
    ADD_OPTIONAL_ATTR(str, slaveDescription, license)
    ADD_OPTIONAL_ATTR(str, slaveDescription, generationTool)
    ADD_OPTIONAL_ATTR(str, slaveDescription, generationDateAndTime)
    TO_STRING_ATTR(str, slaveDescription, variableNamingConvention)


    str += ">";
    NEWLINE(str)
    str += to_string(slaveDescription.OpMode);
    NEWLINE(str)
    str += to_string(slaveDescription.UnitDefinitions);
    NEWLINE(str)
    str += to_string(slaveDescription.TypeDefinitions);
    NEWLINE(str)
    str += to_string(slaveDescription.TimeRes);
    NEWLINE(str)
    str += to_string(slaveDescription.Heartbeat);
    NEWLINE(str)
    str += to_string(slaveDescription.TransportProtocols);
    NEWLINE(str)
    str += to_string(slaveDescription.CapabilityFlags);
    NEWLINE(str)
    str += to_string(slaveDescription.Variables);
    NEWLINE(str)
    str += to_string(slaveDescription.Log);
    NEWLINE(str)

    return str + "</dcpSlaveDescription>";

}


static bool writeDcpSlaveDescription(SlaveDescription_t slaveDescription, const char *dcpxFile) {
    try{
        std::string xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" + to_string(slaveDescription);
        std::ofstream outputFile;
        outputFile.open(dcpxFile);
        outputFile << xml;
        outputFile.close();
        return true;
    } catch (...) {
        return false;
    }
    return false;
}

#endif //DCPLIB_DCPSLAVEDESCRIPTIONWRITER_HPP
