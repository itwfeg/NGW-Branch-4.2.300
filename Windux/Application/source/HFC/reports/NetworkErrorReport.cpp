/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */


#include "NetworkErrorReport.h"
#include "NetworkError.h"
#include "ReportEvent.h"
#include "Session.h"
#include "SessionMessage.h"

#include "HobString.h"
#include "Debug.h"



NetworkErrorReport::NetworkErrorReport(Session* session)
		: name_("Network Errors Report"), currentRecord_(0), database_(0), mySession_(session)
{
}


NetworkErrorReport::~NetworkErrorReport()
{
}


bool NetworkErrorReport::doReport()
{
	Debug::trace("NetworkErrorReport", HobString("    doReport()"));

	//	I would think that we would have first(), next() and last() methods.
	//	For now, we will just fake it numerically.
	NetworkError* newError = 0;
	while ( ++currentRecord_ < 101 )
	{
		newError = NetworkError::findNetworkError(currentRecord_, database_);
		if ( newError )
		{
			// Send the report to the browser.
//			SessionMessage report(ReportManagerDev, NetworkErrorsReportEvent);
//			report.setFormatType(BrowserFormat);
//			report.setHfcObject(newError);
//			mySession_->send(&report, Messenger::DEST_UI);

			return false;
		}
	}

	return true;
}


void NetworkErrorReport::cancelReport()
{
}

