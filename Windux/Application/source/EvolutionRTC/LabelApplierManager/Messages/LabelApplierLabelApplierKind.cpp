//#include "vxWorks.h"
#include "LabelApplierEvent.h"
#include "LabelApplierLabelApplierKind.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierLabelApplierKind::dataType = 0;
std::string LabelApplierLabelApplierKind::dataTypeName("LabelApplierLabelApplierKind"); 
#endif

LabelApplierLabelApplierKind::LabelApplierLabelApplierKind(Hobart::CEvolMsgReceiver *pHobThread, LabelApplierKind laKind) : EvolMessage(LabelApplier, LaKind, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->laKind = laKind;
}

LabelApplierLabelApplierKind::LabelApplierLabelApplierKind(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &laKind;
	for (unsigned int i = 0; i < sizeof(laKind); i++)
	{
		*pChar++ = buffer[j++];
	}
}

LabelApplierLabelApplierKind::~LabelApplierLabelApplierKind()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *LabelApplierLabelApplierKind::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &laKind;
	for (unsigned int i = 0; i < sizeof(laKind); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void LabelApplierLabelApplierKind::show()
{
	bool isTracing = Debug::isTracing("LabelApplierLabelApplierKind");

	if (isTracing == false)
	{
		Debug::addTraceCategory( std::string("LabelApplierLabelApplierKind") );
	}

	Debug::trace("LabelApplierLabelApplierKind", "LabelApplierLabelApplierKind: ");

	EvolMessage::show();

	if (laKind == NoLabelApplier)
	{
		Debug::trace("LabelApplierLabelApplierKind", HobString("laKind: NoLabelApplier"));
	}
	else if (laKind == ILA)
	{
		Debug::trace("LabelApplierLabelApplierKind", HobString("laKind: ILA"));
	}
	else if (laKind == CompactLabelApplier)
	{
		Debug::trace("LabelApplierLabelApplierKind", HobString("laKind: CompactLabelApplier"));
	}
	else if (laKind == CompactLabelApplier_EPSAC2)
	{
		Debug::trace("LabelApplierLabelApplierKind", HobString("laKind: CompactLabelApplier_EPSAC2"));
	}
	else if (laKind == EvolutionLabelApplier)
	{
		Debug::trace("LabelApplierLabelApplierKind", HobString("laKind: EvolutionLabelApplier"));
	}
	else
	{
		Debug::trace("LabelApplierLabelApplierKind", HobString("laKind: Unknown ") +
													 HobString(Integer::toString((UINT16) laKind)));
	}

	if (isTracing == false)
	{
		Debug::removeTraceCategory( std::string("LabelApplierLabelApplierKind") );
	}
}

LabelApplierKind LabelApplierLabelApplierKind::getLabelApplierKind()
{
	return laKind;
}

void LabelApplierLabelApplierKind::setLabelApplierKind(LabelApplierKind laKind)
{
	this->laKind = laKind;
}
