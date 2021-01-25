#include "AbstractDcpManager.hpp"

#include <dcp/model/pdu/DcpPduDatInputOutput.hpp>



void AbstractDcpManager::DAT_input_output(const uint16_t dataId, uint8_t *payload,
                        const uint16_t payloadSize) {
    DcpPduDatInputOutput data = {getNextDataSeqNum(dataId), dataId, payload, payloadSize};
    driver.send(data);
}

void AbstractDcpManager::start() {
    driver.setDcpManager(getDcpManager());
#if defined(DEBUG) || defined(LOGGING)
    driver.setLogManager(logManager);
#endif
    driver.startReceiving();
}

void AbstractDcpManager::stop() {
    driver.stopReceiving();
}

AbstractDcpManager::AbstractDcpManager() {
#if defined(DEBUG) || defined(LOGGING)
    setLogManager({[this](const LogTemplate &logTemplate, uint8_t *payload, size_t size) {
        consume(logTemplate, payload, size);
    }, [this](size_t size) { return alloc(size); }});
#endif
}

/**
 * Returns the next sequence number for an given acuId
 * @param acuId acuId for which the seq. id. will be returned
 */
uint16_t AbstractDcpManager::getNextSeqNum(const uint8_t acuId) {
    int nextSeq = segNumsOut[acuId];
    segNumsOut[acuId] += 1;
    return nextSeq;
}

uint16_t AbstractDcpManager::getNextDataSeqNum(const uint16_t data_id) {
    int nextSeq = dataSegNumsOut[data_id];
    dataSegNumsOut[data_id] += 1;
    return nextSeq;
}

uint16_t AbstractDcpManager::checkSeqId(const uint8_t acuId, const uint16_t seqId) {
    uint16_t old = segNumsIn[acuId];
    if (seqId == (uint16_t)(old + 1)) {
        segNumsIn[acuId] = seqId;
    }
    return seqId - old;
}

uint16_t AbstractDcpManager::checkSeqIdInOut(const uint16_t dataId,
                            const uint16_t seqId) {
    if (dataSegNumsIn.count(dataId)){
        uint16_t old = dataSegNumsIn[dataId];
        if (seqId > old) {
            dataSegNumsIn[dataId] = seqId;
        }
        return seqId - old;
    }
    else {
        dataSegNumsIn[dataId] = seqId;
        return 1;
    }
}

uint16_t AbstractDcpManager::checkSeqIdParam(const uint16_t parameterId,
                            const uint16_t seqId) {
    if (parameterSegNumsIn.count(parameterId)){
        uint16_t old = parameterSegNumsIn[parameterId];
        if (seqId > old) {
            parameterSegNumsIn[parameterId] = seqId;
        }
        return seqId - old;
    }
    else {
        parameterSegNumsIn[parameterId] = seqId;
        return 1;
    }
}

uint16_t AbstractDcpManager::getNextParameterSeqNum(const uint16_t parameterId) {
    int nextSeq = parameterSegNumsOut[parameterId];
    parameterSegNumsOut[parameterId] += 1;
    return nextSeq;
}

void AbstractDcpManager::consume(const LogTemplate &logTemplate, uint8_t *payload, size_t size) {
    LogEntry logEntry(logTemplate, payload, size);
    if (generateLogString) {
        logEntry.applyPayloadToString();
    }
    for (const std::function<void(const LogEntry &)> &logListener: logListeners) {
        logListener(logEntry);
    }
    delete[] payload;
}
