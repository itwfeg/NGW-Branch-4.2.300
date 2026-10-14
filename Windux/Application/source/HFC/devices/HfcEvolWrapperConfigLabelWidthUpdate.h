#ifndef HFCEVOLWRAPPERCONFIGLABELWIDTHUPDATE
#define HFCEVOLWRAPPERCONFIGLABELWIDTHUPDATE
#include "evolution.h"
#include "WrapperGeneric.h"
#include "HfcObject.h"
#include "Printer.h"


typedef enum HfcEvolWrapperConfigLabelWidthFieldOffsets
{
	fieldOffsetEvolWrPrinterTypeOffset = 1,
	fieldOffsetEvolWrPrinterSizeOffset,
	endHfcEvolWrapperConfigLabelWidthFieldOffsets		
} HfcEvolWrapperConfigLabelWidthFieldOffsets_t;

class HfcEvolWrapperConfigLabelWidthUpdate : public WrapperGeneric, public HfcObject
{
	public:
		HfcEvolWrapperConfigLabelWidthUpdate(const char *data);
		~HfcEvolWrapperConfigLabelWidthUpdate();

		PrinterType getPrinterType()const {return whichPrinter_;}
		void setPrinterType(PrinterType type) {whichPrinter_ = type;}	

		LabelWidth getLabelWidth()const {return newSize_;}
		void setLabelWidth(LabelWidth size) {newSize_ = size;}

		bool isFirstPrinter()const {return((whichPrinter_ == FirstPrinter) ? true : false);}
		bool isSecondPrinter()const {return((whichPrinter_ == SecondPrinter) ? true : false);};

		virtual const char *serialize(int format = BrowserFormat)const;



	private:
		PrinterType		whichPrinter_;
		LabelWidth		newSize_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif
