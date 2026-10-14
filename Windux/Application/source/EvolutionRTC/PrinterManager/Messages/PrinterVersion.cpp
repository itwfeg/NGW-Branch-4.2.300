/**
 * @file PrinterVersion.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief The source for creating a PrinterVersion message
 * Created on: Aug 5, 2015 Updated, Formatted 11/6/15 by Alex Zavala
 */
#include "PrinterVersion.h"

PrinterVersion::PrinterVersion( Hobart::CEvolMsgReceiver *pHobThread,
                                PrinterType printerType )
                                : PrinterGeneric(PrVersion, pHobThread, printerType),
                                _softwareMajor(0), _softwareMinor(0), _softwarebuild(0), _hardwareMajor(0), _hardwareMinor(0) {
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterVersion::PrinterVersion( const char *buffer ) : PrinterGeneric( buffer ) {
    PrinterGeneric::serialize();
    int j = getSerializedLength();

    char *pChar = (char *) &_softwareMajor;
    for (unsigned int i = 0; i < sizeof(_softwareMajor); i++) {
        *pChar++ = buffer[j++];
    }

    pChar = (char *) &_softwareMinor;
    for (unsigned int i = 0; i < sizeof(_softwareMinor); i++) {
        *pChar++ = buffer[j++];
    }

    pChar = (char *) &_softwarebuild;
    for (unsigned int i = 0; i < sizeof(_softwarebuild); i++) {
        *pChar++ = buffer[j++];
    }

    pChar = (char *) &_hardwareMajor;
    for (unsigned int i = 0; i < sizeof(_hardwareMajor); i++) {
        *pChar++ = buffer[j++];
    }

    pChar = (char *) &_hardwareMinor;
    for (unsigned int i = 0; i < sizeof(_hardwareMinor); i++) {
        *pChar++ = buffer[j++];
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterVersion::~PrinterVersion() {}

///////////////////////////////////////////////////////////////////////////////////////////////////
const char *PrinterVersion::serialize() const {
    PrinterGeneric::serialize();

    char *pChar = (char *) &_softwareMajor;
    for (unsigned int i = 0; i < sizeof(_softwareMajor); i++) {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *) &_softwareMinor;
    for (unsigned int i = 0; i < sizeof(_softwareMinor); i++) {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *) &_softwarebuild;
    for (unsigned int i = 0; i < sizeof(_softwarebuild); i++) {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *) &_hardwareMajor;
    for (unsigned int i = 0; i < sizeof(_hardwareMajor); i++) {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *) &_hardwareMinor;
    for (unsigned int i = 0; i < sizeof(_hardwareMinor); i++) {
        serializedData.append(1, pChar[i]);
    }

    return serializedData.data();
}