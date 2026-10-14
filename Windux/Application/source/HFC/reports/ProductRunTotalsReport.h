#ifndef RUNTOTALSREPORT_H
#define RUNTOTALSREPORT_H


#include "AbstractReport.h"


class Session;
class Product;
class ProductRunTotal;
class CutTestManager;
class HfcStartReport;
class CutTestDetailsOption1;

typedef enum HfcCutTestTypeFieldOffsets
{
	fieldOffsetCutTestDetails = 1,
	fieldOffsetCutTestInitialContents,
	fieldOffsetCutTestFinalContents,

	endHfcCutTestTypeFieldOffsets	//	MUST be last!
} HfcCutTestTypeFieldOffsets_t;

// A RunTotals Report object deals with several tasks: the ReportManager
// that sets it up with the user request information, if any, and possibly
// controls its overall behavior (when to stop or whether to suspend/resume),
// the PrinterManager that sets it up with label stock information, and the
// RendererManager, which retrieves the data to be printed.

class ProductRunTotalsReport : public AbstractReport
{
	public:
		// --- Constructors and destructor ---
		ProductRunTotalsReport(ReportManager* reportManager, PrinterType type = FirstPrinter, Session* session = 0, int cutTestNumber = 0, int cutTestReportType = 0);
		~ProductRunTotalsReport();


		// --- Accessor methods ---
		const std::string& getName() const { return name_; }

		const std::string& getContentFontFamily() const { return _contentFontFamily_; }

		int getContentFontSize() const { return _contentFontSize_; }

   		// --- Processing methods ---


	protected:
		void doInitialize();
		
	private:
		// --- Object Data Members ---
		std::string		name_;

		int cutTestCode_;
		int cutTestReportType_;

		Session				   *session_;
		Product*			   product_;
		ProductRunTotal*	   productRunTotal_;
		CutTestManager*        cutTestManager_;
		CutTestDetailsOption1* ctdo1_;

		// --- Class Data Members ---
		static const std::string _name_;
		static const std::string _contentFontFamily_;
		static const int _contentFontSize_;

		// --- Processing methods ---
		void composeReport();
		void composeOption1Report();
		void composeErrorReport(int errorType);
		//void updateLabelAndProduct();
		//void createFakeLabel();
		//void createFakeProduct();
		void printLabel();
		void serializeRunTotals(std::string& runTotalsContent);
		void serializePluInformation(std::string& pluInformationContent);

		void serializeOption1Details(std::string& detailsContent);
		void serializeOption1Comments(std::string& commentsContent);
		void serializeOption1FinalComments(std::string& finalCommentsContent);
		void serializeErrorInformation(std::string& errorContent, int errorType);
};



#endif //CONFIGURATIONREPORT_H

