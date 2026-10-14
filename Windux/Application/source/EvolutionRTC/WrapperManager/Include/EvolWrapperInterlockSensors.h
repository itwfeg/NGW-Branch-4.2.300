#ifndef EVOLWRAPPERINTERLOCKSENSORS_H
#define EVOLWRAPPERINTERLOCKSENSORS_H

//#include <string>

#include "Wrapper.h"
#include "HobString.h"

class EvolWrapperInterlockSensors
{
	public:
		EvolWrapperInterlockSensors();
		EvolWrapperInterlockSensors(const char *buffer);
		EvolWrapperInterlockSensors(const EvolWrapperInterlockSensors &sensors);
		~EvolWrapperInterlockSensors();

		const char *serialize()const;
		UINT32 getSerializedLength()const;

		void show();

		bool isStopSwitch()const;
		void setStopSwitch(bool value);

		bool isRightCanopySwitch()const;
		void setRightCanopySwitch(bool value);

		bool isRightSideCover()const;
		void setRightSideCover(bool value);

		bool isFilmSupplyDoor()const;
		void setFilmSupplyDoor(bool value);

		bool isLeftSideCover()const;
		void setLeftSideCover(bool value);

		bool isIntakeCleanout()const;
		void setIntakeCleanout(bool value);

		void setStatus(UINT16 statusBits);

		bool equals(const EvolWrapperInterlockSensors &wrSensors)const;
		void operator=(const EvolWrapperInterlockSensors &sensors);

	private:
		bool	stop;
		bool	rightCanopy;
		bool	rightSideCover;
		bool	filmSupplyDoor;
		bool	leftSideCover;
		bool	intakeCleanout;

		//	Bit Masks for the Interlock switches and sensors
		static const UINT16 StopSwitchMask			= 0x0001;
		static const UINT16 RightCanopyMask			= 0x0002;
		static const UINT16 RightSideCoverMask		= 0x0004;
		static const UINT16 FilmSupplyDoorMask		= 0x0008;
		static const UINT16 LeftSideCoverMask		= 0x0010;
		static const UINT16 IntakeCleanoutMask		= 0x0020;
		
		mutable std::string serializedData;
		//std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif // EVOLWRAPPERINTERLOCKSENSORS_H
