#include "OperatorChangesReport.h"

#include "ReportManager.h"
#include "SessionManager.h"
#include "Session.h"

#include "Configuration.h"
#include "PythonScripter.h"

#include "MultiLanguageText.h"

#include "util/e_string/utilstring.h"
#include <time.h>


const std::string OperatorChangesReport::_contentFontFamily_("Gravilex");
const int OperatorChangesReport::_contentFontSize_ = 6;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperatorChangesReport::dataType = 0;
std::string OperatorChangesReport::dataTypeName("OperatorChangesReport"); 
#endif

OperatorChangesReport::OperatorChangesReport(ReportManager* reportManager, PrinterType type)
		: AbstractReport(reportManager, type) , name_("OperatorChanges Report")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("OperatorChangesReport", 20, 
			"Constructing the OperatorChangesReport for printer " + util_string::inttostring(type));
#endif
}


OperatorChangesReport::~OperatorChangesReport()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("OperatorChangesReport", 20, "Destroying the OperatorChangesReport");
#endif
}


void OperatorChangesReport::doInitialize()
{
#ifndef EVOL_NDEBUG
	Debug::trace("OperatorChangesReport", 20, "Initializing the OperatorChangesReport");
#endif

	if ( !SessionManager::getInstance() || !SessionManager::getInstance()->getPrimaryConsoleSession() )
		return;

	std::string tareDigits = "2";
	bool isTareThreeDigits = Configuration::getInstance()->isThreeDigitTare();
	if( isTareThreeDigits )
		tareDigits = "3";

	// Setup internationalization of text [Reports are only printed on the primary
	// console so we need the language for the primary console.]

	hfc::MultiLanguageText internationalize;

	name_.assign(internationalize("OperatorChanges.ReportTitle"));

	// Generate the contents of the report
	string content;
	std::string data;

	// Date and Time [we'll probably need to internationalize the actual date and time strings ...]
	time_t now = time(&now);
	struct tm * timeinfo = localtime (&now);
	char buffer [80];
	strftime (buffer,80,"%Y-%m-%d",timeinfo);
	content.assign(internationalize("Reports.Date")).append(4, '\t').append(std::string(buffer)).append(1, '\n');
	strftime (buffer,80,"%H:%M",timeinfo);
	content.append(internationalize("Reports.Time")).append(4, '\t').append(std::string(buffer)).append(2, '\n');
	content.append("\x1B\x1B""fn=Gravilex;ps=6\x1B").append(1, '\n');
	content.append(internationalize("OperatorChanges.datetime")).append("  ").append(internationalize("OperatorChanges.transactiontype")).append(" ").append(internationalize("OperatorChanges.plu")).append("   ").append(internationalize("OperatorChanges.changedfield")).append("    ").append(internationalize("OperatorChanges.totalprice")).append("  ").append(internationalize("OperatorChanges.originalvalue")).append("   ").append(internationalize("OperatorChanges.newvalue")).append(2, '\n');
	std::string command;

	setContent(content);

	if( Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED) )
	{
		std::string results;
		std::string report_num = "1";	// 2 is Supervisor Changes report
		std::string file_name = "none";		// reserved for writing to a CSV but could be something in future

		results = "";
		
		command = PythonScripter::buildPythonCall("reportUtils.py");
		command.append(" ").append(report_num);
		command.append(" ").append(tareDigits);
		command.append(" ").append(file_name);

		PAL_absclass::PAL_executeWithResults(command, results);

	    results.append("\x1B\x1B""fn=Gravilex;ps=6\x1B").append(1, '\n');

		Debug::trace("OperatorChangesReport", "Python results\n" + results);
		
		if( results.empty() )
		{
			//content.appendContent(internationalize("OperatorChanges.NoAuditRecs")).append(2, '\n');
			string tmpStr = internationalize("OperatorChanges.NoAuditRecs");
			appendContent(tmpStr);
			Debug::trace("OperatorChangesReport", "No results. The results are empty.");
		}
		else
		{
			// build the report
			Debug::trace("OperatorChangesReport", "Added the following content to the report\n" + results);
			appendContent(results);		//content.append(results);
		}
	}
	else
	{
		//content.append(internationalize("OperatorChanges.pythondisabled")).append(2, '\n');
		string tmpStr = internationalize("OperatorChanges.pythondisabled");
		appendContent( tmpStr );
		Debug::trace("OperatorChangesReport", "python disabled, python script not called, command: " + command);
	}

	
	setContentReady();
}
