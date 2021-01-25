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

#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>

#include <string>
#include <stdexcept>
#include <memory>

struct SlaveDescription_t;

class AciDescriptionReaderErrorHandler : public xercesc::ErrorHandler {
public:
    void warning(const xercesc::SAXParseException &ex);

    void error(const xercesc::SAXParseException &ex);

    void fatalError(const xercesc::SAXParseException &ex);

    void resetErrors();

private:
    void reportParseException(const xercesc::SAXParseException &ex);
};

std::shared_ptr<SlaveDescription_t> readSlaveDescription(const char *acuDFile);

#endif //DCPLIB_DCPSLAVEDESCRIPTIONREADER_H
