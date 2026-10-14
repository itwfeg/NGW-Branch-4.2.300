/**
 * @file WrPPPConfig.h
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Printer plate position configuration message
 * Created on: Mar 9, 2021
 */
#ifndef WRPPPCONFIG_H
#define WRPPPCONFIG_H
#include "WrapperGeneric.h"

class PPPConfig : public WrapperGeneric
{
	public:
    PPPConfig( Hobart::CEvolMsgReceiver	*pHobThread	= NULL, unsigned long priDistance = 0, unsigned long secDistance = 0, unsigned short rate = 0 );
    PPPConfig( const char *buffer );
    ~PPPConfig();

    unsigned short getPrimaryCalDistance() const { return priCalDistance_; }
    void setPrimaryCalDistance( unsigned long distance ) { priCalDistance_ = distance; }

    unsigned short getSecondaryCalDistance() const { return secondCalDistance_; }
    void setSecondaryCalDistance( unsigned long distance ) { secondCalDistance_ = distance; }

    unsigned short getSampleRate() const { return sampleRate_; }
    void setSampleRate( unsigned short milliSeconds ) { sampleRate_ = milliSeconds; }

    float getPrimarySlope() const { return primarySlope_; }
    void setPrimarySlope( float slope ) { primarySlope_ = slope; }

    float getSecondarySlope() const { return secondarySlope_; }
    void setSecondarySlope( float slope ) { secondarySlope_ = slope; }

    unsigned short getPrimaryPlateHomeDistance() const { return priPlateHomeDist_; }
    void setPrimaryPlateHomeDistance( unsigned short value ) { priPlateHomeDist_ = value; }

    unsigned short getSecondaryPlateHomeDistance() const { return secPlateHomeDist_; }
    void setSecondaryPlateHomeDistance( unsigned short value ) { secPlateHomeDist_ = value; }

    unsigned short getMechanicalHomeSpec() const { return mechanicalHomeSpec_; }
    void setMechanicalHomeSpec( unsigned short spec ) { mechanicalHomeSpec_ = spec; }

    #if 1 /* VL53L1X */
    unsigned char getXFieldOfView() const { return x_; }
    void setXFieldOfView( unsigned char x ) { x_ = x; }
    unsigned char getYFieldOfView() const { return y_; }
    void setYFieldOfView( unsigned char y ) { y_ = y; }
    unsigned char getCenterFieldOfView() const { return centerSpad_; }
    void setCenterFieldOfView( unsigned char center ) { centerSpad_ = center; }
    short getSensorACalOffset() const { return sensorAOffset_; }   
    void setSensorACalOffset( short offset ) { sensorAOffset_ = offset; }
    short getSensorBCalOffset() const { return sensorBOffset_; }   
    void setSensorBCalOffset( short offset ) { sensorBOffset_ = offset; }
    #endif
    const char *serialize() const;
    
    void show();

	private:
    unsigned long  priCalDistance_;
    unsigned long  secondCalDistance_;        
    unsigned short sampleRate_;     //in milliseconds
    float primarySlope_;
    float secondarySlope_;
    unsigned short priPlateHomeDist_;
    unsigned short secPlateHomeDist_;
    unsigned short mechanicalHomeSpec_;

    #if 1 /* VL53L1X */
    unsigned char x_;            // (x/2) number of spads from right and left of center
    unsigned char y_;            // (y/2) number of spads from top and bottom of center
    unsigned char centerSpad_;   // center spad number within the x,y coordinates
    short         sensorAOffset_;// calibration offset for sensor A
    short         sensorBOffset_;// calibration offset for sensor B
    #endif
};



#endif