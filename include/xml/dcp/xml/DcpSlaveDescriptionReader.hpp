/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPSLAVEDESCRIPTIONREADER_H
#define DCPLIB_DCPSLAVEDESCRIPTIONREADER_H

#include <set>
#include <vector>
#include <limits>

#include <dcp/model/DcpTypes.hpp>
#include <dcp/xml/DcpSlaveDescriptionElements.hpp>
#include <dcp/xml/XSD.hpp>
#include <dcp/helper/DcpSlaveDescriptionHelper.hpp>


#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>


#include <xercesc/dom/DOM.hpp>

#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>

// Macros

template<typename T>
static std::vector<T> split(std::string &str) {
    std::vector<T> array;
    std::stringstream ss(str);
    T temp;
    while (ss >> temp) {
        array.push_back(temp);
    }
    return array;
}

static inline std::shared_ptr<BinaryStartValue> convertToBinary(std::string binaryStr){

    std::shared_ptr<BinaryStartValue> startValue = std::make_shared<BinaryStartValue>();
    startValue->length = binaryStr.length() / 2;
    startValue->value = new uint8_t[startValue->length];
    for(int i = 0; i < binaryStr.length() - 1; i = i + 2){
        char first = binaryStr.at(i);
        char second = binaryStr.at(i+1);
        uint8_t b = char2int(first) * 16 + char2int(second);
        startValue->value[i / 2] = b;
    }
    return startValue;
}


#define XMLCH_TO_INT(xmlcha) XMLString::parseInt(xmlcha)
#define XMLCH_TO_STRING(xmlcha) XMLString::transcode(xmlcha)
#define XMLCH_TO_FLOAT(xmlcha)  std::stof(XMLString::transcode(xmlcha))
#define XMLCH_TO_DOUBLE(xmlcha) std::stod(XMLString::transcode(xmlcha))
#define XMLCH_TO_BOOL(xmlcha) std::string(XMLCH_TO_STRING(xmlcha)).compare("true") == 0


#define PARSE_ATTR(parentNodePrefix, name, type, transform) \
std::shared_ptr<type> name; \
DOMNode* element##name = parentNodePrefix##Node->getAttributes()->getNamedItem(XMLString::transcode(#name)); \
if(element##name == NULL){ \
    name = std::shared_ptr<type>(nullptr);\
} else {\
    name = std::make_shared<type>(transform(element##name->getNodeValue()));\
}

#define PARSE_ATTR_INT(parentNodePrefix, name, type) PARSE_ATTR(parentNodePrefix, name, type, XMLCH_TO_INT)
#define PARSE_ATTR_FLOAT(parentNodePrefix, name) PARSE_ATTR(parentNodePrefix, name, float32_t, XMLCH_TO_FLOAT)
#define PARSE_ATTR_DOUBLE(parentNodePrefix, name) PARSE_ATTR(parentNodePrefix, name, float64_t, XMLCH_TO_DOUBLE)
#define PARSE_ATTR_STRING(parentNodePrefix, name) PARSE_ATTR(parentNodePrefix, name, std::string, XMLCH_TO_STRING)
#define PARSE_ATTR_BOOL(parentNodePrefix, name) PARSE_ATTR(parentNodePrefix, name, bool, XMLCH_TO_BOOL)


#define PARSE_NODE(parentNode, name)  DOMNode* name##Node = ((DOMElement*) parentNode)->getElementsByTagName(XMLString::transcode(#name))->item(0);
#define PARSE_NODE_NAME(nodePrefix) std::string nodePrefix##NodeName = XMLString::transcode(nodePrefix##Node->getNodeName());

#define DEFINE_LOOP_HEAD(parentNode, name) \
DOMNodeList* name##List = parentNode->getChildNodes();\
const XMLSize_t name##Count = name##List->getLength();\
for (XMLSize_t ix = 0; ix < name##Count; ++ix)

#define DEFINE_NODE_ITEM(name) DOMNode* name##Node = name##List->item(ix);

#define IS_ELEMENT(nodePrefix) nodePrefix##Node->getNodeType() == DOMNode::ELEMENT_NODE

#define ASSIGN_OPTIONAL(parent, name)  \
    if(name != nullptr){ \
        parent.name = name; \
    }

#define ASSIGN_OPTIONAL_PTR(parent, name)  \
    if(name != nullptr){ \
        parent->name = name; \
    }

#define ASSIGN(parent, name)  \
    if(name != nullptr){ \
        parent.name = *name; \
    }

#define ASSIGN_PTR(parent, name)  \
    if(name != nullptr){ \
        parent->name = *name; \
    }

#define PARSE_AND_ASSIGN_INT_PTR(parentNodePrefix, parent, name, type) PARSE_ATTR_INT(parentNodePrefix, name, type) ASSIGN_PTR(parent, name)
#define PARSE_AND_ASSIGN_FLOAT_PTR(parentNodePrefix, parent, name) PARSE_ATTR_FLOAT(parentNodePrefix, name) ASSIGN_PTR(parent, name)
#define PARSE_AND_ASSIGN_DOUBLE_PTR(parentNodePrefix, parent, name) PARSE_ATTR_DOUBLE(parentNodePrefix, name) ASSIGN_PTR(parent, name)
#define PARSE_AND_ASSIGN_STRING_PTR(parentNodePrefix, parent, name) PARSE_ATTR_STRING(parentNodePrefix, name) ASSIGN_PTR(parent, name)
#define PARSE_AND_ASSIGN_BOOL_PTR(parentNodePrefix, parent, name) PARSE_ATTR_BOOL(parentNodePrefix, name) ASSIGN_PTR(parent, name)

#define PARSE_AND_ASSIGN_INT(parentNodePrefix, parent, name, type) PARSE_ATTR_INT(parentNodePrefix, name, type) ASSIGN(parent, name)
#define PARSE_AND_ASSIGN_FLOAT(parentNodePrefix, parent, name) PARSE_ATTR_FLOAT(parentNodePrefix, name) ASSIGN(parent, name)
#define PARSE_AND_ASSIGN_DOUBLE(parentNodePrefix, parent, name) PARSE_ATTR_DOUBLE(parentNodePrefix, name) ASSIGN(parent, name)
#define PARSE_AND_ASSIGN_STRING(parentNodePrefix, parent, name) PARSE_ATTR_STRING(parentNodePrefix, name) ASSIGN(parent, name)
#define PARSE_AND_ASSIGN_BOOL(parentNodePrefix, parent, name) PARSE_ATTR_BOOL(parentNodePrefix, name) ASSIGN(parent, name)

#define PARSE_AND_ASSIGN_OPTIONAL_INT_PTR(parentNodePrefix, parent, name, type) PARSE_ATTR_INT(parentNodePrefix, name, type) ASSIGN_OPTIONAL_PTR(parent, name)
#define PARSE_AND_ASSIGN_OPTIONAL_FLOAT_PTR(parentNodePrefix, parent, name) PARSE_ATTR_FLOAT(parentNodePrefix, name) ASSIGN_OPTIONAL_PTR(parent, name)
#define PARSE_AND_ASSIGN_OPTIONAL_DOUBLE_PTR(parentNodePrefix, parent, name) PARSE_ATTR_DOUBLE(parentNodePrefix, name) ASSIGN_OPTIONAL_PTR(parent, name)
#define PARSE_AND_ASSIGN_OPTIONAL_STRING_PTR(parentNodePrefix, parent, name) PARSE_ATTR_STRING(parentNodePrefix, name) ASSIGN_OPTIONAL_PTR(parent, name)
#define PARSE_AND_ASSIGN_OPTIONAL_BOOL_PTR(parentNodePrefix, parent, name) PARSE_ATTR_BOOL(parentNodePrefix, name) ASSIGN_OPTIONAL_PTR(parent, name)

#define PARSE_AND_ASSIGN_OPTIONAL_INT(parentNodePrefix, parent, name, type) PARSE_ATTR_INT(parentNodePrefix, name, type) ASSIGN_OPTIONAL(parent, name)
#define PARSE_AND_ASSIGN_OPTIONAL_FLOAT(parentNodePrefix, parent, name) PARSE_ATTR_FLOAT(parentNodePrefix, name) ASSIGN_OPTIONAL(parent, name)
#define PARSE_AND_ASSIGN_OPTIONAL_DOUBLE(parentNodePrefix, parent, name) PARSE_ATTR_DOUBLE(parentNodePrefix, name) ASSIGN_OPTIONAL(parent, name)
#define PARSE_AND_ASSIGN_OPTIONAL_STRING(parentNodePrefix, parent, name) PARSE_ATTR_STRING(parentNodePrefix, name) ASSIGN_OPTIONAL(parent, name)
#define PARSE_AND_ASSIGN_OPTIONAL_BOOL(parentNodePrefix, parent, name) PARSE_ATTR_BOOL(parentNodePrefix, name) ASSIGN_OPTIONAL(parent, name)

#define PARSE_INT_DATATYPE_COMMON_CAUS(node, type) \
    PARSE_ATTR_INT(dataType, min, type) \
    PARSE_ATTR_INT(dataType, max, type) \
    PARSE_ATTR_INT(dataType, gradient, type) \
    PARSE_ATTR_STRING(dataType, start)\
    causality = make_CommonCausality_ptr<type>();\
    causality->node->start = std::shared_ptr<std::vector<type>>(\
        new std::vector<type>(split<type>(*start)));\
    causality->node->min = min;\
    causality->node->max = max;\
    causality->node->gradient = gradient;\

#define PARSE_INT_DATATYPE_OUTPUT(node, type) \
    PARSE_ATTR_INT(dataType, min, type) \
    PARSE_ATTR_INT(dataType, max, type) \
    PARSE_ATTR_INT(dataType, gradient, type) \
    PARSE_ATTR_STRING(dataType, start)\
    output = make_Output_ptr<type>();\
    if(start != nullptr){ \
        output->node->start = std::shared_ptr<std::vector<type>>(\
        new std::vector<type>(split<type>(*start)));\
    } \
    output->node->min = min;\
    output->node->max = max;\
    output->node->gradient = gradient;\

#define PARSE_INT_DATATYPE_STRUCT_PARAM(node, type) \
    PARSE_ATTR_STRING(dataType, start)\
    causality = make_StructuralParameter_ptr<type>();\
    causality->node->start = std::shared_ptr<std::vector<type>>(\
        new std::vector<type>(split<type>(*start)));\

#define PARSE_INT_DATATYPE_SIMPLE_TYPE(node, type) \
    PARSE_ATTR_INT(dataType, min, type) \
    PARSE_ATTR_INT(dataType, max, type) \
    PARSE_ATTR_INT(dataType, gradient, type) \
    SimpleType_t simpleType = make_SimpleType<type>(*name);\
    simpleType.node->min = min;\
    simpleType.node->max = max;\
    simpleType.node->gradient = gradient;\
    slaveDescription->TypeDefinitions.push_back(simpleType);\

class AciDescriptionReaderErrorHandler : public xercesc::ErrorHandler {
public:
    void warning(const xercesc::SAXParseException &ex);

    void error(const xercesc::SAXParseException &ex);

    void fatalError(const xercesc::SAXParseException &ex);

    void resetErrors();

private:
    void reportParseException(const xercesc::SAXParseException &ex);
};

void AciDescriptionReaderErrorHandler::reportParseException(const xercesc::SAXParseException &ex) {
    char *message = xercesc::XMLString::transcode(ex.getMessage());
    throw std::invalid_argument(std::string(message) + " at line " + std::to_string(ex.getLineNumber()) +
                                " column " + std::to_string(ex.getColumnNumber()));
    xercesc::XMLString::release(&message);
}

void AciDescriptionReaderErrorHandler::warning(const xercesc::SAXParseException &ex) {
    reportParseException(ex);
}

void AciDescriptionReaderErrorHandler::error(const xercesc::SAXParseException &ex) {
    reportParseException(ex);
}

void AciDescriptionReaderErrorHandler::fatalError(const xercesc::SAXParseException &ex) {
    reportParseException(ex);
}

void AciDescriptionReaderErrorHandler::resetErrors() {
}



std::shared_ptr<SlaveDescription_t> readSlaveDescription(const char *acuDFile) {
    using namespace xercesc;

    // Initialize xerces
    try {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException &toCatch) {
        char *message = XMLString::transcode(toCatch.getMessage());
        XMLString::release(&message);
        return nullptr;
    }

    std::unique_ptr<XercesDOMParser> parser(new XercesDOMParser());
    std::unique_ptr<xercesc::ErrorHandler> handler(new AciDescriptionReaderErrorHandler());

    parser->setExternalNoNamespaceSchemaLocation("slaveDescription.xsd");
    parser->setExitOnFirstFatalError(true);
    parser->setValidationConstraintFatal(true);
    parser->setValidationScheme(XercesDOMParser::Val_Auto);
    parser->setDoNamespaces(true);
    parser->setDoSchema(true);
    parser->setErrorHandler(handler.get());
    parser->useCachedGrammarInParse(true);
    parser->setHandleMultipleImports(true);
    parser->setLoadSchema(false);
    parser->setValidationSchemaFullChecking(false);
    xercesc::MemBufInputSource dcpAnnotationFile(reinterpret_cast<const XMLByte *>(xsd::dcpAnnotation.c_str()),
                                                 xsd::dcpAnnotation.size(), "dcpAnnotation.xsd");
    assert(parser->loadGrammar(dcpAnnotationFile, Grammar::SchemaGrammarType, true));
    xercesc::MemBufInputSource dcpAttributeGroupsFile(
            reinterpret_cast<const XMLByte *>(xsd::dcpAttributeGroups.c_str()), xsd::dcpAttributeGroups.size(),
            "dcpAttributeGroups.xsd");
    assert(parser->loadGrammar(dcpAttributeGroupsFile, Grammar::SchemaGrammarType, true));
    xercesc::MemBufInputSource dcpDataTypesFile(
            reinterpret_cast<const XMLByte *>(xsd::dcpDataTypes.c_str()), xsd::dcpDataTypes.size(),
            "dcpDataTypes.xsd");
    assert(parser->loadGrammar(dcpDataTypesFile, Grammar::SchemaGrammarType, true));
    xercesc::MemBufInputSource dcpTransportProtocolFile(
            reinterpret_cast<const XMLByte *>(xsd::dcpTransportProtocol.c_str()), xsd::dcpTransportProtocol.size(),
            "dcpTransportProtocolTypes.xsd");
    assert(parser->loadGrammar(dcpTransportProtocolFile, Grammar::SchemaGrammarType, true));
    xercesc::MemBufInputSource dcpTypeFile(
            reinterpret_cast<const XMLByte *>(xsd::dcpType.c_str()), xsd::dcpType.size(),
            "dcpType.xsd");
    assert(parser->loadGrammar(dcpTypeFile, Grammar::SchemaGrammarType, true));
    xercesc::MemBufInputSource dcpUnitFile(
            reinterpret_cast<const XMLByte *>(xsd::dcpUnit.c_str()), xsd::dcpUnit.size(),
            "dcpUnit.xsd");
    assert(parser->loadGrammar(dcpUnitFile, Grammar::SchemaGrammarType, true));
    xercesc::MemBufInputSource dcpVariableFile(
            reinterpret_cast<const XMLByte *>(xsd::dcpVariable.c_str()), xsd::dcpVariable.size(),
            "dcpVariable.xsd");
    assert(parser->loadGrammar(dcpVariableFile, Grammar::SchemaGrammarType, true));
    xercesc::MemBufInputSource slaveDescriptionFile(
            reinterpret_cast<const XMLByte *>(xsd::slaveDescription.c_str()), xsd::slaveDescription.size(),
            "slaveDescription.xsd");
    assert(parser->loadGrammar(slaveDescriptionFile, Grammar::SchemaGrammarType, true));

    try {
        parser->parse(XMLString::transcode(acuDFile));
    } catch (const xercesc::XMLException &toCatch) {
        char *message = xercesc::XMLString::transcode(toCatch.getMessage());
        throw std::invalid_argument(message);
    }

    DOMNode *slaveDescriptionNode;
    DOMDocument *doc;
    doc = parser->getDocument();
    slaveDescriptionNode = doc->getDocumentElement();


    /*****************************
    * slaveDescription Attributes
    *****************************/
    PARSE_ATTR_INT(slaveDescription, dcpMajorVersion, uint8_t)
    PARSE_ATTR_INT(slaveDescription, dcpMinorVersion, uint8_t)
    PARSE_ATTR_STRING(slaveDescription, dcpSlaveName)
    PARSE_ATTR_STRING(slaveDescription, uuid)
    std::shared_ptr<SlaveDescription_t> slaveDescription = make_SlaveDescription_ptr(*dcpMajorVersion, *dcpMinorVersion,
                                                                                     *dcpSlaveName,
                                                                                     *uuid);

    PARSE_AND_ASSIGN_OPTIONAL_STRING_PTR(slaveDescription, slaveDescription, description)
    PARSE_AND_ASSIGN_OPTIONAL_STRING_PTR(slaveDescription, slaveDescription, author)
    PARSE_AND_ASSIGN_OPTIONAL_STRING_PTR(slaveDescription, slaveDescription, version)
    PARSE_AND_ASSIGN_OPTIONAL_STRING_PTR(slaveDescription, slaveDescription, copyright)
    PARSE_AND_ASSIGN_OPTIONAL_STRING_PTR(slaveDescription, slaveDescription, license)
    PARSE_AND_ASSIGN_OPTIONAL_STRING_PTR(slaveDescription, slaveDescription, generationTool)
    PARSE_AND_ASSIGN_OPTIONAL_STRING_PTR(slaveDescription, slaveDescription, generationDateAndTime)

    PARSE_ATTR_STRING(slaveDescription, variableNamingConvention)
    if (variableNamingConvention != nullptr) {
        \
        if (*variableNamingConvention == "structured") {
            slaveDescription->variableNamingConvention = VariableNamingConvention::STRUCTURED;
        } else {
            slaveDescription->variableNamingConvention = VariableNamingConvention::FLAT;
        }
    }

    /*****************************
    * OP Modes
    *****************************/
    PARSE_NODE(slaveDescriptionNode, OpMode)
    {
        PARSE_NODE(OpModeNode, HardRealTime)
        if(HardRealTimeNode != nullptr){
            slaveDescription->OpMode.HardRealTime = make_HardRealTime_ptr();
        }
    }
    {
        PARSE_NODE(OpModeNode, SoftRealTime)
        if(SoftRealTimeNode != nullptr){
            slaveDescription->OpMode.SoftRealTime = make_SoftRealTime_ptr();
        }
    }
    {
        PARSE_NODE(OpModeNode, NonRealTime)
        if(NonRealTimeNode != nullptr){
            slaveDescription->OpMode.NonRealTime = make_NonRealTime_ptr();
            PARSE_AND_ASSIGN_INT_PTR(NonRealTime, slaveDescription->OpMode.NonRealTime, defaultSteps, uint32_t)
            PARSE_AND_ASSIGN_BOOL_PTR(NonRealTime, slaveDescription->OpMode.NonRealTime, fixedSteps)
            PARSE_AND_ASSIGN_INT_PTR(NonRealTime, slaveDescription->OpMode.NonRealTime, minSteps, uint32_t)
            PARSE_AND_ASSIGN_INT_PTR(NonRealTime, slaveDescription->OpMode.NonRealTime, maxSteps, uint32_t)
        }

    }

    // <xs:assert test="(count(./HardRealTime) eq 1) or (count(./SoftRealTime) eq 1) or (count(./NonRealTime) eq 1)"/>
    if (!(slaveDescription->OpMode.HardRealTime != nullptr || slaveDescription->OpMode.SoftRealTime != nullptr ||
          slaveDescription->OpMode.NonRealTime != nullptr)) {
        throw std::invalid_argument("Assert \"(count(./HardRealTime) eq 1) or (count(./SoftRealTime) eq 1) or "
                                    "(count(./NonRealTime) eq 1)\" violated");
    }

    /*****************************
    * Unit Definitions
    *****************************/
    PARSE_NODE(slaveDescriptionNode, UnitDefinitions)
    if (UnitDefinitionsNode != NULL) {
        DEFINE_LOOP_HEAD(UnitDefinitionsNode, unit) {
            DEFINE_NODE_ITEM(unit)
            if (IS_ELEMENT(unit)) {
                PARSE_NODE_NAME(unit)
                if (unitNodeName == "Unit") {
                    PARSE_ATTR_STRING(unit, name)
                    Unit_t Unit = make_Unit(*name);
                    DEFINE_LOOP_HEAD(unitNode, children) {
                        DEFINE_NODE_ITEM(children)
                        PARSE_NODE_NAME(children)
                        if (childrenNodeName == "BaseUnit") {
                            std::shared_ptr<BaseUnit_t> BaseUnit = make_BaseUnit_ptr();
                            PARSE_AND_ASSIGN_INT_PTR(children, BaseUnit, kg, int32_t)
                            PARSE_AND_ASSIGN_INT_PTR(children, BaseUnit, m, int32_t)
                            PARSE_AND_ASSIGN_INT_PTR(children, BaseUnit, s, int32_t)
                            PARSE_AND_ASSIGN_INT_PTR(children, BaseUnit, A, int32_t)
                            PARSE_AND_ASSIGN_INT_PTR(children, BaseUnit, K, int32_t)
                            PARSE_AND_ASSIGN_INT_PTR(children, BaseUnit, mol, int32_t)
                            PARSE_AND_ASSIGN_INT_PTR(children, BaseUnit, cd, int32_t)
                            PARSE_AND_ASSIGN_INT_PTR(children, BaseUnit, rad, int32_t)
                            PARSE_AND_ASSIGN_DOUBLE_PTR(children, BaseUnit, factor)
                            PARSE_AND_ASSIGN_DOUBLE_PTR(children, BaseUnit, offset)
                            Unit.BaseUnit = BaseUnit;
                        } else if (childrenNodeName == "DisplayUnit") {
                            PARSE_ATTR_STRING(children, name)
                            DisplayUnit_t DisplayUnit = make_DisplayUnit(*name);
                            PARSE_AND_ASSIGN_DOUBLE(children, DisplayUnit, factor)
                            PARSE_AND_ASSIGN_DOUBLE(children, DisplayUnit, offset)
                            Unit.DisplayUnit.push_back(DisplayUnit);
                        }
                    }
                    slaveDescription->UnitDefinitions.push_back(Unit);
                }
            }
        }
    }

    /*****************************
   * Type Definitions
   *****************************/
    PARSE_NODE(slaveDescriptionNode, TypeDefinitions)
    if (TypeDefinitionsNode != NULL) {
        DEFINE_LOOP_HEAD(TypeDefinitionsNode, SimpleType) {
            DEFINE_NODE_ITEM(SimpleType)
            if (IS_ELEMENT(SimpleType)) {
                PARSE_NODE_NAME(SimpleType)
                if (SimpleTypeNodeName == "SimpleType") {
                    PARSE_ATTR_STRING(SimpleType, name)
                    DEFINE_LOOP_HEAD(SimpleTypeNode, dataType) {
                        DEFINE_NODE_ITEM(dataType)
                        if (IS_ELEMENT(dataType)) {
                            PARSE_NODE_NAME(dataType)
                            if (dataTypeNodeName == "Int8") {
                                PARSE_INT_DATATYPE_SIMPLE_TYPE(Int8, int8_t)
                            } else if (dataTypeNodeName == "Int16") {
                                PARSE_INT_DATATYPE_SIMPLE_TYPE(Int16, int16_t)
                            } else if (dataTypeNodeName == "Int32") {
                                PARSE_INT_DATATYPE_SIMPLE_TYPE(Int32, int32_t)
                            } else if (dataTypeNodeName == "Int64") {
                                PARSE_INT_DATATYPE_SIMPLE_TYPE(Int64, int64_t)
                            } else if (dataTypeNodeName == "Uint8") {
                                PARSE_INT_DATATYPE_SIMPLE_TYPE(Uint8, uint8_t)
                            } else if (dataTypeNodeName == "Uint16") {
                                PARSE_INT_DATATYPE_SIMPLE_TYPE(Uint16, uint16_t)
                            } else if (dataTypeNodeName == "Uint32") {
                                PARSE_INT_DATATYPE_SIMPLE_TYPE(Uint32, uint32_t)
                            } else if (dataTypeNodeName == "Uint64") {
                                PARSE_INT_DATATYPE_SIMPLE_TYPE(Uint64, uint64_t)
                            } else if (dataTypeNodeName == "Float32") {
                                PARSE_ATTR_FLOAT(dataType, min)
                                PARSE_ATTR_FLOAT(dataType, max)
                                PARSE_ATTR_FLOAT(dataType, gradient)
                                PARSE_ATTR_FLOAT(dataType, nominal)
                                PARSE_ATTR_STRING(dataType, start)
                                PARSE_ATTR_STRING(dataType, quantity)
                                PARSE_ATTR_STRING(dataType, unit)
                                PARSE_ATTR_STRING(dataType, displayUnit)
                                SimpleType_t simpleType = make_SimpleType<float32_t>(*name);\
                                simpleType.Float32->min = min;
                                simpleType.Float32->max = max;
                                simpleType.Float32->gradient = gradient;
                                simpleType.Float32->nominal = nominal;
                                simpleType.Float32->quantity = quantity;
                                simpleType.Float32->unit = unit;
                                simpleType.Float32->displayUnit = displayUnit;
                            } else if (dataTypeNodeName == "Float64") {
                                PARSE_ATTR_DOUBLE(dataType, min)
                                PARSE_ATTR_DOUBLE(dataType, max)
                                PARSE_ATTR_DOUBLE(dataType, gradient)
                                PARSE_ATTR_DOUBLE(dataType, nominal)
                                PARSE_ATTR_STRING(dataType, start)
                                PARSE_ATTR_STRING(dataType, quantity)
                                PARSE_ATTR_STRING(dataType, unit)
                                PARSE_ATTR_STRING(dataType, displayUnit)
                                SimpleType_t simpleType = make_SimpleType<float64_t>(*name);\
                                simpleType.Float64->min = min;
                                simpleType.Float64->max = max;
                                simpleType.Float64->gradient = gradient;
                                simpleType.Float64->nominal = nominal;
                                simpleType.Float64->quantity = quantity;
                                simpleType.Float64->displayUnit = displayUnit;
                                simpleType.Float64->unit = unit;
                            } else if (dataTypeNodeName == "String") {
                                PARSE_ATTR_INT(dataType, maxSize, uint32_t)
                                SimpleType_t simpleType = make_SimpleType_String(*name);\
                                simpleType.String->maxSize = maxSize;
                                slaveDescription->TypeDefinitions.push_back(simpleType);
                            } else if (dataTypeNodeName == "Binary") {
                                PARSE_ATTR_INT(dataType, maxSize, uint32_t)
                                PARSE_ATTR_STRING(dataType, mimeType)
                                SimpleType_t simpleType = make_SimpleType_Binary(*name);\
                                simpleType.Binary->maxSize = maxSize;
                                simpleType.Binary->mimeType = mimeType;
                                slaveDescription->TypeDefinitions.push_back(simpleType);
                            }
                        }
                    }
                }
            }
        }
    }

    /*****************************
    * Time Resolution
    *****************************/
    PARSE_NODE(slaveDescriptionNode, TimeRes)
    DEFINE_LOOP_HEAD(TimeResNode, timeRes) {
        DEFINE_NODE_ITEM(timeRes)
        if (IS_ELEMENT(timeRes)) {
            PARSE_NODE_NAME(timeRes)
            if (timeResNodeName == "Resolution") {
                Resolution_t Resolution = make_Resolution();
                PARSE_AND_ASSIGN_INT(timeRes, Resolution, numerator, uint32_t)
                PARSE_AND_ASSIGN_INT(timeRes, Resolution, denominator, uint32_t)
                PARSE_AND_ASSIGN_BOOL(timeRes, Resolution, fixed)
                PARSE_AND_ASSIGN_OPTIONAL_BOOL(timeRes, Resolution, recommended)
                slaveDescription->TimeRes.resolutions.push_back(Resolution);
            } else if (timeResNodeName == "ResolutionRange") {
                PARSE_ATTR_INT(timeRes, numeratorFrom, uint32_t)
                PARSE_ATTR_INT(timeRes, numeratorTo, uint32_t)
                PARSE_ATTR_INT(timeRes, denominator, uint32_t)
                slaveDescription->TimeRes.resolutionRanges.push_back(
                        make_ResolutionRange(*numeratorFrom, *numeratorTo, *denominator));
            }
        }
    }

    // <xs:assert test="((count(./Resolution[@fixed = true()]) eq 1) and (count(./ResolutionRange) eq 0) and
    // (count(./Resolution) eq 1)) or (count(./Resolution[@fixed eq false()]) eq count(./Resolution))"/>
    size_t countResolution = slaveDescription->TimeRes.resolutions.size();
    size_t countResolutionRange = slaveDescription->TimeRes.resolutionRanges.size();
    size_t countResolutionFixed = 0;
    size_t countResolutionNotFixed = 0;
    for (auto &resolution: slaveDescription->TimeRes.resolutions) {
        if (resolution.fixed) {
            countResolutionFixed++;
        } else {
            countResolutionNotFixed++;
        }
    }
    if (!((countResolutionFixed == 1 && countResolutionRange == 0 && countResolution == 1) ||
          (countResolutionNotFixed == countResolution))) {
        throw std::invalid_argument("Assert \"((count(./Resolution[@fixed = true()]) eq 1) and "
                                    "(count(./ResolutionRange) eq 0) and (count(./Resolution) eq 1)) or "
                                    "(count(./Resolution[@fixed eq false()]) eq count(./Resolution))\" violated");
    }

    /*****************************
    * Heartbeat
    *****************************/
    PARSE_NODE(slaveDescriptionNode, Heartbeat)
    if (HeartbeatNode != NULL) {
        slaveDescription->Heartbeat = make_Heartbeat_ptr();
        PARSE_NODE(HeartbeatNode, MaximumPeriodicInterval)
        PARSE_AND_ASSIGN_INT(MaximumPeriodicInterval, slaveDescription->Heartbeat->MaximumPeriodicInterval, numerator,
                             uint32_t)
        PARSE_AND_ASSIGN_INT(MaximumPeriodicInterval, slaveDescription->Heartbeat->MaximumPeriodicInterval, denominator,
                             uint32_t)
    }

    /*****************************
    * Transprt Protocols
    *****************************/
    PARSE_NODE(slaveDescriptionNode, TransportProtocols)
    DEFINE_LOOP_HEAD(TransportProtocolsNode, transport) {
        DEFINE_NODE_ITEM(transport)
        if (IS_ELEMENT(transport)) {
            PARSE_NODE_NAME(transport)
            if (transportNodeName == "UDP_IPv4" || transportNodeName == "TCP_IPv4") {
                std::shared_ptr<Ethernet_t> Ethernet = make_UDP_ptr();
                PARSE_ATTR_INT(transport, maxPduSize, uint32_t)
                Ethernet->maxPduSize = *maxPduSize;
                DEFINE_LOOP_HEAD(transportNode, kind) {
                    DEFINE_NODE_ITEM(kind)
                    PARSE_NODE_NAME(kind)

                    if (kindNodeName == "Control") {
                        std::shared_ptr<Control_t> Control = make_Control_ptr();
                        PARSE_ATTR_STRING(kind, host)
                        PARSE_ATTR_INT(kind, port, uint16_t)
                        Control->host = host;
                        Control->port = port;
                        Ethernet->Control = Control;
                    } else if (kindNodeName == "DAT_input_output" || kindNodeName == "DAT_parameter") {
                        std::shared_ptr<DAT_t> DAT = make_DAT_ptr();
                        DEFINE_LOOP_HEAD(kindNode, port) {
                            DEFINE_NODE_ITEM(port)
                            PARSE_NODE_NAME(port)
                            if (portNodeName == "AvailablePortRange") {
                                PARSE_ATTR_INT(port, from, uint16_t)
                                PARSE_ATTR_INT(port, to, uint16_t)
                                DAT->availablePortRanges.push_back(make_AvailablePortRange(*from, *to));
                            } else if (portNodeName == "AvailablePort") {
                                PARSE_ATTR_INT(port, port, uint16_t)
                                DAT->availablePorts.push_back(make_AvailablePort(*port));
                            }
                        }
                        if (kindNodeName == "DAT_input_output") {
                            Ethernet->DAT_input_output = DAT;
                        } else {
                            Ethernet->DAT_parameter = DAT;
                        }
                    }
                }

                if (transportNodeName == "UDP_IPv4") {
                    slaveDescription->TransportProtocols.UDP_IPv4 = Ethernet;
                } else {
                    slaveDescription->TransportProtocols.TCP_IPv4 = Ethernet;
                }

            } else if (transportNodeName == "CAN") {
                slaveDescription->TransportProtocols.CAN = true;
            } else if (transportNodeName == "USB2") {
                slaveDescription->TransportProtocols.USB = make_USB_ptr();
                PARSE_AND_ASSIGN_OPTIONAL_INT_PTR(transport, slaveDescription->TransportProtocols.USB, maxPower,
                                                  uint8_t)
                PARSE_AND_ASSIGN_INT_PTR(transport, slaveDescription->TransportProtocols.USB, maxPduSize, uint32_t)
                DEFINE_LOOP_HEAD(transportNode, DataPipe) {
                    DEFINE_NODE_ITEM(DataPipe)
                    PARSE_NODE_NAME(DataPipe)
                    if (DataPipeNodeName == "DataPipe") {
                        PARSE_ATTR_STRING(DataPipe, direction)
                        PARSE_ATTR_INT(DataPipe, endpointAddress, uint8_t)
                        PARSE_ATTR_INT(DataPipe, intervall, uint8_t)
                        slaveDescription->TransportProtocols.USB->dataPipes.push_back(
                                make_DataPipe(*direction == "In" ? Direction::USB_DIR_IN : Direction::USB_DIR_OUT,
                                              *endpointAddress, *intervall));
                    }
                }

            } else if (transportNodeName == "Bluetooth") {
                slaveDescription->TransportProtocols.Bluetooth = make_Bluetooth_ptr();
                PARSE_AND_ASSIGN_INT_PTR(transport, slaveDescription->TransportProtocols.Bluetooth, maxPduSize,
                                         uint32_t)
                DEFINE_LOOP_HEAD(transportNode, Address) {
                    DEFINE_NODE_ITEM(Address)
                    PARSE_NODE_NAME(Address)
                    if (AddressNodeName == "Address") {
                        PARSE_ATTR_STRING(Address, bd_addr)
                        PARSE_ATTR_INT(Address, port, uint8_t)
                        Address_t address = make_Address(*bd_addr, *port);
                        PARSE_AND_ASSIGN_OPTIONAL_STRING(Address, address, alias);
                        slaveDescription->TransportProtocols.Bluetooth->addresses.push_back(address);
                    }
                }

            }
        }
    }

    /*****************************
    * Capability Flags
    *****************************/
    PARSE_NODE(slaveDescriptionNode, CapabilityFlags)
    PARSE_AND_ASSIGN_BOOL(CapabilityFlags, slaveDescription->CapabilityFlags, canAcceptConfigPdus)
    PARSE_AND_ASSIGN_BOOL(CapabilityFlags, slaveDescription->CapabilityFlags, canHandleReset)
    PARSE_AND_ASSIGN_BOOL(CapabilityFlags, slaveDescription->CapabilityFlags, canHandleVariableSteps)
    PARSE_AND_ASSIGN_BOOL(CapabilityFlags, slaveDescription->CapabilityFlags, canMonitorHeartbeat)
    PARSE_AND_ASSIGN_BOOL(CapabilityFlags, slaveDescription->CapabilityFlags, canProvideLogOnRequest)
    PARSE_AND_ASSIGN_BOOL(CapabilityFlags, slaveDescription->CapabilityFlags, canProvideLogOnNotification)

    // <xs:assert test="((./CapabilityFlags/@canMonitorHeartbeat eq true()) and boolean(./Heartbeat)) or
    // ((./CapabilityFlags/@canMonitorHeartbeat eq false()) and boolean(./Heartbeat) eq false())"/>
    if (!((slaveDescription->CapabilityFlags.canMonitorHeartbeat && slaveDescription->Heartbeat != nullptr) ||
          (!slaveDescription->CapabilityFlags.canMonitorHeartbeat && slaveDescription->Heartbeat == nullptr))) {
        throw std::invalid_argument("Assert \"((./CapabilityFlags/@canMonitorHeartbeat eq true()) and "
                                    "boolean(./Heartbeat)) or  ((./CapabilityFlags/@canMonitorHeartbeat eq false()) "
                                    "and boolean(./Heartbeat) eq false())\" violated");
    }



    /*****************************
    * Variables
    *****************************/
    PARSE_NODE(slaveDescriptionNode, Variables)
    DEFINE_LOOP_HEAD(VariablesNode, variable) {
        DEFINE_NODE_ITEM(variable)
        if (IS_ELEMENT(variable)) {
            PARSE_NODE_NAME(variable)
            if (variableNodeName == "Variable") {
                PARSE_ATTR_INT(variable, valueReference, uint64_t)
                PARSE_ATTR_STRING(variable, name)
                PARSE_ATTR_STRING(variable, description)
                PARSE_ATTR_DOUBLE(variable, preEdge)
                PARSE_ATTR_DOUBLE(variable, postEdge)
                PARSE_ATTR_INT(variable, maxConsecMissedPdus, uint32_t)
                PARSE_ATTR_STRING(variable, declaredType)
                PARSE_ATTR_STRING(variable, variability)



                Variability variabilityEnum = Variability::CONTINUOUS;
                if (*variability == "fixed") {
                    variabilityEnum = Variability::FIXED;
                } else if (*variability == "tunable") {
                    variabilityEnum = Variability::TUNABLE;
                } else if (*variability == "discrete") {
                    variabilityEnum = Variability::DISCRETE;
                } else if (*variability == "continuous") {
                    variabilityEnum = Variability::CONTINUOUS;
                }

                DEFINE_LOOP_HEAD(variableNode, children) {
                    DEFINE_NODE_ITEM(children)
                    if (IS_ELEMENT(children)) {
                        PARSE_NODE_NAME(children)

                        if (childrenNodeName == "Input" || childrenNodeName == "Parameter") {
                            std::shared_ptr<CommonCausality_t> causality;

                            DEFINE_LOOP_HEAD(childrenNode, dataType) {
                                DEFINE_NODE_ITEM(dataType)
                                if (IS_ELEMENT(dataType)) {
                                    PARSE_NODE_NAME(dataType)
                                    if (dataTypeNodeName == "Int8") {
                                        PARSE_INT_DATATYPE_COMMON_CAUS(Int8, int8_t)
                                    } else if (dataTypeNodeName == "Int16") {
                                        PARSE_INT_DATATYPE_COMMON_CAUS(Int16, int16_t)
                                    } else if (dataTypeNodeName == "Int32") {
                                        PARSE_INT_DATATYPE_COMMON_CAUS(Int32, int32_t)
                                    } else if (dataTypeNodeName == "Int64") {
                                        PARSE_INT_DATATYPE_COMMON_CAUS(Int64, int64_t)
                                    } else if (dataTypeNodeName == "Uint8") {
                                        PARSE_INT_DATATYPE_COMMON_CAUS(Uint8, uint8_t)
                                    } else if (dataTypeNodeName == "Uint16") {
                                        PARSE_INT_DATATYPE_COMMON_CAUS(Uint16, uint16_t)
                                    } else if (dataTypeNodeName == "Uint32") {
                                        PARSE_INT_DATATYPE_COMMON_CAUS(Uint32, uint32_t)
                                    } else if (dataTypeNodeName == "Uint64") {
                                        PARSE_INT_DATATYPE_COMMON_CAUS(Uint64, uint64_t)
                                    } else if (dataTypeNodeName == "Float32") {
                                        PARSE_ATTR_FLOAT(dataType, min)
                                        PARSE_ATTR_FLOAT(dataType, max)
                                        PARSE_ATTR_FLOAT(dataType, gradient)
                                        PARSE_ATTR_FLOAT(dataType, nominal)
                                        PARSE_ATTR_STRING(dataType, start)
                                        PARSE_ATTR_STRING(dataType, quantity)
                                        PARSE_ATTR_STRING(dataType, unit)
                                        PARSE_ATTR_STRING(dataType, displayUnit)

                                        causality = make_CommonCausality_ptr<float32_t>();
                                        causality->Float32->start = std::shared_ptr<std::vector<float32_t>>(
                                                new std::vector<float32_t>(split<float32_t>(*start)));
                                        causality->Float32->min = min;
                                        causality->Float32->max = max;
                                        causality->Float32->gradient = gradient;
                                        causality->Float32->nominal = nominal;
                                        causality->Float32->quantity = quantity;
                                        causality->Float32->unit = unit;
                                        causality->Float32->displayUnit = displayUnit;
                                    } else if (dataTypeNodeName == "Float64") {
                                        PARSE_ATTR_DOUBLE(dataType, min)
                                        PARSE_ATTR_DOUBLE(dataType, max)
                                        PARSE_ATTR_DOUBLE(dataType, gradient)
                                        PARSE_ATTR_DOUBLE(dataType, nominal)
                                        PARSE_ATTR_STRING(dataType, start)
                                        PARSE_ATTR_STRING(dataType, quantity)
                                        PARSE_ATTR_STRING(dataType, unit)
                                        PARSE_ATTR_STRING(dataType, displayUnit)

                                        causality = make_CommonCausality_ptr<float64_t>();
                                        causality->Float64->start = std::shared_ptr<std::vector<float64_t>>(
                                                new std::vector<float64_t>(split<float64_t>(*start)));
                                        causality->Float64->min = min;
                                        causality->Float64->max = max;
                                        causality->Float64->gradient = gradient;
                                        causality->Float64->nominal = nominal;
                                        causality->Float64->quantity = quantity;
                                        causality->Float64->unit = unit;
                                        causality->Float64->displayUnit = displayUnit;
                                    } else if (dataTypeNodeName == "String") {
                                        PARSE_ATTR_INT(dataType, maxSize, uint32_t)
                                        PARSE_ATTR_STRING(dataType, start)

                                        causality = make_CommonCausality_String_ptr();
                                        causality->String->maxSize = maxSize;
                                        causality->String->start = start;
                                    } else if (dataTypeNodeName == "Binary") {
                                        PARSE_ATTR_INT(dataType, maxSize, uint32_t)
                                        PARSE_ATTR_STRING(dataType, mimeType)
                                        PARSE_ATTR_STRING(dataType, start)

                                        causality = make_CommonCausality_Binary_ptr();
                                        causality->Binary->maxSize = maxSize;
                                        causality->Binary->mimeType = mimeType;
                                        causality->Binary->start = convertToBinary(*start);
                                    } else if (dataTypeNodeName == "Dimensions") {
                                        DEFINE_LOOP_HEAD(dataTypeNode, Dimension) {
                                            DEFINE_NODE_ITEM(Dimension)
                                            if (IS_ELEMENT(Dimension)) {
                                                PARSE_NODE_NAME(Dimension)
                                                if (DimensionNodeName == "Dimension") {
                                                    PARSE_ATTR_INT(Dimension, constant, uint64_t)
                                                    PARSE_ATTR_INT(Dimension, linkedVr, uint64_t)
                                                    // <xs:assert test="((@constant and not(@linkedVR))
                                                    // or (not(@constant) and @linkedVR))"/>
                                                    if (!((constant != nullptr && linkedVr == nullptr)
                                                          || (constant == nullptr && linkedVr != nullptr))) {
                                                        throw std::invalid_argument("Assert \"((@constant and "
                                                                                    "not(@linkedVR))  or (not(@constant) "
                                                                                    "and @linkedVR))\" violated");
                                                    }
                                                    if (constant != nullptr) {
                                                        causality->dimensions.push_back(
                                                                make_Dimension(DimensionType::CONSTANT, *constant));
                                                    } else {
                                                        causality->dimensions.push_back(
                                                                make_Dimension(DimensionType::LINKED_VR, *linkedVr));
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            if (childrenNodeName == "Input") {
                                Variable_t var = make_Variable_input(*name, *valueReference, causality);
                                var.description = description;
                                var.variability = variabilityEnum;
                                var.preEdge = preEdge;
                                var.postEdge = postEdge;
                                var.maxConsecMissedPdus = maxConsecMissedPdus;
                                slaveDescription->Variables.push_back(var);
                            } else if (childrenNodeName == "Parameter") {
                                Variable_t var = make_Variable_parameter(*name, *valueReference, causality);
                                var.description = description;
                                var.variability = variabilityEnum;
                                var.preEdge = preEdge;
                                var.postEdge = postEdge;
                                var.maxConsecMissedPdus = maxConsecMissedPdus;
                                slaveDescription->Variables.push_back(var);
                            }
                        } else if (childrenNodeName == "Output") {
                            std::shared_ptr<Output_t> output;
                            PARSE_ATTR_INT(children, defaultSteps, uint32_t)
                            PARSE_ATTR_INT(children, minSteps, uint32_t)
                            PARSE_ATTR_INT(children, maxSteps, uint32_t)
                            PARSE_ATTR_BOOL(children, fixedSteps)
                            PARSE_ATTR_BOOL(children, initialization)

                            DEFINE_LOOP_HEAD(childrenNode, dataType) {
                                DEFINE_NODE_ITEM(dataType)
                                if (IS_ELEMENT(dataType)) {
                                    PARSE_NODE_NAME(dataType)
                                    if (dataTypeNodeName == "Int8") {
                                        PARSE_INT_DATATYPE_OUTPUT(Int8, int8_t)
                                    } else if (dataTypeNodeName == "Int16") {
                                        PARSE_INT_DATATYPE_OUTPUT(Int16, int16_t)
                                    } else if (dataTypeNodeName == "Int32") {
                                        PARSE_INT_DATATYPE_OUTPUT(Int32, int32_t)
                                    } else if (dataTypeNodeName == "Int64") {
                                        PARSE_INT_DATATYPE_OUTPUT(Int64, int64_t)
                                    } else if (dataTypeNodeName == "Uint8") {
                                        PARSE_INT_DATATYPE_OUTPUT(Uint8, uint8_t)
                                    } else if (dataTypeNodeName == "Uint16") {
                                        PARSE_INT_DATATYPE_OUTPUT(Uint16, uint16_t)
                                    } else if (dataTypeNodeName == "Uint32") {
                                        PARSE_INT_DATATYPE_OUTPUT(Uint32, uint32_t)
                                    } else if (dataTypeNodeName == "Uint64") {
                                        PARSE_INT_DATATYPE_OUTPUT(Uint64, uint64_t)
                                    } else if (dataTypeNodeName == "Float32") {
                                        PARSE_ATTR_FLOAT(dataType, min)
                                        PARSE_ATTR_FLOAT(dataType, max)
                                        PARSE_ATTR_FLOAT(dataType, gradient)
                                        PARSE_ATTR_FLOAT(dataType, nominal)
                                        PARSE_ATTR_STRING(dataType, start)
                                        PARSE_ATTR_STRING(dataType, quantity)
                                        PARSE_ATTR_STRING(dataType, unit)
                                        PARSE_ATTR_STRING(dataType, displayUnit)
                                        output = make_Output_ptr<float32_t>();
                                        if(start != nullptr){
                                            output->Float32->start = std::shared_ptr<std::vector<float32_t>>(
                                                    new std::vector<float32_t>(split<float32_t>(*start)));
                                        }
                                        output->Float32->min = min;
                                        output->Float32->max = max;
                                        output->Float32->gradient = gradient;
                                        output->Float32->nominal = nominal;
                                        output->Float32->quantity = quantity;
                                        output->Float32->unit = unit;
                                        output->Float32->displayUnit = displayUnit;
                                    } else if (dataTypeNodeName == "Float64") {
                                        PARSE_ATTR_DOUBLE(dataType, min)
                                        PARSE_ATTR_DOUBLE(dataType, max)
                                        PARSE_ATTR_DOUBLE(dataType, gradient)
                                        PARSE_ATTR_DOUBLE(dataType, nominal)
                                        PARSE_ATTR_STRING(dataType, start)
                                        PARSE_ATTR_STRING(dataType, quantity)
                                        PARSE_ATTR_STRING(dataType, unit)
                                        PARSE_ATTR_STRING(dataType, displayUnit)

                                        output = make_Output_ptr<float64_t>();
                                        if(start != nullptr) {
                                            output->Float64->start = std::shared_ptr<std::vector<float64_t>>(
                                                    new std::vector<float64_t>(split<float64_t>(*start)));
                                        }
                                        output->Float64->min = min;
                                        output->Float64->max = max;
                                        output->Float64->gradient = gradient;
                                        output->Float64->nominal = nominal;
                                        output->Float64->quantity = quantity;
                                        output->Float64->unit = unit;
                                        output->Float64->displayUnit = displayUnit;
                                    } else if (dataTypeNodeName == "String") {
                                        PARSE_ATTR_INT(dataType, maxSize, uint32_t)
                                        PARSE_ATTR_STRING(dataType, start)

                                        output = make_Output_String_ptr();
                                        output->String->maxSize = maxSize;
                                        if(start != nullptr){
                                            output->String->start = start;
                                        }
                                    } else if (dataTypeNodeName == "Binary") {
                                        PARSE_ATTR_INT(dataType, maxSize, uint32_t)
                                        PARSE_ATTR_STRING(dataType, mimeType)
                                        PARSE_ATTR_STRING(dataType, start)

                                        output = make_Output_Binary_ptr();
                                        output->Binary->maxSize = maxSize;
                                        output->Binary->mimeType = mimeType;
                                        if(start != nullptr){
                                            output->Binary->start = convertToBinary(*start);
                                        }
                                    } else if (dataTypeNodeName == "Dimensions") {
                                        DEFINE_LOOP_HEAD(dataTypeNode, Dimension) {
                                            DEFINE_NODE_ITEM(Dimension)
                                            if (IS_ELEMENT(Dimension)) {
                                                PARSE_NODE_NAME(Dimension)
                                                if (DimensionNodeName == "Dimension") {
                                                    PARSE_ATTR_INT(Dimension, constant, uint64_t)
                                                    PARSE_ATTR_INT(Dimension, linkedVr, uint64_t)
                                                    // <xs:assert test="((@constant and not(@linkedVR))
                                                    // or (not(@constant) and @linkedVR))"/>
                                                    if (!((constant != nullptr && linkedVr == nullptr)
                                                          || (constant == nullptr && linkedVr != nullptr))) {
                                                        throw std::invalid_argument("Assert \"((@constant and not(@linkedVR)) "
                                                                                    "or (not(@constant) and @linkedVR))\" violated");
                                                    }
                                                    if (constant != nullptr) {
                                                        output->dimensions.push_back(
                                                                make_Dimension(DimensionType::CONSTANT, *constant));
                                                    } else {
                                                        output->dimensions.push_back(
                                                                make_Dimension(DimensionType::LINKED_VR, *linkedVr));
                                                    }
                                                }
                                            }
                                        }
                                    } else if (dataTypeNodeName == "Dependencies") {
                                        output->Dependencies = make_Dependencies_ptr();
                                        DEFINE_LOOP_HEAD(dataTypeNode, state) {
                                            DEFINE_NODE_ITEM(state)
                                            if (IS_ELEMENT(state)) {
                                                PARSE_NODE_NAME(state)
                                                if (stateNodeName == "Initialization" ||
                                                    stateNodeName == "Run") {
                                                    std::shared_ptr<DependencyState_t> depState;
                                                    if (stateNodeName == "Run") {
                                                        output->Dependencies->Run = make_DependecyState_ptr();
                                                        depState =  output->Dependencies->Run;
                                                    } else if(stateNodeName == "Initialization"){
                                                        output->Dependencies->Initialization = make_DependecyState_ptr();
                                                        depState =  output->Dependencies->Initialization;
                                                    }
                                                    DEFINE_LOOP_HEAD(stateNode, Dependency) {
                                                        DEFINE_NODE_ITEM(Dependency)
                                                        if (IS_ELEMENT(Dependency)) {
                                                            PARSE_NODE_NAME(Dependency)
                                                            if (DependencyNodeName == "Dependency") {
                                                                PARSE_ATTR_INT(Dependency, vr, uint64_t)
                                                                PARSE_ATTR_STRING(Dependency, dependencyKind);
                                                                depState->dependecies.push_back(make_Dependency(*vr,
                                                                                                               *dependencyKind ==
                                                                                                               "dependent"
                                                                                                               ?
                                                                                                               DependencyKind::DEPENDENT
                                                                                                               :
                                                                                                               DependencyKind::LINEAR));
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            output->fixedSteps = *fixedSteps;
                            output->minSteps = minSteps;
                            output->maxSteps = maxSteps;
                            output->initialization = *initialization;
                            Variable_t var = make_Variable_output(*name, *valueReference, output);
                            var.description = description;
                            var.variability = variabilityEnum;
                            var.preEdge = preEdge;
                            var.postEdge = postEdge;
                            var.maxConsecMissedPdus = maxConsecMissedPdus;
                            slaveDescription->Variables.push_back(var);
                            // <xs:assert test="(@initialization eq true()) and
                            //      (./Dependencies/Run/@none eq true()) or (@initialization eq false)"/>
                            if (!((output->initialization &&
                                   output->Dependencies != nullptr && output->Dependencies->Run == nullptr)
                                  || !output->initialization)) {
                                throw std::invalid_argument("Assert \"(@initialization eq true()) and "
                                                            "(./Dependencies/Run/@none eq true()) or "
                                                            "(@initialization eq false)\" violated");
                            }
                            // test="(@fixedSteps eq true() and @defaultSteps >= 1 and not(@minSteps) and not(@maxSteps))
                            // or (@fixedSteps eq false() and @minSteps and @maxSteps and (@maxSteps > @minSteps))"
                            if (!(((*fixedSteps && *defaultSteps >= 1 && minSteps == nullptr && maxSteps == nullptr) ||
                                   (!*fixedSteps && minSteps != nullptr && maxSteps != nullptr && *maxSteps > *minSteps)
                            ))) {
                                throw std::invalid_argument("Assert \"(@fixedSteps eq true() and @defaultSteps >= 1 and "
                                                            "not(@minSteps) and not(@maxSteps)) or (@fixedSteps eq false() "
                                                            "and @minSteps and @maxSteps and (@maxSteps > @minSteps))\" violated");
                            }
                        } else if (childrenNodeName == "StructuralParameter") {
                            std::shared_ptr<StructuralParameter_t> causality;

                            DEFINE_LOOP_HEAD(childrenNode, dataType) {
                                DEFINE_NODE_ITEM(dataType)
                                if (IS_ELEMENT(dataType)) {
                                    PARSE_NODE_NAME(dataType)
                                    if (dataTypeNodeName == "Uint8") {
                                        PARSE_INT_DATATYPE_STRUCT_PARAM(Uint8, uint8_t)
                                    } else if (dataTypeNodeName == "Uint16") {
                                        PARSE_INT_DATATYPE_STRUCT_PARAM(Uint16, uint16_t)
                                    } else if (dataTypeNodeName == "Uint32") {
                                        PARSE_INT_DATATYPE_STRUCT_PARAM(Uint32, uint32_t)
                                    } else if (dataTypeNodeName == "Uint64") {
                                        PARSE_INT_DATATYPE_STRUCT_PARAM(Uint64, uint64_t)
                                    }
                                }
                            }
                            Variable_t var = make_Variable_structuralParameter(*name, *valueReference, causality);
                            var.description = description;
                            var.variability = variabilityEnum;
                            var.preEdge = preEdge;
                            var.postEdge = postEdge;
                            var.maxConsecMissedPdus = maxConsecMissedPdus;
                            if(declaredType != nullptr){
                                var.declaredType = declaredType;
                            }
                            slaveDescription->Variables.push_back(var);
                        }
                    }
                }
                // <xs:assert test="(@variability='fixed'  and boolean(./Parameter)) or
                //                  (@variability='tunable' and boolean(./Parameter)) or
                //                  (@variability='fixed'  and boolean(./StructuralParameter)) or
                //                  (@variability='tunable' and boolean(./StructuralParameter)) or
                //                  (@variability='discrete' and boolean(./Input)) or
                //                  (@variability='continuous' and boolean(./Input)) or
                //                  (@variability='continuous' and boolean(./Output)) or
                //                  (@variability='discrete' and boolean(./Output))"/>
                if (!((slaveDescription->Variables.back().variability == Variability::FIXED &&
                       slaveDescription->Variables.back().Parameter != nullptr) ||
                      (slaveDescription->Variables.back().variability == Variability::TUNABLE &&
                       slaveDescription->Variables.back().Parameter != nullptr) ||
                      (slaveDescription->Variables.back().variability == Variability::FIXED &&
                       slaveDescription->Variables.back().StructuralParameter != nullptr) ||
                      (slaveDescription->Variables.back().variability == Variability::TUNABLE &&
                       slaveDescription->Variables.back().StructuralParameter != nullptr) ||
                      (slaveDescription->Variables.back().variability == Variability::DISCRETE &&
                       slaveDescription->Variables.back().Input != nullptr) ||
                      (slaveDescription->Variables.back().variability == Variability::CONTINUOUS &&
                       slaveDescription->Variables.back().Input != nullptr) ||
                      (slaveDescription->Variables.back().variability == Variability::DISCRETE &&
                       slaveDescription->Variables.back().Output != nullptr) ||
                      (slaveDescription->Variables.back().variability == Variability::CONTINUOUS &&
                       slaveDescription->Variables.back().Output != nullptr))) {
                    throw std::invalid_argument( "Assert \"(@variability='fixed'  and boolean(./Parameter)) or "
                                                 "(@variability='tunable' and boolean(./Parameter)) or "
                                                 "(@variability='fixed'  and boolean(./StructuralParameter)) or "
                                                 "(@variability='tunable' and boolean(./StructuralParameter)) or "
                                                 "(@variability='discrete' and boolean(./Input)) or  "
                                                 "(@variability='continuous' and boolean(./Input)) or "
                                                 "(@variability='continuous' and boolean(./Output)) or  "
                                                 "(@variability='discrete' and boolean(./Output))\" violated");
                }
            }
        }
    }

    //<xs:assert test="every $linkedVR in Variable/*/Dimensions/Dimension/@linkedVR satisfies
    //                     count(Variable[@valueReference eq $linkedVR]/StructuralParameter) = 1"/>
    for(auto& variable: slaveDescription->Variables){
        std::vector<Dimension_t>* v;
        if(variable.Input != nullptr){
            v = &variable.Input->dimensions;
        } else if(variable.Output != nullptr){
            v = &variable.Output->dimensions;
        } if(variable.Parameter != nullptr){
            v = &variable.Parameter->dimensions;
        }
        for(auto& dimension : *v){
            if(dimension.type == DimensionType::LINKED_VR){
                bool found = false;
                for(auto& variable: slaveDescription->Variables){
                    if(variable.valueReference == dimension.value){
                        if(variable.StructuralParameter == nullptr){
                            throw std::invalid_argument("Assert \"every $linkedVR in Variable/*/Dimensions/Dimension/@linkedVR "
                                                        "satisfies count(Variable[@valueReference eq $linkedVR]/StructuralParameter) "
                                                        "= 1\" violated");
                        } else {
                            found = true;
                            break;
                        }
                    }
                }
                if(!found){
                    throw std::invalid_argument("Assert \"every $linkedVR in Variable/*/Dimensions/Dimension/@linkedVR "
                                                "satisfies count(Variable[@valueReference eq $linkedVR]/StructuralParameter) "
                                                "= 1\" violated");
                }
            }
        }
    }

    /*****************************
   * Log
   *****************************/
    PARSE_NODE(slaveDescriptionNode, Log)
    if (LogNode != NULL) {
        slaveDescription->Log = make_Log_ptr();
        PARSE_NODE(LogNode, Categories)
        DEFINE_LOOP_HEAD(CategoriesNode, category) {
            DEFINE_NODE_ITEM(category)
            if (IS_ELEMENT(category)) {
                PARSE_NODE_NAME(category)
                if (categoryNodeName == "Category") {
                    PARSE_ATTR_INT(category, id, uint8_t)
                    PARSE_ATTR_STRING(category, name)
                    slaveDescription->Log->categories.push_back(make_Category(*id, *name));
                }
            }
        }

        PARSE_NODE(LogNode, Templates)
        DEFINE_LOOP_HEAD(TemplatesNode, templateEl) {
            DEFINE_NODE_ITEM(templateEl)
            if (IS_ELEMENT(templateEl)) {
                PARSE_NODE_NAME(templateEl)
                if (templateElNodeName == "Template") {
                    PARSE_ATTR_INT(templateEl, id, uint8_t)
                    PARSE_ATTR_INT(templateEl, category, uint8_t)
                    PARSE_ATTR_INT(templateEl, level, uint8_t)
                    PARSE_ATTR_STRING(templateEl, msg)
                    slaveDescription->Log->templates.push_back(make_Template(*id, *category, *level, *msg));
                }
            }
        }
    }
    // <xs:assert test="((./CapabilityFlags/@canProvideLogOnRequest eq true() or
    // ./CapabilityFlags/@canProvideLogOnNotification eq true()) and boolean(./Log)) or
    // (./CapabilityFlags/@canProvideLogOnRequest eq false() and
    // ./CapabilityFlags/@canProvideLogOnNotification eq false() and boolean(./Log) eq false())"/>
    if(!(((slaveDescription->CapabilityFlags.canProvideLogOnNotification ||
           slaveDescription->CapabilityFlags.canProvideLogOnRequest)
          && slaveDescription->Log != nullptr) ||
         ((!slaveDescription->CapabilityFlags.canProvideLogOnNotification &&
           !slaveDescription->CapabilityFlags.canProvideLogOnRequest) &&
          slaveDescription->Log == nullptr))){
        throw std::invalid_argument("Assert \"((./CapabilityFlags/@canProvideLogOnRequest eq true() or "
                                    "./CapabilityFlags/@canProvideLogOnNotification eq true()) and boolean(./Log)) or  "
                                    "./CapabilityFlags/@canProvideLogOnRequest eq false() and  "
                                    "./CapabilityFlags/@canProvideLogOnNotification eq false() and "
                                    "boolean(./Log) eq false())\" violated");
    }
    return slaveDescription;
}


#endif //DCPLIB_DCPSLAVEDESCRIPTIONREADER_H
