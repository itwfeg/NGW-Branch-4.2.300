/**
 * @file PrinterVersion.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Class for creating a printer version message object
 * Created on: Aug 5, 2015 Updated, Formatted 11/6/15 by Alex Zavala
 * @addtogroup CANMessaging
 * @{
 */
#ifndef PRINTERVERSION_H_
#define PRINTERVERSION_H_
#include "PrinterGeneric.h"

/**
 * @class PrinterVersion
 * @brief Printer version message class
 * 
 */
class PrinterVersion : public PrinterGeneric {
public:
    /**
     * @brief Constructor by initialization list
     *  
     * @param pHobThread the message receiver class
     * @param printerType the printer number
     */
    PrinterVersion(Hobart::CEvolMsgReceiver *pHobThread = NULL, PrinterType printerType = FirstPrinter);
    
    /**
     * @brief Constructor by char buffer
     * Constructs a printer version message object using a message buffer
     * @param buffer 
     */
    PrinterVersion(const char *buffer);
    
    /**
     * @brief Destructor
     * 
     */
    ~PrinterVersion();
    
    /**
     * @brief Gets the software major version
     * 
     * @return the software major version
     */
    UINT16 getSoftwareMajor() { return _softwareMajor; }
    
    /**
     * @brief Sets the software major version
     * Sets the software major version to the version passed in
     * @param major the version number
     */
    void setSoftwareMajor( UINT16 major ) { _softwareMajor = major; }
    
    /**
     * @brief Gets the software minor version
     * 
     * @return the software minor version
     */
    UINT16 getSoftwareMinor() { return _softwareMinor; }
    
    /**
     * @brief Sets the software minor version
     * Sets the software minor version to the version passed in
     * @param minor the version number
     */
    void setSoftwareMinor( UINT16 minor ) { _softwareMinor = minor; }
    


    UINT16 getSoftwareBuild() { return _softwarebuild; }

    void setSoftwareBuild( UINT16 build ) { _softwarebuild = build; }

    /**
     * @brief Gets the hardware major version
     * 
     * @return the hardware major version
     */
    UINT8 getHardwareMajor() { return _hardwareMajor; }
    
    /**
     * @brief Sets the hardware major version
     * Sets the hardware major version to the version passed in
     * @param major the version number
     */
    void setHardwareMajor( UINT8 major ) { _hardwareMajor = major; }
    
    /**
     * @brief Gets the hardware minor version
     * 
     * @return the hardware minor version
     */
    UINT8 getHardwareMinor() { return _hardwareMinor; }
    
    /**
     * @brief Sets the hardware minor version
     * Sets the hardware minor version the version passed in
     * @param minor the version number
     */
    void setHardwareMinor( UINT8 minor ) { _hardwareMinor = minor; }
    
    /**
     * @brief Serializes the object into a buffer
     * 
     * @return the printer version object in a char buffer
     */
    const char *serialize() const;
private:
    UINT16 _softwareMajor;
    UINT16 _softwareMinor;
    UINT16 _softwarebuild;
    UINT8 _hardwareMajor;
    UINT8 _hardwareMinor;
};
/** @} */
#endif /* PRINTERVERSION_H_ */