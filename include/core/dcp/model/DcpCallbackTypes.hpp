/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPCALLBACKTYPES_HPP
#define DCPLIB_DCPCALLBACKTYPES_HPP
enum class DcpCallbackTypes {
    PREPARE, CONFIGURE, SYNCHRONIZING_NRT_STEP, SYNCHRONIZED_NRT_STEP, RUNNING_NRT_STEP, STOP, TIME_RES, STEPS, OPERATION_INFORMATION, CONFIGURATION_CLEARED,
    RUNTIME, CONTROL_MISSED, IN_OUT_MISSED, PARAM_MISSED, STATE_CHANGED, ERROR_LI, INITIALIZE,
    ACK, NACK, STATE_ACK, ERROR_ACK, PDU_MISSED, DATA, RSP_log_ack, NTF_LOG, SYNCHRONIZING_STEP, SYNCHRONIZED_STEP, RUNNING_STEP, synchronize,
};

enum FunctionType {
    SYNC, ASYNC,
};
#endif //DCPLIB_DCPCALLBACKTYPES_HPP
