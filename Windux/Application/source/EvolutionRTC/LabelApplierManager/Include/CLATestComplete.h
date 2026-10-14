#ifndef CLATESTCOMPLETE_H
#define CLATESTCOMPLETE_H

#include <string>
#include "EvolMessage.h"
#include "LabelApplierEvent.h"
#include "LabelApplier.h"
#include "System.h"



class CLATestComplete : public EvolMessage
{
	public:
		CLATestComplete(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		CLATestComplete(const char *buffer);
		~CLATestComplete();

		const char *serialize()const;
		
		void setTest(UINT8 test) {test_ = test;}
		UINT8 getTest() const {return test_;}

		void setData1(UINT16 data) {data1_ = data;}
		UINT16 getData1() const {return data1_;}

		void setData2(UINT16 data) {data2_ = data;}
		UINT16 getData2() const {return data2_;}

		void setResult(bool result) {result_ = result;}
		bool getResult() const {return result_;}

		void show();
		
	private:

		UINT8	test_;
		UINT16		data1_;
		UINT16		data2_;
		bool	result_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif