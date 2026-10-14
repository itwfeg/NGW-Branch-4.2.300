/**
 * @file PrinterHeadType.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Carlos Guzman
 * @brief The source for creating a PrinterHeadType message
 * Created on: Oct 11, 2019
 */
#include "PrinterHeadType.h"

PrinterHeadType::PrinterHeadType( Hobart::CEvolMsgReceiver *pHobThread,
                                PrinterType printerType )
                                : PrinterGeneric(PrHeadType, pHobThread, printerType),
                                _headType(1), _headSize(2){
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterHeadType::PrinterHeadType( const char *buffer ) : PrinterGeneric( buffer ) {
    PrinterGeneric::serialize();
    int j = getSerializedLength();

    char *pChar = (char *) &_headType;
    for (unsigned int i = 0; i < sizeof(_headType); i++) {
        *pChar++ = buffer[j++];
    }

    pChar = (char *) &_headSize;
    for (unsigned int i = 0; i < sizeof(_headSize); i++) {
        *pChar++ = buffer[j++];
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterHeadType::~PrinterHeadType() {}

///////////////////////////////////////////////////////////////////////////////////////////////////
const char *PrinterHeadType::serialize() const {
    PrinterGeneric::serialize();

    char *pChar = (char *) &_headType;
    for (unsigned int i = 0; i < sizeof(_headType); i++) {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *) &_headSize;
    for (unsigned int i = 0; i < sizeof(_headSize); i++) {
        serializedData.append(1, pChar[i]);
    }
    return serializedData.data();
}



