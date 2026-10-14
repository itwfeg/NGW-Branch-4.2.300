/*
 * NGWWrapBoxComplete.h
 *
 *  Created on: Oct 29, 2015
 *      Author: aaron
 */

#ifndef NGWWRAPBOXCOMPLETE_H_
#define NGWWRAPBOXCOMPLETE_H_
#include "WrapperGeneric.h"

class NGWWrapBoxComplete : public WrapperGeneric
{

	public:
	NGWWrapBoxComplete(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWWrapBoxComplete(const char *buffer);
	~NGWWrapBoxComplete();

};

#endif /* NGWWRAPBOXCOMPLETE_H_ */
