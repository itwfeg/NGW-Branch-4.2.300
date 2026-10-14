#include "WrapperEvent.h"
#include "EvolWrapperCycle.h"

#include "Debug.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperCycle::_dataType_ = 0;
std::string EvolWrapperCycle::_dataTypeName_("EvolWrapperCycle"); 
#endif


EvolWrapperCycle::EvolWrapperCycle(Hobart::CEvolMsgReceiver *pHobThread,
						   bool pullFilm) : EvolMessage(Wrapper, 112, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	this->pullFilm = pullFilm;
}

EvolWrapperCycle::EvolWrapperCycle(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &pullFilm;
	for (unsigned int i = 0; i < sizeof(pullFilm); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperCycle::~EvolWrapperCycle()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
 
const char *EvolWrapperCycle::serialize()const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &pullFilm;
	for (unsigned int i = 0; i < sizeof(pullFilm); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void EvolWrapperCycle::show()
{
	if (Debug::isTracing("EvolWrapperCycle"))
	{
		Debug::trace("EvolWrapperCycle", "EvolWrapperCycle: ");

		EvolMessage::show();

	    Debug::trace("EvolWrapperCycle", HobString("pullFilm: ") +
									 HobString(Bool::toString(pullFilm)));
	}
}

bool EvolWrapperCycle::isPullFilm() const
{
	return pullFilm;
}

void EvolWrapperCycle::setPullFilm()
{
	pullFilm = true;
}

void EvolWrapperCycle::clearPullFilm()
{
	pullFilm = false;
}
