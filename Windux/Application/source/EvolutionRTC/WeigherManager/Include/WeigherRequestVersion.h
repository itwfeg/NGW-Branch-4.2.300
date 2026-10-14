/*
 * WeigherRequestVersion.h
 *
 *  Created on: Aug 6, 2015
 *      Author: aaron
 */

#ifndef WEIGHERREQUESTVERSION_H_
#define WEIGHERREQUESTVERSION_H_
#include "WeigherGeneric.h"

class WeigherRequestVersion : public WeigherGeneric
{
	public:
	WeigherRequestVersion( Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherRequestVersion( const char *buffer );
	~WeigherRequestVersion();


	private:
};



#endif /* WEIGHERREQUESTVERSION_H_ */
