//#include "vxWorks.h"
#include "WrapperStatusInfo.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperStatusInfo::dataType = 0;
std::string WrapperStatusInfo::dataTypeName("WrapperStatusInfo"); 
#endif

WrapperStatusInfo::WrapperStatusInfo()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	rate = 0;
	reportStatus = false;
	pusherHome = false;
	rateAnalysis = false;
	openWindow = false;
	mainPower = false;
	mainMotor = false;
	limpHome = false;
	packWidth = FilmWidthIdle;
	filmTension = LowFilmTension;
}

WrapperStatusInfo::WrapperStatusInfo(const char *buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	int j = 0;

	char *pChar = (char *) &rate;
	for (unsigned int i = 0; i < sizeof(rate); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &reportStatus;
	for (unsigned int i = 0; i < sizeof(reportStatus); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pusherHome;
	for (unsigned int i = 0; i < sizeof(pusherHome); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &rateAnalysis;
	for (unsigned int i = 0; i < sizeof(rateAnalysis); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &openWindow;
	for (unsigned int i = 0; i < sizeof(openWindow); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &mainPower;
	for (unsigned int i = 0; i < sizeof(mainPower); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &mainMotor;
	for (unsigned int i = 0; i < sizeof(mainMotor); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &limpHome;
	for (unsigned int i = 0; i < sizeof(limpHome); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packWidth;
	for (unsigned int i = 0; i < sizeof(packWidth); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &filmTension;
	for (unsigned int i = 0; i < sizeof(filmTension); i++)
	{
		*pChar++ = buffer[j++];
	}

	stateInfo = WrapperStateInfo(&buffer[j]);

	stateInfo.serialize();
	j += stateInfo.getSerializedLength();
}

WrapperStatusInfo::~WrapperStatusInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WrapperStatusInfo::serialize() const
{
	serializedData = "";

	char *pChar = (char *) &rate;
	for (unsigned int i = 0; i < sizeof(rate); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &reportStatus;
	for (unsigned int i = 0; i < sizeof(reportStatus); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pusherHome;
	for (unsigned int i = 0; i < sizeof(pusherHome); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &rateAnalysis;
	for (unsigned int i = 0; i < sizeof(rateAnalysis); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &openWindow;
	for (unsigned int i = 0; i < sizeof(openWindow); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &mainPower;
	for (unsigned int i = 0; i < sizeof(mainPower); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &mainMotor;
	for (unsigned int i = 0; i < sizeof(mainMotor); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &limpHome;
	for (unsigned int i = 0; i < sizeof(limpHome); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packWidth;
	for (unsigned int i = 0; i < sizeof(packWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmTension;
	for (unsigned int i = 0; i < sizeof(filmTension); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	const char *pCC = stateInfo.serialize();
	int mLength = stateInfo.getSerializedLength();

	serializedData.append(pCC, mLength);

	return serializedData.data();
}

UINT32 WrapperStatusInfo::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void WrapperStatusInfo::show()
{
	Debug::addTraceCategory("WrapperStatusInfo");
	Debug::trace("WrapperStatusInfo", "WrapperStatusInfo: ");

	Debug::trace("WrapperStatusInfo", HobString("rate: ") +
									  HobString(Integer::toString(rate)));

	if (reportStatus == true)
	{
		Debug::trace("WrapperStatusInfo", "reportStatus ON!");
	}

	if (pusherHome == true)
	{
		Debug::trace("WrapperStatusInfo", "pusherHome ON!");
	}

	if (rateAnalysis == true)
	{
		Debug::trace("WrapperStatusInfo", "rateAnalysis ON!");
	}

	if (openWindow == true)
	{
		Debug::trace("WrapperStatusInfo", "openWindow: ON");
	}
	else
	{
		Debug::trace("WrapperStatusInfo", "openWindow: OFF");
	}

	if (mainPower == true)
	{
		Debug::trace("WrapperStatusInfo", "mainPower: ON");
	}
	else
	{
		Debug::trace("WrapperStatusInfo", "mainPower: OFF");
	}

	if (mainMotor == true)
	{
		Debug::trace("WrapperStatusInfo", "mainMotor: ON");
	}
	else
	{
		Debug::trace("WrapperStatusInfo", "mainMotor: OFF");
	}

	if (limpHome == true)
	{
		Debug::trace("WrapperStatusInfo", "limpHome ON!");
	}

	if (packWidth == NarrowPackage)
	{
		Debug::trace("WrapperStatusInfo", "packWidth: NarrowPackage");
	}
	else if (packWidth == WidePackage)
	{
		Debug::trace("WrapperStatusInfo", "packWidth: WidePackage");
	}
	else if (packWidth == FilmWidthIdle)
	{
		Debug::trace("WrapperStatusInfo", "packWidth: FilmWidthIdle");
	}
	else if (packWidth == AutoWidthSelect)
	{
		Debug::trace("WrapperStatusInfo", "packWidth: AutoWidthSelect");
	}
	else
	{
		Debug::trace("WrapperStatusInfo", HobString("packWidth: Unknown ") +
										  HobString(Integer::toString((UINT16) packWidth)));
	}

	if (filmTension == LowFilmTension)
	{
		Debug::trace("WrapperStatusInfo", "filmTension: LowFilmTension");
	}
	else if (filmTension == MediumFilmTension)
	{
		Debug::trace("WrapperStatusInfo", "filmTension: MediumFilmTension");
	}
	else if (filmTension == HighFilmTension)
	{
		Debug::trace("WrapperStatusInfo", "filmTension: HighFilmTension");
	}
	else
	{
		Debug::trace("WrapperStatusInfo", HobString("filmTension: Unknown ") +
										  HobString(Integer::toString((UINT16) filmTension)));
	}
	
	stateInfo.show();

	Debug::removeTraceCategory("WrapperStatusInfo");
}

void WrapperStatusInfo::setRate(UINT16 rate)
{
	this->rate = rate;
}

UINT16 WrapperStatusInfo::getRate() const
{
	return rate;
}

bool WrapperStatusInfo::isReportStatus() const
{
	return reportStatus;
}

void WrapperStatusInfo::setReportStatus()
{
	reportStatus = true;
}

void WrapperStatusInfo::clearReportStatus()
{
	reportStatus = false;
}

bool WrapperStatusInfo::isPusherHome() const
{
	return pusherHome;
}

void WrapperStatusInfo::setPusherHome()
{
	pusherHome = true;
}

void WrapperStatusInfo::clearPusherHome()
{
	pusherHome = false;
}

bool WrapperStatusInfo::isRateAnalysis() const
{
	return rateAnalysis;
}

void WrapperStatusInfo::setRateAnalysis()
{
	rateAnalysis = true;
}

void WrapperStatusInfo::clearRateAnalysis()
{
	rateAnalysis = false;
}

bool WrapperStatusInfo::isOpenWindow() const
{
	return openWindow;
}

void WrapperStatusInfo::setOpenWindow()
{
	openWindow = true;
}

void WrapperStatusInfo::clearOpenWindow()
{
	openWindow = false;
}

bool WrapperStatusInfo::isMainPower() const
{
	return mainPower;
}

void WrapperStatusInfo::setMainPower()
{
	mainPower = true;
}

void WrapperStatusInfo::clearMainPower()
{
	mainPower = false;
}

bool WrapperStatusInfo::isMainMotor() const
{
	return mainMotor;
}

void WrapperStatusInfo::setMainMotor()
{
	mainMotor = true;
}

void WrapperStatusInfo::clearMainMotor()
{
	mainMotor = false;
}

bool WrapperStatusInfo::isLimpHome() const
{
	return limpHome;
}

void WrapperStatusInfo::setLimpHome()
{
	limpHome = true;
}

void WrapperStatusInfo::clearLimpHome()
{
	limpHome = false;
}

WrPackWidth WrapperStatusInfo::getPackWidth() const
{
	return packWidth;
}

void WrapperStatusInfo::setPackWidth(WrPackWidth packWidth)
{
	this->packWidth = packWidth;
}

// Pack Width Variants
bool WrapperStatusInfo::isNarrowPackage() const
{
	return (packWidth == NarrowPackage);
}

bool WrapperStatusInfo::isWidePackage() const
{
	return (packWidth == WidePackage);
}

bool WrapperStatusInfo::isFilmWidthIdle() const
{
	return (packWidth == FilmWidthIdle);
}

bool WrapperStatusInfo::isAutoWidthSelect() const
{
	return (packWidth == AutoWidthSelect);
}

void WrapperStatusInfo::setNarrowPackage()
{
	packWidth = NarrowPackage;
}

void WrapperStatusInfo::setWidePackage()
{
	packWidth = WidePackage;
}

void WrapperStatusInfo::setFilmWidthIdle()
{
	packWidth = FilmWidthIdle;
}

void WrapperStatusInfo::setAutoWidthSelect()
{
	packWidth = AutoWidthSelect;
}

// Film Tension Variants
WrFilmTension WrapperStatusInfo::getFilmTension() const
{
	return filmTension;
}

void WrapperStatusInfo::setFilmTension(WrFilmTension filmTension)
{
	this->filmTension = filmTension;
}

bool WrapperStatusInfo::isLowFilmTension() const
{
	return (filmTension == LowFilmTension);
}

bool WrapperStatusInfo::isMediumFilmTension() const
{
	return (filmTension == MediumFilmTension);
}

bool WrapperStatusInfo::isHighFilmTension() const
{
	return (filmTension == HighFilmTension);
}

void WrapperStatusInfo::setLowFilmTension()
{
	filmTension = LowFilmTension;
}

void WrapperStatusInfo::setMediumTension()
{
	filmTension = MediumFilmTension;
}

void WrapperStatusInfo::setHighFilmTension()
{
	filmTension = HighFilmTension;
}

void WrapperStatusInfo::setStateInfo(WrapperStateInfo &stateInfo)
{
	this->stateInfo = stateInfo;
}

const WrapperStateInfo &WrapperStatusInfo::getStateInfo() const
{
	return stateInfo;
}

void WrapperStatusInfo::setStatusBits(WrStatusBits statusBits)
{
	rate = statusBits.rate;

	if (statusBits.report_status == 1)
	{
		reportStatus = true;
	}
	else
	{
		reportStatus = false;
	}

	if (statusBits.pusher_home == 1)
	{
		pusherHome = true;
	}
	else
	{
		pusherHome = false;
	}

	if (statusBits.rate_analysis == 1)
	{
		rateAnalysis = true;
	}
	else
	{
		rateAnalysis = false;
	}

	if (statusBits.open_window == 1)
	{
		openWindow = true;
	}
	else
	{
		openWindow = false;
	}

	if (statusBits.main_power == 1)
	{
		mainPower = true;
	}
	else
	{
		mainPower = false;
	}

	if (statusBits.main_motor == 1)
	{
		mainMotor = true;
	}
	else
	{
		mainMotor = false;
	}

	if (statusBits.limp_home == 1)
	{
		limpHome = true;
	}
	else
	{
		limpHome = false;
	}

	packWidth = (WrPackWidth) statusBits.pack_width;
	filmTension = (WrFilmTension) statusBits.film_tension;
	stateInfo.setWrapperState((WrapperState) statusBits.state);
}

WrStatusBits WrapperStatusInfo::getStatusBits() const
{
	WrStatusBits wrStatusBits;

	memset (&wrStatusBits, 0, sizeof (wrStatusBits));

	rate = wrStatusBits.rate;

	if (reportStatus == true)
	{
		wrStatusBits.report_status = 1;
	}

	if (pusherHome == true)
	{
		wrStatusBits.pusher_home = 1;
	}

	if (rateAnalysis == true)
	{
		wrStatusBits.rate_analysis = 1;
	}

	if (openWindow == true)
	{
		wrStatusBits.open_window = 1;
	}

	if (mainPower == true)
	{
		wrStatusBits.main_power = 1;
	}

	if (mainMotor == true)
	{
		wrStatusBits.main_motor = 1;
	}

	if (limpHome == true)
	{
		wrStatusBits.limp_home = 1;
	}

	wrStatusBits.pack_width = packWidth;
	wrStatusBits.film_tension = filmTension;
	wrStatusBits.state = stateInfo.getWrapperState();

	return wrStatusBits;	
}

void WrapperStatusInfo::setStatusBits(UINT32 statusBits)
{
	WrStatusBits wrStatusBits;

	memcpy(&wrStatusBits, &statusBits, sizeof(statusBits));

	setStatusBits(wrStatusBits);
}

bool WrapperStatusInfo::equals(WrapperStatusInfo &wrStatusInfo)
{
	bool rv = false;

	if ((rate == wrStatusInfo.getRate()) &&
		(reportStatus == wrStatusInfo.isReportStatus()) &&
		(pusherHome == wrStatusInfo.isPusherHome()) &&
		(rateAnalysis == wrStatusInfo.isRateAnalysis()) &&
		(openWindow == wrStatusInfo.isOpenWindow()) &&
		(mainPower == wrStatusInfo.isMainPower()) &&
		(mainMotor == wrStatusInfo.isMainMotor()) &&
		(limpHome == wrStatusInfo.isLimpHome()) &&
		(packWidth == wrStatusInfo.getPackWidth()) &&
		(filmTension == wrStatusInfo.getFilmTension()) &&
		(stateInfo.equals(wrStatusInfo.getStateInfo()) == true))
	{
		rv = true;
	}

	return rv;
}

