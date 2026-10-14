#ifndef QAPRINTERSTATUS_H
#define QAPRINTERSTATUS_H


namespace StarMicronics_QAsura
{


typedef struct QAPrinterStatus_
{
    // printer status 1
    bool coverOpen;
    bool offline;
    bool compulsionSwitch;

    // printer status 2
    bool overTemp;
    bool unrecoverableError;
    bool cutterError;
    bool mechError;
    bool headThermistorError;

    // printer status 3
    bool receiveBufferOverflow;
    bool pageModeCmdError;
    bool blackMarkError;
    bool presenterPaperJamError;
    bool headUpError;
    bool voltageError;

    // printer status 4
    bool receiptBlackMarkDetection;
    bool receiptPaperEmpty;
    bool receiptPaperLow;

    // printer status 6
    bool etbAvailable;
    unsigned char etbCounter;

    // printer status 7
    bool receiptPaperJam;
    bool powerButtonPressed;

    // raw
    unsigned int rawLength;
    unsigned char raw[63];
} QAPrinterStatus;


} // namespace StarMicronics_QAsura


#endif // QAPRINTERSTATUS_H
