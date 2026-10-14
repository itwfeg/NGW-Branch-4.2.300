//#include "vxWorks.h"
#include "NumericNumericKind.h"

#include "NumericEvent.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int NumericNumericKind::dataType = 0;
std::string NumericNumericKind::dataTypeName("NumericNumericKind"); 
#endif

NumericNumericKind::NumericNumericKind(Hobart::CEvolMsgReceiver *pHobThread, NumericKind numericKind) : EvolMessage(Numeric, NmKind, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->numericKind_ = numericKind;
}

NumericNumericKind::NumericNumericKind(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &numericKind_;
	for (unsigned int i = 0; i < sizeof(numericKind_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

NumericNumericKind::~NumericNumericKind()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *NumericNumericKind::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &numericKind_;
	for (unsigned int i = 0; i < sizeof(numericKind_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void NumericNumericKind::show()
{
	bool isTracing = Debug::isTracing("NumericNumericKind");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("NumericNumericKind") );
#else
		Debug::addTraceCategory("NumericNumericKind");
#endif
	}

	Debug::trace("NumericNumericKind", "NumericNumericKind: ");

	EvolMessage::show();

	if (numericKind_ == NmNoNumeric)
	{
		Debug::trace("NumericNumericKind", HobString("numericKind: NmNoNumeric"));
	}
	else if (numericKind_ == NmUsbEvol)
	{
		Debug::trace("NumericNumericKind", HobString("numericKind: NmUsbEvol"));
	}
	else if (numericKind_ == NmUsbApc)
	{
		Debug::trace("NumericNumericKind", HobString("numericKind: NmUsbApc"));
	}
	else
	{
		Debug::trace("NumericNumericKind", HobString("numericKind: Unknown ") +
											 HobString(Integer::toString((UINT16) numericKind_)));
	}

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("NumericNumericKind") );
#else
		Debug::removeTraceCategory("NumericNumericKind");
#endif
	}
}

void NumericNumericKind::setNumericKind(NumericKind numericKind)
{
	this->numericKind_ = numericKind;
}

NumericKind NumericNumericKind::getNumericKind()
{
	return numericKind_;
}
