/**
 * @file PrinterHeadType.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Carlos Guzman
 * @brief Class for creating a Printer Head Type message object
 * Created on: Oct 10, 2019
 * @addtogroup CANMessaging
 * @{
 */
#ifndef PRINTERHEADTYPE_H_
#define PRINTERHEADTYPE_H_

#include "PrinterGeneric.h"


/**
 * @class PrinterHeadType
 * @brief PrinterHeadType message class
 *
 */
class PrinterHeadType : public PrinterGeneric {
public:
    /**
     * @brief Constructor by initialization list
     *
     * @param pHobThread the message receiver class
     * @param printerType the printer number
     */
    PrinterHeadType(Hobart::CEvolMsgReceiver *pHobThread = NULL, PrinterType printerType = FirstPrinter);

    /**
     * @brief Constructor by char buffer
     * Constructs a printer Head Type message object using a message buffer
     * @param buffer
     */
    PrinterHeadType(const char *buffer);

    /**
     * @brief Destructor
     *
     */
    ~PrinterHeadType();

    /**
     * @brief getHeadType
     *
     * @return the head type
     */
    UINT16 getHeadType() const { return _headType; }

    /**
     * @brief setHeadType
     * Sets the Head Type to the value passed in
     * @param type
     */
    void setHeadType( UINT16 type ) { _headType = type; }


    /**
     * @brief getHeadSize
     *
     * @return the head size
     */
    UINT16 getHeadSize() const { return _headSize; }

    /**
     * @brief setHeadSize
     * Sets the Head Size to the value passed in
     * @param size
     */
    void setHeadSize( UINT16 size ) { _headSize = size; }

    /**
     * @brief Serializes the object into a buffer
     *
     * @return the printer head type object in a char buffer
     */
    const char *serialize() const;

private:
    UINT16	_headType;
    UINT16 	_headSize;
};

/** @} */
#endif /* PRINTERHEADTYPE_H_ */
