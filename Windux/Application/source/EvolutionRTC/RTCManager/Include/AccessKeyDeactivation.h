#ifndef ACCESSKEYDEACTIVATION_H
#define ACCESSKEYDEACTIVATION_H
#include "EvolMessage.h"

//class HobThread;


class AccessKeyDeactivation : public EvolMessage
{
	public:
		AccessKeyDeactivation(int keys = 0, Hobart::CEvolMsgReceiver *pHobThread = NULL);
		AccessKeyDeactivation(const char *buffer);
		~AccessKeyDeactivation();

		int getKeys() const {return keys_;}

		const char *serialize() const;
		void show();

	private:
		int keys_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif

};



#endif	//ACCESSKEYDEACTIVATION_H
