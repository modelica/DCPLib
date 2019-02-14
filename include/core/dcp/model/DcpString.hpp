/*
 * Copyright (C) 2019, FG Simulation und Modellierung, Leibniz Universität Hannover, Germany
 *
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-CLause license.  See the LICENSE file for details.
 */

#ifndef ACOSAR_ACISTRING_H
#define ACOSAR_ACISTRING_H

#include <cstdint>
#include <string>
#include <math.h>
#include <assert.h>


class DcpString {
public:

    DcpString(uint32_t maxSize){
        payload = new char[maxSize + 4];
        *((uint32_t*) payload) = 0;
        managed = true;
    }

    DcpString(std::string& str, uint32_t maxSize){
        payload = new char[maxSize + 4];
        setString(str);
        managed = true;
    }

    DcpString(char* payload){
        this->payload = payload;
        managed = false;
    }

    ~DcpString(){
        if(managed){
            delete[] payload;
        }
    }

    const char* getChar() const{
        return payload + 4;
    }

    const void setString(std::string& str){
        assert(str.size() < pow(2, 32));
        *((uint32_t*) payload) = str.size();
        strcpy(payload + 4, str.c_str());
    }

    const std::string getString() const {
        return std::string(payload + 4, getSize());
    }

    const uint32_t getSize() const{
        return *((uint16_t*) payload);
    }

    const char* getPayload() const {
        return payload;
    }

private:
    char* payload;
    bool managed;
};




#endif //ACOSAR_ACISTRING_H
