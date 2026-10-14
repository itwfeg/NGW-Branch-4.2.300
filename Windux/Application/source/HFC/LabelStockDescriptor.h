#ifndef LABELSTOCKDESCRIPTOR_H
#define LABELSTOCKDESCRIPTOR_H



#include "Printer.h"
#include <string>

class Blob;
class HfcPrinterStockMetrics;


class LabelStockDescriptor
{
	public:
		LabelStockDescriptor(UINT16 length = 0,
			 LabelWidth width = NarrowLabel,
			 LabelOrientation orientation = PortraitLabel,
			 INT16 skew = 0);
		~LabelStockDescriptor();

		UINT16 getLength() const;
		void setLength(UINT16 length);

		UINT16 getOriginalWidthDots() const;
		void setOriginalWidthDots(UINT16 widthDots);

		UINT16 getWidthDots() const;
		void setWidthDots(UINT16 widthDots);

		LabelWidth getWidth() const;
		void setWidth(LabelWidth width);

		LabelOrientation getLabelOrientation() const;
		void setLabelOrientation(LabelOrientation orientation);

		UINT16 getLabelOrientationDegrees() const;
		void setLabelOrientationDegrees(UINT16 orientationDegrees);

		INT16 getSkew() const;
		void setSkew(INT16 labelSkew);

		double getLengthInUnits(bool isMetric) const;
		static double getLengthInUnits(UINT16 myLength, bool isMetric);
		double getWidthInUnits(bool isMetric) const;
		static double getWidthInUnits(LabelWidth myWidth, bool isMetric);

		bool isPrintable(LabelStockDescriptor *stockToCompare) const;
		bool isExactMatch(HfcPrinterStockMetrics *printerStockMetrics) const;

		void writeBinaryData(Blob& binaryData);

		static UINT32 getSize();

		void show() const;



	private:
		UINT16 length;
		UINT16 originalWidthDots;
		UINT16 widthDots;
		LabelWidth width;
		LabelOrientation labelOrientation;
		UINT16 labelOrientationDegrees;
		INT16 skew;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif //LABELSTOCKDESCRIPTOR_H

