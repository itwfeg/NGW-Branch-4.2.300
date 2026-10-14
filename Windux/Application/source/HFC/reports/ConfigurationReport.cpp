#include "ConfigurationReport.h"


#include "ReportManager.h"
#include "CfgAll.h"

#include "MultiLanguageText.h"

#include "util/e_string/utilstring.h"


const std::string ConfigurationReport::_contentFontFamily_("Gravilex");
const int ConfigurationReport::_contentFontSize_ = 7;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ConfigurationReport::dataType = 0;
std::string ConfigurationReport::dataTypeName("ConfigurationReport"); 
#endif

ConfigurationReport::ConfigurationReport(ReportManager* reportManager, PrinterType type)
		: AbstractReport(reportManager, type) , name_("Configuration Report")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("ConfigurationReport", 20, 
			"Constructing the ConfigurationReport for printer " + inttostring(type));
#endif
}


ConfigurationReport::~ConfigurationReport()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("ConfigurationReport", 20, "Destroying the ConfigurationReport");
#endif
}


void ConfigurationReport::doInitialize()
{
#ifndef EVOL_NDEBUG
	Debug::trace("ConfigurationReport", 20, "Initializing the ConfigurationReport");
#endif


	// Setup internationalization of text [Reports are only printed on the primary
	// console so we need the language for the primary console.]

	hfc::MultiLanguageText internationalize;

	name_.assign(internationalize("ConfigReport.Title"));


	// Generate the contents of the report
	CfgAll cfg;
	std::string content;

	// Date and Time [we'll probably need to internationalize the actual date and time strings ...]
	util_time::Date dnow;
	content.assign(internationalize("Reports.Date")).append(4, '\t').append(dnow.standardLook()).append(1, '\n');
	util_time::Time tnow;
	content.append(internationalize("Reports.Time")).append(4, '\t').append(tnow.standardLook()).append(2, '\n');

	// Rest of report
	content.append(cfg.serialize(PrinterReportFormat));

	setContent(content);
	setContentReady();

#ifndef EVOL_NDEBUG
	Debug::trace("ConfigurationReport", 20, 
			"Received content of report as:\n" + std::string(content));
#endif
}
