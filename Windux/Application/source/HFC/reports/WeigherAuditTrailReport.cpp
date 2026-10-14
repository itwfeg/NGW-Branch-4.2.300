#include "WeigherAuditTrailReport.h"

#include "ReportManager.h"
#include "SessionManager.h"
#include "Session.h"
#include "AllCat3AuditTrailRecords.h"

#include "MultiLanguageText.h"

#include "util/e_string/utilstring.h"
#include <time.h>


const std::string WeigherAuditTrailReport::_contentFontFamily_("Gravilex");
const int WeigherAuditTrailReport::_contentFontSize_ = 7;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherAuditTrailReport::dataType = 0;
std::string WeigherAuditTrailReport::dataTypeName("WeigherAuditTrailReport"); 
#endif

WeigherAuditTrailReport::WeigherAuditTrailReport(ReportManager* reportManager, PrinterType type)
		: AbstractReport(reportManager, type) , name_("WeigherAuditTrail Report")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("WeigherAuditTrailReport", 20, 
			"Constructing the WeigherAuditTrailReport for printer " + util_string::inttostring(type));
#endif
}


WeigherAuditTrailReport::~WeigherAuditTrailReport()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("WeigherAuditTrailReport", 20, "Destroying the WeigherAuditTrailReport");
#endif
}


void WeigherAuditTrailReport::doInitialize()
{
#ifndef EVOL_NDEBUG
	Debug::trace("WeigherAuditTrailReport", 20, "Initializing the WeigherAuditTrailReport");
#endif

	if ( !SessionManager::getInstance() || !SessionManager::getInstance()->getPrimaryConsoleSession() )
		return;

	// Setup internationalization of text [Reports are only printed on the primary
	// console so we need the language for the primary console.]

	hfc::MultiLanguageText internationalize;

	name_.assign(internationalize("WeigherAuditTrail.ReportTitle"));


	// Generate the contents of the report
	std::string content;
	std::string data;

	// Date and Time [we'll probably need to internationalize the actual date and time strings ...]
	time_t now = time(&now);
	struct tm * timeinfo = localtime (&now);
	char buffer [80];
	strftime (buffer,80,"%Y-%m-%d",timeinfo);
	content.assign(internationalize("Reports.Date")).append(4, '\t').append(std::string(buffer)).append(1, '\n');
	strftime (buffer,80,"%H:%M",timeinfo);
	content.append(internationalize("Reports.Time")).append(4, '\t').append(std::string(buffer)).append(2, '\n');

	content.append(internationalize("WeigherAuditTrail.Legend")).append(1, '\n');
	content.append("0 = ").append(internationalize("WeigherAuditTrail.com")).append(1, '\n');
	content.append("1 = ").append(internationalize("WeigherAuditTrail.scaleFactor")).append(1, '\n');
	content.append("2 = ").append(internationalize("WeigherAuditTrail.maxWeight")).append(1, '\n');
	content.append("3 = ").append(internationalize("WeigherAuditTrail.gainFactor")).append(1, '\n');
	content.append("4 = ").append(internationalize("WeigherAuditTrail.zeroReference")).append(1, '\n');
	content.append("5 = ").append(internationalize("WeigherAuditTrail.calGainCoeff")).append(1, '\n');
	content.append("6 = ").append(internationalize("WeigherAuditTrail.calOffsetCoeff")).append(1, '\n');
	content.append("7 = ").append(internationalize("WeigherAuditTrail.filterSpeed")).append(1, '\n');
	content.append("8 = ").append(internationalize("WeigherAuditTrail.minWeightToPrint")).append(1, '\n');
	content.append("9 = ").append(internationalize("WeigherAuditTrail.model")).append(1, '\n');
	content.append("10 = ").append(internationalize("WeigherAuditTrail.divisionSize")).append(1, '\n');
	content.append("11 = ").append(internationalize("WeigherAuditTrail.software")).append(1, '\n');
	content.append("12 = ").append(internationalize("WeigherAuditTrail.gravityConstant")).append(1, '\n');
	content.append("13 = ").append(internationalize("WeigherAuditTrail.fractionEnabled")).append(1, '\n');
	content.append("14 = ").append(internationalize("WeigherAuditTrail.poundsForEnabled")).append(1, '\n');
	content.append("15 = ").append(internationalize("WeigherAuditTrail.switchPressed")).append(1, '\n');
	content.append("16 = ").append(internationalize("WeigherAuditTrail.mode")).append(1, '\n');
	content.append("17 = ").append(internationalize("WeigherAuditTrail.decimalSeparator")).append(1, '\n');
	content.append("18 = ").append(internationalize("WeigherAuditTrail.currencySymbolPlacement")).append(1, '\n');
	content.append("19 = ").append(internationalize("WeigherAuditTrail.totalPriceDigits")).append(1, '\n');
	content.append("20 = ").append(internationalize("WeigherAuditTrail.unitPriceDigits")).append(1, '\n');
	content.append("21 = ").append(internationalize("WeigherAuditTrail.roundingFactor")).append(1, '\n');
	content.append("22 = ").append(internationalize("WeigherAuditTrail.monetaryRoundingMethod")).append(1, '\n');
	content.append("23 = ").append(internationalize("WeigherAuditTrail.primaryMonetarySymbol")).append(1, '\n');
	content.append("24 = ").append(internationalize("WeigherAuditTrail.primaryPrecisionDigits")).append(1, '\n');
	content.append("25 = ").append(internationalize("WeigherAuditTrail.secondaryMonetarySymbol")).append(1, '\n');
	content.append("26 = ").append(internationalize("WeigherAuditTrail.secondaryPrecisoinDigits")).append(3, '\n');
	content.append(internationalize("WeigherAuditTrail.ColHeader")).append(2, '\n');

	Util::CSmartPtr<AllCat3AuditTrailRecords> allAuditRecs = SessionManager::getInstance()->getPrimaryConsoleSession()->getCurrentAllCat3AuditTrailRecords();
	if ( allAuditRecs.notNull() )
	{
		if ( allAuditRecs->getRecordCount() <= 0 )
			content.append(internationalize("WeigherAuditTrail.NoAuditRecs")).append(2, '\n');
		else
		{
			std::string logs;
			allAuditRecs->createReportString(logs);
			content.append(logs).append(2, '\n');
		}
	}
	else
		content.append(internationalize("WeigherAuditTrail.NoAuditRecs")).append(2, '\n');

	setContent(content);
	setContentReady();

#ifndef EVOL_NDEBUG
	Debug::trace("WeigherAuditTrailReport", 20, "Received content of report as:\n" + std::string(content));
#endif
}
