/**
 * @file NGWEngException.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Wrapper internal exception CAN message for engineering debug.
 * Created on: Mar 16, 2018
 *
 **/
#include "NGWEngException.h"
#include "WrapperEvent.h"
#include "Debug.h"
#include "Integer.h"

NGWEngException::NGWEngException( Hobart::CEvolMsgReceiver *pHobThread,  MechanismType mech, NGWException error )
								: EvolMessage( Wrapper, WrNGWENGException, pHobThread )
{
	mechanism = mech;
	exception = error;
}

NGWEngException::NGWEngException(const char *buffer) : EvolMessage(buffer)
{

	EvolMessage::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &mechanism;
	for (unsigned int i = 0; i < sizeof(mechanism); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &exception;
	for (unsigned int i = 0; i < sizeof(exception); i++)
	{
		*pChar++ = buffer[j++];
	}
}

NGWEngException::~NGWEngException()
{}

const char *NGWEngException::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &mechanism;
	for (unsigned int i = 0; i < sizeof(mechanism); i++)
	{
		serializedData.append(1, pChar[i]);
	}


	pChar = (char *)&exception;
	for (unsigned int i = 0; i < sizeof(exception); i++)
	{
		serializedData.append(1, pChar[i] );
	}

	return serializedData.data();

}

void NGWEngException::show()
{
	Debug::trace( "NGWEngException", "Exception: " + Integer::toString( exception ) );
	switch( exception )
	{
		case _NGW_INTAKE_STILL_HOME: {
			Debug::trace( "NGWEngException", "Primary intake failed to move off home." );
			break;
		}
		case _NGW_INTAKE_LOW_MOTOR_CURRENT: {
			Debug::trace( "NGWEngException", "Primary intake low motor current." );
			break;
		}
		case _NGW_INTAKE_MOTOR_MOVE_ERROR: {
			Debug::trace( "NGWEngException", "Primary intake movement error." );
			break;
		}
		case _NGW_SECINTAKE_LOW_MOTOR_CURRENT: {
			Debug::trace( "NGWEngException", "Secondary intake low motor current." );
			break;
		}
		case _NGW_CAMERA_HEIGHT_CAL_ERROR: {
			Debug::trace( "NGWEngException", "Camera height calibration error." );
			break;
		}
		case _NGW_REARFOLDER_STILL_HOME: {
			Debug::trace( "NGWEngException", "Rear folder failed to move off home." );
			break;
		}
		case _NGW_REARFOLDER_LOW_MOTOR_CURRENT: {
			Debug::trace( "NGWEngException", "Rear folder low motor current." );
			break;
		}
		case _NGW_REARFOLDER_MOTOR_MOVE_ERROR: {
			Debug::trace( "NGWEngException", "Rear folder movement error." );
			break;
		}
		case _NGW_FRONTFOLDER_STILL_HOME: {
			Debug::trace( "NGWEngException", "Front folder failed to move off home." );
			break;
		}
		case _NGW_FRONTFOLDER_LOW_MOTOR_CURRENT: {
			Debug::trace( "NGWEngException", "Front folder low motor current." );
			break;
		}
		case _NGW_SEALERBELT_LOW_MOTOR_CURRENT: {
			Debug::trace( "NGWEngException", "Sealer low motor current." );
			break;
		}
		case _NGW_PUSHER_STILL_HOME: {
			Debug::trace( "NGWEngException", "Pusher failed to move off home." );
			break;
		}
		case _NGW_PUSHER_LOW_MOTOR_CURRENT: {
			Debug::trace( "NGWEngException", "Pusher folder low motor current." );
			break;
		}
		case _NGW_INTAKE_NOT_AT_HOME: {
			Debug::trace( "NGWEngException", "Primary intake failed to home." );
			break;
		}
		case _NGW_INTAKE_BLOCKED: {
			Debug::trace( "NGWEngException", "Primary intake blocked." );
			break;
		}
		case _NGW_INTAKE_JAMMED: {
			Debug::trace( "NGWEngException", "Primary intake jammed." );
			break;
		}
		case _NGW_INTAKE_DIDNOTFIND_HOME: {
			Debug::trace( "NGWEngException", "Primary intake did not find home." );
			break;
		}
		case _NGW_PACKAGE_BLOCKING_INTAKE: {
			Debug::trace( "NGWEngException", "Package blocking intake." );
			break;
		}
		case _NGW_SECINTAKE_NOT_CLROFPADDLES: {
			Debug::trace( "NGWEngException", "Secondary intake did not clear primary paddles." );
			break;
		}
		case _NGW_PACKAGE_DIMENSION_ERROR: {
			Debug::trace( "NGWEngException", "Package dimension error." );
			break;
		}
		case _NGW_ELEVATOR_SENSOR_NOT_TRIPPED: {
			Debug::trace( "NGWEngException", "Elevator sensor not tripped." );
			break;
		}
		case _NGW_SECINTAKE_JAMMED: {
			Debug::trace( "NGWEngException", "Secondary intake jammed." );
			break;
		}
		case _NGW_SHIFTER_NOT_AT_HOME: {
			Debug::trace( "NGWEngException", "Shifter not at home." );
			break;
		}
		case _NGW_CAMERA_ALIGNMENT_NEEDED: {
			Debug::trace( "NGWEngException", "Cameras need alignment." );
			break;
		}
		case _NGW_SHIFTER_LEFT_LIMIT_ERROR: {
			Debug::trace( "NGWEngException", "Shifter limit error left." );
			break;
		}
		case _NGW_SHIFTER_RIGHT_LIMIT_ERROR: {
			Debug::trace( "NGWEngException", "Shifter limit error right." );
			break;
		}
		case _NGW_SHIFTER_DIDNOTFIND_HOME: {
			Debug::trace( "NGWEngException", "Shifter did not find home." );
			break;
		}
		case _NGW_SECINTAKE_MOTOR_MOVE_ERROR: {
			Debug::trace( "NGWEngException", "Secondary intake movement error." );
			break;
		}
		case _NGW_ELEVATOR_NOT_AT_HOME: {
			Debug::trace( "NGWEngException", "Elevator not at home." );
			break;
		}
		case _NGW_ELEVATOR_JAMMED: {
			Debug::trace( "NGWEngException", "Elevator jammed." );
			break;
		}
		case _NGW_ELEVATOR_LOW_MOTOR_CURRENT: {
			Debug::trace( "NGWEngException", "Elevator low motor current." );
			break;
		}
		case _NGW_ELEVATOR_DIDNOTFIND_HOME: {
			Debug::trace( "NGWEngException", "Elevator did not find home." );
			break;
		}
		case _NGW_ELEVATOR_STILL_HOME: {
			Debug::trace( "NGWEngException", "Elevator failed to move from home." );
			break;
		}
		case _NGW_ELEVATOR_SENSOR_NOTWORKING: {
			Debug::trace( "NGWEngException", "Elevator sensor not working." );
			break;
		}
		case _NGW_ELEVATOR_MOTOR_MOVE_ERROR: {
			Debug::trace( "NGWEngException", "Elevator movement error." );
			break;
		}
		case _NGW_FILMSELECTOR_TO_SMALL_FOR_PACKAGE: {
			Debug::trace( "NGWEngException", "Film selector film to small for package." );
			break;
		}
		case _NGW_FILMSELECTOR_FILM_NOT_PULLED: {
			Debug::trace( "NGWEngException", "Gripper did not pull film from film selector." );
			break;
		}
		case _NGW_GRIPPER_NOT_AT_HOME: {
			Debug::trace( "NGWEngException", "Gripper not at home." );
			break;
		}
		case _NGW_KNIFE_SENSOR_NOT_TRIPPED: {
			Debug::trace( "NGWEngException", "Knife sensor did not trip." );
			break;
		}
		case _NGW_GRIPPER_JAMMED: {
			Debug::trace( "NGWEngException", "Gripper jammed." );
			break;
		}
		case _NGW_GRIPPER_DIDNOTFIND_HOME: {
			Debug::trace( "NGWEngException", "Gripper did not find home." );
			break;
		}
		case _NGW_GRIPPER_STILL_HOME: {
			Debug::trace( "NGWEngException", "Gripper did not move off home." );
			break;
		}
		case _NGW_GRIPPER_LOW_MOTOR_CURRENT: {
			Debug::trace( "NGWEngException", "Gripper low motor current." );
			break;
		}
		case _NGW_KNIFE_FIRE_TIME_EXCEEDED: {
			Debug::trace( "NGWEngException", "Knife mechanism too slow." );
			break;
		}
		case _NGW_CLAMPS_NOT_AT_HOME: {
			Debug::trace( "NGWEngException", "Clamps not at home." );
			break;
		}
		case _NGW_CLAMPS_JAMMED: {
			Debug::trace( "NGWEngException", "Clamps jammed." );
			break;
		}
		case _NGW_SIDEFOLDER_NOT_AT_HOME: {
			Debug::trace( "NGWEngException", "Side folder not at home." );
			break;
		}
		case _NGW_SIDEFOLDER_JAMMED: {
			Debug::trace( "NGWEngException", "Side folder jammed." );
			break;
		}
		case _NGW_REARFOLDER_NOT_AT_HOME: {
			Debug::trace( "NGWEngException", "Rear folder not at home." );
			break;
		}
		case _NGW_REARFOLDER_JAMMED: {
			Debug::trace( "NGWEngException", "Rear folder jammed." );
			break;
		}
		case _NGW_FRONTFOLDER_NOT_AT_HOME: {
			Debug::trace( "NGWEngException", "Front folder not at home." );
			break;
		}
		case _NGW_FRONTFOLDER_JAMMMED: {
			Debug::trace( "NGWEngException", "Front folder jammed." );
			break;
		}
		case _NGW_PUSHER_NOT_AT_HOME: {
			Debug::trace( "NGWEngException", "Pusher not at home." );
			break;
		}
		case _NGW_PUSHER_JAMMED: {
			Debug::trace( "NGWEngException", "Pusher jammed." );
			break;
		}
		case _NGW_SEALERBELT_JAMMED: {
			Debug::trace( "NGWEngException", "Sealer belt jammed" );
			break;
		}
		case _NGW_KNIFE_SENSOR_NOT_CLEAR: {
			Debug::trace( "NGWEngException", "Knife sensor not clear." );
			break;
		}
		case _NGW_APPLIER_WAND_DIDNOTFIND_HOME: {
			Debug::trace( "NGWEngException", "Applier did not find home." );
			break;
		}
		case _NGW_TEMPERATURE_ERROR: {
			Debug::trace( "NGWEngException", "Temperature error." );
			break;
		}
		case _NGW_HEATER_ERROR: {
			Debug::trace( "NGWEngException", "Heater error." );
			break;
		}
		case _NGW_GRIPPER_MOTOR_MOVE_ERROR: {
			Debug::trace( "NGWEngException", "Gripper movement error." );
			break;
		}
		case _NGW_CLAMPS_STILL_AT_HOME: {
			Debug::trace( "NGWEngException", "Clamps did not move off home." );
			break;
		}
		case _NGW_CLAMPS_LOW_MOTOR_CURRENT: {
			Debug::trace( "NGWEngException", "Clamps low motor current." );
			break;
		}
		case _NGW_CLAMPS_MOTOR_MOVE_ERROR: {
			Debug::trace( "NGWEngException", "Clamps movement error." );
			break;
		}
		case _NGW_NO_CAN_COMM: {
			Debug::trace( "NGWEngException", "No CAN communications." );
			break;
		}
		case _NGW_FRONTFOLDER_MOTOR_MOVE_ERROR: {
			Debug::trace( "NGWEngException", "Front folder movement error." );
			break;
		}
		case _NGW_INTERLOCK_RIGHT_CANOPY: {
			Debug::trace( "NGWEngException", "Clamps movement error." );
			break;
		}
		case _NGW_INTERLOCK_RIGHT_COVER: {
			Debug::trace( "NGWEngException", "Right cover interlock." );
			break;
		}
		case _NGW_INTERLOCK_LEFT_COVER: {
			Debug::trace( "NGWEngException", "Left cover interlock." );
			break;
		}
		case _NGW_INTERLOCK_LEFT_CANOPY: {
			Debug::trace( "NGWEngException", "Left canopy interlock." );
			break;
		}
		case _NGW_INTERLOCK_KNIFE_ACCESS: {
			Debug::trace( "NGWEngException", "Knife access interlock." );
			break;
		}
		case _NGW_INTERLOCK_FILM_DOOR: {
			Debug::trace( "NGWEngException", "Film door interlock." );
			break;
		}
		case _NGW_SIDEFOLDER_STILL_HOME: {
			Debug::trace( "NGWEngException", "Side folder failed to move off home." );
			break;
		}
		case _NGW_SIDEFOLDER_LOW_MOTOR_CURRENT: {
			Debug::trace( "NGWEngException", "Side folder low motor current." );
			break;
		}
		case _NGW_CLAMPS_DIDNOTFIND_HOME: {
			Debug::trace( "NGWEngException", "Clamps did not find home." );
			break;
		}
		case _NGW_SIDEFOLDER_DIDNOTFIND_HOME: {
			Debug::trace( "NGWEngException", "Side folder did not find home." );
			break;
		}
		case _NGW_REARFOLDER_DIDNOTFIND_HOME: {
			Debug::trace( "NGWEngException", "Rear folder did not find home." );
			break;
		}
		case _NGW_FRONTFOLDER_DIDNOTFIND_HOME: {
			Debug::trace( "NGWEngException", "Front folder did not find home." );
			break;
		}
		case _NGW_PUSHER_DIDNOTFIND_HOME: {
			Debug::trace( "NGWEngException", "Pusher did not find home." );
			break;
		}
		case _NGW_CAMERA_CALIBRATION_FAILURE: {
			Debug::trace( "NGWEngException", "Camera calibration error." );
			break;
		}
		case _NGW_SECINTAKE_PACKAGE_STOPPED: {
			Debug::trace( "NGWEngException", "Secondary intake package stopped." );
			break;
		}
		case _NGW_SIDEFOLDER_MOTOR_MOVE_ERROR: {
			Debug::trace( "NGWEngException", "Side folder movement error." );
			break;
		}
		case _NGW_GHOST_PKG_DETECTED: {
			Debug::trace( "NGWEngException", "Ghost package." );
			break;
		}
		case _NGW_PKG_TOO_TAll: {
			Debug::trace( "NGWEngException", "Package too tall for intake." );
			break;
		}
		case _NGW_NODE1_MOTOR_FUSE_BLOWN: {
			Debug::trace( "NGWEngException", "Node 1 motor fuse blown." );
			break;
		}
		case _NGW_NODE2_MOTOR_FUSE_BLOWN: {
			Debug::trace( "NGWEngException", "Node 2 motor fuse blown." );
			break;
		}
		case _NGW_NODE3_MOTOR_FUSE_BLOWN: {
			Debug::trace( "NGWEngException", "Node 3 motor fuse blown." );
			break;
		}
		case _NGW_NODE4_MOTOR_FUSE_BLOWN: {
			Debug::trace( "NGWEngException", "Node 4 motor fuse blown." );
			break;
		}
		case _NGW_NODE5_MOTOR_FUSE_BLOWN: {
			Debug::trace( "NGWEngException", "Node 5 motor fuse blown." );
			break;
		}
		case _NGW_TOP_ROLL_FILM_LOW_WARN: {
			Debug::trace( "NGWEngException", "Top film roll low." );
			break;
		}
		case _NGW_BOTTOM_ROLL_FILM_LOW_WARN: {
			Debug::trace( "NGWEngException", "Bottom film roll low." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_NULL_NODE1: {
			Debug::trace( "NGWEngException", "Node 1 data flash erased." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_NULL_NODE2: {
			Debug::trace( "NGWEngException", "Node 2 data flash erased." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_NULL_NODE3: {
			Debug::trace( "NGWEngException", "Node 3 data flash erased." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_NULL_NODE4: {
			Debug::trace( "NGWEngException", "Node 4 data flash erased." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_NULL_NODE5: {
			Debug::trace( "NGWEngException", "Node 5 data flash erased." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_CORRUPT_NODE1: {
			Debug::trace( "NGWEngException", "Node 1 data flash corruption." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_CORRUPT_NODE2: {
			Debug::trace( "NGWEngException", "Node 2 data flash corruption." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_CORRUPT_NODE3: {
			Debug::trace( "NGWEngException", "Node 3 data flash corruption." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_CORRUPT_NODE4: {
			Debug::trace( "NGWEngException", "Node 4 data flash corruption." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_CORRUPT_NODE5: {
			Debug::trace( "NGWEngException", "Node 5 data flash corruption." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_MISSING_HB_NODE1: {
			Debug::trace( "NGWEngException", "Node 1 height box missing from timings." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_MISSING_HB_NODE2: {
			Debug::trace( "NGWEngException", "Node 2 height box missing from timings." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_MISSING_HB_NODE3: {
			Debug::trace( "NGWEngException", "Node 3 height box missing from timings." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_MISSING_HB_NODE4: {
			Debug::trace( "NGWEngException", "Node 4 height box missing from timings." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_MISSING_HB_NODE5: {
			Debug::trace( "NGWEngException", "Node 5 height box missing from timings." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_TO_MANY_NODE1: {
			Debug::trace( "NGWEngException", "Node 1 wrap box timings to large." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_TO_MANY_NODE2: {
			Debug::trace( "NGWEngException", "Node 2 wrap box timings to large." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_TO_MANY_NODE3: {
			Debug::trace( "NGWEngException", "Node 3 wrap box timings to large." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_TO_MANY_NODE4: {
			Debug::trace( "NGWEngException", "Node 4 wrap box timings to large." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_TO_MANY_NODE5: {
			Debug::trace( "NGWEngException", "Node 5 wrap box timings to large." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_MISSING_WB_NODE1: {
			Debug::trace( "NGWEngException", "Node 1 wrap box missing from timings." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_MISSING_WB_NODE2: {
			Debug::trace( "NGWEngException", "Node 2 wrap box missing from timings." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_MISSING_WB_NODE3: {
			Debug::trace( "NGWEngException", "Node 3 wrap box missing from timings." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_MISSING_WB_NODE4: {
			Debug::trace( "NGWEngException", "Node 4 wrap box missing from timings." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_MISSING_WB_NODE5: {
			Debug::trace( "NGWEngException", "Node 5 wrap box missing from timings." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_CORRUPT_CONFIG_NODE1: {
			Debug::trace( "NGWEngException", "Node 1 wrap box corrupt configuration." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_CORRUPT_CONFIG_NODE2: {
			Debug::trace( "NGWEngException", "Node 2 wrap box corrupt configuration." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_CORRUPT_CONFIG_NODE3: {
			Debug::trace( "NGWEngException", "Node 3 wrap box corrupt configuration." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_CORRUPT_CONFIG_NODE4: {
			Debug::trace( "NGWEngException", "Node 4 wrap box corrupt configuration." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_CORRUPT_CONFIG_NODE5: {
			Debug::trace( "NGWEngException", "Node 5 wrap box corrupt configuration." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_CONFIG_TO_MANY_NODE1: {
			Debug::trace( "NGWEngException", "Node 1 wrap box configuration to many." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_CONFIG_TO_MANY_NODE2: {
			Debug::trace( "NGWEngException", "Node 2 wrap box configuration to many." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_CONFIG_TO_MANY_NODE3: {
			Debug::trace( "NGWEngException", "Node 3 wrap box configuration to many." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_CONFIG_TO_MANY_NODE4: {
			Debug::trace( "NGWEngException", "Node 4 wrap box configuration to many." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_CONFIG_TO_MANY_NODE5: {
			Debug::trace( "NGWEngException", "Node 5 wrap box configuration to many." );
			break;
		}
		case _NGW_WRAP_BOX_READ_BUFFER_CORRUPTION: {
			Debug::trace( "NGWEngException", "Wrap box read buffer corruption." );
			break;
		}
		case _NGW_WRAP_BOX_WRITE_BUFFER_CORRUPTION: {
			Debug::trace( "NGWEngException", "Wrap box write buffer corruption." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_READ_BUFFER_TO_MANY: {
			Debug::trace( "NGWEngException", "Wrap box read buffer to many." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_WRITE_BUFFER_TO_MANY: {
			Debug::trace( "NGWEngException", "Wrap box write buffer top many." );
			break;
		}
		case _NGW_ELEVATOR_SENSOR_BLOCKED: {
			Debug::trace( "NGWEngException", "Elevator sensor blocked." );
			break;
		}
		case _NGW_DATA_FLASH_UPDATE_TIMEOUT: {
			Debug::trace( "NGWEngException", "Data flash update timeout." );
			break;
		}
		case _NGW_DATA_FLASH_UPDATE_CALLOC_ERROR: {
			Debug::trace( "NGWEngException", "Data flash calloc error." );
			break;
		}
		case _NGW_NODE1_CAN_COMM_FAILURE: {
			Debug::trace( "NGWEngException", "Node 1 CAN communications failure." );
			break;
		}
		case _NGW_NODE3_CAN_COMM_FAILURE: {
			Debug::trace( "NGWEngException", "Node 3 CAN communications failure." );
			break;
		}
		case _NGW_NODE4_CAN_COMM_FAILURE: {
			Debug::trace( "NGWEngException", "Node 4 CAN communications failure." );
			break;
		}
		case _NGW_NODE5_CAN_COMM_FAILURE: {
			Debug::trace( "NGWEngException", "Node 5 CAN communications failure." );
			break;
		}
		case _NGW_PUSHER_MOTOR_MOVE_ERROR: {
			Debug::trace( "NGWEngException", "Pusher movement error." );
			break;
		}
		case _NGW_WRAP_BOX_TIMINGS_CORRUPT: {
			Debug::trace( "NGWEngException", "Wrap box timings corruption." );
			break;
		}
		case _NGW_INTAKE_STALL: {
			Debug::trace( "NGWEngException", "Primary intake stalled." );
			break;
		}
		case _NGW_SECINTAKE_STALL: {
			Debug::trace( "NGWEngException", "Secondary intake stalled." );
			break;
		}
		case _NGW_ELEVATOR_STALL: {
			Debug::trace( "NGWEngException", "Elevator stalled." );
			break;
		}
		case _NGW_REARFOLDER_STALL: {
			Debug::trace( "NGWEngException", "Rear folder stalled." );
			break;
		}
		case _NGW_FRONTFOLDER_STALL: {
			Debug::trace( "NGWEngException", "Front folder stalled." );
			break;
		}
		case _NGW_SIDEFOLDER_STALL: {
			Debug::trace( "NGWEngException", "Side folder stalled." );
			break;
		}
		case _NGW_CLAMPS_STALL: {
			Debug::trace( "NGWEngException", "Clamps stalled." );
			break;
		}
		case _NGW_SEALERBELT_STALL: {
			Debug::trace( "NGWEngException", "Sealer belt stalled." );
			break;
		}
		case _NGW_SHIFTER_STALL: {
			Debug::trace( "NGWEngException", "Shifterr stalled." );
			break;
		}
		case _NGW_GRIPPER_STALL: {
			Debug::trace( "NGWEngException", "Gripper stalled." );
			break;
		}
		case _NGW_PUSHER_STALL: {
			Debug::trace( "NGWEngException", "Pusher stalled." );
			break;
		}
		case _NGW_SEALERBELT_MOTOR_MOVE_ERROR: {
			Debug::trace( "NGWEngException", "Sealer belt movement error." );
			break;
		}
		case _NGW_APPLIER_PRIMARY_LABEL_TAKEN_ERROR: {
			Debug::trace( "NGWEngException", "Primary applier label taken error." );
			break;
		}
		case _NGW_APPLIER_SECONDARY_LABEL_TAKEN_ERROR: {
			Debug::trace( "NGWEngException", "Secondary applier label taken error." );
			break;
		}
		case _NGW_INCOMPATIBLE_PKG_TIMING_DATA: {
			Debug::trace( "NGWEngException", "Incompatible package timing data." );
			break;
		}
		case _NGW_GRIPPER_NARROW_FAILED_TO_CLOSE: {
			Debug::trace( "NGWEngException", "Gripper narrow failed to close." );
			break;
		}
		case _NGW_GRIPPER_NARROW_FAILED_TO_OPEN: {
			Debug::trace( "NGWEngException", "Gripper narrow failed to open." );
			break;
		}
		case _NGW_GRIPPER_LEFT_FAILED_TO_CLOSE: {
			Debug::trace( "NGWEngException", "Gripper left failed to close." );
			break;
		}
		case _NGW_GRIPPER_LEFT_FAILED_TO_OPEN: {
			Debug::trace( "NGWEngException", "Gripper left failed to open." );
			break;
		}
		case _NGW_GRIPPER_RIGHT_FAILED_TO_CLOSE: {
			Debug::trace( "NGWEngException", "Gripper right failed to close." );
			break;
		}
		case _NGW_GRIPPER_RIGHT_FAILED_TO_OPEN: {
			Debug::trace( "NGWEngException", "Gripper right failed to open." );
			break;
		}
		case _NGW_CLAMPS_LEFT_FAILED_TO_CLOSE: {
			Debug::trace( "NGWEngException", "Clamps left failed to close." );
			break;
		}
		case _NGW_CLAMPS_LEFT_FAILED_TO_OPEN: {
			Debug::trace( "NGWEngException", "Clamps left failed to open." );
			break;
		}
		case _NGW_CLAMPS_RIGHT_FAILED_TO_CLOSE: {
			Debug::trace( "NGWEngException", "Clamps right failed to close." );
			break;
		}
		case _NGW_CLAMPS_RIGHT_FAILED_TO_OPEN: {
			Debug::trace( "NGWEngException", "Clamps right failed to open." );
			break;
		}
		case _NGW_FILMSELECTOR_UPPER_FAILED_TO_SWITCH: {
			Debug::trace( "NGWEngException", "Film selector failed to switch upper." );
			break;
		}
		case _NGW_FILMSELECTOR_LOWER_FAILED_TO_SWITCH: {
			Debug::trace( "NGWEngException", "Film selector failed to switch lower." );
			break;
		}
		case _NGW_FILMSELECTOR_UNLATCHED: {
			Debug::trace( "NGWEngException", "Film selector unlatched." );
			break;
		}
		case _NGW_APPLIER_UP_FAILURE: {
			Debug::trace( "NGWEngException", "Applier arm up failure." );
			break;
		}
		case _NGW_APPLIER_DOWN_FAILURE: {
			Debug::trace( "NGWEngException", "Applier arm down failure." );
			break;
		}
		case _NGW_APPLIER_HOME_FAILURE: {
			Debug::trace( "NGWEngException", "Applier home failure." );
			break;
		}
		case _NGW_APPLIER_VACUUM_FAILURE: {
			Debug::trace( "NGWEngException", "Applier vacuum failure." );
			break;
		}
		case _NGW_COMPRESSOR_PRESSURE_FAILURE: {
			Debug::trace( "NGWEngException", "Compressor pressure failure." );
			break;
		}
		case _NGW_REGULATED_PRESSURE_FAIURE: {
			Debug::trace( "NGWEngException", "Compressor pressure regulation failure." );
			break;
		}
		case _NGW_ACTUATOR_PRESSURE_FAILURE: {
			Debug::trace( "NGWEngException", "Actuator pressure failure." );
			break;
		}
		case _NGW_UPPER_FILM_PULL_ERROR: {
			Debug::trace( "NGWEngException", "Upper film pull error." );
			break;
		}
		case _NGW_LOWER_FILM_PULL_ERROR: {
			Debug::trace( "NGWEngException", "Lower film pull error." );
			break;
		}
		case _NGW_POWER_SUPPLY_24VDC_SWITCHED_FAILURE: {
			Debug::trace( "NGWEngException", "24v power supply failure." );
			break;
		}
		case _NGW_POWER_SUPPLY_170VDC_INRUSH_FAILURE: {
			Debug::trace( "NGWEngException", "170v in-rush failure." );
			break;
		}
		case _NGW_POWER_SUPPLY_170VDC_FAILURE: {
			Debug::trace( "NGWEngException", "170v power supply failure." );
			break;
		}
		case _NGW_UPPER_BRAKE_FAILURE: {
			Debug::trace( "NGWEngException", "Upper brake failure." );
			break;
		}
		case _NGW_LOWER_BRAKE_FAILURE: {
			Debug::trace( "NGWEngException", "Lower brake failure." );
			break;
		}
		case _NGW_UPPER_ROLL_LOOSE: {
			Debug::trace( "NGWEngException", "Upper film roll loose." );
			break;
		}
		case _NGW_LOWER_ROLL_LOOSE: {
			Debug::trace( "NGWEngException", "Lower film roll loose." );
			break;
		}
		case _NGW_PRINTER_PLATE_DISENGAGED: {
			Debug::trace( "NGWEngException", "Printer plate disengaged." );
			break;
		}
		case _NGW_SEALERBELT_SENSOR_BLOCKED: {
			Debug::trace( "NGWEngException", "Sealer belt sensor blocked." );
			break;
		}
		// dont know where came from.Special build.Carlos said get rid of 5 - 2 - 18
		//case _NGW_DATA_FLASH_DRIVER_WRITE_FAILURE: {
		//	Debug::trace( "NGWEngException", "Data flash driver write failure. Mechanism: " + Integer::toString(mechanism));
		//	break;
		//}
        // dont know where came from.Special build.Carlos said get rid of 5 - 2 - 18
		//case _NGW_DATA_FLASH_DRIVER_CHECKSUM_WRITE_FAILURE: {
		//	Debug::trace( "NGWEngException", "Data flash driver checksum write failure. Mechanism: " + Integer::toString(mechanism) );
		//	break;
		//}
		case _NGS_PRESS_START_BUTTON: {
			Debug::trace("NGWEngException", "Sealer belt sensor blocked.");
			break;
		}
		case _NGS_INTAKE_PKG_SENSOR_BLOCKED: {
			Debug::trace("NGWEngException", "Sealer belt sensor blocked.");
			break;
		}
		case _NGW_REGULATED_PRESSURE_TOO_HIGH: {
			Debug::trace("NGWEngException", "Sealer belt sensor blocked.");
			break;
		}
		default: {
			Debug::trace( "NGWEngException", "Unknown exception." );
		}

	}

}
