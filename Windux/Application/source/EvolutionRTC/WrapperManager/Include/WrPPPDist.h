/**
 * @file WrPPPDist.h
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Printer plate position message
 * Created on: Mar 9, 2021
 */

#ifndef WRPPPDIST_H
#define WRPPPDIST_H
#include "WrapperGeneric.h"

class PPPDist : public WrapperGeneric
{
	public:

    typedef enum
    {
        _UNKNOWN_POSITION,
        _PRIMARY_POSITION,
        _SECONDARY_POSITION
    }PrPlate;
        
    PPPDist( Hobart::CEvolMsgReceiver *pHobThread = NULL, PrPlate plate = _UNKNOWN_POSITION, unsigned long dmm = 0, unsigned long din = 0 );
    PPPDist( const char *buffer );
    ~PPPDist();


    unsigned long getDistanceMillimeter() const { return distanceMM_; }
    void setDistanceMillimeter( unsigned long distance ) { distanceMM_ = distance; }

    unsigned short getDistanceInches() const { return distanceIN_; }
    void setDistanceInches( unsigned long distance ) { distanceIN_ = distance; }

    PrPlate getPlate() const { return plate_; } 
    void setPlate( PrPlate plate ) { plate_ = plate; }

    const char *serialize() const;
    
    void show();

	private:
    PrPlate        plate_;
    unsigned long  distanceMM_;     // millimeters
    unsigned long  distanceIN_;     // inches   
         
};
#endif