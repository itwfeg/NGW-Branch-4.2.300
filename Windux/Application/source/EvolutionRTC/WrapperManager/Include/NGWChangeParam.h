/*
 * NGWChangeParam.h
 *
 *  Created on: Oct 30, 2015
 *      Author: aaron
 */

#ifndef NGWCHANGEPARAM_H_
#define NGWCHANGEPARAM_H_

#include "WrapperGeneric.h"

class NGWChangeParam : public WrapperGeneric
{

	public:
	NGWChangeParam(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWChangeParam(const char *buffer);
	~NGWChangeParam();

	UINT8 getWrapBoxIndex() { return _wrapBoxIndex; }
	void setWrapBoxIndex( UINT8 index ) { _wrapBoxIndex = index; }

	UINT8 getHeightBoxIndex() { return _heightBoxIndex; }
	void setHeightBoxIndex( UINT8 index ) { _heightBoxIndex = index; }

	UINT16 getSecondaryDistance() { return _sec_distance; }
	void setSecondaryDistance( UINT16 distance ) { _sec_distance = distance; }

	UINT16 getGripperPull() { return _gripper_pull; }
	void setGripperPull( UINT16 pull ) { _gripper_pull = pull; }

	UINT16 getSideClampPosition() { return _sideclamp_pos; }
	void setSideCampPosition( UINT16 position ) { _sideclamp_pos = position; }

	UINT16 getIntakeSpeed() { return _intake_speed; }
	void setIntakeSpeed( UINT16 speed ) { _intake_speed = speed; }

	UINT16 getDischargeSpeed() { return _discharge_speed; }
	void setDischargeSpeed( UINT16 speed ) { _discharge_speed = speed; }


	const char *serialize() const;

	private:
	UINT8 _wrapBoxIndex;
	UINT8 _heightBoxIndex;
	UINT16 _sec_distance;
	UINT16 _gripper_pull;
	UINT16 _sideclamp_pos;
	UINT16 _intake_speed;
	UINT16 _discharge_speed;

};




#endif /* NGWCHANGEPARAM_H_ */
