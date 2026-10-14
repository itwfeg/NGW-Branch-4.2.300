#include "SupervisorChangesReport.h"

#include "ReportManager.h"
#include "SessionManager.h"
#include "Session.h"

#include "MultiLanguageText.h"

#include "Configuration.h"
#include "PythonScripter.h"

#include "util/e_string/utilstring.h"
#include <time.h>


const std::string SupervisorChangesReport::_contentFontFamily_("Gravilex");
const int SupervisorChangesReport::_contentFontSize_ = 6;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SupervisorChangesReport::dataType = 0;
std::string SupervisorChangesReport::dataTypeName("SupervisorChangesReport"); 
#endif

SupervisorChangesReport::SupervisorChangesReport(ReportManager* reportManager, PrinterType type)
		: AbstractReport(reportManager, type) , name_("SupervisorChanges Report")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("SupervisorChangesReport", 20, 
			"Constructing the SupervisorChangesReport for printer " + util_string::inttostring(type));
#endif
}


SupervisorChangesReport::~SupervisorChangesReport()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("SupervisorChangesReport", 20, "Destroying the SupervisorChangesReport");
#endif
}


void SupervisorChangesReport::doInitialize()
{
#ifndef EVOL_NDEBUG
	Debug::trace("SupervisorChangesReport", 20, "Initializing the SupervisorChangesReport");
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

	name_.assign(internationalize("SupervisorChanges.ReportTitle"));


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
	content.append(internationalize("Reports.Time")).append(4, '\t').append(std::string(buffer)).append(4, '\n');
	
	content.append("\x1B\x1B""fn=Gravilex;ps=6\x1B").append(1, '\n');
	content.append(internationalize("SupervisorChanges.datetime")).append("  ").append(internationalize("SupervisorChanges.plu")).append("   ").append(internationalize("SupervisorChanges.changedfield")).append("    ").append(internationalize("OperatorChanges.totalprice")).append("  ").append(internationalize("SupervisorChanges.originalvalue")).append("  ").append(internationalize("SupervisorChanges.newvalue")).append(2, '\n');
	std::string  command;

	setContent(content);

	if( Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED) )
	{
		std::string results;
		std::string report_num = "2";	// 1 is Supervisor Changes report
		std::string file_name  = "none";    // reserved for writing to a CSV but could be something in future

		results = "";

		command = PythonScripter::buildPythonCall("reportUtils.py");
		command.append(" ").append(report_num);
		command.append(" ").append(tareDigits);
		command.append(" ").append(file_name);

		PAL_absclass::PAL_executeWithResults(command, results);

		results.append("\x1B\x1B""fn=Gravilex;ps=6\x1B").append(1, '\n');
		Debug::trace("SupervisorChangesReport", "Python results\n" + results);
		
		if( results.empty() )
		{
			//content.appendContent(internationalize("OperatorChanges.NoAuditRecs")).append(2, '\n');
			string tmpStr = internationalize("OperatorChanges.NoAuditRecs");
			appendContent(tmpStr);
			Debug::trace("SupervisorChangesReport", "No results. The results are empty.");
		}
		else
		{
			// build the report
			Debug::trace("SupervisorChangesReport", "Added the following content to the report\n" + results);
			appendContent(results);		//content.append(results);
		}
	}
	else
	{
		//content.append(internationalize("OperatorChanges.pythondisabled")).append(2, '\n');
		string tmpStr = internationalize("OperatorChanges.pythondisabled");
		appendContent( tmpStr );
		Debug::trace("SupervisorChangesReport", "python disabled, python script not called, command: " + command);
	}

	setContentReady();
}
