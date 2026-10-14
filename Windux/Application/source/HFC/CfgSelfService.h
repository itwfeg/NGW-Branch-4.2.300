/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGSELFSERVICE_H
#define CFGSELFSERVICE_H

#include "evolution.h"
#include "CfgBase.h"
#include "HfcObject.h"

#include <list>


class CfgSelfService : public CfgBase, public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		CfgSelfService();
		CfgSelfService(const CfgSelfService& cfgSelfService);
		CfgSelfService(const char *data, int len, int formatType);
		~CfgSelfService();


		// Read the scale configuration data from the database.
		void load(DBAccess *dbAccess);

		void updateField(int fieldCode, const HobString &token);
		void update(const CfgBase *newValues, bool scaleOnlyFields = false);

		void save(DBAccess *dbAccess, const CfgBase *newValues = (CfgBase *)NULL, bool scaleOnlyFields = false);

		void show();

		void operator=(const CfgSelfService &cfgSelfService);

		void getFieldAsString(int fieldCode, HobString &result, int format) const;
		const HobString& getField(int fieldCode, int format) const;


		virtual const char *serialize(int format) const;


		int getGridXElements() const {return gridXElements;}
		void setGridXElements(int num) {if (gridXElements != num) {gridXElements = num; saveNeeded = true;}}

		int getGridYElements() const {return gridYElements;}
		void setGridYElements(int num) {if (gridYElements != num) {gridYElements = num; saveNeeded = true;}}

		int getNumOfMostCommonCategories() const {return numOfMostCommonCategories;}
		void setNumOfMostCommonCategories(int num) {if (numOfMostCommonCategories != num) {numOfMostCommonCategories = num; saveNeeded = true;}}

		int getImageTypeOnStartScreen() const {return imageTypeOnStartScreen;}
		void setImageTypeOnStartScreen(int type) {if (imageTypeOnStartScreen != type) {imageTypeOnStartScreen = type; saveNeeded = true;}}

		HobRecordNum getInitialStreamingGraphicNum() const {return initialStreamingGraphicNum;}
		void setInitialStreamingGraphicNum(HobRecordNum num) {if (initialStreamingGraphicNum != num) {initialStreamingGraphicNum = num; saveNeeded = true;}}

		int getProductStatsHoursMethod() const {return productStatsHoursMethod;}
		void setProductStatsHoursMethod(int method) {if (productStatsHoursMethod != method) {productStatsHoursMethod = method; saveNeeded = true;}}

		// in seconds
		int getSelfServeInactivityTimeout() const {return selfServeInactivityTimeout;}
		void setSelfServeInactivityTimeout(int timeout) {if (selfServeInactivityTimeout != timeout) {selfServeInactivityTimeout = timeout; saveNeeded = true;}}

		const HobString &getLogoGraphicFileName() const {return logoGraphicFileName;}
		void setLogoGraphicFileName(const HobString &name) {if (logoGraphicFileName != name) {logoGraphicFileName = name; saveNeeded = true;}}

		const HobString &getNoGraphicAvailableFileName1() const {return noGraphicAvailableFileName1;}
		void setNoGraphicAvailableFileName1(const HobString &name) {if (noGraphicAvailableFileName1 != name) {noGraphicAvailableFileName1 = name; saveNeeded = true;}}

		const HobString &getNoGraphicAvailableFileName2() const {return noGraphicAvailableFileName2;}
		void setNoGraphicAvailableFileName2(const HobString &name) {if (noGraphicAvailableFileName2 != name) {noGraphicAvailableFileName2 = name; saveNeeded = true;}}

		const HobString &getPreviousGraphicFileName1() const {return previousGraphicFileName1;}
		void setPreviousGraphicFileName1(const HobString &name) {if (previousGraphicFileName1 != name) {previousGraphicFileName1 = name; saveNeeded = true;}}

		const HobString &getPreviousGraphicFileName2() const {return previousGraphicFileName2;}
		void setPreviousGraphicFileName2(const HobString &name) {if (previousGraphicFileName2 != name) {previousGraphicFileName2 = name; saveNeeded = true;}}

		const HobString &getNextGraphicFileName1() const {return nextGraphicFileName1;}
		void setNextGraphicFileName1(const HobString &name) {if (nextGraphicFileName1 != name) {nextGraphicFileName1 = name; saveNeeded = true;}}

		const HobString &getNextGraphicFileName2() const {return nextGraphicFileName2;}
		void setNextGraphicFileName2(const HobString &name) {if (nextGraphicFileName2 != name) {nextGraphicFileName2 = name; saveNeeded = true;}}


		HobString quoteForSerialize(HobString& string);
		bool isChanged();


		static void initFieldList();		

	
	protected:

		// The number of products/categories on the horizontal axis in the self-service grid.
		int gridXElements;

		// The number of products/categories on the vertical axis in the self-service grid.
		int gridYElements;

		// How many buttons are on the Main Self-Service screen that hold the 'most common categories'.
		int numOfMostCommonCategories;

		/* The type of image shown on the Self-Service Start screen.
		 * 0 = Store's logo, or some static image.
		 * 1 = A slide show.
		 * 2 = A streaming video.
		 */
		int imageTypeOnStartScreen;
		
		// The number of the initial streaming graphic to show.
		HobRecordNum initialStreamingGraphicNum;

		/* The methodology for determining which hour should be supplied to the query to get the most common products
	    * 0 = don't use hours; just use hour zero to accumulate all product statistics
		 * 1 = accumulate statistics on an hour-of-the-day basis, each hour's statistics accumulated separately; use hours 0-23.
		 * 2 = accumulate statistics on a day-of-the-week basis, each day-of-the-week's statistics accumulated separately; use hours 0-6 (0 = Sunday).
		 * 3 = accumulate statistics on an hour-of-the-week basis, use separate hours for each hour of the week, for a total of 24*7=168 individual hourly statistics; use hours 0-167.
		 * 4 = accumulate statistics on a twice-daily basis, midnight to noon in hour 0, noon to midnight in hour 1.
		 */
		int productStatsHoursMethod;

		/* The number of seconds without user input, before which the scale will automatically
		 * return to the startup screen.
		 */
		int selfServeInactivityTimeout;

		// This may no longer be needed.
		HobString logoGraphicFileName;

		/* The name of the "No Graphic Available" graphic file.
		 * This graphic is used when either the product graphic is NULL or empty,
		 * or when the product graphic doesn't exist in the 'productGraphicsDirectoryName'.
		 * This applies to category graphics as well.
		 */
		// Name1 is for the primary language, Name2 is for the secondary language.
		HobString noGraphicAvailableFileName1;
		HobString noGraphicAvailableFileName2;

		/* The name of the "Previous" graphic file.
		 * This graphic is shown in the upper left corner of the self-service products/categories
		 *  grid when there are more products than will fit in the grid, and the user accessed the
		 * second, third, ... page of products/categories by pressing the 'next' cell.
		 */
		// Name1 is for the primary language, Name2 is for the secondary language.
		HobString previousGraphicFileName1;
		HobString previousGraphicFileName2;

		/* The name of the "Next" graphic file.
		 * This graphic is shown in the lower right corner of the self-service products/categories
		 * grid when there are more products than will fit in the grid, and the user is on the first,
		 * second, ... page, other than the last page, of products/categories.
		 */
		// Name1 is for the primary language, Name2 is for the secondary language.
		HobString nextGraphicFileName1;
		HobString nextGraphicFileName2;


	private:
		mutable HobString tempValue_;

		static std::list<CfgFieldOffsets> cfgSSFieldList, browserFieldList, evolFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGSELFSERVICE_H
