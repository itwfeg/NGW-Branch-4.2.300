#ifndef ABSTRACTREPORT_H
#define ABSTRACTREPORT_H



#include <string>


#include "Report.h"
#include "Printer.h"                  // PrinterType
#include "PrinterStockMetrics.h"

#include "CSmartPtr.h"


class ReportManager;
class LabelType;
class FormRecord;
class Product;


class AbstractReport : public Report
{
	public:
		// --- Constructors and destructor ---
		AbstractReport(ReportManager* reportManager, PrinterType type = FirstPrinter);
		~AbstractReport();


		// --- Accessor methods ---
		virtual const std::string& getName() const = 0;

		virtual const std::string& getContentFontFamily() const = 0;

		virtual int getContentFontSize() const = 0;

		bool isContentReady() const { return contentReady_; }
		void setContentReady(bool ready = true) { contentReady_ = ready; }

		void setContent(const char* content);
		void setContent(const std::string& content);
		void appendContent(const std::string& content);

		bool isCanceled() const { return canceled_; }
		void setCanceled(bool state = true) { canceled_ = state; }


		// --- Processing methods ---
		void initialize();
		void handleMessage(const char* buffer);
		bool doReport();
		void cancelReport();


	protected:
		// ReportManager instance used to send messages
		ReportManager* reportManager_;


		virtual void doInitialize() = 0;

		virtual void doHandlePrinterMessage(UINT8 UNUSED(event), const char* UNUSED(buffer)) {}
		virtual void doHandleWrapperMessage(UINT8 UNUSED(event), const char* UNUSED(buffer)) {}
		virtual void doHandleWrapperSystemMessage(UINT8 UNUSED(event), const char* UNUSED(buffer)) {}


	private:
		// --- Object Data Members ---
		bool contentReady_;
		bool printingReady_;
		bool canceled_;
		std::string content_;

		PrinterStockMetrics metrics_;
		PrinterType printerType_;
		LabelWidth stockWidth_;
		int stockLength_;
		FormRecord* reportForm_;
		LabelType* fakeLabel_;
		Product* fakeProduct_;
		Util::CSmartPtr<Product> fakeProductSmartPtr_;
		bool labelComplete_;
		bool reportComplete_;
		bool metricsChanged_;


		// --- Processing methods ---
		void handlePrinterMessage(UINT8 event, const char* buffer);

		void updateLabelAndProduct();
		void createFakeLabel();
		void createFakeProduct();
		void printLabel();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif //ABSTRACTREPORT_H

