#include "EvolWrapperErrorTranslator.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperErrorTranslator::dataType = 0;
std::string EvolWrapperErrorTranslator::dataTypeName("EvolWrapperErrorTranslator"); 
#endif


EvolWrapperErrorTranslator::EvolWrapperErrorTranslator()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	//create the translation table for aws wrapper errors.

	//Primary mechanism errors
	const std::string errorDescPj = "Primary conveyor jammed";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PrimaryConveyorJammed, errorDescPj));
	const std::string errorDescPh = "Primary conveyor failed to home";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PrimaryConveyorDidNotFindHome, errorDescPh));
	const std::string errorDescPb = "Primary sensor or belt failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PrimaryConveyorStillHome, errorDescPb));
	const std::string errorDescPe = "Primary motor electrical failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PrimaryMotorLowCurrent, errorDescPe));
	const std::string errorDescSp = "Slow primary intake mechanism";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PrimaryMotorMoveError, errorDescSp));
	const std::string errorDescPse = "Primary intake motor stalled";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PrimaryIntakeStalled, errorDescPse));
//	const std::string errorDescPbp = "camera and height sensors blocked";
//	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PrimaryConveyorBlocked, errorDescPbp));

		
	//Secondary mechanism errors
	const std::string errorDescSc = "Secondary conveyor jammed";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SecondaryConveyorJammed, errorDescSc));
	const std::string errorDescSe = "Secondary motor electrical failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SecondaryMotorLowCurrent, errorDescSe));
	const std::string errorDescSs = "Slow secondary mechanism";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SecondaryMotorMoveError, errorDescSs));
	const std::string errorDescSis = "Secondary intake motor stalled";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SecondaryIntakeStalled, errorDescSis));
	const std::string errorDescSes = "Secondary elevator fall sync";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SecondaryElevatorSyncError, errorDescSes));
	const std::string errorDescSps = "Secondary elevator rise sync";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PackageStoppedOnSecondaryConveyor, errorDescSps));

	
	//Gripper mechanism errors
	const std::string errorDescGj = "Gripper jammed";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::GripperJammed, errorDescGj));
	const std::string errorDescGh = "Gripper failed to home";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::GripperDidNotFindHome, errorDescGh));
	const std::string errorDescGsb = "Gripper sensor or belt failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::GripperStillHome, errorDescGsb));
	const std::string errorDescGef = "Gripper motor electrical failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::GripperMotorLowCurrent, errorDescGef));
	const std::string errorDescGes = "Slow gripper";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::GripperMotorMoveError, errorDescGes));
	const std::string errorDescGet = "Gripper stalled";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::GripperStalled, errorDescGet));


	//Elevator mechanism errors
	const std::string errorDescEh = "Elevator failed to home";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::ElevatorDidNotFindHome, errorDescEh));
	const std::string errorDescEs = "Package did not trip elevator intake sensor";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PackageOnElevatorSensorNotTripped, errorDescEs));
	const std::string errorDescEmf = "Elevator motor electrical failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::ElevatorMotorLowCurrent, errorDescEmf));
	const std::string errorDescEsb = "Elevator sensor or cam failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::ElevatorStillHome, errorDescEsb));
	const std::string errorDescEse = "Slow elevator";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::ElevatorMotorMoveError, errorDescEse));
	const std::string errorDescEjd = "Elevator jammed";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::ElevatorJammed, errorDescEjd));
	const std::string errorDescEis = "Elevator sensor not working";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::ElevatorSensorNotWorking, errorDescEis));
	const std::string errorDescEsd = "Elevator stalled";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::ElevatorStalled, errorDescEsd));


	//Rear Folder mechanism errors
	const std::string errorDescRj = "Rear folder jammed";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::RearFolderJammed, errorDescRj));
	const std::string errorDescRf = "Rear folder failed to home";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::RearFolderDidNotFindHome, errorDescRf));
	const std::string errorDescRb = "Rear folder sensor or belt failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::RearFolderStillHome, errorDescRb));
	const std::string errorDescRe = "Rear folder motor electrical failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::RearFolderMotorLowCurrent, errorDescRe));
	const std::string errorDescSrF = "Slow rear folder";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::RearFolderMotorMoveError, errorDescSrF));
	const std::string errorDescRfs = "Rear folder motor stalled";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::RearFolderStalled, errorDescRfs));


	//Front Folder mechanism errors
	const std::string errorDescFj = "Front folder jammed";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::FrontFolderJammed, errorDescFj));
	const std::string errorDescFh = "Front folder failed to home";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::FrontFolderDidNotFindHome, errorDescFh));
	const std::string errorDescFfsb = "Front folder sensor or belt failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::FrontFolderStillHome, errorDescFfsb));
	const std::string errorDescFfe = "Front folder motor electrical failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::FrontFolderMotorLowCurrent, errorDescFfe));
	const std::string errorDescFfs = "Slow front folder";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::FrontFolderMotorMoveError, errorDescFfs));
	const std::string errorDescFFs = "Front folder motor stalled";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::FrontFolderStalled, errorDescFFs));


	//Pusher mechanism errors
	const std::string errorDescUj = "Pusher jammed";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PusherJammed, errorDescUj));
	const std::string errorDescUh = "Pusher failed to home";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PusherDidNotFindHome, errorDescUh));
	const std::string errorDescUsb = "Pusher sensor or belt failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PusherStillHome, errorDescUsb));
	const std::string errorDescUef = "Pusher motor electrical failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PusherMotorLowCurrent, errorDescUef));
	const std::string errorDescUs = "Slow pusher";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PusherMotorMoveError, errorDescUs));
	const std::string errorDescUa = "Pusher motor stall";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PusherStalled, errorDescUa));


	//Side Folder mechanism errors
	const std::string errorDescIj = "Side folders jammed";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SideFoldersJammed, errorDescIj));
	const std::string errorDescIh = "Side folder failed to home";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SideFoldersDidNotFindHome, errorDescIh));
	const std::string errorDescSfsb = "Side folder sensor or belt";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SideFolderStillHome, errorDescSfsb));
	const std::string errorDescSfef = "Side folder motor electrical failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SideFolderMotorLowCurrent, errorDescSfef));
	const std::string errorDescSsf = "Slow side folder";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SideFolderMotorMoveError, errorDescSsf));
	const std::string errorDescsfs = "Side folder motor stalled";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SideFolderStalled, errorDescsfs));


	//Side Clamps mechanism errors
	const std::string errorDescCj = "Side clamps jammed";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SideClampsJammed, errorDescCj));
	const std::string errorDescCf = "Side clamps failed to home";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SideClampsDidNotFindHome, errorDescCf));
	const std::string errorDescScsb = "Side clamps sensor or belt failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SideClampsStillHome, errorDescScsb));
	const std::string errorDescScmf = "Side clamps motor electrical failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SideClampsMotorLowCurrent, errorDescScmf));
	const std::string errorDescScs = "Slow side clamps";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SideClampsMotorMoveError, errorDescScs));
	const std::string errorDescSCs = "Side clamp motor stalled";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SideClampStalled, errorDescSCs));


	//Shifter mechanism errors
	const std::string errorDescSj = "Shifter jammed";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SecondaryShiftJammed, errorDescSj));
	const std::string errorDescSh = "Shifter failed to home";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SecondaryShiftDidNotFindHome, errorDescSh));


	//Knife mechanism errors
	const std::string errorDescKf = "Knife to long to fire";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::KnifeFireTimeExceeded, errorDescKf));
	const std::string errorDescKse = "Knife sensor did not trip";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::KnifeSensorNoTrip, errorDescKse));
	const std::string errorDescKje = "Knife jammed or sensor failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::KnifeSensorDidntClear, errorDescKje));


	//SealerBelt mechanism errors
	const std::string errorDescBj = "Sealer belt jammed";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SealerBeltJammed, errorDescBj));
	const std::string errorDescSbs = "Sealer belt motor electrical failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SealerBeltMotorLowCurrent, errorDescSbs));


	//Node Board errors
	const std::string errorDescFu = "Node 1 fuse blown";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::Node1_MotorFuseBlown, errorDescFu));
	const std::string errorDescFu1 = "Node 2 fuse blown";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::Node2_MotorFuseBlown, errorDescFu1));
	const std::string errorDescFu2 = "Node 3 fuse blown";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::Node3_MotorFuseBlown, errorDescFu2));
	const std::string errorDescFu3 = "Node 4 fuse blown";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::Node4_MotorFuseBlown, errorDescFu3));
	const std::string errorDescFu4 = "Node 5 fuse blown";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::Node5_MotorFuseBlown, errorDescFu4));
	const std::string errorDescN1c = "Node 1 CAN communication failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::Node1CanCommFailure, errorDescN1c));
	const std::string errorDescN3c = "Node 3 CAN communication failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::Node3CanCommFailure, errorDescN3c));
	const std::string errorDescN4c = "Node 4 CAN communication failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::Node4CanCommFailure, errorDescN4c));
	const std::string errorDescN5c = "Node 5 CAN communication failure";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::Node5CanCommFailure, errorDescN5c));


	//Wrap Box Timings errors
	const std::string errorDescWf = "Wrapper timings failed to update";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::DataFlashUpdateTimeout, errorDescWf));
	const std::string errorDescWm = "Wrapper timings update memory failed";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::DataFlashUpdateCallocError, errorDescWm));
	const std::string errorDescAd = "Could not adjust net stretch enough for package";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::FilmTooSmallForPackage, errorDescAd));
	const std::string errorDescWbc = "Wrap box corruption";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsCorrupt, errorDescWbc));
	const std::string errorDescNwt1 = "Wrap box timings empty on node 1";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsNullNode1, errorDescNwt1));
	const std::string errorDescNwt2 = "Wrap box timings empty on node 2";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsNullNode2, errorDescNwt2));
	const std::string errorDescNwt3 = "Wrap box timings empty on node 3";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsNullNode3, errorDescNwt3));
	const std::string errorDescNwt4 = "Wrap box timings empty on node 4";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsNullNode4, errorDescNwt4));
	const std::string errorDescNwt5 = "Wrap box timings empty on node 5";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsNullNode5, errorDescNwt5));
	const std::string errorDescNwc1 = "Wrap box timings corruption node 1";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsCorruptNode1, errorDescNwc1));
	const std::string errorDescNwc2 = "Wrap box timings corruption node 2";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsCorruptNode2, errorDescNwc2));
	const std::string errorDescNwc3 = "Wrap box timings corruption node 3";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsCorruptNode3, errorDescNwc3));
	const std::string errorDescNwc4 = "Wrap box timings corruption node 4";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsCorruptNode4, errorDescNwc4));
	const std::string errorDescNwc5 = "Wrap box timings corruption node 5";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsCorruptNode5, errorDescNwc5));
	const std::string errorDescWbc1 = "Missing height box node 1";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsMissingHBNode1, errorDescWbc1));
	const std::string errorDescWbc2 = "Missing height box node 2";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsMissingHBNode2, errorDescWbc2));
	const std::string errorDescWbc3 = "Missing height box node 3";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsMissingHBNode3, errorDescWbc3));
	const std::string errorDescWbc4 = "Missing height box node 4";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsMissingHBNode4, errorDescWbc4));
	const std::string errorDescWbc5 = "Missing height box node 5";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsMissingHBNode5, errorDescWbc5));
	const std::string errorDescWtm1 = "Too many wrap boxes node 1";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsTooManyNode1, errorDescWtm1));
	const std::string errorDescWtm2 = "Too many wrap boxes node 2";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsTooManyNode2, errorDescWtm2));
	const std::string errorDescWtm3 = "Too many wrap boxes node 3";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsTooManyNode3, errorDescWtm3));
	const std::string errorDescWtm4 = "Too many wrap boxes node 4";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsTooManyNode4, errorDescWtm4));
	const std::string errorDescWtm5 = "Too many wrap boxes node 5";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsTooManyNode5, errorDescWtm5));
	const std::string errorDescWmb1 = "Missing wrap box node 1";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsMissingWBNode1, errorDescWmb1));
	const std::string errorDescWmb2 = "Missing wrap box node 2";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsMissingWBNode2, errorDescWmb2));
	const std::string errorDescWmb3 = "Missing wrap box node 3";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsMissingWBNode3, errorDescWmb3));
	const std::string errorDescWmb4 = "Missing wrap box node 4";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsMissingWBNode4, errorDescWmb4));
	const std::string errorDescWmb5 = "Missing wrap box node 5";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsMissingWBNode5, errorDescWmb5));
	const std::string errorDescWbcf1 = "Wrap box config corruption node 1";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsCorrupt_CONFIG_Node1, errorDescWbcf1));
	const std::string errorDescWbcf2 = "Wrap box config corruption node 2";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsCorrupt_CONFIG_Node2, errorDescWbcf2));
	const std::string errorDescWbcf3 = "Wrap box config corruption node 3";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsCorrupt_CONFIG_Node3, errorDescWbcf3));
	const std::string errorDescWbcf4 = "Wrap box config corruption node 4";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsCorrupt_CONFIG_Node4, errorDescWbcf4));
	const std::string errorDescWbcf5 = "Wrap box config corruption node 5";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsCorrupt_CONFIG_Node5, errorDescWbcf5));
	const std::string errorDescWctm1 = "Too many wrap boxes in config node 1";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsConfigTooManyNode1, errorDescWctm1));
	const std::string errorDescWctm2 = "Too many wrap boxes in config node 2";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsConfigTooManyNode2, errorDescWctm2));
	const std::string errorDescWctm3 = "Too many wrap boxes in config node 3";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsConfigTooManyNode3, errorDescWctm3));
	const std::string errorDescWctm4 = "Too many wrap boxes in config node 4";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsConfigTooManyNode4, errorDescWctm4));
	const std::string errorDescWctm5 = "Too many wrap boxes in config node 5";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsConfigTooManyNode5, errorDescWctm5));
	const std::string errorDescRbc = "Wrap box read buffer corruption";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxReadBufferCorruption, errorDescRbc));
	const std::string errorDescWbcc = "Wrap box write buffer corruption";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxWriteBufferCorruption, errorDescWbcc));
	const std::string errorDescWbtb = "Too many wrap boxes in read buffer";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsReadBufferTooMany, errorDescWbtb));
	const std::string errorDescWbtbw = "Too many wrap boxes in write buffer";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::WrapBoxTimingsWriteBufferTooMany, errorDescWbtbw));

	// NGW Errors
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::NarrowGripperFailedToClose, std::string("The narrow gripper sensor has failed to close")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::NarrowGripperFailedToOpen, std::string("The narrow gripper sensor has failed to open")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::LeftGripperFailedToClose, std::string("The left gripper sensor has failed to close")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::LeftGripperFailedToOpen, std::string("The left gripper sensor has failed to open")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::RightGripperFailedToClose, std::string("The right gripper sensor has failed to close")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::RightGripperFailedToOpen, std::string("The right gripper sensor has failed to open")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::LeftSideClampFailedToClose, std::string("The left side clamp sensor has failed to close")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::LeftSideClampFailedToOpen, std::string("The left side clamp sensor has failed to open")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::RightSideClampFailedToClose, std::string("The right side clamp sensor has failed to close")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::RightSideClampFailedToOpen, std::string("The right side clamp sensor has failed to open")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::UpperFilmSelectorFailedToSwitch, std::string("Upper Film Roll Failed to Switch")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::LowerFilmSelectorFailedToSwitch, std::string("Lower Film Roll Failed to Switch")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::FilmSelectorUnlatched, std::string("Film Selector Lever is unlatched")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::ApplierWandUpFailure, std::string("Primary Applier Wand Failed to Move Up")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::ApplierWandDownFailure, std::string("Primary Applier Wand Failed to Move Down")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::ApplierHandHomeFailure, std::string("Primary Applier Hand Failed to Home")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::ApplierWandVacuumFailure, std::string("Primary Applier Wand Vacuum Failure")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::AirCompressorFailure, std::string("Air Compressor Failure")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::RegulatedAirPressureFailure, std::string("Regulated Air Pressure Failure")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::ActuatorAirPressureFailure, std::string("Actuator Air Pressure Failure")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::BadUpperFilmPull, std::string("Bad Upper Film Pull")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::BadLowerFilmPull, std::string("Bad Lower Film Pull")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PowerSupplyFailure24V, std::string("24V Power Supply Failure")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::InrushFailure170V, std::string("170V Inrush Failure")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PowerSupplyFailure170V, std::string("170V Power Supply Failure")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::UpperFilmBrakeFailure, std::string("Upper Film Brake Failure")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::LowerFilmBrakeFailure, std::string("Lower Film Brake Failure")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::UpperFilmRollLooseArbor, std::string("Upper Film Roll is Loose")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::LowerFilmRollLooseArbor, std::string("Lower Film Roll is Loose")));

	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PrinterDockFailure, std::string("Printer Dock Failure")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::BlockedPackageSensor, std::string("Sealer Belt Package Sensor Blocked")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PressStartButton, std::string("Press Start")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::IntakeSensorBlocked, std::string("Intake Sensor Blocked")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::RegulatedPressureTooHigh, std::string("Regulated Air Pressure too high")));

	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::ActuatorAirPressureFailure, std::string("Actuator Air Pressure Failure")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::BadUpperFilmPull, std::string("Bad Upper Film Pull")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::BadLowerFilmPull, std::string("Bad Lower Film Pull")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PowerSupplyFailure24V, std::string("24V Power Supply Failure")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::InrushFailure170V, std::string("170V Inrush Failure")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PowerSupplyFailure170V, std::string("170V Power Supply Failure")));

	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::UpperFilmBrakeFailure, std::string("Upper Film Brake Failure")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::LowerFilmBrakeFailure, std::string("Lower Film Brake Failure")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::UpperFilmRollLooseArbor, std::string("Upper Film Roll Loose Arbor")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::LowerFilmRollLooseArbor, std::string("Lower Film Roll Loose Arbor")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PrinterDockFailure, std::string("Printer Dock Failure")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::BlockedPackageSensor, std::string("Blocked Package Sensor")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PressStartButton, std::string("Press Start Button")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::IntakeSensorBlocked, std::string("Intake Sensor Blocked")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::RegulatedPressureTooHigh, std::string("Regulated Pressure Too High")));

	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SecApplierWandUpFailure, std::string("Secondary Applier Wand Failed to Move Up")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SecApplierWandDownFailure, std::string("Secondary Applier Wand Failed to Move Down")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SecApplierHandHomeFailure, std::string("Secondary Applier Hand Failed to Home")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SecApplierWandVacuumFailure, std::string("Secondary Applier Wand Vacuum Failure")));

	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SecApplierControllerTimeout, std::string("Second Applier Controller Timed Out")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SecApplierControllerJammed, std::string("Second Applier Controller Jammed ")));
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::SecApplierControllerHomingFailure, std::string("Second Applier Controller Homing Failure")));

	//Camera errors
	const std::string errorDescCd = "Dimension error from cameras";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PackageDimensionError, errorDescCd));
	const std::string errorDescCHf = "Height sensors failed auto cal";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::CameraHeightCalError, errorDescCHf));
	const std::string errorDescCAn = "Cameras alignment needed";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::CameraAlignmentNeeded, errorDescCAn));

		
//	const std::string errorDescPi = "package blocking intake";
//	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PackageBlockingIntake, errorDescPi));
	const std::string errorDescAh = "Label applier failed to home";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::LabelApplierDidntHome, errorDescAh));

	//	PTR 1554
	const std::string errorDescWbpseb = "Package sensor across elevator is blocked";
	errorTranslator.insert(make_pair(EvolWrapperErrorInfo::PkgSensorAcrossElevatorBlocked, errorDescWbpseb));
}

EvolWrapperErrorTranslator::~EvolWrapperErrorTranslator()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

EvolWrapperErrorTranslator *EvolWrapperErrorTranslator::getInstance()
{
	if (instance == 0)
	{
		instance = new EvolWrapperErrorTranslator();
		Debug::trace("EvolWrapperErrorTranslator", "Created instance of: EvolWrapperErrorTranslator");
	}
	return instance;
}


EvolWrapperErrorTranslator *EvolWrapperErrorTranslator::instance = (EvolWrapperErrorTranslator *) NULL;

const std::string EvolWrapperErrorTranslator::translateEvolWrapperError(UINT16 wrapperError)
{
	
	EvolWrapperErrorInfo::EvolWrapperError evolWrapperError = (EvolWrapperErrorInfo::EvolWrapperError)(501 + wrapperError);
	std::map<EvolWrapperErrorInfo::EvolWrapperError, const std::string>::iterator i = errorTranslator.find(evolWrapperError);
	
	if(i != errorTranslator.end())
	{
		return(i->second);
	}
	else
	{
		return("Unknown wrapper error");
	}
}
