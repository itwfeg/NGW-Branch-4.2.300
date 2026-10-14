/*
 * WeigherRequestSerialNum.h
 *
 *  Created on: Aug 6, 2015
 *      Author: aaron
 */

#ifndef WEIGHERREQUESTSERIALNUM_H_
#define WEIGHERREQUESTSERIALNUM_H_
#include "WeigherGeneric.h"

class WeigherRequestSerialNum : public WeigherGeneric
{
	public:
	WeigherRequestSerialNum( Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherRequestSerialNum( const char *buffer );
	~WeigherRequestSerialNum();

	private:
};


#endif /* WEIGHERREQUESTSERIALNUM_H_ */
