/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef DCPLIB_DCPSLAVEREADER_HPP
#define DCPLIB_DCPSLAVEREADER_HPP

#include <zip.h>
#include <dcp/xml/DcpSlaveDescriptionReader.hpp>
#include <fstream>



static std::string uncompressFileToTemp(zip* zip, std::string filename){
    char const *folder = getenv("TMPDIR");
    struct zip_stat stat;
    zip_stat_init(&stat);
    zip_stat(zip, filename.c_str(), 0, &stat);

    char *contents = new char[stat.size];

    zip_file *file = zip_fopen(zip, filename.c_str(), 0);
    if(file == nullptr){
        throw std::invalid_argument("Unable to find " + filename + " in zip file.");
    }
    zip_fread(file, contents, stat.size);
    zip_fclose(file);
    auto current = std::chrono::system_clock::now();
    std::string tmpFile(std::string(folder) + "fmi2dcp_" + std::to_string(rand()) + "_"+ std::to_string(current.time_since_epoch().count()));
    if (!std::ofstream(tmpFile).write(contents, stat.size)) {
        delete[] contents;
        throw  std::runtime_error("Error while reading fmu: Error writing file " + tmpFile);
    }
    delete[] contents;
    return tmpFile;
}

static std::shared_ptr<SlaveDescription_t> getSlaveDescriptionFromDcpFile(uint8_t majorVersion, uint8_t minorVersion, std::string zipFile){
    assert(majorVersion > 0 && majorVersion <= 1 && minorVersion <= 0);
    int err = 0;
    zip *zip = zip_open(zipFile.c_str(), 0, &err);
    if(zip == nullptr){
        throw std::invalid_argument("Unable to open zip file.");
    }
    std::string fileName = "v" + std::to_string(majorVersion) + "." + std::to_string(minorVersion) + "/dcpSlaveDescription.dcpx";

    std::string tmpFile = uncompressFileToTemp(zip, fileName);
    zip_close(zip);
    return readSlaveDescription(tmpFile.c_str());
}

#endif //DCPLIB_DCPSLAVEREADER_HPP
