//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "WrapperErrorHistory.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperErrorHistory::dataType = 0;
std::string WrapperErrorHistory::dataTypeName("WrapperErrorHistory"); 
#endif

WrapperErrorHistory::WrapperErrorHistory(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Wrapper, WrErrorHistory, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	currentIndex = 0;
}

WrapperErrorHistory::WrapperErrorHistory(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items

	for (int i = 0; i < MaxErrorHistoryBuffers; i++)
	{
		errorInfo[i] = WrapperErrorInfo(&buffer[j]);

		errorInfo[i].serialize();
		j += errorInfo[i].getSerializedLength();

		packageInfo[i] = WrapperPackageInfo(&buffer[j]);

		packageInfo[i].serialize();
		j += packageInfo[i].getSerializedLength();
	}
}

WrapperErrorHistory::~WrapperErrorHistory()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WrapperErrorHistory::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	for (int i = 0; i < MaxErrorHistoryBuffers; i++)
	{
		const char *pCC = errorInfo[i].serialize();
		int mLength = errorInfo[i].getSerializedLength();

		serializedData.append(pCC, mLength);

		pCC = packageInfo[i].serialize();
		mLength = packageInfo[i].getSerializedLength();

		serializedData.append(pCC, mLength);
	}

	return serializedData.data();
}

void WrapperErrorHistory::show()
{
	Debug::addTraceCategory("WrapperErrorHistory");
	Debug::trace("WrapperErrorHistory", "WrapperErrorHistory: ");

	EvolMessage::show();

	for (int i = 0; i < MaxErrorHistoryBuffers; i++)
	{
		Debug::trace("WrapperErrorHistory", HobString("errorNumber: ") +
									 		HobString(Integer::toString(i)));
		errorInfo[i].show();
		packageInfo[i].show();
	}

	Debug::removeTraceCategory("WrapperErrorHistory");
}

WrErrorCode WrapperErrorHistory::getFirstErrorCode(int *index) const
{
	currentIndex = 0;

	if (index != NULL)
	{
		*index = currentIndex;
	}

	return errorInfoToErrorCode(currentIndex);
}

bool WrapperErrorHistory::isReportableError(WrapperErrorInfo::WrapperErrorSource errorValue)
{
	bool rv = false;

    switch(errorValue)
    {
        case WrapperErrorInfo::FrontCoverInterlock:
        case WrapperErrorInfo::SealerBeltInterlock:
        case WrapperErrorInfo::ElevatorInterlock:
        case WrapperErrorInfo::TooTallInterlock:
        case WrapperErrorInfo::WideFilmFailure:
        case WrapperErrorInfo::NarrowFilmFailure:
        case WrapperErrorInfo::PackageJammed:
        case WrapperErrorInfo::MainMotorJammed:
        case WrapperErrorInfo::IntakeHomeFailure:
        case WrapperErrorInfo::IntakeWidthFailure:
        case WrapperErrorInfo::IntakeLengthFailure:
        case WrapperErrorInfo::PackageTooLarge:
        case WrapperErrorInfo::IntakeMotorJammed:
        case WrapperErrorInfo::IntakeOutOfSync:
			rv = true;
			break;
        default:
			rv = false;
			break;
    }

	return rv;
}

WrErrorCode WrapperErrorHistory::getNextErrorCode(int *index) const
{
	currentIndex++;

	if (currentIndex == MaxErrorHistoryBuffers)
	{
		currentIndex = 0;
	}

	if (index != NULL)
	{
		*index = currentIndex;
	}

	return errorInfoToErrorCode(currentIndex);
}

void WrapperErrorHistory::setErrorInfo(WrapperErrorInfo &errorInfo, UINT16 index)
{
	this->errorInfo[index] = errorInfo;
}

WrapperErrorInfo &WrapperErrorHistory::getErrorInfo(UINT16 index)
{
	return errorInfo[index];
}

const WrapperErrorInfo& WrapperErrorHistory::getErrorInfo(UINT16 index) const
{
	return errorInfo[index];
}

void WrapperErrorHistory::setPackageInfo(WrapperPackageInfo &packageInfo, UINT16 index)
{
	this->packageInfo[index] = packageInfo;
}

WrapperPackageInfo &WrapperErrorHistory::getPackageInfo(UINT16 index)
{
	return packageInfo[index];
}

const WrapperPackageInfo& WrapperErrorHistory::getPackageInfo(UINT16 index) const
{
	return packageInfo[index];
}

bool WrapperErrorHistory::equals(WrapperErrorHistory &wrErrorHistory)
{
	bool rv = true;
	UINT16 i = 0;

	while ((i < MaxErrorHistoryBuffers) && (rv == true))
	{
		if ((errorInfo[i].equals(wrErrorHistory.getErrorInfo(i)) == false) ||
			(packageInfo[i].equals(wrErrorHistory.getPackageInfo(i)) == false))
		{
			rv = false;
		}
		i++;
	}

	return rv;
}

WrErrorCode WrapperErrorHistory::errorInfoToErrorCode(UINT16 index) const
{
	WrErrorCode errorCode = WrNoErrorCode;

	if (errorInfo[index].isEepWriteFailure() == true)
	{
		errorCode = WrEepWriteFailure;
	}
	else if (errorInfo[index].isHeaterFailure() == true)
	{
		errorCode = WrHeaterFailure;
	}
	else if (errorInfo[index].isPackageTooLarge() == true)
	{
		errorCode = WrPackageTooLarge;
	}
	else if (errorInfo[index].isGeneralFault1() == true)
	{
		errorCode = WrGeneralFault1;
	}
	else if (errorInfo[index].isGeneralFault2() == true)
	{
		errorCode = WrGeneralFault2;
	}
	else if (errorInfo[index].isSealerBeltInterlock() == true)
	{
		errorCode = WrSealerBeltInterlock;
	}
	else if (errorInfo[index].isElevatorInterlock() == true)
	{
		errorCode = WrElevatorInterlock;
	}
	else if (errorInfo[index].isScalePlatterInterlock() == true)
	{
		errorCode = WrScalePlatterInterlock;
	}
	else if (errorInfo[index].isHandCrankDoorInterlock() == true)
	{
		errorCode = WrHandCrankDoorInterlock;
	}
	else if (errorInfo[index].isFilmDoorInterlock() == true)
	{
		errorCode = WrFilmDoorInterlock;
	}
	else if (errorInfo[index].isTooTallInterlock() == true)
	{
		errorCode = WrTooTallInterlock;
	}
	else if (errorInfo[index].isLeftCanopyInterlock() == true)
	{
		errorCode = WrLeftCanopyInterlock;
	}
	else if (errorInfo[index].isRightCanopyInterlock() == true)
	{
		errorCode = WrRightCanopyInterlock;
	}
	else if (errorInfo[index].isFrontCoverInterlock() == true)
	{
		errorCode = WrFrontCoverInterlock;
	}
	else if (errorInfo[index].isLeftSideCoverInterlock() == true)
	{
		errorCode = WrLeftSideCoverInterlock;
	}
	else if (errorInfo[index].isRightSideCoverInterlock() == true)
	{
		errorCode = WrRightSideCoverInterlock;
	}
	else if (errorInfo[index].isWideFilmSupplyFailure() == true)
	{
		errorCode = WrWideFilmSupplyFailure;
	}
	else if (errorInfo[index].isNarrowFilmSupplyFailure() == true)
	{
		errorCode = WrNarrowFilmSupplyFailure;
	}
	else if (errorInfo[index].isPackageJammed() == true)
	{
		errorCode = WrPackageJammed;
	}
	else if (errorInfo[index].isMainMotorStalled() == true)
	{
		errorCode = WrMainMotorStalled;
	}
	else if (errorInfo[index].isIntakeHomeFailure() == true)
	{
		errorCode = WrIntakeHomeFailure;
	}
	else if (errorInfo[index].isIntakeWidthFailure() == true)
	{
		errorCode = WrIntakeWidthFailure;
	}
	else if (errorInfo[index].isIntakeLengthFailure() == true)
	{
		errorCode = WrIntakeLengthFailure;
	}
	else if (errorInfo[index].isIntakeMotorJammed() == true)
	{
		errorCode = WrIntakeMotorJammed;
	}
	else if (errorInfo[index].isIntakePackagePresent() == true)
	{
		errorCode = WrIntakePackagePresent;
	}
	else if (errorInfo[index].isIntakeOutOfSync() == true)
	{
		errorCode = WrIntakeOutOfSync;
	}
	else if (errorInfo[index].isStopSwitchInterlock() == true)
	{
		errorCode = WrStopSwitchInterlock;
	}
	else
	{
		errorCode = WrNoErrorCode;
	}

	return errorCode;
}
