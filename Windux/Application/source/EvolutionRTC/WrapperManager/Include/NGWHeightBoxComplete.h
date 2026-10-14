/*
 * NGWHeightBoxComplete.h
 *
 *  Created on: Oct 30, 2015
 *      Author: aaron
 */

#ifndef NGWHEIGHTBOXCOMPLETE_H_
#define NGWHEIGHTBOXCOMPLETE_H_

#include "WrapperGeneric.h"

class NGWHeightBoxComplete : public WrapperGeneric
{

	public:
	NGWHeightBoxComplete(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWHeightBoxComplete(const char *buffer);
	~NGWHeightBoxComplete();

	const char *serialize() const;

	private:
	UINT8 _wrapBoxIndex;
	UINT8 _heightBoxIndex;

};

#endif /* NGWHEIGHTBOXCOMPLETE_H_ */
