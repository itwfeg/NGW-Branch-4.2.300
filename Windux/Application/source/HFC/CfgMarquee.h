/* Access Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGMARQUEE_H
#define CFGMARQUEE_H

#include "evolution.h"
#include "CfgBase.h"
#include "HfcObject.h"

#include <list>


class CfgMarquee : public CfgBase, public HfcObject
{
	public:

		CfgMarquee();
		CfgMarquee(const CfgMarquee& cfgMarquee);
		CfgMarquee(const char *data, int len, int formatType);
		~CfgMarquee();


		// Read the configuration parameters from the database.
		void load(DBAccess *dbAccess);

		void updateField(int fieldCode, const HobString &token);
		void update(const CfgBase *newValues, bool scaleOnlyFields = false);

		void save(DBAccess *dbAccess, const CfgBase *newValues = (CfgBase *)NULL, bool scaleOnlyFields = false);

		void show();

		void operator=(const CfgMarquee &cfgMarquee);

		void getFieldAsString(int fieldCode, HobString &result, int format) const;
		const HobString& getField(int fieldCode, int format);

		virtual const char *serialize(int format) const;

		HobRecordNum getPermanentMarqueeNumber() const {return permanentMarqueeNumber;}
		void setPermanentMarqueeNumber(HobRecordNum marqueeNumber)
		{
			if ( permanentMarqueeNumber != marqueeNumber )
			{
				permanentMarqueeNumber = marqueeNumber;
				saveNeeded = true;
			}
			if (permanentMarqueeNumber <= 0)
				permanentMarqueeNumber = RESERVED_PERMANENT_MARQUEE_NUMBER;
		}

		const HobString &getPermanentMarquee() const {return permanentMarquee;}
		void setPermanentMarquee(const HobString &marquee) { permanentMarquee = marquee; }

		bool isPermanentMarqueeDisplayed() const {return permanentMarqueeDisplayed;}
		void setPermanentMarqueeDisplayed(bool shouldDisplay) {if (permanentMarqueeDisplayed != shouldDisplay) {permanentMarqueeDisplayed = shouldDisplay; saveNeeded = true;}}

		bool isProductMarqueeDisplayed() const {return productMarqueeDisplayed;}
		void setProductMarqueeDisplayed(bool shouldDisplay) {if (productMarqueeDisplayed != shouldDisplay) {productMarqueeDisplayed = shouldDisplay; saveNeeded = true;}}

		short getProductDescMarqueeCounter() const {return productDescMarqueeCounter;}
		void setProductDescMarqueeCounter(short counter) {if (productDescMarqueeCounter != counter) {productDescMarqueeCounter = counter; saveNeeded = true;}}


		static void initFieldList();
		static const HobRecordNum RESERVED_PERMANENT_MARQUEE_NUMBER = 999901;


	private:
	
		// The Quantum uses tag 'Pm' to send the plu number containing the permanent marquee
		HobRecordNum permanentMarqueeNumber;
		HobString permanentMarquee;

		// Flag indicating whether or not to display the permanent marquee, which is not based solely on the permanent marquee containing text to display.
		bool permanentMarqueeDisplayed;

		// Flag indicating whether or not to display the product marquees, which is not based solely on a product's product-marquee containing text to display.
		// The Quantum uses tag 'pM' to indicate whether or not the Product Marquees should be displayed: '0'=no, '1'=yes.
		bool productMarqueeDisplayed;

		// The number of times to display the product description on the marquee when a product is current in operate mode.
		// The Quantum uses tag 'pd', and sends data as follows:
		// 0=never display the description as the marquee, 1-14= 1 to 14 times, 15=continuous
		short productDescMarqueeCounter;


		mutable HobString tempValue_;

		static std::list<CfgFieldOffsets> cfgMqFieldList, browserFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGMARQUEE_H
