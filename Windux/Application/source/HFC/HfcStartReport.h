/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCSTARTREPORT_H
#define HFCSTARTREPORT_H



#include "HfcObject.h"
#include "ReportEvent.h"
#include "Printer.h"



typedef enum HfcStartReportFieldOffsets
{
	fieldOffsetReportType = 1,
	fieldOffsetPrinterType,
	fieldOffsetReportSubType,
	fieldOffsetCutTestNumber,
	fieldOffsetCutTestReportType,
	fieldOffsetReportTitle,
	fieldOffsetReportContent,

	endHfcStartReportFieldOffsets	//	MUST be last!
} HfcStartReportFieldOffsets_t;



class HfcStartReport : public HfcObject
{
	public:
		// --- Constructors and destructor ---
		HfcStartReport() 
				: reportType_(UnknownReportType), printerType_(FirstPrinter), reportSubType_(-1), cutTestNumber_(0),cutTestReportType_(0) {}
		HfcStartReport(const char *data, int len, int formatType);
		~HfcStartReport();


		// --- Accessor methods ---
		ReportType getReportType() const { return reportType_; }
		void setReportType(ReportType type) { reportType_ = type; }

		PrinterType getPrinterType() const { return printerType_; }
		void setPrinterType(PrinterType type) { printerType_ = type; }

		int getReportSubType() const { return reportSubType_; }
		void setReportSubType(int subtype) { reportSubType_ = subtype; }

		int getCutTestNumber() const { return cutTestNumber_; }
		void setCutTestNumber(int cuttest) { cutTestNumber_ = cuttest; }

		int getCutTestReportType() const { return cutTestReportType_; }
		void setgetCutTestReportType(int cuttestreporttype) { cutTestReportType_ = cuttestreporttype; }

		HobString getReportTitle() const { return reportTitle_; }
		void setReportTitle(const HobString &title) { reportTitle_ = title; }

		HobString getReportContent() const { return reportContent_; }
		void setReportContent(const HobString &content) { reportContent_ = content; }

	private:
		// --- Data members ---
		ReportType reportType_;
		PrinterType printerType_;
		int reportSubType_;
		int cutTestNumber_;
		int cutTestReportType_;

		HobString reportTitle_;
		HobString reportContent_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif	//HFCSTARTREPORT_H

