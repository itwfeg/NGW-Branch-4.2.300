#ifndef EVOLWRAPPERPKGDETECTORSTATUS
#define EVOLWRAPPERPKGDETECTORSTATUS

#include "WrapperGeneric.h"
#include <string>

class HobThread;

class EvolWrapperPkgDetectorStatus : public WrapperGeneric
{
	public:

		typedef enum
		{
			ElevatorSensor,
			SealerBeltSensor,
			Reserved1,
			Reserved2,
			NoLocation
		}SensorLocation;
		
		typedef enum
		{
			Detector455kHz,
			Detector56kHz
		}DetectorType;
	
		EvolWrapperPkgDetectorStatus(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		EvolWrapperPkgDetectorStatus(const char *buffer);
		~EvolWrapperPkgDetectorStatus();

		SensorLocation getSensorLocation()const{return sensorLocation_;}
		void setSensorLocation(UINT16 location){sensorLocation_ = (SensorLocation)location;} 
		bool isSensorLocationElevator(){return((sensorLocation_ == ElevatorSensor) ? true : false);}
		bool isSensorLocationSealer(){return((sensorLocation_ == SealerBeltSensor) ? true : false);}

		UINT16 getEmitterPower()const{return emitterPowerLevel_;}
		void setEmitterPower(UINT16 powerLevel){emitterPowerLevel_ = powerLevel;} 

		bool getSensorOperational()const{return sensorOperational_;}
		void setSensorOperational(bool operational){sensorOperational_ = operational;}

		bool getIsOldDetector()const{return isOldDetector_;}
		void setIsOldDetector(bool isDetectorold){isOldDetector_ = isDetectorold;}
		
		UINT16 getDetectorType()const{return (UINT16)detectorType_;}
		void setDetectorType(UINT16 detectorType);

		std::string getDetectorDescription() const {return detectorDescription_;}
		

		const char *serialize()const;
		void show();
	
	private:
		//note: if any new descriptions are to be added, make sure that the all descriptions size are the same
		std::string detectorDescriptionA;// = "56kHz package detector ";
		std::string detectorDescriptionB;// = "455kHz package detector";	


		SensorLocation		sensorLocation_;	
		DetectorType		detectorType_;

		UINT16				emitterPowerLevel_;		
		bool				sensorOperational_;
		
		bool				isOldDetector_;
		std::string			detectorDescription_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};


#endif