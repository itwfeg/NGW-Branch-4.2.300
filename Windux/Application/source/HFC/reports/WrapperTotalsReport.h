#ifndef WRAPPERTOTALSREPORT_H
#define WRAPPERTOTALSREPORT_H



#include "AbstractReport.h"
#include "Wrapper.h"
#include "WrapperInquire.h"
#include "WrapperPackageHistory.h"
#include "WrapperErrorHistory.h"
#include "types.h"



class WrapperTotalsReport : public AbstractReport
{
	public:
		// --- Types and subtypes ---
		typedef enum
		{
			ReceivedNone              = 0x00,

			ReceivedWrInquire         = 0x01,
			ReceivedWrPackageHistory  = 0x02,
			ReceivedWrErrorHistory    = 0x04,

			ReceivedAll               = ReceivedWrInquire | ReceivedWrPackageHistory | ReceivedWrErrorHistory
		} MessagesReceived;


		// --- Constructors and destructor ---
		WrapperTotalsReport(ReportManager* reportManager, PrinterType type = FirstPrinter);
		~WrapperTotalsReport();


		// --- Accessor methods ---
		const std::string& getName() const { return _name_; }

		const std::string& getContentFontFamily() const { return _contentFontFamily_; }

		int getContentFontSize() const { return _contentFontSize_; }


		// --- Processing methods ---


	protected:
		void doInitialize();

		void doHandleWrapperMessage(UINT8 event, const char* buffer);


	private:
		// --- Object Data Members ---
		Flags received_;
		WrapperInquire wrapperInquire_;
		WrapperPackageHistory wrapperPackageHistory_;
		WrapperErrorHistory wrapperErrorHistory_;

		// --- Class Data Members ---
		static const std::string _name_;
		static const std::string _contentFontFamily_;
		static const int _contentFontSize_;


		// --- Processing methods ---
		void composeReport();
		void serializeGrandTotals(std::string& grandTotalsContent);
		void serializePackageTotals(std::string& packageTotalsContent);
		void serializePackageHistory(std::string& packageHistoryContent);
		void serializeErrorTotals(std::string& errorTotalsContent);
		void serializeErrorHistory(std::string& errorHistoryContent);
		const char* serializeWrapperErrorCode(WrErrorCode errorCode, int* tabs);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif //WRAPPERTOTALSREPORT_H
