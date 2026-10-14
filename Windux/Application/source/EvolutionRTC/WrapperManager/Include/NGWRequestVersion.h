/*
 * NGWRequestVersion.h
 *
 *  Created on: Aug 10, 2015
 *      Author: aaron
 */

#ifndef NGWREQUESTVERSION_H_
#define NGWREQUESTVERSION_H_
#include "WrapperGeneric.h"

class NGWRequestVersion : public WrapperGeneric
{
	public:
	NGWRequestVersion(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWRequestVersion(const char *buffer);
	~NGWRequestVersion();


	private:


};


#endif /* NGWREQUESTVERSION_H_ */
