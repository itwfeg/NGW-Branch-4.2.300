/* HT Scale Project
 * COPYRIGHT (c) 2019.  All Rights Reserved.
 * ITWFEG Hobart. Troy, Ohio  USA
 */

#ifndef REMOTEPRINTTEMPLATEREQUEST_H
#define REMOTEPRINTTEMPLATEREQUEST_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>


/*
printRemoteTemplateLabel|t0|t1|t2|t3|t4|t5|t6|t7|t8|t9|t10|t11|t12|t13|t14|t15|t16|t17|t18|t19|t20|t21|t22|t22|t23|t24|t25|

0*  Template number (not printed)  String    15   Prepared on        String
1   Order number                   String    16   Item Description   String
2   Customer name                  String    17   Best if used by    String
3   Customer phone number          String    18   Quantity           String
4   Pickup date/time               String    19   Price              String
5   Comments/Special instructions  String    20   Barcode            String
6   Cake size                      String    21   Future purpose 1   String
7   Flavor                         String    22   Future purpose 2   String
8   Filling                        String    23   Future purpose 3   String
9   Icing & color                  String    24   Future purpose 4   String
10  Top border                     String    25   Future purpose 5   String
11  Bottom border                  String
12  Decorations                    String
13  Custom message                 String
14  Message color                  String
 *  Template number field is required; the rest of the fields can be empty. 
*/


class RemotePrintTemplateRequest : public HfcObject
{
	public:
		RemotePrintTemplateRequest();
		RemotePrintTemplateRequest(const char *data, int len, int formatType);

		~RemotePrintTemplateRequest();

		HobRecordNum getLabelTypeNum() const { return labelTypeNum; }
		void setLabelTypeNum(HobRecordNum num) { labelTypeNum = num; }

		std::string getLabelString(int offset) const;
		size_t getNumOfLabelStrings() const { return labelStrings.size(); }

		std::string getBarcodeString() const;

		virtual const char *serialize(int format) const;

		void show();


	private:

		HobRecordNum labelTypeNum;
		std::vector<std::string> labelStrings;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //REMOTEPRINTTEMPLATEREQUEST_H
