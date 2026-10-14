/* HT-Series Scale Project
 * COPYRIGHT (c) 2016.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#ifndef CUTTESTDETAILSOPTION1_H
#define CUTTESTDETAILSOPTION1_H

#include "evolution.h"
#include "HfcObject.h"
#include "CuttingTest.h"

class HobList;
class DBAccess;

class CutTestDetailsOption1 : public HfcObject
{
	public:

		CutTestDetailsOption1();
		~CutTestDetailsOption1();
		
		HobString     cutTestNumber_;
		HobString     operatorName_;
		HobString     productNum_;
		HobString     vendorNum_;
		HobString     lotNumber_;
		HobString     primalCost_;
		HobString     primalWeight_;
		HobString     packingDate_;
		HobString     startDate_;
		HobString     endDate_;

		HobString     comments_;
		HobString     finalComments_;

		long          totalWaste_;
		long          totalWeightOfCutItems_;
		long          totalPriceOfCutItems_;

		const char *serialize(int format = BrowserFormat) const;

		void createDetailsOption1(DBAccess *dbAccess, const HobString &cutTestNum);

		void exportToFile(std::string outputFilename, LanguageSelection whichLang);


	private:

		CuttingTest *cutTestRec_;

		long totalPrimalWeight_;
		long shrinkYield_;
		long productYield_;
		
		CuttingTest::CutTestStatus status_;
		HobString     operatorID_;		// operatorID

		int     packingHours;
		int     packingMinutes;

		HobList *pluNumList;  
		HobList *descriptionList;  
		HobList *packagesList;
		HobList *netWeightList;      
		HobList *unitPriceList;
		HobList *totalPriceList;  
		HobList *yieldList;


		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CUTTESTDETAILSOPTION1_H
