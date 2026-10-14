/*
 * NGWChangeParamComplete.h
 *
 *  Created on: Oct 30, 2015
 *      Author: aaron
 */

#ifndef NGWCHANGEPARAMCOMPLETE_H_
#define NGWCHANGEPARAMCOMPLETE_H_

#include "WrapperGeneric.h"

class NGWChangeParamComplete : public WrapperGeneric
{

	public:
	NGWChangeParamComplete(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWChangeParamComplete(const char *buffer);
	~NGWChangeParamComplete();

	UINT8 getWrapBoxIndex() { return _wrapBoxIndex; }
	void setWrapBoxIndex( UINT8 index ) {  _wrapBoxIndex = index; }

	UINT8 getHeightBoxIndex() { return _heightBoxIndex; }
	void setHeightBoxIndex( UINT8 index ) { _heightBoxIndex = index; }

	const char *serialize() const;

	private:
	UINT8 _wrapBoxIndex;
	UINT8 _heightBoxIndex;
};



#endif /* NGWCHANGEPARAMCOMPLETE_H_ */
