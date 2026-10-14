//#include "vxWorks.h"
#include "MarqueeMarqueeKind.h"

#include "MarqueeEvent.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int MarqueeMarqueeKind::dataType = 0;
std::string MarqueeMarqueeKind::dataTypeName("MarqueeMarqueeKind"); 
#endif

MarqueeMarqueeKind::MarqueeMarqueeKind(Hobart::CEvolMsgReceiver *pHobThread, MarqueeKind marqueeKind) : EvolMessage(Marquee, MqKind, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->marqueeKind_ = marqueeKind;
}

MarqueeMarqueeKind::MarqueeMarqueeKind(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &marqueeKind_;
	for (unsigned int i = 0; i < sizeof(marqueeKind_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

MarqueeMarqueeKind::~MarqueeMarqueeKind()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *MarqueeMarqueeKind::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &marqueeKind_;
	for (unsigned int i = 0; i < sizeof(marqueeKind_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void MarqueeMarqueeKind::show()
{
	bool isTracing = Debug::isTracing("MarqueeMarqueeKind");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("MarqueeMarqueeKind") );
#else
		Debug::addTraceCategory("MarqueeMarqueeKind");
#endif
	}

	Debug::trace("MarqueeMarqueeKind", "MarqueeMarqueeKind: ");

	EvolMessage::show();

	if (marqueeKind_ == MqNoMarquee)
	{
		Debug::trace("MarqueeMarqueeKind", HobString("marqueeKind: MqNoMarquee"));
	}
	else if (marqueeKind_ == MqUsbEvol)
	{
		Debug::trace("MarqueeMarqueeKind", HobString("marqueeKind: MqUsbEvol"));
	}
	else if (marqueeKind_ == MqUsbApc)
	{
		Debug::trace("MarqueeMarqueeKind", HobString("marqueeKind: MqUsbApc"));
	}
	else
	{
		Debug::trace("MarqueeMarqueeKind", HobString("marqueeKind: Unknown ") +
											 HobString(Integer::toString((UINT16) marqueeKind_)));
	}

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("MarqueeMarqueeKind") );
#else
		Debug::removeTraceCategory("MarqueeMarqueeKind");
#endif
	}
}

void MarqueeMarqueeKind::setMarqueeKind(MarqueeKind marqueeKind)
{
	this->marqueeKind_ = marqueeKind;
}

MarqueeKind MarqueeMarqueeKind::getMarqueeKind()
{
	return marqueeKind_;
}
