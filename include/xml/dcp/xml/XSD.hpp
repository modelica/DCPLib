/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_XSD_HPP
#define DCPLIB_XSD_HPP

#include <string>

namespace xsd {

    static const std::string slaveDescription = R"delimiter(
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:vc="http://www.w3.org/2007/XMLSchema-versioning" elementFormDefault="qualified" attributeFormDefault="unqualified">
	<xs:annotation>
		<xs:documentation>Copyright(c) 2008-2011 MODELISAR consortium, 2012-2018 Modelica Association Project "FMI", 2015-2018 ACOSAR consortium, 2018-2019 Modelica Association Project "DCP".
             All rights reserved.

This file is licensed by the copyright holders under the 3-Clause BSD License
(https://opensource.org/licenses/BSD-3-Clause):

----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
----------------------------------------------------------------------------
		</xs:documentation>
	</xs:annotation>
	<xs:include schemaLocation="dcpUnit.xsd"/>
	<xs:include schemaLocation="dcpAnnotation.xsd"/>
	<xs:include schemaLocation="dcpVariable.xsd"/>
	<xs:include schemaLocation="dcpType.xsd"/>
	<xs:include schemaLocation="dcpTransportProtocolTypes.xsd"/>
	<xs:element name="dcpSlaveDescription">
		<xs:complexType>
			<xs:sequence>
				<xs:element name="OpMode">
					<xs:complexType>
						<xs:sequence>
							<xs:element name="HardRealTime" minOccurs="0"/>
							<xs:element name="SoftRealTime" minOccurs="0"/>
							<xs:element name="NonRealTime" minOccurs="0">
								<xs:complexType>
									<xs:attribute name="defaultSteps" type="xs:unsignedInt" default="1"/>
									<xs:attribute name="fixedSteps" type="xs:boolean" default="true"/>
									<xs:attribute name="minSteps" type="xs:unsignedInt">
										<xs:annotation>
											<xs:documentation>If fixedCommStepSize is not true.</xs:documentation>
										</xs:annotation>
									</xs:attribute>
									<xs:attribute name="maxSteps" type="xs:unsignedInt">
										<xs:annotation>
											<xs:documentation>If fixedCommStepSize is not true.</xs:documentation>
										</xs:annotation>
									</xs:attribute>


								</xs:complexType>
							</xs:element>
						</xs:sequence>

					</xs:complexType>
				</xs:element>
				<xs:element name="UnitDefinitions" minOccurs="0">
					<xs:complexType>
						<xs:sequence maxOccurs="unbounded">
							<xs:element name="Unit" type="dcpUnit">
								<xs:unique name="distinctDisplayUnitName">
									<xs:selector xpath="DisplayUnit"/>
									<xs:field xpath="@name"/>
								</xs:unique>
							</xs:element>
						</xs:sequence>
					</xs:complexType>
					<xs:unique name="distinctUnitName">
						<xs:selector xpath="Unit"/>
						<xs:field xpath="@name"/>
					</xs:unique>
				</xs:element>
				<xs:element name="TypeDefinitions" minOccurs="0">
					<xs:complexType>
						<xs:sequence maxOccurs="unbounded">
							<xs:element name="SimpleType" type="dcpSimpleType"/>
						</xs:sequence>
					</xs:complexType>
					<xs:unique name="distinctSimpleTypeName">
						<xs:selector xpath="SimpleType"/>
						<xs:field xpath="@name"/>
					</xs:unique>
				</xs:element>
				<xs:element name="VendorAnnotations" type="dcpAnnotation" minOccurs="0"/>
				<xs:element name="TimeRes">
					<xs:complexType>
						<xs:sequence minOccurs="0" maxOccurs="unbounded">
							<xs:choice>
								<xs:element name="Resolution">
									<xs:complexType>
										<xs:attribute name="numerator" type="xs:unsignedInt" default="1"/>
										<xs:attribute name="denominator" type="xs:unsignedInt" default="1000"/>
										<xs:attribute name="fixed" type="xs:boolean" default="true">
											<xs:annotation>
												<xs:documentation>if it's fixed, there can only be one resolution given</xs:documentation>
											</xs:annotation>
										</xs:attribute>
										<xs:attribute name="recommended" type="xs:boolean">
											<xs:annotation>
												<xs:documentation>multiple recommended resolutions possible</xs:documentation>
											</xs:annotation>
										</xs:attribute>
									</xs:complexType>
								</xs:element>
								<xs:element name="ResolutionRange">
									<xs:complexType>
										<xs:attribute name="numeratorFrom" type="xs:unsignedInt" use="required"/>
										<xs:attribute name="numeratorTo" type="xs:unsignedInt" use="required"/>
										<xs:attribute name="denominator" type="xs:unsignedInt" use="required"/>
									</xs:complexType>
								</xs:element>
							</xs:choice>
						</xs:sequence>

					</xs:complexType>
				</xs:element>
				<xs:element name="Heartbeat" minOccurs="0">
					<xs:complexType>
						<xs:sequence>
							<xs:element name="MaximumPeriodicInterval">
								<xs:complexType>
									<xs:attribute name="numerator" type="xs:unsignedInt" use="optional" default="1"/>
									<xs:attribute name="denominator" type="xs:unsignedInt" use="optional" default="1"/>
								</xs:complexType>
							</xs:element>
						</xs:sequence>
					</xs:complexType>
				</xs:element>
				<xs:element name="TransportProtocols">
					<xs:complexType>
						<xs:sequence>
							<xs:element name="UDP_IPv4" minOccurs="0">
								<xs:complexType>
									<xs:complexContent>
										<xs:extension base="dcpIPv4Type">
											<xs:attribute name="maxPduSize" type="xs:unsignedInt" default="65507"/>
										</xs:extension>
									</xs:complexContent>
								</xs:complexType>
							</xs:element>
							<xs:element name="CAN" minOccurs="0">
								<xs:complexType/>
							</xs:element>
							<xs:element name="USB2" minOccurs="0">
								<xs:complexType>
									<xs:complexContent>
										<xs:extension base="dcpUSB2Type">
											<xs:attribute name="maxPduSize" type="xs:unsignedInt" default="1024"/>
										</xs:extension>
									</xs:complexContent>
								</xs:complexType>
							</xs:element>
							<xs:element name="Bluetooth" minOccurs="0">
								<xs:complexType>
									<xs:complexContent>
										<xs:extension base="dcpBluetoothType">
											<xs:attribute name="maxPduSize" type="xs:unsignedInt" default="1011"/>
										</xs:extension>
									</xs:complexContent>
								</xs:complexType>
							</xs:element>
							<xs:element name="TCP_IPv4" minOccurs="0">
								<xs:complexType>
									<xs:complexContent>
										<xs:extension base="dcpIPv4Type">
											<xs:attribute name="maxPduSize" type="xs:unsignedInt" default="4294967267"/>
										</xs:extension>
									</xs:complexContent>
								</xs:complexType>
							</xs:element>
						</xs:sequence>
					</xs:complexType>
				</xs:element>
				<xs:element name="CapabilityFlags">
					<xs:complexType>
						<xs:attribute name="canAcceptConfigPdus" type="xs:boolean" default="false"/>
						<xs:attribute name="canHandleReset" type="xs:boolean" default="false"/>
						<xs:attribute name="canHandleVariableSteps" type="xs:boolean" default="false"/>
						<xs:attribute name="canMonitorHeartbeat" type="xs:boolean" default="false"/>
						<xs:attribute name="canProvideLogOnRequest" type="xs:boolean" use="optional" default="false"/>
						<xs:attribute name="canProvideLogOnNotification" type="xs:boolean" use="optional" default="false"/>
					</xs:complexType>
				</xs:element>
				<xs:element name="Variables">
					<xs:complexType>
						<xs:sequence maxOccurs="unbounded">
							<xs:element name="Variable" type="dcpVariable"/>
						</xs:sequence>

					</xs:complexType>
					<xs:unique name="distinctVariableName">
						<xs:selector xpath="Variable"/>
						<xs:field xpath="@name"/>
					</xs:unique>
					<xs:unique name="distinctVariableValueReference">
						<xs:selector xpath="Variable"/>
						<xs:field xpath="@valueReference"/>
					</xs:unique>
				</xs:element>
				<xs:element name="Log" minOccurs="0">
					<xs:complexType>
						<xs:sequence>
							<xs:element name="Categories">
								<xs:complexType>
									<xs:sequence maxOccurs="unbounded">
										<xs:element name="Category">
											<xs:complexType>
												<xs:attribute name="id" type="xs:unsignedByte" use="required"/>
												<xs:attribute name="name" type="xs:normalizedString" use="required"/>
											</xs:complexType>
										</xs:element>
									</xs:sequence>
								</xs:complexType>
								<xs:unique name="distinctCategoryId">
									<xs:selector xpath="Category"/>
									<xs:field xpath="@id"/>
								</xs:unique>
								<xs:unique name="distinctCategoryName">
									<xs:selector xpath="Category"/>
									<xs:field xpath="@name"/>
								</xs:unique>
							</xs:element>
							<xs:element name="Templates">
								<xs:complexType>
									<xs:sequence maxOccurs="unbounded">
										<xs:element name="Template">
											<xs:complexType>
												<xs:attribute name="id" type="xs:unsignedByte" use="required"/>
												<xs:attribute name="category" type="xs:unsignedByte" use="required"/>
												<xs:attribute name="level" type="xs:unsignedByte" use="required"/>
												<xs:attribute name="msg" type="xs:normalizedString" use="required"/>
											</xs:complexType>
										</xs:element>
									</xs:sequence>
								</xs:complexType>
								<xs:unique name="distinctTemplateId">
									<xs:selector xpath="Template"/>
									<xs:field xpath="@id"/>
								</xs:unique>
							</xs:element>
						</xs:sequence>
					</xs:complexType>
					<xs:key name="LogCategoryKey">
						<xs:selector xpath="Categories/Category"/>
						<xs:field xpath="@id"/>
					</xs:key>
					<xs:keyref name="LogCategoryKeyRef" refer="LogCategoryKey">
						<xs:selector xpath="Templates/Template"/>
						<xs:field xpath="@category"/>
					</xs:keyref>
				</xs:element>
			</xs:sequence>
			<xs:attribute name="dcpMajorVersion" type="xs:unsignedByte" use="required">
				<xs:annotation>
					<xs:documentation>Major version of implemented DCP specification</xs:documentation>
				</xs:annotation>
			</xs:attribute>
			<xs:attribute name="dcpMinorVersion" type="xs:unsignedByte" use="required">
				<xs:annotation>
					<xs:documentation>Minor version of implemented DCP specification</xs:documentation>
				</xs:annotation>
			</xs:attribute>
			<xs:attribute name="dcpSlaveName" type="xs:string" use="required">
				<xs:annotation>
					<xs:documentation>Name of the DCP slave</xs:documentation>
				</xs:annotation>
			</xs:attribute>
			<xs:attribute name="uuid" use="required">
				<xs:annotation>
					<xs:documentation>Fingerprint of xml-file content to verify that xml-file and implementation are compatible to each other</xs:documentation>
				</xs:annotation>
				<xs:simpleType>
					<xs:restriction base="xs:string">
						<xs:length value="36"/>
						<xs:pattern value="[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}"/>
					</xs:restriction>
				</xs:simpleType>
			</xs:attribute>
			<xs:attribute name="description" type="xs:string"/>
			<xs:attribute name="author" type="xs:string"/>
			<xs:attribute name="version" type="xs:normalizedString">
				<xs:annotation>
					<xs:documentation>Version of DCP slave, e.g., "1.4.1"</xs:documentation>
				</xs:annotation>
			</xs:attribute>
			<xs:attribute name="copyright" type="xs:string">
				<xs:annotation>
					<xs:documentation>Information on intellectual property copyright for this DCP slave</xs:documentation>
				</xs:annotation>
			</xs:attribute>
			<xs:attribute name="license" type="xs:string">
				<xs:annotation>
					<xs:documentation>Information on intellectual property licensing for this DCP slave</xs:documentation>
				</xs:annotation>
			</xs:attribute>
			<xs:attribute name="generationTool" type="xs:normalizedString"/>
			<xs:attribute name="generationDateAndTime" type="xs:dateTime"/>
			<xs:attribute name="variableNamingConvention" use="optional" default="flat">
				<xs:simpleType>
					<xs:restriction base="xs:normalizedString">
						<xs:enumeration value="flat"/>
						<xs:enumeration value="structured"/>
					</xs:restriction>
				</xs:simpleType>
			</xs:attribute>


		</xs:complexType>
		<xs:key name="SimpleTypeKey">
			<xs:selector xpath="TypeDefinitions/SimpleType"/>
			<xs:field xpath="@name"/>
		</xs:key>
		<xs:keyref name="SimpleTypeKeyRef" refer="SimpleTypeKey">
			<xs:selector xpath="Variables/Variable"/>
			<xs:field xpath="@declaredType"/>
		</xs:keyref>
	</xs:element>
</xs:schema>
    )delimiter";

    static const std::string dcpAttributeGroups = R"delimiter(
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:vc="http://www.w3.org/2007/XMLSchema-versioning" elementFormDefault="qualified" attributeFormDefault="unqualified">
	<xs:annotation>
		<xs:documentation>Copyright(c) 2008-2011 MODELISAR consortium, 2012-2018 Modelica Association Project "FMI", 2015-2018 ACOSAR consortium, 2018-2019 Modelica Association Project "DCP".
             All rights reserved.

This file is licensed by the copyright holders under the 3-Clause BSD License
(https://opensource.org/licenses/BSD-3-Clause):

----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
----------------------------------------------------------------------------
		</xs:documentation>
	</xs:annotation>
	<xs:attributeGroup name="commonAttributes"/>
	<xs:attributeGroup name="dcpInteger8Attributes">
		<xs:attribute name="min" type="xs:byte"/>
		<xs:attribute name="max" type="xs:byte">
			<xs:annotation>
				<xs:documentation/>
			</xs:annotation>
		</xs:attribute>
		<xs:attribute name="gradient" type="xs:byte"/>
	</xs:attributeGroup>
	<xs:attributeGroup name="dcpInteger16Attributes">
		<xs:attribute name="min" type="xs:short"/>
		<xs:attribute name="max" type="xs:short">
			<xs:annotation>
				<xs:documentation/>
			</xs:annotation>
		</xs:attribute>
		<xs:attribute name="gradient" type="xs:short"/>
	</xs:attributeGroup>
	<xs:attributeGroup name="dcpInteger32Attributes">
		<xs:attribute name="min" type="xs:int"/>
		<xs:attribute name="max" type="xs:int">
			<xs:annotation>
				<xs:documentation/>
			</xs:annotation>
		</xs:attribute>
		<xs:attribute name="gradient" type="xs:int"/>
	</xs:attributeGroup>
	<xs:attributeGroup name="dcpInteger64Attributes">
		<xs:attribute name="min" type="xs:long"/>
		<xs:attribute name="max" type="xs:long">
			<xs:annotation>
				<xs:documentation/>
			</xs:annotation>
		</xs:attribute>
		<xs:attribute name="gradient" type="xs:long"/>
	</xs:attributeGroup>
	<xs:attributeGroup name="dcpUnsignedInteger8Attributes">
		<xs:attribute name="min" type="xs:unsignedByte"/>
		<xs:attribute name="max" type="xs:unsignedByte"/>
		<xs:attribute name="gradient" type="xs:unsignedByte"/>
	</xs:attributeGroup>
	<xs:attributeGroup name="dcpUnsignedInteger16Attributes">
		<xs:attribute name="min" type="xs:unsignedShort"/>
		<xs:attribute name="max" type="xs:unsignedShort"/>
		<xs:attribute name="gradient" type="xs:unsignedShort"/>
	</xs:attributeGroup>
	<xs:attributeGroup name="dcpUnsignedInteger32Attributes">
		<xs:attribute name="min" type="xs:unsignedInt"/>
		<xs:attribute name="max" type="xs:unsignedInt"/>
		<xs:attribute name="gradient" type="xs:unsignedInt"/>
	</xs:attributeGroup>
	<xs:attributeGroup name="dcpUnsignedInteger64Attributes">
		<xs:attribute name="min" type="xs:unsignedLong"/>
		<xs:attribute name="max" type="xs:unsignedLong"/>
		<xs:attribute name="gradient" type="xs:unsignedLong"/>
	</xs:attributeGroup>
	<xs:attributeGroup name="dcpFloat32Attributes">
		<xs:attribute name="min" type="xs:float"/>
		<xs:attribute name="max" type="xs:float"/>
		<xs:attribute name="nominal" type="xs:float"/>
		<xs:attribute name="gradient" type="xs:float"/>
		<xs:attributeGroup ref="dcpFloatAttributes"/>
	</xs:attributeGroup>
	<xs:attributeGroup name="dcpFloat64Attributes">
		<xs:attribute name="min" type="xs:double"/>
		<xs:attribute name="max" type="xs:double"/>
		<xs:attribute name="nominal" type="xs:double"/>
		<xs:attribute name="gradient" type="xs:double"/>
		<xs:attributeGroup ref="dcpFloatAttributes"/>
	</xs:attributeGroup>
	<xs:attributeGroup name="dcpStringAttributes">
		<xs:attribute name="maxSize" type="xs:unsignedInt"/>
	</xs:attributeGroup>
	<xs:attributeGroup name="dcpBinaryAttributes">
		<xs:attribute name="mimeType" type="xs:normalizedString" default="application/octet-stream"/>
		<xs:attribute name="maxSize" type="xs:unsignedInt"/>
	</xs:attributeGroup>
	<xs:attributeGroup name="dcpFloatAttributes">
		<xs:attribute name="quantity" type="xs:normalizedString"/>
		<xs:attribute name="unit" type="xs:normalizedString"/>
		<xs:attribute name="displayUnit" type="xs:normalizedString"/>
	</xs:attributeGroup>
</xs:schema>
    )delimiter";

    static const std::string dcpAnnotation = R"delimiter(
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema" elementFormDefault="qualified" attributeFormDefault="unqualified">
	<xs:annotation>
		<xs:documentation>Copyright(c) 2008-2011 MODELISAR consortium, 2012-2018 Modelica Association Project "FMI", 2015-2018 ACOSAR consortium, 2018-2019 Modelica Association Project "DCP".
             All rights reserved.

This file is licensed by the copyright holders under the 3-Clause BSD License
(https://opensource.org/licenses/BSD-3-Clause):

----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
----------------------------------------------------------------------------
		</xs:documentation>
	</xs:annotation>
	<xs:complexType name="dcpAnnotation">
		<xs:sequence maxOccurs="unbounded">
			<xs:element name="Tool">
				<xs:annotation>
					<xs:documentation>Tool specific annotation (ignored by other tools).</xs:documentation>
				</xs:annotation>
				<xs:complexType>
					<xs:sequence>
						<xs:any namespace="##any" processContents="lax" minOccurs="0"/>
					</xs:sequence>
					<xs:attribute name="name" type="xs:normalizedString" use="required">
						<xs:annotation>
							<xs:documentation>Name of tool that can interpret the annotation. "name" must be unique with respect to all other elements of the VendorAnnotation list.</xs:documentation>
						</xs:annotation>
					</xs:attribute>
				</xs:complexType>
			</xs:element>
		</xs:sequence>
	</xs:complexType>
</xs:schema>
    )delimiter";

    static const std::string dcpDataTypes = R"delimiter(
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:vc="http://www.w3.org/2007/XMLSchema-versioning" elementFormDefault="qualified" attributeFormDefault="unqualified" vc:minVersion="1.1">
	<xs:annotation>
		<xs:documentation>Copyright(c) 2008-2011 MODELISAR consortium, 2012-2018 Modelica Association Project "FMI", 2015-2018 ACOSAR consortium, 2018-2019 Modelica Association Project "DCP".
             All rights reserved.

This file is licensed by the copyright holders under the 3-Clause BSD License
(https://opensource.org/licenses/BSD-3-Clause):

----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
----------------------------------------------------------------------------
		</xs:documentation>
	</xs:annotation>
	<xs:include schemaLocation="dcpAttributeGroups.xsd"/>
	<xs:complexType name="baseType">
		<xs:attributeGroup ref="commonAttributes"/>
	</xs:complexType>
	<xs:complexType name="dcpInteger8Type">
		<xs:complexContent>
			<xs:extension base="baseType">
				<xs:attributeGroup ref="dcpInteger8Attributes"/>
			</xs:extension>
		</xs:complexContent>
	</xs:complexType>
	<xs:complexType name="dcpInteger16Type">
		<xs:complexContent>
			<xs:extension base="baseType">
				<xs:attributeGroup ref="dcpInteger16Attributes"/>
			</xs:extension>
		</xs:complexContent>
	</xs:complexType>
	<xs:complexType name="dcpInteger32Type">
		<xs:complexContent>
			<xs:extension base="baseType">
				<xs:attributeGroup ref="dcpInteger32Attributes"/>
			</xs:extension>
		</xs:complexContent>
	</xs:complexType>
	<xs:complexType name="dcpInteger64Type">
		<xs:complexContent>
			<xs:extension base="baseType">
				<xs:attributeGroup ref="dcpInteger64Attributes"/>
			</xs:extension>
		</xs:complexContent>
	</xs:complexType>
	<xs:complexType name="dcpUnsignedInteger8Type">
		<xs:complexContent>
			<xs:extension base="baseType">
				<xs:attributeGroup ref="dcpUnsignedInteger8Attributes"/>
			</xs:extension>
		</xs:complexContent>
	</xs:complexType>
	<xs:complexType name="dcpUnsignedInteger16Type">
		<xs:complexContent>
			<xs:extension base="baseType">
				<xs:attributeGroup ref="dcpUnsignedInteger16Attributes"/>
			</xs:extension>
		</xs:complexContent>
	</xs:complexType>
	<xs:complexType name="dcpUnsignedInteger32Type">
		<xs:complexContent>
			<xs:extension base="baseType">
				<xs:attributeGroup ref="dcpUnsignedInteger32Attributes"/>
			</xs:extension>
		</xs:complexContent>
	</xs:complexType>
	<xs:complexType name="dcpUnsignedInteger64Type">
		<xs:complexContent>
			<xs:extension base="baseType">
				<xs:attributeGroup ref="dcpUnsignedInteger64Attributes"/>
			</xs:extension>
		</xs:complexContent>
	</xs:complexType>
	<xs:complexType name="dcpFloat32Type">
		<xs:complexContent>
			<xs:extension base="baseType">
				<xs:attributeGroup ref="dcpFloat32Attributes"/>
			</xs:extension>
		</xs:complexContent>
	</xs:complexType>
	<xs:complexType name="dcpFloat64Type">
		<xs:complexContent>
			<xs:extension base="baseType">
				<xs:attributeGroup ref="dcpFloat64Attributes"/>
			</xs:extension>
		</xs:complexContent>
	</xs:complexType>
	<xs:complexType name="dcpStringType">
		<xs:complexContent>
			<xs:extension base="baseType">
				<xs:attributeGroup ref="dcpStringAttributes"/>
			</xs:extension>
		</xs:complexContent>
	</xs:complexType>
	<xs:complexType name="dcpBinaryType">
		<xs:complexContent>
			<xs:extension base="baseType">
				<xs:attributeGroup ref="dcpBinaryAttributes"/>
			</xs:extension>
		</xs:complexContent>
	</xs:complexType>
</xs:schema>
    )delimiter";

    static const std::string dcpTransportProtocol = R"delimiter(
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:vc="http://www.w3.org/2007/XMLSchema-versioning" elementFormDefault="qualified" attributeFormDefault="unqualified">
	<xs:annotation>
		<xs:documentation>Copyright(c) 2008-2011 MODELISAR consortium, 2012-2018 Modelica Association Project "FMI", 2015-2018 ACOSAR consortium, 2018-2019 Modelica Association Project "DCP".
             All rights reserved.

This file is licensed by the copyright holders under the 3-Clause BSD License
(https://opensource.org/licenses/BSD-3-Clause):

----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
----------------------------------------------------------------------------
		</xs:documentation>
	</xs:annotation>
	<xs:complexType name="dcpIPv4Type">
		<xs:sequence>
			<xs:element name="Control" minOccurs="0">
				<xs:complexType>
					<xs:attribute name="host" type="xs:normalizedString"/>
					<xs:attribute name="port" type="xs:unsignedShort"/>
				</xs:complexType>
			</xs:element>
			<xs:element name="DAT_input_output" minOccurs="0">
				<xs:complexType>
					<xs:sequence minOccurs="0" maxOccurs="unbounded">
						<xs:choice>
							<xs:element name="AvailablePortRange">
								<xs:complexType>
									<xs:attribute name="from" type="xs:unsignedShort" use="required"/>
									<xs:attribute name="to" type="xs:unsignedShort" use="required"/>
								</xs:complexType>
							</xs:element>
							<xs:element name="AvailablePort">
								<xs:complexType>
									<xs:attribute name="port" type="xs:unsignedShort" use="required"/>
								</xs:complexType>
							</xs:element>
						</xs:choice>
					</xs:sequence>
					<xs:attribute name="host" type="xs:normalizedString"/>
				</xs:complexType>
			</xs:element>
			<xs:element name="DAT_parameter" minOccurs="0">
				<xs:complexType>
					<xs:sequence minOccurs="0" maxOccurs="unbounded">
						<xs:choice>
							<xs:element name="AvailablePortRange">
								<xs:complexType>
									<xs:attribute name="from" type="xs:unsignedShort" use="required"/>
									<xs:attribute name="to" type="xs:unsignedShort" use="required"/>
								</xs:complexType>
							</xs:element>
							<xs:element name="AvailablePort">
								<xs:complexType>
									<xs:attribute name="port" type="xs:unsignedShort" use="required"/>
								</xs:complexType>
							</xs:element>
						</xs:choice>
					</xs:sequence>
					<xs:attribute name="host" type="xs:normalizedString"/>
				</xs:complexType>
			</xs:element>
		</xs:sequence>
	</xs:complexType>
	<xs:complexType name="dcpBluetoothType">
		<xs:sequence>
			<xs:element name="Address">
				<xs:complexType>
					<xs:attribute name="bd_addr" use="required">
						<xs:simpleType>
							<xs:restriction base="xs:string">
								<xs:pattern value="([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})"/>
							</xs:restriction>
						</xs:simpleType>
					</xs:attribute>
					<xs:attribute name="port" use="required">
						<xs:simpleType>
							<xs:restriction base="xs:unsignedByte">
								<xs:minInclusive value="1"/>
								<xs:maxInclusive value="30"/>
							</xs:restriction>
						</xs:simpleType>
					</xs:attribute>
					<xs:attribute name="alias" type="xs:normalizedString" use="optional"/>
				</xs:complexType>
			</xs:element>
		</xs:sequence>
	</xs:complexType>
	<xs:complexType name="dcpUSB2Type">
		<xs:sequence>
			<xs:sequence minOccurs="0" maxOccurs="unbounded">
				<xs:element name="DataPipe">
					<xs:complexType>
						<xs:attribute name="direction">
							<xs:simpleType>
								<xs:restriction base="xs:string">
									<xs:enumeration value="In"/>
									<xs:enumeration value="Out"/>
								</xs:restriction>
							</xs:simpleType>
						</xs:attribute>
						<xs:attribute name="endpointAddress" type="xs:unsignedByte" use="required">
							<xs:annotation>
								<xs:documentation>Has to be constructed according to the USB standard. The endpoint number has to be greater than 2.</xs:documentation>
							</xs:annotation>
						</xs:attribute>
						<xs:attribute name="intervall" type="xs:unsignedByte" use="required">
							<xs:annotation>
								<xs:documentation>Interval for polling endpoint data transfers. Value in frame counts.</xs:documentation>
							</xs:annotation>
						</xs:attribute>
					</xs:complexType>
				</xs:element>
			</xs:sequence>
		</xs:sequence>
		<xs:attribute name="maxPower" type="xs:unsignedByte">
			<xs:annotation>
				<xs:documentation>Maximum Power Consumption in 2mA units.</xs:documentation>
			</xs:annotation>
		</xs:attribute>
	</xs:complexType>
</xs:schema>
    )delimiter";

    static const std::string dcpType = R"delimiter(
<xs:schema xmlns:ds="http://www.dcp-standard.org/dcpType.xsd" xmlns:xs="http://www.w3.org/2001/XMLSchema" elementFormDefault="qualified" attributeFormDefault="unqualified">
	<xs:annotation>
		<xs:documentation>Copyright(c) 2008-2011 MODELISAR consortium, 2012-2018 Modelica Association Project "FMI", 2015-2018 ACOSAR consortium, 2018-2019 Modelica Association Project "DCP".
             All rights reserved.

This file is licensed by the copyright holders under the 3-Clause BSD License
(https://opensource.org/licenses/BSD-3-Clause):

----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
----------------------------------------------------------------------------
		</xs:documentation>
	</xs:annotation>
	<xs:include schemaLocation="dcpAttributeGroups.xsd"/>
	<xs:include schemaLocation="dcpDataTypes.xsd"/>
	<xs:complexType name="dcpSimpleType">
		<xs:annotation>
			<xs:documentation>Type attributes of a scalar variable</xs:documentation>
		</xs:annotation>
		<xs:choice>
			<xs:element name="Int8">
				<xs:complexType>
					<xs:complexContent>
						<xs:extension base="dcpInteger8Type"/>
					</xs:complexContent>
				</xs:complexType>
			</xs:element>
			<xs:element name="Int16" type="dcpInteger16Type"/>
			<xs:element name="Int32" type="dcpInteger32Type"/>
			<xs:element name="Int64" type="dcpInteger64Type"/>
			<xs:element name="Uint8" type="dcpUnsignedInteger8Type"/>
			<xs:element name="Uint16" type="dcpUnsignedInteger16Type"/>
			<xs:element name="Uint32" type="dcpUnsignedInteger32Type"/>
			<xs:element name="Uint64" type="dcpUnsignedInteger64Type"/>
			<xs:element name="Float32" type="dcpFloat32Type"/>
			<xs:element name="Float64" type="dcpFloat64Type"/>
			<xs:element name="String" type="dcpStringType"/>
			<xs:element name="Binary" type="dcpBinaryType"/>
		</xs:choice>
		<xs:attribute name="name" type="xs:normalizedString" use="required">
			<xs:annotation>
				<xs:documentation>Name of SimpleType element. "name" must be unique with respect to all other elements of the TypeDefinitions list. Furthermore,  "name" of a SimpleType must be different to all "name"s of ScalarVariable.</xs:documentation>
			</xs:annotation>
		</xs:attribute>
		<xs:attribute name="description" type="xs:string">
			<xs:annotation>
				<xs:documentation>Description of the SimpleType</xs:documentation>
			</xs:annotation>
		</xs:attribute>
	</xs:complexType>
</xs:schema>
    )delimiter";

    static const std::string dcpUnit = R"delimiter(
<!-- edited with XMLSpy v2014 rel. 2 (http://www.altova.com) by Martin Krammer (Kompetenzzentrum -) --><xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema" elementFormDefault="qualified" attributeFormDefault="unqualified">
	<xs:annotation>
		<xs:documentation>Copyright(c) 2008-2011 MODELISAR consortium, 2012-2018 Modelica Association Project "FMI", 2015-2018 ACOSAR consortium, 2018-2019 Modelica Association Project "DCP".
             All rights reserved.

This file is licensed by the copyright holders under the 3-Clause BSD License
(https://opensource.org/licenses/BSD-3-Clause):

----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
----------------------------------------------------------------------------
		</xs:documentation>
	</xs:annotation>
	<xs:complexType name="dcpUnit">
		<xs:annotation>
			<xs:documentation>Unit definition (with respect to SI base units) and default display units</xs:documentation>
		</xs:annotation>
		<xs:sequence>
			<xs:element name="BaseUnit" minOccurs="0">
				<xs:annotation>
					<xs:documentation>BaseUnit_value = factor*Unit_value + offset</xs:documentation>
				</xs:annotation>
				<xs:complexType>
					<xs:attribute name="kg" type="xs:int" default="0">
						<xs:annotation>
							<xs:documentation>Exponent of SI base unit "kg"</xs:documentation>
						</xs:annotation>
					</xs:attribute>
					<xs:attribute name="m" type="xs:int" default="0">
						<xs:annotation>
							<xs:documentation>Exponent of SI base unit "m"</xs:documentation>
						</xs:annotation>
					</xs:attribute>
					<xs:attribute name="s" type="xs:int" default="0">
						<xs:annotation>
							<xs:documentation>Exponent of SI base unit "s"</xs:documentation>
						</xs:annotation>
					</xs:attribute>
					<xs:attribute name="A" type="xs:int" default="0">
						<xs:annotation>
							<xs:documentation>Exponent of SI base unit "A"</xs:documentation>
						</xs:annotation>
					</xs:attribute>
					<xs:attribute name="K" type="xs:int" default="0">
						<xs:annotation>
							<xs:documentation>Exponent of SI base unit "K"</xs:documentation>
						</xs:annotation>
					</xs:attribute>
					<xs:attribute name="mol" type="xs:int" default="0">
						<xs:annotation>
							<xs:documentation>Exponent of SI base unit "mol"</xs:documentation>
						</xs:annotation>
					</xs:attribute>
					<xs:attribute name="cd" type="xs:int" default="0">
						<xs:annotation>
							<xs:documentation>Exponent of SI base unit "cd"</xs:documentation>
						</xs:annotation>
					</xs:attribute>
					<xs:attribute name="rad" type="xs:int" default="0">
						<xs:annotation>
							<xs:documentation>Exponent of SI derived unit "rad"</xs:documentation>
						</xs:annotation>
					</xs:attribute>
					<xs:attribute name="factor" type="xs:double" default="1"/>
					<xs:attribute name="offset" type="xs:double" default="0"/>
				</xs:complexType>
			</xs:element>
			<xs:sequence minOccurs="0" maxOccurs="unbounded">
				<xs:element name="DisplayUnit">
					<xs:annotation>
						<xs:documentation>DisplayUnit_value = factor*Unit_value + offset</xs:documentation>
					</xs:annotation>
					<xs:complexType>
						<xs:attribute name="name" type="xs:normalizedString" use="required">
							<xs:annotation>
								<xs:documentation>Name of DisplayUnit element, e.g. <Unit name="rad"/>, <DisplayUnit name="deg" factor="57.29..."/>. "name" must be unique with respect to all other "names" of the DisplayUnit definitions of the same Unit (different Unit elements may have the same DisplayUnit names).</xs:documentation>
							</xs:annotation>
						</xs:attribute>
						<xs:attribute name="factor" type="xs:double" default="1"/>
						<xs:attribute name="offset" type="xs:double" default="0"/>
					</xs:complexType>
				</xs:element>
			</xs:sequence>
		</xs:sequence>
		<xs:attribute name="name" type="xs:normalizedString" use="required">
			<xs:annotation>
				<xs:documentation>Name of Unit element, e.g. "N.m", "Nm",  "%/s". "name" must be unique will respect to all other elements of the UnitDefinitions list. The variable values in DATA PDUs are with respect to this unit.</xs:documentation>
			</xs:annotation>
		</xs:attribute>
	</xs:complexType>
</xs:schema>
    )delimiter";

    static const std::string dcpVariable = R"delimiter(
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:vc="http://www.w3.org/2007/XMLSchema-versioning" elementFormDefault="qualified"
    attributeFormDefault="unqualified" vc:minVersion="1.1">
    <xs:annotation>
        <xs:documentation>Copyright(c) 2008-2011 MODELISAR consortium, 2012-2018 Modelica Association Project "FMI", 2015-2018 ACOSAR consortium, 2018-2019 Modelica Association Project "DCP".
             All rights reserved.

This file is licensed by the copyright holders under the 3-Clause BSD License
(https://opensource.org/licenses/BSD-3-Clause):

----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
----------------------------------------------------------------------------
		</xs:documentation>
    </xs:annotation>
    <xs:include schemaLocation="dcpType.xsd"/>
    <xs:include schemaLocation="dcpAnnotation.xsd"/>
    <xs:include schemaLocation="dcpDataTypes.xsd"/>
    <xs:attribute name="declaredType" type="xs:normalizedString">
        <xs:annotation>
            <xs:documentation>If present, name of type defined with TypeDefinitions / SimpleType providing defaults.</xs:documentation>
        </xs:annotation>
    </xs:attribute>
    <xs:attributeGroup name="timeInstanceDeviation">
        <xs:attribute name="preEdge" type="xs:double"/>
        <xs:attribute name="postEdge" type="xs:double"/>
        <xs:attribute name="maxConsecMissedPdus" type="xs:unsignedInt"/>
    </xs:attributeGroup>
    <xs:complexType name="dcpVariable">
        <xs:annotation>
            <xs:documentation>Properties of a scalar variable</xs:documentation>
        </xs:annotation>
        <xs:sequence>
            <xs:choice>
                <xs:element name="Input">
                    <xs:complexType>
                        <xs:sequence>
                            <xs:choice>
                                <xs:element name="Int8">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpInteger8Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:byte"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Int16">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpInteger16Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:short"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Int32">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpInteger32Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:integer"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Int64">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpInteger64Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:long"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Uint8">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpUnsignedInteger8Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:unsignedByte"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Uint16">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpUnsignedInteger16Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:unsignedShort"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Uint32">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpUnsignedInteger32Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:unsignedInt"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Uint64">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpUnsignedInteger64Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:unsignedLong"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Float32">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpFloat32Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:float"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Float64">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpFloat64Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:double"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="String">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpStringType">
                                                <xs:attribute name="start"
                                                  type="xs:normalizedString" use="required"/>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Binary">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpBinaryType">
                                                <xs:attribute name="start" type="xs:hexBinary"
                                                  use="required"/>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                            </xs:choice>
                            <xs:element name="Dimensions" type="dcpDimensionsType" minOccurs="0"/>
                        </xs:sequence>
                    </xs:complexType>
                </xs:element>
                <xs:element name="Output">
                    <xs:complexType>
                        <xs:sequence>
                            <xs:choice>
                                <xs:element name="Int8">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpInteger8Type">
                                                <xs:attribute name="start">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:byte"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Int16">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpInteger16Type">
                                                <xs:attribute name="start">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:short"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Int32">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpInteger32Type">
                                                <xs:attribute name="start">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:integer"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Int64">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpInteger64Type">
                                                <xs:attribute name="start">
)delimiter"
R"delimiter(
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:long"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Uint8">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpUnsignedInteger8Type">
                                                <xs:attribute name="start">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:unsignedInt"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Uint16">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpUnsignedInteger16Type">
                                                <xs:attribute name="start">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:unsignedShort"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Uint32">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpUnsignedInteger32Type">
                                                <xs:attribute name="start">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:unsignedInt"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Uint64">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpUnsignedInteger64Type">
                                                <xs:attribute name="start">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:unsignedLong"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Float32">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpFloat32Type">
                                                <xs:attribute name="start">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:float"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Float64">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpFloat64Type">
                                                <xs:attribute name="start">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:double"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="String">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpStringType">
                                                <xs:attribute name="start"
                                                  type="xs:normalizedString"/>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Binary">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpBinaryType">
                                                <xs:attribute name="start" type="xs:hexBinary"/>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                            </xs:choice>
                            <xs:element name="Dimensions" type="dcpDimensionsType" minOccurs="0"/>
                            <xs:element name="Dependencies" minOccurs="0">
                                <xs:complexType>
                                    <xs:sequence>
                                        <xs:element name="Initialization" minOccurs="0">
                                            <xs:complexType>
                                                <xs:sequence minOccurs="0" maxOccurs="unbounded">
                                                  <xs:element name="Dependency"
                                                  type="dcpDependencyType"/>
                                                </xs:sequence>
                                            </xs:complexType>
                                        </xs:element>
                                        <xs:element name="Run" minOccurs="0">
                                            <xs:complexType>
                                                <xs:sequence minOccurs="0" maxOccurs="unbounded">
                                                  <xs:element name="Dependency"
                                                  type="dcpDependencyType"/>
                                                </xs:sequence>
                                            </xs:complexType>
                                        </xs:element>
                                    </xs:sequence>
                                </xs:complexType>
                            </xs:element>
                        </xs:sequence>
                        <xs:attribute name="defaultSteps" default="1">
                            <xs:simpleType>
                                <xs:restriction base="xs:unsignedInt">
                                    <xs:minInclusive value="1"/>
                                </xs:restriction>
                            </xs:simpleType>
                        </xs:attribute>
                        <xs:attribute name="fixedSteps" type="xs:boolean" default="true"/>
                        <xs:attribute name="minSteps">
                            <xs:simpleType>
                                <xs:restriction base="xs:unsignedInt">
                                    <xs:minInclusive value="1"/>
                                </xs:restriction>
                            </xs:simpleType>
                        </xs:attribute>
                        <xs:attribute name="maxSteps">
                            <xs:simpleType>
                                <xs:restriction base="xs:unsignedInt">
                                    <xs:minExclusive value="1"/>
                                </xs:restriction>
                            </xs:simpleType>
                        </xs:attribute>
                        <xs:attribute name="initialization" type="xs:boolean" default="false"/>
                    </xs:complexType>
                </xs:element>
                <xs:element name="Parameter">
                    <xs:complexType>
                        <xs:sequence>
                            <xs:choice>
                                <xs:element name="Int8">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpInteger8Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:byte"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Int16">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpInteger16Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:short"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Int32">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpInteger32Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:int"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Int64">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpInteger64Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:long"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Uint8">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpUnsignedInteger8Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:unsignedByte"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Uint16">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpUnsignedInteger16Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:unsignedShort"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Uint32">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpUnsignedInteger32Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:unsignedInt"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Uint64">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpUnsignedInteger64Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:unsignedLong"/>
)delimiter"
R"delimiter(
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Float32">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpFloat32Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:float"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Float64">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpFloat64Type">
                                                <xs:attribute name="start" use="required">
                                                  <xs:simpleType>
                                                  <xs:list itemType="xs:double"/>
                                                  </xs:simpleType>
                                                </xs:attribute>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="String">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpStringType">
                                                <xs:attribute name="start"
                                                  type="xs:normalizedString" use="required"/>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                                <xs:element name="Binary">
                                    <xs:complexType>
                                        <xs:complexContent>
                                            <xs:extension base="dcpBinaryType">
                                                <xs:attribute name="start" type="xs:hexBinary"
                                                  use="required"/>
                                            </xs:extension>
                                        </xs:complexContent>
                                    </xs:complexType>
                                </xs:element>
                            </xs:choice>
                            <xs:element name="Dimensions" type="dcpDimensionsType" minOccurs="0"/>
                        </xs:sequence>
                    </xs:complexType>
                </xs:element>
                <xs:element name="StructuralParameter">
                    <xs:complexType>
                        <xs:choice>
                            <xs:element name="Uint8">
                                <xs:complexType>
                                    <xs:attribute name="start" use="required">
                                        <xs:simpleType>
                                            <xs:restriction base="xs:unsignedByte">
                                                <xs:minInclusive value="1"/>
                                            </xs:restriction>
                                        </xs:simpleType>
                                    </xs:attribute>
                                </xs:complexType>
                            </xs:element>
                            <xs:element name="Uint16">
                                <xs:complexType>
                                    <xs:attribute name="start" use="required">
                                        <xs:simpleType>
                                            <xs:restriction base="xs:unsignedShort">
                                                <xs:minInclusive value="1"/>
                                            </xs:restriction>
                                        </xs:simpleType>
                                    </xs:attribute>
                                </xs:complexType>
                            </xs:element>
                            <xs:element name="Uint32">
                                <xs:complexType>
                                    <xs:attribute name="start" use="required">
                                        <xs:simpleType>
                                            <xs:restriction base="xs:unsignedInt">
                                                <xs:minInclusive value="1"/>
                                            </xs:restriction>
                                        </xs:simpleType>
                                    </xs:attribute>
                                </xs:complexType>
                            </xs:element>
                            <xs:element name="Uint64">
                                <xs:complexType>
                                    <xs:attribute name="start" use="required">
                                        <xs:simpleType>
                                            <xs:restriction base="xs:unsignedLong">
                                                <xs:minInclusive value="1"/>
                                            </xs:restriction>
                                        </xs:simpleType>
                                    </xs:attribute>
                                </xs:complexType>
                            </xs:element>
                        </xs:choice>
                    </xs:complexType>
                </xs:element>
            </xs:choice>
            <xs:element name="Annotations" type="dcpAnnotation" minOccurs="0">
                <xs:annotation>
                    <xs:documentation>Additional data of the scalar variable, e.g., for the dialog menu or the graphical layout</xs:documentation>
                </xs:annotation>
            </xs:element>
        </xs:sequence>
        <xs:attribute name="name" type="xs:normalizedString" use="required">
            <xs:annotation>
                <xs:documentation>Identifier of variable, e.g. "a.b.mod[3,4].'#123'.c". "name" must be unique with respect to all other elements of the ModelVariables list</xs:documentation>
            </xs:annotation>
        </xs:attribute>
        <xs:attribute name="valueReference" type="xs:unsignedLong" use="required">
            <xs:annotation>
                <xs:documentation>Identifier for variable value in DCP PDUs (unique with respect to all variables)</xs:documentation>
            </xs:annotation>
        </xs:attribute>
        <xs:attribute name="description" type="xs:string"/>
        <xs:attribute name="variability" default="continuous">
            <xs:annotation>
                <xs:documentation>constant: value never changes
fixed: value fixed after initialization
tunable: value constant between external events
discrete: value constant between internal events
continuous: no restriction on value changes</xs:documentation>
            </xs:annotation>
            <xs:simpleType>
                <xs:restriction base="xs:normalizedString">
                    <xs:enumeration value="fixed"/>
                    <xs:enumeration value="tunable"/>
                    <xs:enumeration value="discrete"/>
                    <xs:enumeration value="continuous"/>
                </xs:restriction>
            </xs:simpleType>
        </xs:attribute>
        <xs:attributeGroup ref="timeInstanceDeviation"/>
        <xs:attribute ref="declaredType"/>
    </xs:complexType>
    <xs:complexType name="dcpDimensionsType">
        <xs:sequence maxOccurs="unbounded">
            <xs:element name="Dimension">
                <xs:complexType>
                    <xs:attribute name="constant">
                        <xs:simpleType>
                            <xs:restriction base="xs:unsignedLong">
                                <xs:minInclusive value="1"/>
                            </xs:restriction>
                        </xs:simpleType>
                    </xs:attribute>
                    <xs:attribute name="linkedVR" type="xs:unsignedLong"/>
                </xs:complexType>
            </xs:element>
        </xs:sequence>
    </xs:complexType>
    <xs:complexType name="dcpVariableType">
        <xs:sequence>
            <xs:choice>
                <xs:element name="Int8" type="dcpInteger8Type"/>
                <xs:element name="Int16" type="dcpInteger16Type"/>
                <xs:element name="Int32" type="dcpInteger32Type"/>
                <xs:element name="Int64" type="dcpInteger64Type"/>
                <xs:element name="Uint8" type="dcpUnsignedInteger8Type"/>
                <xs:element name="Uint16" type="dcpUnsignedInteger16Type"/>
                <xs:element name="Uint32" type="dcpUnsignedInteger32Type"/>
                <xs:element name="Uint64" type="dcpUnsignedInteger64Type"/>
                <xs:element name="Float32" type="dcpFloat32Type"/>
                <xs:element name="Float64" type="dcpFloat64Type"/>
                <xs:element name="String" type="dcpStringType"/>
                <xs:element name="Binary" type="dcpBinaryType"/>
            </xs:choice>
            <xs:element name="Dimensions" type="dcpDimensionsType" minOccurs="0"/>
        </xs:sequence>
    </xs:complexType>
    <xs:complexType name="dcpDependencyType">
        <xs:attribute name="vr" type="xs:unsignedLong" use="required"/>
        <xs:attribute name="dependencyKind" use="required">
            <xs:simpleType>
                <xs:restriction base="xs:string">
                    <xs:enumeration value="dependent"/>
                    <xs:enumeration value="linear"/>
                </xs:restriction>
            </xs:simpleType>
        </xs:attribute>
    </xs:complexType>
</xs:schema>
    )delimiter";
}


#endif //DCPLIB_XSD_HPP
