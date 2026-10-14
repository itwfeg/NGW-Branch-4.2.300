#ifndef ACCESSKEYACTIVATION_H
#define ACCESSKEYACTIVATION_H
#include "EvolMessage.h"

//class HobThread;


class AccessKeyActivation : public EvolMessage
{
	public:
		AccessKeyActivation(int keys = 0, Hobart::CEvolMsgReceiver *pHobThread = NULL);
		AccessKeyActivation(const char *buffer);
		~AccessKeyActivation();

		int getKeys() const {return keys_;}
		void setKeys(int keys) {keys_ = keys;}

		const char *serialize() const;
		void show();

	private:
		int keys_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif

};



#endif	//ACCESSKEYACTIVATION_H
