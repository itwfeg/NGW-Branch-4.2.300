/*
 * NGWRequestWrapBox.h
 *
 *  Created on: Oct 30, 2015
 *      Author: aaron
 */

#ifndef NGWREQUESTWRAPBOX_H_
#define NGWREQUESTWRAPBOX_H_

#include "WrapperGeneric.h"

class NGWRequestWrapBox : public WrapperGeneric
{

	public:
	NGWRequestWrapBox(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWRequestWrapBox(const char *buffer);
	~NGWRequestWrapBox();

};




#endif /* NGWREQUESTWRAPBOX_H_ */
