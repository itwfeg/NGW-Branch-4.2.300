/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */



#include "HfcStartReport.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcStartReport::dataType = 0;
std::string HfcStartReport::dataTypeName("HfcStartReport"); 
#endif

HfcStartReport::HfcStartReport(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while ( tok.next(&token) != -1 )
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetReportType:
						setReportType((ReportType)Integer::valueOf(token));
						break;
					case fieldOffsetPrinterType:
						setPrinterType((PrinterType)Integer::valueOf(token));
						break;
					case fieldOffsetReportSubType:
						setReportSubType(Integer::valueOf(token));
						break;
					case fieldOffsetCutTestNumber:
						setCutTestNumber(Integer::valueOf(token));
						break;
					case fieldOffsetCutTestReportType:
						setgetCutTestReportType(Integer::valueOf(token));
						break;
					case fieldOffsetReportTitle:
						token.unQuoteForCSV();
						setReportTitle(token);
						break;
					case fieldOffsetReportContent:
						token.unQuoteForCSV();
						setReportContent(token);
						break;
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcStartReport::~HfcStartReport()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

