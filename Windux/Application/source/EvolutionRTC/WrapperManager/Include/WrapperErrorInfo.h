#ifndef WRAPPERERRORINFO_H
#define WRAPPERERRORINFO_H

#include <string>

#include "Wrapper.h"

class WrapperErrorInfo
{
	public:
		WrapperErrorInfo();
		WrapperErrorInfo(const char *buffer);
		~WrapperErrorInfo();

		const char *serialize() const;
		UINT32 getSerializedLength() const;

		void show();

		bool isEepWriteFailure() const;
		void setEepWriteFailure();
		void clearEepWriteFailure();

		bool isHeaterFailure() const;
		void setHeaterFailure();
		void clearHeaterFailure();

		bool isPackageTooLarge() const;
		void setPackageTooLarge();
		void clearPackageTooLarge();

		bool isGeneralFault1() const;
		void setGeneralFault1();
		void clearGeneralFault1();

		bool isGeneralFault2() const;
		void setGeneralFault2();
		void clearGeneralFault2();

		bool isSealerBeltInterlock() const;
		void setSealerBeltInterlock();
		void clearSealerBeltInterlock();

		bool isElevatorInterlock() const;
		void setElevatorInterlock();
		void clearElevatorInterlock();

		bool isScalePlatterInterlock() const;
		void setScalePlatterInterlock();
		void clearScalePlatterInterlock();

		bool isHandCrankDoorInterlock() const;
		void setHandCrankDoorInterlock();
		void clearHandCrankDoorInterlock();

		bool isFilmDoorInterlock() const;
		void setFilmDoorInterlock();
		void clearFilmDoorInterlock();

		bool isTooTallInterlock() const;
		void setTooTallInterlock();
		void clearTooTallInterlock();

		bool isLeftCanopyInterlock() const;
		void setLeftCanopyInterlock();
		void clearLeftCanopyInterlock();

		bool isRightCanopyInterlock() const;
		void setRightCanopyInterlock();
		void clearRightCanopyInterlock();

		bool isFrontCoverInterlock() const;
		void setFrontCoverInterlock();
		void clearFrontCoverInterlock();

		bool isLeftSideCoverInterlock() const;
		void setLeftSideCoverInterlock();
		void clearLeftSideCoverInterlock();

		bool isRightSideCoverInterlock() const;
		void setRightSideCoverInterlock();
		void clearRightSideCoverInterlock();

		bool isWideFilmSupplyFailure() const;
		void setWideFilmSupplyFailure();
		void clearWideFilmSupplyFailure();

		bool isNarrowFilmSupplyFailure() const;
		void setNarrowFilmSupplyFailure();
		void clearNarrowFilmSupplyFailure();

		bool isPackageJammed() const;
		void setPackageJammed();
		void clearPackageJammed();

		bool isMainMotorStalled() const;
		void setMainMotorStalled();
		void clearMainMotorStalled();

		bool isIntakeHomeFailure() const;
		void setIntakeHomeFailure();
		void clearIntakeHomeFailure();

		bool isIntakeWidthFailure() const;
		void setIntakeWidthFailure();
		void clearIntakeWidthFailure();

		bool isIntakeLengthFailure() const;
		void setIntakeLengthFailure();
		void clearIntakeLengthFailure();

		bool isIntakeMotorJammed() const;
		void setIntakeMotorJammed();
		void clearIntakeMotorJammed();

		bool isIntakePackagePresent() const;
		void setIntakePackagePresent();
		void clearIntakePackagePresent();

		bool isIntakeOutOfSync() const;
		void setIntakeOutOfSync();
		void clearIntakeOutOfSync();

		bool isStopSwitchInterlock() const;
		void setStopSwitchInterlock();
		void clearStopSwitchInterlock();

		void setErrorBits(WrErrorBits errorBits);
		WrErrorBits getErrorBits() const;

		bool equals(WrapperErrorInfo &wrErrorInfo);

		typedef enum WrapperErrorClass 
		{
			NoErrorClass,
			ActiveErrorClass,
			IdleErrorClass,
			OtherErrorClass,
			UnknownErrorClass
		} WrapperErrorClass_t;

		typedef enum 
		{
			NoErrorSource,
			WrapperCommLost,
			StopSwitchInterlock,	// IdleWrapError
			EepWriteFailure,
			IntakeOutOfSync,		// OtherWrapError
			IntakePackagePresent,	// OtherWrapError
			MainMotorJammed,
			IntakeMotorJammed,
			IntakeLengthFailure,
			IntakeWidthFailure,
			IntakeHomeFailure,
			PackageJammed,
			WideFilmFailure,
			NarrowFilmFailure,
			RightSideCoverInterlock,
			LeftSideCoverInterlock,
			FrontCoverInterlock, 
			RightCanopyInterlock,
			LeftCanopyInterlock,
			TooTallInterlock,
			FilmDoorInterlock, 
			HandCrankDoorInterlock,
			IntakeInterlock,
			ElevatorInterlock,
			SealerBeltInterlock,
			GeneralFault2,
			GeneralFault1,
			PackageTooLarge,
			HeaterFailure,
			InvalidPackSize = 100
		} WrapperErrorSource;

		bool isWrapperError() const;
		const WrapperErrorInfo::WrapperErrorClass &getErrorClass() const;
		const WrapperErrorInfo::WrapperErrorSource &getErrorSource() const;

	private:
		void initClassMembers();
		void setError(WrapperErrorSource errorSource, WrapperErrorClass errorClass);
		void setError();

		WrapperErrorClass errorClass;
		WrapperErrorSource errorSource;

		bool	eepWriteFailure;
		bool	heaterFailure;
		bool	packageTooLarge;
		bool	generalFault1;
		bool	generalFault2;
		bool	sealerBeltInterlock;
		bool	elevatorInterlock; 
		bool	scalePlatterInterlock;
		bool	handCrankDoorInterlock; 
		bool	filmDoorInterlock;
		bool	tooTallInterlock;
		bool	leftCanopyInterlock;
		bool	rightCanopyInterlock;
		bool	frontCoverInterlock;
		bool	leftSideCoverInterlock;
		bool	rightSideCoverInterlock;
		bool	wideFilmSupplyFailure;
		bool	narrowFilmSupplyFailure;
		bool	packageJammed;
		bool	mainMotorStalled;
		bool	intakeHomeFailure;
		bool	intakeWidthFailure;
		bool	intakeLengthFailure;
		bool	intakeMotorJammed;
		bool	intakePackagePresent;
		bool	intakeOutOfSync;
		bool	stopSwitchInterlock;

		mutable std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};
#endif // WRAPPERERRORINFO_H
