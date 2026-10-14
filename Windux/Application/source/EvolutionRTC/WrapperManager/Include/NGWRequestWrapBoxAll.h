/*
 * NGWRequestWrapBoxAll.h
 *
 *  Created on: Oct 30, 2015
 *      Author: aaron
 */

#ifndef NGWREQUESTWRAPBOXALL_H_
#define NGWREQUESTWRAPBOXALL_H_

#include "WrapperGeneric.h"

class NGWRequestWrapBoxAll : public WrapperGeneric
{

	public:
	NGWRequestWrapBoxAll(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWRequestWrapBoxAll(const char *buffer);
	~NGWRequestWrapBoxAll();

};



#endif /* NGWREQUESTWRAPBOXALL_H_ */
