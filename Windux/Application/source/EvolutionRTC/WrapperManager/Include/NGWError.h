/*
 * NgwError.h
 *
 *  Created on: Aug 10, 2015
 *      Author: aaron
 */

#ifndef NGWERROR_H_
#define NGWERROR_H_
#include "WrapperGeneric.h"

class NGWError : public WrapperGeneric
{
	public:
	NGWError(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWError(const char *buffer);
	~NGWError();

	UINT16 getMechanism() { return _mechanism; }
	void setMechanism( UINT16 mech ) { _mechanism = mech; }

	UINT16 getError() { return _error; }
	void setError( UINT16 err ) { _error = err; }

	const char *serialize()const;

	private:
	UINT16 _mechanism;
	UINT16 _error;
};




#endif /* NGWERROR_H_ */
