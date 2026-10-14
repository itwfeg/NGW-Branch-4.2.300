#ifndef EVOLWRAPPERTEST_H
#define EVOLWRAPPERTEST_H

#include "EvolMessage.h"
#include "HobString.h"

class HobThread;

class EvolWrapperTest : public EvolMessage
{
	public:
		static const short OFF	= 0;
		static const short ON	= 1;

		const static char DECIMAL_POINT = '.';

		typedef enum WrEvolTestCode
		{
			NoServiceTest,
			PrimaryIntakeTest,			//	Just cycles
			SecondaryIntakeTest,		//	Just cycles
			CameraTest,
			ElevatorTest,				//	Just cycles
			ElevatorWidePackageTest,	//	Takes parameters On/Off
			FilmSelectTest,				//	Takes parameters WrEvolFilmTestParam
			KnifeTest,					//	Takes parameters On/Off
			UpperFilmBrakeTest,			//	Takes parameters On/Off
			LowerFilmBrakeTest,			//	Takes parameters On/Off
			FilmSensorTest,				//	Takes parameters On/Off
			GripperNarrowTest,			//	Takes parameters On/Off
			GripperWideTest,			//	Takes parameters On/Off
			GripperTest,				//	Takes parameters WrEvolGripperTestParam
			SideClampsTest,				//	Takes parameters WrEvolSideClampTestParam
			RearFolderTest,				//	Just cycles
			FrontFolderTest,			//	Just cycles
			PusherTest,					//	Just cycles
			SealerTest,					//	Just cycles
			CycleIntakeTest,			//	Just cycles
			LabelApplierTest,			//	Takes parameters WrEvolLATestParam
			ShifterTest,				//	Takes parameters WrEvolShifterTestParam
			AirCompressorTest,			//	Takes parameters On/Off
			SideFolderTest,				//	Just cycles
			WrapperCycle,				//	Just cycles - Takes PLU# as the parameter
			WrapperMechResetTest,		//	No parameters needed. Resets wrapper mechs.
			CalibrateWrapper,			//	Takes parameters Mechanism # and tray length
			SecondLabelApplierTest,		//	Takes parameters WrEvolLATestParam
			WrReservedForFirmware_1,	// DO NOT USE INTERNAL TEST ONLY = CYCLE_ACTUATOR_TEST              
			WrReservedForFirmware_2,    // DO NOT USE INTERNAL TEST ONLY = CYCLE_ACTUATOR_STARTUP_TEST  
			WrReservedForFirmware_3,    // DO NOT USE INTERNAL TEST ONLY = Verify 24v and 170v powered up properly
			WrWrapperCleanTest,
			endWrEvolTestCode
		}WrEvolTestCode_t;

		typedef enum WrEvolFilmTestParam
		{
			UpperFilm,
			LowerFilm
		}WrEvolFilmTestParam_t;

		typedef enum WrEvolGripperTestParam
		{
			GripperIn,
			GripperOut,
			GripperHome,
			GripperCycle
		}WrEvolGripperTestParam_t;

		typedef enum WrEvolSideClampTestParam
		{
			ClampsOff,
			ClampsOn,
			ClampsCycle
		}WrEvolSideClampTestParam_t;

		typedef enum WrEvolLATestParam
		{
			WandUp,
			WandDown,
			ApplierUp,
			ApplierDown,
			VacuumOn,
			VacuumOff,
			LACycle
		}WrEvolLATestParam_t;

		typedef enum WrEvolShifterTestParam
		{
			ShifterLeft,
			ShifterRight,
			ShifterHome,
			ShifterCalibrate
		} WrEvolShifterTestParam_t;

		typedef enum WrEvolCameraTestParam
		{
			RightCameraScanDigital,
			RightCameraScanAnalog,
			LeftCameraScanDigital,
			LeftCameraScanAnalog,
			CameraHeight,
			CalCameraRight,
			CalCameraLeft
		}WrEvolCameraTestParam_t;

		EvolWrapperTest(Hobart::CEvolMsgReceiver *pHobThread = NULL, WrEvolTestCode wrTestCode = NoServiceTest);
		EvolWrapperTest(const char *buffer);
		virtual ~EvolWrapperTest();
		 
		const char *serialize()const;

		void show();	
		static void getTestName(WrEvolTestCode test, HobString &result);

		WrEvolTestCode getTestCode() const;
		void setTestCode(WrEvolTestCode wrTestCode);

		INT16 getDataInt1() const;
		void setDataInt1(INT16 value);

		INT16 getDataInt2() const;
		void setDataInt2(INT16 value);

		UINT32 getDataLong() const;
		void setDataLong(UINT32 value);

		bool doUseIntParams() const;
		void setUseIntParams();
		void setUseLongParam();


	private:
		WrEvolTestCode	testCode;
		INT16			data1;
		INT16			data2;
		UINT32			dataLong;
		bool			useIntParams;		//	Use the 2 INTs (true) or the ULONG (false)

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif //EVOLWRAPPERTEST_H
