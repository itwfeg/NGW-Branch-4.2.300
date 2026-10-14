#include "WrapperTotalsReport.h"


#include "ReportManager.h"

#include "WrapperEvent.h"
#include "WrapperRequestInquire.h"
#include "WrapperRequestPackageHistory.h"
#include "WrapperRequestErrorHistory.h"

#include "Debug.h"

#include "time/date.h"
#include "time/dbtime.h"
#include "e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperTotalsReport::dataType = 0;
std::string WrapperTotalsReport::dataTypeName("WrapperTotalsReport"); 
#endif

using namespace std;


const string WrapperTotalsReport::_name_("Wrapper Totals Report");
const string WrapperTotalsReport::_contentFontFamily_("Gravilex");
const int WrapperTotalsReport::_contentFontSize_ = 7;


WrapperTotalsReport::WrapperTotalsReport(ReportManager* reportManager, PrinterType type)
		: AbstractReport(reportManager, type), received_(ReceivedNone) 
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("WrapperTotalsReport", 20, 
		"Constructing the WrapperTotalsReport for printer " + util_string::inttostring(type));
#endif
}


WrapperTotalsReport::~WrapperTotalsReport()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("WrapperTotalsReport", 20, "Destroying the WrapperTotalsReport");
#endif
}


void WrapperTotalsReport::doInitialize()
{
	// Subscribe to the messages that provide the raw information
	reportManager_->registerEventTemporarily(Wrapper, WrInquire);
	reportManager_->registerEventTemporarily(Wrapper, WrPackageHistory);
	reportManager_->registerEventTemporarily(Wrapper, WrErrorHistory);

	// Request those messages
	WrapperRequestInquire request1(reportManager_->getMessageReceiver());
	reportManager_->dispatchEvent(&request1);
	WrapperRequestPackageHistory request2(reportManager_->getMessageReceiver());
	reportManager_->dispatchEvent(&request2);
	WrapperRequestErrorHistory request3(reportManager_->getMessageReceiver());
	reportManager_->dispatchEvent(&request3);
}


void WrapperTotalsReport::doHandleWrapperMessage(UINT8 event, const char* buffer)
{
#ifndef EVOL_NDEBUG
	Debug::trace("WrapperTotalsReport", 20, "Received Wrapper message");
#endif

	switch ( event )
	{
		case WrInquire:
		{
#ifndef EVOL_NDEBUG
			Debug::trace("WrapperTotalsReport", 20, "Message: Wrapper - WrInquire");
#endif

			if ( !isContentReady() )
			{
				WrapperInquire wi(buffer);
				wrapperInquire_ = wi;

				received_.setFlag(ReceivedWrInquire);

				composeReport();
			}
#ifndef EVOL_NDEBUG
			else
				Debug::trace("WrapperTotalsReport", 11,
						"Received a Wrapper - WrInquire message with the content ready");
#endif

			break;
		}
		case WrPackageHistory:
		{
#ifndef EVOL_NDEBUG
			Debug::trace("WrapperTotalsReport", 20, "Message: Wrapper - WrPackageHistory");
#endif

			if ( !isContentReady() )
			{
				WrapperPackageHistory wph(buffer);
				wrapperPackageHistory_ = wph;
				wrapperPackageHistory_.show();

				received_.setFlag(ReceivedWrPackageHistory);

				composeReport();
			}
#ifndef EVOL_NDEBUG
			else
				Debug::trace("WrapperTotalsReport", 11,
						"Received a Wrapper - WrInquire message with the content ready");
#endif

			break;
		}
		case WrErrorHistory:
		{
#ifndef EVOL_NDEBUG
			Debug::trace("WrapperTotalsReport", 20, "Message: Wrapper - WrErrorHistory");
#endif

			if ( !isContentReady() )
			{
				WrapperErrorHistory weh(buffer);
				wrapperErrorHistory_ = weh;

				received_.setFlag(ReceivedWrErrorHistory);

				composeReport();
			}
#ifndef EVOL_NDEBUG
			else
				Debug::trace("WrapperTotalsReport", 11,
						"Received a Wrapper - WrErrorHistory message with the content ready");
#endif

			break;
		}
		default:
		{
			break;
		}
	}
}


void WrapperTotalsReport::composeReport()
{
	if ( received_.isFlags(ReceivedAll) && !isContentReady() )
	{
		// Date and Time at top of Report
		string dateTimeContent;
		Date dnow;
		dateTimeContent.assign("Date:").append(5, '\t').append(dnow.standardLook()).append(1, '\n');
		Time tnow;
		dateTimeContent.append("Time:").append(5, '\t').append(tnow.standardLook()).append(2, '\n');
		setContent(dateTimeContent);

		// Grand Totals section is the next one
		string grandTotalsContent;
		serializeGrandTotals(grandTotalsContent);
		appendContent(grandTotalsContent);

		// Package Totals is the next section
		string packageTotalsContent;
		serializePackageTotals(packageTotalsContent);
		appendContent(packageTotalsContent);

		// Package History goes next
		string packageHistoryContent;
		serializePackageHistory(packageHistoryContent);
		appendContent(packageHistoryContent);

		// Error Totals is next
		string errorTotalsContent;
		serializeErrorTotals(errorTotalsContent);
		appendContent(errorTotalsContent);

		// Error History is last
		string errorHistoryContent;
		serializeErrorHistory(errorHistoryContent);
		appendContent(errorHistoryContent);

		// Report is now complete
		setContentReady();
	}
}


void WrapperTotalsReport::serializeGrandTotals(std::string& grandTotalsContent)
{
	// Grand Totals header
	grandTotalsContent.assign("\t\t\x1B\x1B""fn=Gravilex;ps=9\x1B").
			append("Grand Totals").append("\x1B\x1B""fn=Gravilex;ps=7\x1B").append(1, '\n');

	// Total Package Count
	grandTotalsContent.append("Total Package Count:").append(2, '\t');
	grandTotalsContent.append(util_string::uinttostring(wrapperInquire_.getPackageGrandTotal())).append(1, '\n');

	// Total Cycles Count
	grandTotalsContent.append("Total Cycles Count:").append(2, '\t');
	grandTotalsContent.append(util_string::uinttostring(wrapperInquire_.getWrapperCycleTotal())).append(2, '\n');
}


void WrapperTotalsReport::serializePackageTotals(std::string& packageTotalsContent)
{
	// Package Totals header
	packageTotalsContent.assign("\t\t\x1B\x1B""fn=Gravilex;ps=9\x1B").
			append("Package Totals").append("\x1B\x1B""fn=Gravilex;ps=7\x1B").append(1, '\n');
	packageTotalsContent.append("Pack Code").append(3, '\t').append("  Packages").
			append(1, '\t').append("   Jams").append(1, '\n');

	unsigned int totalPackages = 0, totalJams = 0;
	int tabs [12] = { 2, 5, 4, 5, 5, 2, 5, 5, 2, 2, 2, 2 };
	for ( int packCodeNumber = PackCode0; packCodeNumber < NumberPackCodes; ++packCodeNumber )
	{
		const WrapperPackCodeInfo& packCode = wrapperInquire_.getWrapperPackCodeInfo((WrPackCode)packCodeNumber);

		// Packcode name/description
		UINT8 packCodeName[PackCodeDescriptionLength];
		packCode.getDescription(packCodeName);
		packageTotalsContent.append((const char*)packCodeName).append(tabs[packCodeNumber], '\t');

		//	Package counts
		string packageCounts(util_string::uinttostring(packCode.getPackageTotal()));
		packageTotalsContent.append(7 - packageCounts.size(), ' ').append(packageCounts).append(2, '\t');
		totalPackages += packCode.getPackageTotal();

		//	Jam counts
		string jamCounts(util_string::uinttostring(packCode.getJamTotal()));
		packageTotalsContent.append(6 - packageCounts.size(), ' ').append(jamCounts).append(1, '\n');
		totalJams += packCode.getJamTotal();
	}

	// PackCode totals
	packageTotalsContent.append("  Totals").append(4, '\t');
	string totalPackageCounts(util_string::uinttostring(totalPackages));
	packageTotalsContent.append(7 - totalPackageCounts.size(), ' ').append(totalPackageCounts).append(2, '\t');
	string totalJamCounts(util_string::uinttostring(totalJams));
	packageTotalsContent.append(6 - totalJamCounts.size(), ' ').append(totalJamCounts).append(1, '\n');

	// Packcode totals last reset date
	try
	{
		packageTotalsContent.append("  Date Reset:").append(3, '\t').append(Date(wrapperInquire_.getPackCodeTotalsDate()).hobartLook()).append(2, '\n');
	}
	catch ( Date::InvalidDate& )
	{
		packageTotalsContent.append("  Date Reset:").append(2, '\n');
	}
}


void WrapperTotalsReport::serializePackageHistory(std::string& packageHistoryContent)
{
	// Recent Packages (Package History) header
	packageHistoryContent.assign("\t\t\x1B\x1B""fn=Gravilex;ps=9\x1B").
			append("Recent Packages").append("\x1B\x1B""fn=Gravilex;ps=7\x1B").append(1, '\n');
	packageHistoryContent.append("Pack Code").append(4, '\t').append("Width");
	packageHistoryContent.append(1, '\t').append("Length").append(1, '\n');

	string number, width, length;
	UINT8 packCodeName[PackCodeDescriptionLength];
	int tabs [12] = { 2, 5, 4, 5, 5, 2, 5, 5, 2, 2, 2, 2 };
	WrapperPackageHistory::const_iterator iter = wrapperPackageHistory_.begin();
	while ( iter != wrapperPackageHistory_.end() )
	{
		iter.show();

		const WrapperPackCodeInfo& packCode = wrapperInquire_.getWrapperPackCodeInfo(iter->packCode_);

		// Packcode number
		number.assign(util_string::inttostring(iter->packCode_ + 1));
		Debug::trace("WrapperTotalsReport", 20, "PackCode Number " + number);
		packageHistoryContent.append(2 - number.size(), ' ').append(number).append(2, ' ');

		// Packcode name/description
		packCode.getDescription(packCodeName);
		Debug::trace("WrapperTotalsReport", 20, "PackCode Name " + string((const char*)packCodeName));
		packageHistoryContent.append((const char*)packCodeName).append(tabs[iter->packCode_], '\t');

		// Package width
		width.assign(util_string::inttostring(iter->width_));
		Debug::trace("WrapperTotalsReport", 20, "Package Width " + string(width));
		packageHistoryContent.append(4 - width.size(), ' ').append(width).append(1, '\t');

		// Package length
		length.assign(util_string::inttostring(iter->length_));
		Debug::trace("WrapperTotalsReport", 20, "Package Length " + string(length));
		packageHistoryContent.append(5 - length.size(), ' ').append(length).append(1, '\n');

		++iter;
	}

	packageHistoryContent.append(1, '\n');
}


void WrapperTotalsReport::serializeErrorTotals(std::string& errorTotalsContent)
{
	// Error Totals header
	unsigned int totalErrors = 0;
	errorTotalsContent.assign("\t\t\x1B\x1B""fn=Gravilex;ps=9\x1B").
			append("Error Totals").append("\x1B\x1B""fn=Gravilex;ps=7\x1B").append(1, '\n');
	errorTotalsContent.append("Error Type").append(4, '\t').append("Quantity").append(1, '\n');

	// Elevator Collapse
	string errorCount(util_string::uinttostring(wrapperInquire_.getElevatorCollapseTotal()));
	errorTotalsContent.append("Elevator Collapse\t\t\t").append(5 - errorCount.size(), ' ').append(errorCount).append(1, '\n');
	totalErrors += wrapperInquire_.getElevatorCollapseTotal();

	// Intake Jam
	errorCount = util_string::uinttostring(wrapperInquire_.getIntakeJamTotal());
	errorTotalsContent.append("Intake Jam\t\t\t\t").append(5 - errorCount.size(), ' ').append(errorCount).append(1, '\n');
	totalErrors += wrapperInquire_.getIntakeJamTotal();

	// Length Jam
	errorCount = util_string::uinttostring(wrapperInquire_.getLengthJamTotal());
	errorTotalsContent.append("Length Jam\t\t\t\t").append(5 - errorCount.size(), ' ').append(errorCount).append(1, '\n');
	totalErrors += wrapperInquire_.getLengthJamTotal();

	// Width Jam
	errorCount = util_string::uinttostring(wrapperInquire_.getWidthJamTotal());
	errorTotalsContent.append("Width Jam\t\t\t\t").append(5 - errorCount.size(), ' ').append(errorCount).append(1, '\n');
	totalErrors += wrapperInquire_.getWidthJamTotal();

	// Cover Open
	errorCount = util_string::uinttostring(wrapperInquire_.getCoverOpenTotal());
	errorTotalsContent.append("Cover Open\t\t\t\t").append(5 - errorCount.size(), ' ').append(errorCount).append(1, '\n');
	totalErrors += wrapperInquire_.getCoverOpenTotal();

	// Sealer Belt Open
	errorCount = util_string::uinttostring(wrapperInquire_.getSealerBeltOpenTotal());
	errorTotalsContent.append("Sealer Belt Open\t\t\t").append(5 - errorCount.size(), ' ').append(errorCount).append(1, '\n');
	totalErrors += wrapperInquire_.getSealerBeltOpenTotal();

	// Package Too Tall
	errorCount = util_string::uinttostring(wrapperInquire_.getPackageTooTallTotal());
	errorTotalsContent.append("Package Too Tall\t\t\t").append(5 - errorCount.size(), ' ').append(errorCount).append(1, '\n');
	totalErrors += wrapperInquire_.getPackageTooTallTotal();

	// Pusher Home
	errorCount = util_string::uinttostring(wrapperInquire_.getPusherHomeErrorTotal());
	errorTotalsContent.append("Pusher Home\t\t\t\t").append(5 - errorCount.size(), ' ').append(errorCount).append(1, '\n');
	totalErrors += wrapperInquire_.getPusherHomeErrorTotal();

	// No Film Pulled
	errorCount = util_string::uinttostring(wrapperInquire_.getFilmPullErrorTotal());
	errorTotalsContent.append("No Film Pulled\t\t\t").append(5 - errorCount.size(), ' ').append(errorCount).append(1, '\n');
	totalErrors += wrapperInquire_.getFilmPullErrorTotal();

	// Error totals
	errorTotalsContent.append("  Total\t\t\t\t\t");
	std::string totalErrorString(util_string::uinttostring(totalErrors));
	errorTotalsContent.append(5 - totalErrorString.size(), ' ').append(totalErrorString).append(1, '\n');

	// Error totals last reset date
	try
	{
		errorTotalsContent.append("  Date Reset:").append(3, '\t').append(Date(wrapperInquire_.getErrorTotalsDate()).hobartLook()).append(2, '\n');
	}
	catch ( Date::InvalidDate& )
	{
		errorTotalsContent.append("  Date Reset:").append(2, '\n');
	}
}


void WrapperTotalsReport::serializeErrorHistory(std::string& errorHistoryContent)
{
	// Recent Errors (Error History) header
	errorHistoryContent.assign("\t\t\x1B\x1B""fn=Gravilex;ps=9\x1B").
			append("Recent Errors").append("\x1B\x1B""fn=Gravilex;ps=7\x1B").append(1, '\n');
	errorHistoryContent.append("Wrapper Error").append(5, '\t').append("Pack Code").append(1, '\n');

	int i = 0, tabs = 0;
	string number;
	WrErrorCode errorCode = wrapperErrorHistory_.getFirstErrorCode();
	for ( ; i < MaxErrorHistoryBuffers; ++i, errorCode = wrapperErrorHistory_.getNextErrorCode() )
	{
		const WrapperPackageInfo& packageInfo = wrapperErrorHistory_.getPackageInfo(i);
		WrPackCode packCode = packageInfo.getPackCode();

		// Wrapper error description
		errorHistoryContent.append(serializeWrapperErrorCode(errorCode, &tabs));

		if ( errorCode != WrNoErrorCode && (PackCode0 <= packCode && packCode <= PackCode11) )
		{
			// Pack Code number
			number.assign(util_string::inttostring(packageInfo.getPackCode() + 1));
			errorHistoryContent.append(tabs, '\t').append(2 - number.size(), ' ').append(number).append(1, '\n');
		}
		else
			errorHistoryContent.append(1, '\n');
	}
}


const char* WrapperTotalsReport::serializeWrapperErrorCode(WrErrorCode errorCode, int* tabs)
{
	switch ( errorCode )
	{
		case WrNoErrorCode:
			*tabs = 8;
			return "No Error";
		case WrPackageTooLarge:
			*tabs = 1;
			return "Package on wrapper intake too long";
		case WrHeaterFailure:
			*tabs = 7;
			return "Heater failure";
		case WrEepWriteFailure:
			*tabs = 6;
			return "EEP write failure";
		case WrTooTallInterlock:
			*tabs = 1;
			return "Package on wrapper intake too tall";
		case WrFilmDoorInterlock:
			*tabs = 4;
			return "Film door interlock error";
		case WrHandCrankDoorInterlock:
			*tabs = 2;
			return "Hand crank door interlock error";
		case WrScalePlatterInterlock:
			*tabs = 3;
			return "Scale platter interlock error";
		case WrElevatorInterlock:
			*tabs = 3;
			return "Wrapper elevator collapsed";
		case WrSealerBeltInterlock:
			*tabs = 2;
			return "Sealer belt not in locked position";
		case WrGeneralFault2:
			*tabs = 6;
			return "General fault 2";
		case WrGeneralFault1:
			*tabs = 6;
			return "General fault 1";
		case WrLeftCanopyInterlock:
			*tabs = 3;
			return "Left canopy interlock error";
		case WrRightCanopyInterlock:
			*tabs = 3;
			return "Right canopy interlock error";
		case WrFrontCoverInterlock:
			*tabs = 3;
			return "Wrapper clean out pan open";
		case WrLeftSideCoverInterlock:
			*tabs = 3;
			return "Left side cover interlock error";
		case WrRightSideCoverInterlock:
			*tabs = 2;
			return "Right side cover interlock error";
		case WrWideFilmSupplyFailure:
			*tabs = 3;
			return "Wrapper did not pull wide film";
		case WrNarrowFilmSupplyFailure:
			*tabs = 2;
			return "Wrapper did not pull narrow film";
		case WrPackageJammed:
			*tabs = 4;
			return "Wrapper package jammed";
		case WrMainMotorStalled:
			*tabs = 3;
			return "Wrapper main motor stalled";
		case WrIntakeHomeFailure:
			*tabs = 2;
			return "Intake pusher unable to find home";
		case WrIntakeWidthFailure:
			*tabs = 3;
			return "Wrapper width switch error";
		case WrIntakeLengthFailure:
			*tabs = 1;
			return "Package on wrapper intake too long";
		case WrIntakeMotorJammed:
			*tabs = 3;
			return "Wrapper intake pusher jammed";
		case WrIntakePackagePresent:
			*tabs = 4;
			return "Intake package present";
		case WrIntakeOutOfSync:
			*tabs = 2;
			return "Remove packages from wrapper";
		case WrStopSwitchInterlock:
			*tabs = 5;
			return "Stop switch interlock";
		default:
			*tabs = 4;
			return "Undefined error number";
	}
}
