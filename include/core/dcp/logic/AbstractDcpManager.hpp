/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef ACI_DRIVER_ABSTRACTACIDRIVERMANAGER_H_
#define ACI_DRIVER_ABSTRACTACIDRIVERMANAGER_H_

#include <cstdint>
#include <map>

#include <dcp/logic/DcpManager.hpp>
#include <dcp/driver/DcpDriver.hpp>
#if defined(DEBUG) || defined(LOGGING)
#include <dcp/logic/Logable.hpp>
#include <dcp/helper/LogHelper.hpp>
#endif

class LogEntry;

/**
 * General interface for the AciLogic.
 *
 * @author Christian Kater
 */
class AbstractDcpManager
#if defined(DEBUG) || defined(LOGGING)
    : public Logable
#endif
        {
public:

    ~AbstractDcpManager() {}

    void DAT_input_output(const uint16_t dataId, uint8_t *payload,
                          const uint16_t payloadSize);

    void start();

    void stop();
    
    virtual void receive(DcpPdu &msg) = 0;

    void addLogListener(std::function<void(const LogEntry &)> fct) {
        logListeners.push_back(std::move(fct));
    }

    void setGenerateLogString(bool generateLogString) {
        this->generateLogString = generateLogString;
    }

    virtual void reportError(const DcpError errorCode) = 0;

    virtual DcpManager getDcpManager() = 0;

protected:
    /**
     * DCP Driver instance
     */
    DcpDriver driver;

    /**
     * last seq. id which was send out
     */
    std::map<uint8_t, uint16_t> segNumsOut;
    /**
     * last seq. id which was received
     */
    std::map<uint8_t, uint16_t> segNumsIn;

    /**
     * last seq. id which was send out
     */
    std::map<uint16_t, uint16_t> dataSegNumsOut;
    /**
     * last seq. id which was received
     */
    std::map<uint16_t, uint16_t> dataSegNumsIn;

/**
    * last seq. id which was send out
    */
    std::map<uint16_t, uint16_t> parameterSegNumsOut;
    /**
     * last seq. id which was received
     */
    std::map<uint16_t, uint16_t> parameterSegNumsIn;

    std::vector<std::function<void(const LogEntry &)>> logListeners;
    bool generateLogString;

    uint8_t dcpId;
    uint8_t masterId;

protected:
    AbstractDcpManager();

    /**
     * Returns the next sequence number for an given acuId
     * @param acuId acuId for which the seq. id. will be returned
     */
    uint16_t getNextSeqNum(const uint8_t acuId);

    uint16_t getNextDataSeqNum(const uint16_t data_id);

    uint16_t checkSeqId(const uint8_t acuId, const uint16_t seqId);
    
    uint16_t checkSeqIdInOut(const uint16_t dataId,
                             const uint16_t seqId);

    uint16_t checkSeqIdParam(const uint16_t parameterId,
                             const uint16_t seqId);

    uint16_t getNextParameterSeqNum(const uint16_t parameterId);

    virtual void consume(const LogTemplate &logTemplate, uint8_t *payload, size_t size);

    virtual uint8_t *alloc(size_t size) {
        return new uint8_t[size];
    }
};

#endif /* ACI_DRIVER_ABSTRACTACIDRIVERMANAGER_H_ */
