#ifndef WRPPPCALIBRATION_H
#define WRPPPCALIBRATION_H
#include "WrapperGeneric.h"

class PPPCalib : public WrapperGeneric
{
	public:

    typedef enum
    {
        _READY,
        _SAMPLING,
        _VERIFY_HOME,
        _HOME_COMPLETE,
        _SAMPLING_SLOPE,
        _SAVED_CONFIG,
        _COMPLETE,
        _WAIT_FOR_MODE_CHANGE,
        _PLATE0_NOT_HOME,               
        _PLATE1_NOT_HOME,
        _PLATE0_TARGET_ALIGNMENT,       //error: TOF cannot see the target
        _PLATE1_TARGET_ALIGNMENT,       //error: TOF cannot see the target
        _ERROR_SENSOR0,                 //error: TOF should never read 0
        _ERROR_SENSOR1,                 //error: TOF should never read 0
        _ERROR_CONFIG                   //error: could not save cal values to flash    
    }CalStatus;
        
    PPPCalib( Hobart::CEvolMsgReceiver *pHobThread = NULL );
    PPPCalib( const char *buffer );
    ~PPPCalib();

    CalStatus getCalPlate0Status() const { return plate0Status_; }
    void setCalPlate0Status( CalStatus status ) { plate0Status_ = status; }
    unsigned long getCalDistance0Millimeter() const { return calDistance0MM_; }
    void setCalDistance0Millimeter( unsigned long distance ) { calDistance0MM_ = distance; }
    unsigned short getCalDistance0Inches() const { return calDistance0IN_; }
    void setCalDistance0Inches() { calDistance0IN_ = (unsigned long)( (float)calDistance0MM_ * (float).0393701 ); }
    
    CalStatus getCalPlate1Status() const { return plate1Status_; }
    void setCalPlate1Status( CalStatus status ) { plate1Status_ = status; }
    unsigned long getCalDistance1Millimeter() const { return calDistance1MM_; }
    void setCalDistance1Millimeter( unsigned long distance ) { calDistance1MM_ = distance; }    
    unsigned short getCalDistance1Inches() const { return calDistance1IN_; }
    void setCalDistance1Inches() { calDistance1IN_ = (unsigned long)( (float)calDistance1MM_ * (float).0393701 ); }

    const char *serialize() const;
    
    void show();

	private:
    CalStatus      plate0Status_;
    unsigned long  calDistance0MM_;     // millimeters
    unsigned long  calDistance0IN_;     // getDistanceInches  
    CalStatus      plate1Status_;
    unsigned long  calDistance1MM_;     // millimeters
    unsigned long  calDistance1IN_;     // getDistanceInches  
         
};
#endif