#ifndef LEAPSTATUSWRITE_H
#define LEAPSTATUSWRITE_H

#include "EvolMessage.h"

#include "CEvolMsgReceiver.h"
//class HobThread;

class LeapStatusWrite : public EvolMessage
{
	public:
		LeapStatusWrite(UINT16 code,
			Hobart::CEvolMsgReceiver *pHobThread = NULL);
		~LeapStatusWrite();

		const char *serialize() const;
		void show();

		UINT16 getCode() const {return code_;}
		void setCode(UINT16 code) {code_ = code;}

	private:
		UINT16 code_;

};

#endif	//LEAPSTATUSWRITE_H
