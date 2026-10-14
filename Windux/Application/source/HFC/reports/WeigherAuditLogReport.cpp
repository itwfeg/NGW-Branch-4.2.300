#include "WeigherAuditLogReport.h"

#include "ReportManager.h"
#include "SessionManager.h"
#include "Session.h"
#include "AllCat3AuditTrailRecords.h"

#include "MultiLanguageText.h"

#include "util/e_string/utilstring.h"


const std::string WeigherAuditLogReport::_contentFontFamily_("Gravilex");
const int WeigherAuditLogReport::_contentFontSize_ = 7;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherAuditLogReport::dataType = 0;
std::string WeigherAuditLogReport::dataTypeName("WeigherAuditLogReport"); 
#endif

WeigherAuditLogReport::WeigherAuditLogReport(ReportManager* reportManager, PrinterType type)
		: AbstractReport(reportManager, type) , name_("WeigherAuditLog Report")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("WeigherAuditLogReport", 20, 
			"Constructing the WeigherAuditLogReport for printer " + util_string::inttostring(type));
#endif
}


WeigherAuditLogReport::~WeigherAuditLogReport()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("WeigherAuditLogReport", 20, "Destroying the WeigherAuditLogReport");
#endif
}


void WeigherAuditLogReport::doInitialize()
{
#ifndef EVOL_NDEBUG
	Debug::trace("WeigherAuditLogReport", 20, "Initializing the WeigherAuditLogReport");
#endif

	if ( !SessionManager::getInstance() || !SessionManager::getInstance()->getPrimaryConsoleSession() )
		return;

	// Setup internationalization of text [Reports are only printed on the primary
	// console so we need the language for the primary console.]

	hfc::MultiLanguageText internationalize;

	name_.assign(internationalize("WeigherAuditLogReport.Title"));


	// Generate the contents of the report
	std::string content;
	std::string data;

	// Date and Time [we'll probably need to internationalize the actual date and time strings ...]
	util_time::Date dnow;
	content.assign(internationalize("Reports.Date")).append(4, '\t').append(dnow.standardLook()).append(1, '\n');
	util_time::Time tnow;
	content.append(internationalize("Reports.Time")).append(4, '\t').append(tnow.standardLook()).append(2, '\n');

/*
	// Report header
	data = internationalize("WeigherAuditLogReport.Header1");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header2");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header3");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header4");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header5");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header6");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header7");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header8");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header9");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header10");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header11");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header12");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header13");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header14");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header15");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header16");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header17");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header18");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header19");
	content.append(data).append(1, '\n');

	data = internationalize("WeigherAuditLogReport.Header20");
	content.append(data).append(2, '\n');
*/
	content.append(" Legend for Param column:").append(1, '\n');
	content.append(" 0 = Center of Maintenance Zone").append(1, '\n');
	content.append(" 1 = Scale Factor").append(1, '\n');
	content.append(" 2 = Max Weight").append(1, '\n');
	content.append(" 3 = Gain Factor").append(1, '\n');
	content.append(" 4 = Zero Reference").append(1, '\n');
	content.append(" 5 = Calibration Gain Coefficient").append(1, '\n');
	content.append(" 6 = Calibration Offset Coefficient").append(1, '\n');
	content.append(" 7 = Filter Speed").append(1, '\n');
	content.append(" 8 = Minimum Weight to Print").append(1, '\n');
	content.append(" 9 = Weigher Model").append(1, '\n');
	content.append("10 = Division Size").append(1, '\n');
	content.append("11 = Software Update").append(1, '\n');
	content.append("12 = Gravity Constant").append(1, '\n');
	content.append("13 = Fractional Pricing").append(1, '\n');
	content.append("14 = Pounds-For Multiplier").append(1, '\n');
	content.append("15 = Service Switch Pressed").append(1, '\n');
	content.append("16 = Weigher Mode MY TEST").append(1, '\n');
	content.append("17 = Currency Symbol Placement").append(1, '\n');
	content.append("18 = Total Price Digits").append(1, '\n');
	content.append("19 = Unit Price Digits").append(1, '\n');
	content.append("20 = Rounding Factor").append(1, '\n');
	content.append("21 = Monetary Rounding Method").append(1, '\n');
	content.append("22 = Primary Monetary Symbol").append(1, '\n');
	content.append("23 = Primary Precision Digits").append(1, '\n');
	content.append("24 = Secondary Monetary Symbol").append(1, '\n');
	content.append("25 = Secondary Precision Digits").append(2, '\n');

	Util::CSmartPtr<AllCat3AuditTrailRecords> allAuditRecs = SessionManager::getInstance()->getPrimaryConsoleSession()->getCurrentAllCat3AuditTrailRecords();
	if ( allAuditRecs.notNull() )
	{
		if ( allAuditRecs->getRecordCount() <= 0 )
			content.append("No audit records to display").append(2, '\n');
		else
		{
			std::string logs;
			allAuditRecs->createReportString(logs);
			content.append(logs).append(2, '\n');
		}
	}
	else
		content.append("No audit records to display").append(2, '\n');

	setContent(content);
	setContentReady();

#ifndef EVOL_NDEBUG
	Debug::trace("WeigherAuditLogReport", 20, "Received content of report as:\n" + std::string(content));
#endif
}
