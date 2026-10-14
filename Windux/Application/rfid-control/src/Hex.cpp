/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include <string.h>
#include "Hex.h"

#if !defined(__linux__)
#pragma warning(disable:4996)
#endif

std::string Hex::nibblize(const char *data, size_t len)
{
    return toString(data, len, false);
}

std::string Hex::toString(const char *data, size_t len, bool useSpace)
{
    std::string hexstr = "";
    char tmp[20];

    for (size_t i = 0; i < len; i++) {
        if (useSpace)
            sprintf(tmp, "%02X ", (unsigned char)(*(data + i)));
        else
            sprintf(tmp, "%02X", (unsigned char)(*(data + i)));
        hexstr += tmp;
    }
    return hexstr;
}

void Hex::nibblize(std::string &destination, const char *data, size_t len, bool append)
{
    toString(destination, data, len, append);
}

void Hex::toString(std::string &destination, const char *data, size_t len, bool append, bool useSpace)
{
    if (!append)
        destination = "";

    char tmp[20];

    for (size_t i = 0; i < len; i++) {
        if (useSpace)
            sprintf(tmp, "%02X ", (unsigned char)(*(data + i)));
        else
            sprintf(tmp, "%02X", (unsigned char)(*(data + i)));
        destination += tmp;
    }
}

std::string Hex::toString(const void *data)
{
    char hexstrbuf[HEX_STR_SIZE];
#if INTPTR_MAX == INT64_MAX
    sprintf(hexstrbuf, "%016llX", (unsigned long long)data);
#else
    sprintf(hexstrbuf, "%08lX", (unsigned long)data);
#endif
    return hexstrbuf;
}

std::string Hex::toString(const char *data)
{
    return toString(data, (int)strlen(data));
}

std::string Hex::toString(long l)
{
    char hexstrbuf[HEX_STR_SIZE];
    sprintf(hexstrbuf, "%08lX", l);
    return hexstrbuf;
}

std::string Hex::toString(long long l, bool stripLeadingSpaces)
{
    char hexstrbuf[HEX_STR_SIZE];
    sprintf(hexstrbuf, "%16llX", l);
    if (stripLeadingSpaces) {
        unsigned int i = 0;
        while ((hexstrbuf[i] == ' ') && (i < HEX_STR_SIZE))
            ++i;
        return &hexstrbuf[i];
    }
    return hexstrbuf;
}

std::string Hex::toString(unsigned long l)
{
    char hexstrbuf[HEX_STR_SIZE];
    sprintf(hexstrbuf, "%08lX", l);
    return hexstrbuf;
}

std::string Hex::toString(int i)
{
    char hexstrbuf[HEX_STR_SIZE];
    sprintf(hexstrbuf, "%04X", i);
    return hexstrbuf;
}

std::string Hex::toString(unsigned int i)
{
    char hexstrbuf[HEX_STR_SIZE];
    sprintf(hexstrbuf, "%04X", i);
    return hexstrbuf;
}

std::string Hex::toString(unsigned short value)
{
    char hexstrbuf[HEX_STR_SIZE];
    sprintf(hexstrbuf, "%04X", value);
    return hexstrbuf;
}

std::string Hex::toString(const char c)
{
    char hexstrbuf[HEX_STR_SIZE];
    sprintf(hexstrbuf, "%02X", c);
    return hexstrbuf;
}

std::string Hex::toString(const unsigned char c)
{
    char hexstrbuf[HEX_STR_SIZE];
    sprintf(hexstrbuf, "%02X", c);
    return hexstrbuf;
}

unsigned int Hex::valueOf(const char *input)
{
    unsigned int i;
    sscanf(input, "%02X", &i);
    return i;
}

unsigned int Hex::valueOf(const char input)
{
    unsigned int i = 0;
    sscanf(&input, "%1x", &i);
    return i;
}

long Hex::longValueOf(const std::string &s)
{
    long i;
    sscanf(s.c_str(), "%08lX", &i);
    return i;
}

std::string Hex::toHTMLColorString(long l)
{
    char hexstrbuf[HEX_STR_SIZE];
    sprintf(hexstrbuf, "%06lX", l);
    return hexstrbuf;
}

std::string Hex::toHTMLColorString(unsigned long l)
{
    char hexstrbuf[HEX_STR_SIZE];
    sprintf(hexstrbuf, "%06lX", l);
    return hexstrbuf;
}

// private:
Hex::Hex() {}