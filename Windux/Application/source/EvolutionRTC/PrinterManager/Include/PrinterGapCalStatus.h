#ifndef PRINTERGAPCALSTATUS_H
#define PRINTERGAPCALSTATUS_H
/**
 * @file PrinterGapCalComplete.h
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Printer gap (shoot through) sensor calibration states and results.
 * Created on: Dec 1, 2021
 * @addtogroup CANMessaging
 * @{
 */

#include "PrinterGeneric.h"
#include "CEvolMsgReceiver.h"

typedef enum GapCalStates
{
    _CalInit,
    _BackingPaper,
    _LabelPlusBacking,
    _CalcDeflections,
    _FailureSettingI,
    _FailureMemory,
    _Done
}GapCalState;

class PrinterGapCalStatus : public PrinterGeneric
{
	public:
    PrinterGapCalStatus(Hobart::CEvolMsgReceiver *pHobThread = NULL, PrinterType printerType = FirstPrinter, GapCalState state = _CalInit );
    PrinterGapCalStatus( const char *buffer );
    ~PrinterGapCalStatus();

    GapCalState getState() const { return state_; }
    
    float getDeflectionVoltage() const { return deflectionVoltage_; }
    void setDeflectionVoltage( float voltage ) { deflectionVoltage_ = voltage; }

    float getDriveCurrent() const { return driveCurrent_; }
    void setDriveCurrent( float i ) { driveCurrent_ = i; }

    float getBackingVoltage() const { return backingVoltage_; }
    void setBackingVoltage( float voltage ) { backingVoltage_ = voltage; }

    float getlabelBackingVoltage() const { return labelBackingVoltage_; }
    void setlabelBackingVoltage( float voltage ) { labelBackingVoltage_ = voltage; }

	const char *serialize()const;

	private:
    GapCalState state_;
    float deflectionVoltage_;
    float driveCurrent_;
    float backingVoltage_;
    float labelBackingVoltage_;
    
};
#endif


