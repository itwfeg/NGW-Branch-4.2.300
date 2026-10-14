#include "WrapperSettingsReport.h"


#include "ReportManager.h"

#include "HfcStartReport.h"
#include "WrapperSystemRequestStatus.h"
#include "WrapperSystemStatus.h"
#include "WrapperRequestStatus.h"
#include "WrapperStatus.h"
#include "WrapperEvent.h"
#include "WrapperInquire.h"
#include "WrapperRequestInquire.h"
#include "SystemEvent.h"
#include "SystemStatusInfo.h"

#include "Debug.h"

#include "time/date.h"
#include "time/dbtime.h"
#include "e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperSettingsReport::dataType = 0;
std::string WrapperSettingsReport::dataTypeName("WrapperSettingsReport"); 
#endif


using namespace std;


const string WrapperSettingsReport::_name_("Wrapper Settings Report");
const string WrapperSettingsReport::_contentFontFamily_("Gravilex");
const int WrapperSettingsReport::_contentFontSize_ = 7;


WrapperSettingsReport::WrapperSettingsReport(const HfcStartReport* requestMessage,
		ReportManager* reportManager, PrinterType type)
		: AbstractReport(reportManager, type),
		packCode_(requestMessage->getReportSubType()), received_(ReceivedNone) 
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("WrapperSettingsReport", 20, 
		"Constructing the WrapperSettingsReport for printer " + util_string::inttostring(type) +
			" and PackCode " + util_string::inttostring(packCode_));
#endif

	if ( PackCode0 <= packCode_ && packCode_ <= PackCode11 )
		received_.setFlag(ReceivedAll & (~ReceivedWrInquire));
}


WrapperSettingsReport::~WrapperSettingsReport()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("WrapperSettingsReport", 20, "Destroying the WrapperSettingsReport");
#endif

//	if ( !(PackCode0 <= packCode_ && packCode_ <= PackCode11) )
//	{
//		reportManager_->deregisterEvent(WrapperSystem, SysStatus);
//		reportManager_->deregisterEvent(Wrapper, WrStatus);
//	}
//	reportManager_->deregisterEvent(Wrapper, WrInquire);
}


void WrapperSettingsReport::doInitialize()
{
	// Subscribe to the messages that provide the raw information
	if ( !(PackCode0 <= packCode_ && packCode_ <= PackCode11) )
	{
		reportManager_->registerEventTemporarily(WrapperSystem, SysStatus);
		reportManager_->registerEventTemporarily(Wrapper, WrStatus);
	}
	reportManager_->registerEventTemporarily(Wrapper, WrInquire);

	// Request those messages
	if ( !(PackCode0 <= packCode_ && packCode_ <= PackCode11) )
	{
		WrapperSystemRequestStatus request1(reportManager_->getMessageReceiver());
		reportManager_->dispatchEvent(&request1);
		WrapperRequestStatus request2(reportManager_->getMessageReceiver());
		reportManager_->dispatchEvent(&request2);
	}
	Util::CSmartPtr< EvolMessage > request3 ( new WrapperRequestInquire (reportManager_->getMessageReceiver()));
	reportManager_->dispatchEvent(request3);
}


void WrapperSettingsReport::doHandleWrapperMessage(UINT8 event, const char* buffer)
{
#ifndef EVOL_NDEBUG
	Debug::trace("WrapperSettingsReport", 20, "Received Printer message");
#endif

	switch ( event )
	{
		case WrStatus:
		{
#ifndef EVOL_NDEBUG
			Debug::trace("WrapperSettingsReport", 20, "Message: Wrapper - WrInquire");
#endif

			if ( !isContentReady() )
			{
				WrapperStatus ws(buffer);
				serializeWrapperStatus(ws);

				received_.setFlag(ReceivedWrStatus);
				composeReport();
			}
#ifndef EVOL_NDEBUG
			else
				Debug::trace("WrapperSettingsReport", 11,
						"Received a Wrapper - WrStatus message with the content ready");
#endif

			break;
		}
		case WrInquire:
		{
#ifndef EVOL_NDEBUG
			Debug::trace("WrapperSettingsReport", 20, "Message: Wrapper - WrInquire");
#endif

			if ( !isContentReady() )
			{
				WrapperInquire wi(buffer);
				serializeWrapperInquire(wi);

				received_.setFlag(ReceivedWrInquire);
				composeReport();
			}
#ifndef EVOL_NDEBUG
			else
				Debug::trace("WrapperSettingsReport", 11,
						"Received a Wrapper - WrInquire message with the content ready");
#endif

			break;
		}
		default:
		{
			break;
		}
	}
}


void WrapperSettingsReport::doHandleWrapperSystemMessage(UINT8 event, const char* buffer)
{
#ifndef EVOL_NDEBUG
	Debug::trace("WrapperSettingsReport", 20, "Received Printer message");
#endif

	switch ( event )
	{
		case SysStatus:
		{
#ifndef EVOL_NDEBUG
			Debug::trace("WrapperSettingsReport", 20, "Message: WrapperSystem - SysStatus");
#endif

			if ( !isContentReady() )
			{
				WrapperSystemStatus status(buffer);
				serializeSystemStatus(status);

				received_.setFlag(ReceivedSysStatus);
				composeReport();
			}
#ifndef EVOL_NDEBUG
			else
				Debug::trace("WrapperSettingsReport", 11,
						"Received a WrapperSystem - SysStatus message with the content ready");
#endif

			break;
		}
		default:
		{
			break;
		}
	}
}


void WrapperSettingsReport::composeReport()
{
	if ( received_.isFlags(ReceivedAll) && !isContentReady() )
	{
		if ( !(PackCode0 <= packCode_ && packCode_ <= PackCode11) )
		{
			// Date and Time at top of Report
			string dateTimeContent;
			Date dnow;
			dateTimeContent.assign("Date:").append(5, '\t').append(dnow.standardLook()).append(1, '\n');
			Time tnow;
			dateTimeContent.append("Time:").append(5, '\t').append(tnow.standardLook()).append(2, '\n');
			setContent(dateTimeContent);
		}

		if ( packCode_ == -1 )
		{
			// Powerup test results goes next
			appendContent(powerupTestResultsContent_);

			// Wrapper settings is the next section
			appendContent(wrapperStatusSettingsContent_);
			appendContent(wrapperInquireSettingsContent_);
		}

		// Pack code settings is next
		appendContent(packCodeSettingsContent_);

		// Report is now complete
		setContentReady();
	}
}


void WrapperSettingsReport::serializeSystemStatus(const WrapperSystemStatus& stat)
{
	const SystemStatusInfo *wrSystemStatusInfo = stat.getSystemStatusInfo();

	// Powerup test results header
	powerupTestResultsContent_.assign("\t\t\x1B\x1B""fn=Gravilex;ps=9\x1B").
			append("Powerup Test Results").append("\x1B\x1B""fn=Gravilex;ps=7\x1B").append(1, '\n');

	// Calculated Checksum
	powerupTestResultsContent_.append("Calculated Checksum:").append(3, '\t').append("0");
	powerupTestResultsContent_.append(util_string::uinttohexstring(wrSystemStatusInfo->getSystemChecksum())).append("H").append(1, '\n');

	// Stored Checksum
	powerupTestResultsContent_.append("Stored Checksum:").append(3, '\t').append("0");
	powerupTestResultsContent_.append(util_string::uinttohexstring(wrSystemStatusInfo->getROM_Checksum())).append("H").append(1, '\n');

	// EEP Checksum
	powerupTestResultsContent_.append("EEP Checksum:").append(4, '\t');
	powerupTestResultsContent_.append(wrSystemStatusInfo->isEepromError() ? "Failed" : "Passed").append(1, '\n');

	// Word Test
	powerupTestResultsContent_.append("Word Test:").append(5, '\t');
	powerupTestResultsContent_.append(wrSystemStatusInfo->isBitError() ? "Failed" : "Passed").append(1, '\n');

	// Address Test
	powerupTestResultsContent_.append("Address Test:").append(4, '\t');
	powerupTestResultsContent_.append(wrSystemStatusInfo->isAddressError() ? "Failed" : "Passed").append(2, '\n');
}


void WrapperSettingsReport::serializeWrapperStatus(const WrapperStatus& stat)
{
	const WrapperStatusInfo& statusInfo = stat.getStatusInfo();

	// Wrapper Setup header
	wrapperStatusSettingsContent_.assign("\t\t\x1B\x1B""fn=Gravilex;ps=9\x1B").
			append("Wrapper Setup").append("\x1B\x1B""fn=Gravilex;ps=7\x1B").append(1, '\n');

	// Wrapper Rate
	wrapperStatusSettingsContent_.append("Wrapper Rate").append(4, '\t');
	wrapperStatusSettingsContent_.append(util_string::uinttostring(statusInfo.getRate()/10));
	wrapperStatusSettingsContent_.append(1, '.').append(util_string::uinttostring(statusInfo.getRate()%10));
	wrapperStatusSettingsContent_.append(" pkg/min").append(1, '\n');
}


void WrapperSettingsReport::serializeWrapperInquire(const WrapperInquire& wi)
{

	// First the Wrapper Settings portion of the report
	wrapperInquireSettingsContent_.assign("Idle Cycles").append(5, '\t');
	wrapperInquireSettingsContent_.append(util_string::inttostring(wi.getIdleTimeout())).append(2, '\n');

	wrapperInquireSettingsContent_.append("Narrow Film Pull Test").append(3, '\t');
	wrapperInquireSettingsContent_.append( wi.isNarrowFilmTestEnable() ? "Enabled" : "Disabled" ).append(1, '\n');

	wrapperInquireSettingsContent_.append("Wide Film Pull Test").append(3, '\t');
	wrapperInquireSettingsContent_.append( wi.isWideFilmTestEnable() ? "Enabled" : "Disabled" ).append(1, '\n');

	wrapperInquireSettingsContent_.append("Sealer Temperature").append(3, '\t');
	wrapperInquireSettingsContent_.append( wi.isCelsiusEnable() ? "Celsius" : "Fahrenheit" ).append(2, '\n');



	// Now the Packcode Settings portion of the report
	if ( packCode_ == -1 )
	{
		// Pack Code Settings header
		packCodeSettingsContent_.assign("\t\t\x1B\x1B""fn=Gravilex;ps=9\x1B").
				append("Pack Code Settings").append("\x1B\x1B""fn=Gravilex;ps=7\x1B").append(1, '\n');

		for ( int packCodeNumber = PackCode0; packCodeNumber <= PackCode11; ++packCodeNumber )
		{
			serializePackCode(wi.getWrapperPackCodeInfo((WrPackCode)packCodeNumber), packCodeSettingsContent_);
			if ( packCodeNumber < PackCode11 )
				packCodeSettingsContent_.append(3, '\n');
		}
	}
	else
	{
		if ( PackCode0 <= packCode_ && packCode_ <= PackCode11 )
			serializePackCode(wi.getWrapperPackCodeInfo((WrPackCode)packCode_), packCodeSettingsContent_);
#ifndef EVOL_NDEBUG
		else
			Debug::trace("WrapperSettingsReport", 20, "Invalid PackCode (" + util_string::inttostring(packCode_) +
					") to print a Wrapper Settings Report.");
#endif
	}
}


void WrapperSettingsReport::serializePackCode(const WrapperPackCodeInfo& packCode, std::string& content)
{
	// Pack Code Number
	content.append("Pack Code ").append(util_string::inttostring(packCode.getPackCode() + 1)).append(": ");

	// Packcode name/description
	UINT8 packCodeName[PackCodeDescriptionLength];
	packCode.getDescription(packCodeName);
	content.append((const char*)packCodeName).append(1, '\n');

	// Package Size Headers
	content.append("  Package Size").append(2, '\t').append("Min").append(3, '\t').append("Max").append(1, '\n');

	// Widths
	std::string min1(util_string::uinttostring(packCode.getPackSizeMinWidth()));
	std::string max1(util_string::uinttostring(packCode.getPackSizeMaxWidth()));
	content.append("  Width").append(4, '\t');
	content.append(3 - min1.size(), ' ').append(min1).append(3, '\t');
	content.append(3 - max1.size(), ' ').append(max1).append(1, '\n');

	// Lengths
	min1 = util_string::uinttostring(packCode.getPackSizeMinLength());
	max1 = util_string::uinttostring(packCode.getPackSizeMaxLength());
	content.append("  Length").append(4, '\t');
	content.append(3 - min1.size(), ' ').append(min1).append(3, '\t');
	content.append(3 - max1.size(), ' ').append(max1).append(2, '\n');


	// Timings Headers
	content.append("  Timings").append(2, '\t').append("  Energize").append(1, '\t').append("  Release").append(1, '\n');

	// Side Clamps
	min1 = util_string::uinttostring(packCode.getSideClampOnValue());
	max1 = util_string::uinttostring(packCode.getSideClampOffValue());
	content.append("  Side Clamps").append(2, '\t');
	content.append(3 - min1.size(), ' ').append(min1).append(3, '\t');
	content.append(3 - max1.size(), ' ').append(max1).append(1, '\n');

	// Gripper
	min1 = util_string::uinttostring(packCode.getGripperOnValue());
	max1 = util_string::uinttostring(packCode.getGripperOffValue());
	content.append("  Gripper").append(3, '\t');
	content.append(3 - min1.size(), ' ').append(min1).append(3, '\t');
	content.append(3 - max1.size(), ' ').append(max1).append(1, '\n');

	// Knife
//	min = uinttostring(packCode.getKnifeOnMax());
	max1 = util_string::uinttostring(packCode.getKnifeOnValue());
	content.append("  Knife").append(7, '\t');
//	content.append("      Knife").append(4, '\t');
//	content.append(3 - min1.size(), ' ').append(min1).append(3, '\t');
	content.append(3 - max1.size(), ' ').append(max1).append(2, '\n');

	// Slide Block
	content.append("  Slide Block").append(2, '\t');
	content.append(packCode.isSlideBlock() ? "On" : "Off").append(2, '\t');

	// Film Width
	content.append("Film Width").append(1, '\t');
	switch ( packCode.getPackWidth() )
	{
		case NarrowPackage:
			content.append("Narrow").append(1, '\n');
			break;
		case WidePackage:
			content.append("Wide").append(1, '\n');
			break;
		case FilmWidthIdle:
			content.append("Idle").append(1, '\n');
			break;
		case AutoWidthSelect:
			content.append("AutoSelect").append(1, '\n');
			break;
		default:
			content.append("Unknown").append(1, '\n');
			break;
	}

	// Package Booster
	content.append("  P. Booster").append(2, '\t');
	content.append(packCode.isBooster() ? "On" : "Off").append(2, '\t');

	// Film Length
	content.append("Film Length").append(1, '\t');
	switch ( packCode.getPackLength() )
	{
	    case ShortPackage:
			content.append("Short");
			break;
	    case MediumPackage:
			content.append("Med.");
			break;
	    case LongPackage:
			content.append("Long");
			break;
	    case FilmLengthIdle:
			content.append("Idle");
			break;
	    case AutoLengthSelect:
			content.append("AutoSelect");
			break;
		default:
			content.append("Unknown");
			break;
	}
}
