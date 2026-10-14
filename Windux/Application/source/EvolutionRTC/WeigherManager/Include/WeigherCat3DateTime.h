/**
 * @file WeigherCat3DateTime.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief message for setting epoch date and time to weigher
 * Created on: July 1, 2016
 */

#ifndef WEIGHERCAT3DATETIME_H_
#define WEIGHERCAT3DATETIME_H_
#include "EvolMessage.h"
#include "Weigher.h"
#include "CEvolMsgReceiver.h"

class WeigherCat3DateTime : public EvolMessage
{
	public:
		WeigherCat3DateTime( Hobart::CEvolMsgReceiver *pHobThread = NULL, UINT32 time = 0 );
		WeigherCat3DateTime(const char *buffer);
		~WeigherCat3DateTime();

		UINT32 getEpochTime()			{ return epochTime; }
		void setEpochTime(UINT32 time)	{ epochTime = time;}

		const char *serialize() const;

		void show();
	private:
		UINT32 epochTime;
};
#endif
