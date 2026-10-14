#ifndef SUPERVISORCHANGESREPORT_H
#define SUPERVISORCHANGESREPORT_H

#include "AbstractReport.h"

// An Operator Changes Report object deals with several tasks: the ReportManager
// that sets it up with the user request information, if any, and possibly
// controls its overall behavior (when to stop or whether to suspend/resume),
// the PrinterManager that sets it up with label stock information, and the
// RendererManager, which retrieves the data to be printed.


class SupervisorChangesReport : public AbstractReport
{

	public:
		// --- Constructors and destructor ---
		SupervisorChangesReport(ReportManager* reportManager, PrinterType type = FirstPrinter);
		~SupervisorChangesReport();


		// --- Accessor methods ---
		const std::string& getName() const { return name_; }

		const std::string& getContentFontFamily() const { return _contentFontFamily_; }

		int getContentFontSize() const { return _contentFontSize_; }


		// --- Processing methods ---


	protected:
		void doInitialize();


	private:
		// --- Object Data Members ---
		std::string name_;
		std::string data_;


		// --- Class Data Members ---
		static const std::string _contentFontFamily_;
		static const int _contentFontSize_;


		// --- Processing methods ---
		void updateLabelAndProduct();
		void createFakeLabel();
		void createFakeProduct();
		void printLabel();


#ifdef USE_ACCOUNTANT
static int dataType;
static std::string dataTypeName;
#endif
};

#endif //SUPERVISORCHANGESREPORT_H
