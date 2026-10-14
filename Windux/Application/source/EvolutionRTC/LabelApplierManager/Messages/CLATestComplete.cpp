#include "CLATestComplete.h"
#include "LabelApplierEvent.h"
//Alex Zavala 8/21
#include "Debug.h"
#include "Bool.h"
#include "Integer.h"
#include "Long.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CLATestComplete::_dataType_ = 0;
std::string CLATestComplete::_dataTypeName_("CLATestComplete"); 
#endif


CLATestComplete::CLATestComplete(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(LabelApplier, LaTestComplete, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	test_ = 0;
	data1_ = 0;
	data2_ = 0;
	result_ = false;
}

CLATestComplete::CLATestComplete(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	EvolMessage::serialize();
	int j = getSerializedLength();

	
	char *pChar = (char *) &test_;
	for (unsigned int i = 0; i < sizeof(test_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &data1_;
	for (unsigned int i = 0; i < sizeof(data1_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &data2_;
	for (unsigned int i = 0; i < sizeof(data2_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &result_;
	for (unsigned int i = 0; i < sizeof(result_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

CLATestComplete::~CLATestComplete()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}



const char *CLATestComplete::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &test_;
	for (unsigned int i = 0; i < sizeof(test_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &data1_;
	for (unsigned int i = 0; i < sizeof(data1_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &data2_;
	for (unsigned int i = 0; i < sizeof(data2_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &result_;
	for (unsigned int i = 0; i < sizeof(result_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void CLATestComplete::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("CLATestComplete") );
#else
	Debug::addTraceCategory("CLATestComplete");
#endif
	
	Debug::trace("CLATestComplete", HobString("test_: ") +
		HobString(Integer::toString(data1_)));

	Debug::trace("CLATestComplete", HobString("data1_: ") +
		HobString(Integer::toString(data1_)));

	Debug::trace("CLATestComplete", HobString("data2_: ") +
		HobString(Integer::toString(data2_)));

	Debug::trace("CLATestComplete", HobString("result_: ") +
		HobString(Bool::toString(result_)));

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("CLATestComplete") );
#else
	Debug::removeTraceCategory("CLATestComplete");
#endif
}