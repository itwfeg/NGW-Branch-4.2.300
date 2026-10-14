#include "LabelApplierUltimaAutoMode.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierUltimaAutoMode::_dataType_ = 0;
std::string LabelApplierUltimaAutoMode::_dataTypeName_("LabelApplierUltimaAutoMode");
#endif

LabelApplierUltimaAutoMode::LabelApplierUltimaAutoMode(Hobart::CEvolMsgReceiver *pHobThread, UINT8 packageId) : EvolMessage(LabelApplier, LaUltimaAutoMode, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	this->packageId = packageId;
}

LabelApplierUltimaAutoMode::LabelApplierUltimaAutoMode(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		*pChar++ = buffer[j++];
	}
}

LabelApplierUltimaAutoMode::~LabelApplierUltimaAutoMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *LabelApplierUltimaAutoMode::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void LabelApplierUltimaAutoMode::show()
{
	if (Debug::isTracing("LabelApplierCloseWindow"))
	{
		Debug::trace("LabelApplierUltimaAutoMode", "LabelApplierUltimaAutoMode: ");

		EvolMessage::show();

		Debug::trace("LabelApplierUltimaAutoMode", HobString("packageId: ") +
													HobString(Integer::toString(packageId)));
	}
}

UINT8 LabelApplierUltimaAutoMode::getPackageId()
{
	return packageId;
}

void LabelApplierUltimaAutoMode::setPackageId(UINT8 packageId)
{
	this->packageId = packageId;
}
