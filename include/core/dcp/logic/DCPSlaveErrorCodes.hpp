/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPSLAVEERRORCODES_HPP
#define DCPLIB_DCPSLAVEERRORCODES_HPP

#include <dcp/model/LogTemplate.hpp>

static const LogTemplate HEARTBEAT_IGNORED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_INFORMATION,
                                                  "In ADU-D Heartbeat is not defined, but canMonitorHeartBeat. Heartbeat will not be monitored.",
                                                  {});
static const LogTemplate HEARTBEAT_STARTED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_INFORMATION,
                                                  "Monitoring Heartbeat started.", {});
static const LogTemplate HEARTBEAT_STOPPED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_INFORMATION,
                                                  "Monitoring Heartbeat stopped.", {});
static const LogTemplate HEARTBEAT_MISSED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_FATAL,
                                                 "Heartbeat missed. Checked Time: %string. Last state request: %string.",
                                                 {DcpDataType::string, DcpDataType::string});
static const LogTemplate COMPUTING_STARTED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                  "Computing routine started.", {});
static const LogTemplate COMPUTING_FINISHED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                   "Computing routine finished.", {});
static const LogTemplate COMPUTING_INTERRUPTED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                      "Computing routine was interrupted. State was not changed.",
                                                      {});
static const LogTemplate STOPPING_STARTED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                 "Stopping routine started.", {});
static const LogTemplate STOPPING_FINISHED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                  "Stopping routine finished.", {});
static const LogTemplate CONFIGURING_STARTED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                    "Configuring routine started.", {});
static const LogTemplate CONFIGURING_FINISHED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                     "Configuring routine finished.", {});
static const LogTemplate CONFIGURING_INTERRUPTED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                        "Configuring routine was interrupted. State was not changed.",
                                                        {});
static const LogTemplate PREPARING_STARTED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                  "Preparing routine started.", {});
static const LogTemplate PREPARING_FINISHED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                   "Preparing routine finished.", {});
static const LogTemplate PREPARING_INTERRUPTED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                      "Preparing routine was interrupted. State was not changed.",
                                                      {});
static const LogTemplate INITIALIZING_STARTED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                     "Initializing routine started.", {});
static const LogTemplate INITIALIZING_FINISHED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                      "Initializing routine finished.", {});
static const LogTemplate INITIALIZING_INTERRUPTED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                         "Initializing routine was interrupted. State was not changed.",
                                                         {});
static const LogTemplate SYNCHRONIZING_STARTED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                      "Synchronizing routine started.", {});
static const LogTemplate SYNCHRONIZING_FINISHED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                       "Synchronizing routine finished.", {});
static const LogTemplate SYNCHRONIZING_INTERRUPTED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                          "Synchronizing routine was interrupted. State was not changed.",
                                                          {});
static const LogTemplate STATE_CHANGED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                              "DCP state has changed to %uint8", {DcpDataType::state});
static const LogTemplate DATA_BUFFER_CREATED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                    "Buffer for data id %uint16 with buffer size %uint32 created.",
                                                    {DcpDataType::uint16, DcpDataType::uint32});
static const LogTemplate NEXT_SEQUENCE_ID_FROM_MASTER = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                             DcpLogLevel::LVL_DEBUG,
                                                             "Expected next pdu_seq_id from the master to be %uint16",
                                                             {DcpDataType::uint16});
static const LogTemplate NEW_INPUT_CONFIG = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                 "Added input configuration for value reference %uint64 with source datatype %uint8 to data_id %uint16",
                                                 {DcpDataType::uint64, DcpDataType::dataType, DcpDataType::uint16});
static const LogTemplate NEW_OUTPUT_CONFIG = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                  "Added output configuration for value reference %uint64 to data_id %uint16",
                                                  {DcpDataType::uint64, DcpDataType::uint16});
static const LogTemplate NEW_TUNABLE_CONFIG = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                   "Added tunable parameter configuration for value reference %uint64 with source datatype %uint8 to data_id %uint16",
                                                   {DcpDataType::uint64, DcpDataType::dataType, DcpDataType::uint16});
static const LogTemplate STEP_SIZE_NOT_SET = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                  "Step size was not set for data id %uint16.",
                                                  {DcpDataType::uint16});
static const LogTemplate ASSIGNED_INPUT = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                               "Assigned input value for value reference %uint64 (%uint8 -> %uint8):",
                                               {DcpDataType::uint64, DcpDataType::dataType,
                                                DcpDataType::dataType});
static const LogTemplate NOT_SUPPORTED_RSP_ACK = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                      "It is not supported to receive RSP_ack as slave.", {});
static const LogTemplate NOT_SUPPORTED_RSP_NACK = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                       "It is not supported to receive RSP_nack as slave.", {});
static const LogTemplate NOT_SUPPORTED_RSP_STATE_ACK = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                            DcpLogLevel::LVL_ERROR,
                                                            "It is not supported to receive RSP_state_ack as slave.",
                                                            {});
static const LogTemplate NOT_SUPPORTED_RSP_ERROR_ACK = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                            DcpLogLevel::LVL_ERROR,
                                                            "It is not supported to receive RSP_error_ack as slave.",
                                                            {});

static const LogTemplate NOT_SUPPORTED_LOG_ON_REQUEST = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                             DcpLogLevel::LVL_DEBUG,
                                                             "Log on request is not supported. ", {});
static const LogTemplate NOT_SUPPORTED_LOG_ON_NOTIFICATION = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                                  DcpLogLevel::LVL_DEBUG,
                                                                  "Log on notification is not supported. ", {});


static const LogTemplate INVALID_TYPE_ID = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                "A PDU with invalid type id (%uint8) received. PDU will be dropped.",
                                                {DcpDataType::pduType});
static const LogTemplate INVALID_RECEIVER = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                 "A PDU with invalid receiver (%uint8) received. PDU will be dropped.",
                                                 {DcpDataType::uint8});
static const LogTemplate UNKNOWN_DATA_ID = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                "A PDU with unknown data_id (%uint16) received. PDU will be dropped.",
                                                {DcpDataType::uint16});
static const LogTemplate UNKNOWN_PARAM_ID = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                 "A PDU with unknown param id (%uint16) received. PDU will be dropped.",
                                                 {DcpDataType::uint16});
static const LogTemplate CTRL_PDU_MISSED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                "A CTRL PDU was missed.", {});
static const LogTemplate IN_OUT_PDU_MISSED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                  "A Dat_input_output PDU was missed.", {});
static const LogTemplate PARAM_PDU_MISSED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                 "A Dat_parameter PDU was missed.", {});
static const LogTemplate OLD_CTRL_PDU_RECEIVED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                      "A old CTRL PDU was received. PDU will be dropped.", {});
static const LogTemplate OLD_IN_OUT_PDU_RECEIVED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                        "A old Dat_input_output PDU was received. PDU will be dropped.",
                                                        {});
static const LogTemplate OLD_PARAM_PDU_RECEIVED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                       "An old Dat_parameter PDU was received. PDU will be dropped.",
                                                       {});
static const LogTemplate INVALID_LENGTH = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                               "A PDU with invalid length received. %uint16 (received) != %uint16 (expected).",
                                               {DcpDataType::uint16, DcpDataType::uint16});
static const LogTemplate ONLY_NRT = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                         "The received PDU is only allowed in NRT. Current op mode is %uint8.",
                                         {DcpDataType::opMode});
static const LogTemplate MSG_NOT_ALLOWED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                "It is not allowed to receive %uint8 in state %uint8.",
                                                {DcpDataType::pduType, DcpDataType::state});
static const LogTemplate INVALID_UUID = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                             "UUID does not match %string (slave) != %string (received).",
                                             {DcpDataType::string, DcpDataType::string});
static const LogTemplate INVALID_OP_MODE = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                "Operation Mode %uint8 is not supported.", {DcpDataType::opMode});
static const LogTemplate INVALID_MAJOR_VERSION = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                      "The requested major version (%uint8) is not supported by this slave (DCP %uint8.%uint8)",
                                                      {DcpDataType::uint8, DcpDataType::uint8, DcpDataType::uint8});
static const LogTemplate INVALID_MINOR_VERSION = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                      "The requested minor version (%uint8) is not supported by this slave (DCP %uint8.%uint8)",
                                                      {DcpDataType::uint8, DcpDataType::uint8, DcpDataType::uint8});
static const LogTemplate INCOMPLETE_CONFIGURATION_GAP_INPUT_POS = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                                       DcpLogLevel::LVL_ERROR,
                                                                       "State change to Configuring is not possible. CFG_input with position %string was not received for data id %uint16, but max. pos was %uint16.",
                                                                       {DcpDataType::string, DcpDataType::uint16,
                                                                        DcpDataType::uint16});
static const LogTemplate INCOMPLETE_CONFIGURATION_GAP_OUTPUT_POS = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                                        DcpLogLevel::LVL_ERROR,
                                                                        "State change to Configuring is not possible. CFG_output with position %string was not received for data id %uint16, but max. pos was %uint16.",
                                                                        {DcpDataType::string, DcpDataType::uint16,
                                                                         DcpDataType::uint16});
static const LogTemplate INCOMPLETE_CONFIGURATION_GAP_PARAM_POS = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                                       DcpLogLevel::LVL_ERROR,
                                                                       "State change to Configuring is not possible. CFG_tunable_parameter with position %string was not received for data id %uint16, but max. pos was %uint16.",
                                                                       {DcpDataType::string, DcpDataType::uint16,
                                                                        DcpDataType::uint16});
static const LogTemplate INCOMPLETE_CONFIGURATION_STEPS = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                               DcpLogLevel::LVL_ERROR,
                                                               "State change to Configuring is not possible. Steps was not set for data id %uint16.",
                                                               {DcpDataType::uint16});
static const LogTemplate INCOMPLETE_CONFIGURATION_TIME_RESOLUTION = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                                         DcpLogLevel::LVL_ERROR,
                                                                         "State change to Configuring is not possible. Time resolution was not set.",
                                                                         {});
static const LogTemplate INCOMPLETE_CONFIG_NW_INFO_INPUT = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                                DcpLogLevel::LVL_ERROR,
                                                                "State change to Configuring is not possible. CFG_source_network_information was not set for data id %uint16.",
                                                                {DcpDataType::uint16});
static const LogTemplate INCOMPLETE_CONFIG_NW_INFO_OUTPUT = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                                 DcpLogLevel::LVL_ERROR,
                                                                 "State change to Configuring is not possible. CFG_target_network_information was not set for data id %uint16.",
                                                                 {DcpDataType::uint16});
static const LogTemplate INCOMPLETE_CONFIG_NW_INFO_TUNABLE = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                                  DcpLogLevel::LVL_ERROR,
                                                                  "State change to Configuring is not possible. CFG_pram_network_information was not set for data id %uint16.",
                                                                  {DcpDataType::uint16});
static const LogTemplate INCOMPLETE_CONFIG_SCOPE = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                        "State change to Configuring is not possible. CFG_scope was not set for data id %uint16.",
                                                        {DcpDataType::uint16});

static const LogTemplate DATA_NOT_ALLOWED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                 "It is not allowed to receive Data PDUs in state %uint8. PDU will be dropped.",
                                                 {DcpDataType::state});

static const LogTemplate START_TIME = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                           "Simulation starts at %string.", {DcpDataType::string});
static const LogTemplate INVALID_START_TIME = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                   "Start time (%string) is before current time (%string)",
                                                   {DcpDataType::string, DcpDataType::string});

static const LogTemplate INVALID_STEPS = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                              "Step %uint32 is not supported. It is expected to be one of %string.",
                                              {DcpDataType::uint32, DcpDataType::string});
static const LogTemplate NOT_SUPPORTED_VARIABLE_STEPS = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                             DcpLogLevel::LVL_ERROR,
                                                             "Variable steps are not supported. Current steps is %uint32. Last was %uint32.",
                                                             {DcpDataType::uint32, DcpDataType::uint32});
static const LogTemplate INVALID_LOG_CATEGORY = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                     "Log category %uint8 is not known by the slave.",
                                                     {DcpDataType::uint8});
static const LogTemplate INVALID_LOG_LEVEL = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                  "%uint8 is not a valid log level.", {DcpDataType::uint8});
static const LogTemplate INVALID_LOG_MODE = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                 "%uint8 is not a valid log mode.", {DcpDataType::uint8});

static const LogTemplate INVALID_SCOPE = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                              "%uint8 is not a valid scope.", {DcpDataType::uint8});


static const LogTemplate FIX_TIME_RESOLUTION = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                    "Setting time resolution not possible, it is fixed.", {});
static const LogTemplate INVALID_TIME_RESOLUTION = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                        "Time resolution %uint32/%uint32 is not supported. It is expected to be %string.",
                                                        {DcpDataType::uint32, DcpDataType::uint32,
                                                         DcpDataType::string});
static const LogTemplate INVALID_STEPS_OUTPUT = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                     "Step %uint32 is not supported by output with vr %uint64_t. It is expected to be one of %string.",
                                                     {DcpDataType::uint32, DcpDataType::uint64,
                                                      DcpDataType::string});
static const LogTemplate INVALID_VALUE_REFERENCE_INPUT = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                              DcpLogLevel::LVL_ERROR,
                                                              "Value reference %uint64 is not part of the DCP slave or not a input.",
                                                              {DcpDataType::uint64});
static const LogTemplate INVALID_VALUE_REFERENCE_OUTPUT = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                               DcpLogLevel::LVL_ERROR,
                                                               "Value reference %uint64 is not part of the DCP slave or not a output.",
                                                               {DcpDataType::uint64});
static const LogTemplate INVALID_VALUE_REFERENCE_PARAMETER = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE,
                                                                  DcpLogLevel::LVL_ERROR,
                                                                  "Value reference %uint64 is not part of the DCP slave or not a parameter.",
                                                                  {DcpDataType::uint64});
static const LogTemplate INVALID_SOURCE_DATA_TYPE = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                         "A PDU with invalid source datatype received. %uint8 (recieved) is not compatible to %uint8 (slave).",
                                                         {DcpDataType::dataType, DcpDataType::dataType});
static const LogTemplate INVALID_PORT = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                             "Port %uint16 is not supported. It is expected to be %string.",
                                             {DcpDataType::uint16, DcpDataType::string});
static const LogTemplate INVALID_TRANSPORT_PROTOCOL = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                           "%uint8 is not a valid transport protocol.",
                                                           {DcpDataType::uint8});
static const LogTemplate CONFIGURATION_CLEARED = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                      "Configuration cleared.", {});
static const LogTemplate TIME_RES_SET = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                             "Time resolution was set to %uint32 / %uint32 s.",
                                             {DcpDataType::uint32, DcpDataType::uint32});
static const LogTemplate STEP_SET = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                         "Steps for data_id %uint16 is set to %uint32.",
                                         {DcpDataType::uint16, DcpDataType::uint32});
static const LogTemplate DCP_ID_SET = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                           "DCP id is set to %uint8.", {DcpDataType::uint8});
static const LogTemplate OP_MODE_SET = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                            "Operation mode is set to %uint8.", {DcpDataType::opMode});
static const LogTemplate INVALID_STATE_ID = LogTemplate(logId++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                 "State id (%uint8) in received state change PDU do not match current state (%uint8).",
                                                 {DcpDataType::state, DcpDataType::state});
#endif //DCPLIB_DCPSLAVEERRORCODES_HPP
