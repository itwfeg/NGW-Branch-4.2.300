/* HT Scale Project
 * COPYRIGHT (c) 2019.  All Rights Reserved.
 * ITWFEG Hobart. Troy, Ohio  USA
 */

#ifndef REMOTEPRINTREQUEST_H
#define REMOTEPRINTREQUEST_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobDateTime.h"
#include <list>


/*
param args: printRemoteLabel|{plu}|[unit_price]|[by_count]|[expanded_text_number]|[label_type_id]|[printed_locally]|[streamed_labels]|{id}
    1: plu: Required field, the plu number to print.
    2: unit_price: (optional) the unit price of the plu to print.
    3: by_count: (optional) the by count of the plu to print.
    4: expanded_text_number: (optional) the expanded text number of the plu to print.
    5: label_type_id: (optional) the label type id of the plu to print.
    6: printed_locally: (06-14-17) If the job originated from the scale itself.
    7: streamed_labels: The number of labels that will be streamed
    8: id: A unique identification number for the command. This has no purpose to smBackend, send any int value. (APPLICATION DOES NOT GET THIS)
*/
typedef enum RemotePrintRequestFieldOffsets
{
	fieldOffsetRemotePrintProductNum = 1,
	fieldOffsetRemotePrintUnitPrice,
	fieldOffsetRemotePrintByCount,
	fieldOffsetRemotePrintExpandedTextNum,
	fieldOffsetRemotePrintLabelTypeNum,
	fieldOffsetRemotePrintPrintedLocally,
	fieldOffsetRemotePrintNumOfLabels,
	fieldOffsetRemotePrintSellByDate,
	endRemotePrintRequestFieldOffsets
}   RemotePrintRequestFieldOffsets_t;

class RemotePrintRequest : public HfcObject
{
	public:
		RemotePrintRequest();
		RemotePrintRequest(const char *data, int len, int formatType);

		~RemotePrintRequest();


		HobRecordNum getProductNum() const { return productNum; }
		void setProductNum(HobRecordNum num) { productNum = num; }

		HobPrice getUnitPrice() const { return unitPrice; }
		void setUnitPrice(HobPrice price) { unitPrice = price; }
		
		int getByCount() const { return byCount; }
		void setByCount(int count) { byCount = count; }
		
		HobRecordNum getExpandedTextNum() const { return expandedTextNum; }
		void setExpandedTextNum(HobRecordNum num) { expandedTextNum = num; }

		HobRecordNum getLabelTypeNum() const { return labelTypeNum; }
		void setLabelTypeNum(HobRecordNum num) { labelTypeNum = num; }

		// true means print cmd came from external host location, not from same scale
		bool isPrintedLocally() const { return printedLocally; }
		void setPrintedLocally(bool local) { printedLocally = local; }

		int getNumOfLabels() const { return numOfLabels; }
		void setNumOfLabels(int num) { numOfLabels = num; }

		int getSellByDays() const { return numOfDays; }
		void setSellByDays(int days) { numOfDays = days; }

		bool updateField(int fieldOffset, const HobString &newValue);

		void getFieldAsString(int fieldOffset, HobString &result, int format) const;
		virtual const char *serialize(int format) const;

		void show();

		static void initFieldList();


	private:

		HobRecordNum productNum;
		HobPrice unitPrice;
		int byCount;
		HobRecordNum expandedTextNum;
		HobRecordNum labelTypeNum;
		bool printedLocally;
		int numOfLabels;
		int numOfDays;

		static std::list<RemotePrintRequestFieldOffsets> fieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //REMOTEPRINTREQUEST_H
