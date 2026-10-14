/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SOFTKEYLISTS_H
#define SOFTKEYLISTS_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"

class Product;
class Category;

#define NEXT_FOR_CLARE


class SoftKeyLists : public HfcObject
{
	public:

		static const int sortOrder_FIFO = 0;
		static const int sortOrder_Alphabetical = 1;

		SoftKeyLists();
		~SoftKeyLists();

		const HobList &getDataTypeList() const;
		const HobList &getRecordNumList() const;
		const HobList &getDescriptionList() const;
		const HobList &getgraphicFileNameList() const;
		const HobList &getOverlayGraphicFileNameList() const;

		void add(const Product &product, int languageSelection);
		void add(const Category &category, int languageSelection);
		//void add(short type);

		void insertNextandPreviousCells(int languageSelection);

		void gotoFirstPage();
		void gotoNextPage();
		void gotoPreviousPage();

		void blankFillLists(unsigned int totalListEntries);

		const char *serialize(int format = BrowserFormat) const;


	private:

		HobList dataTypeList;
		HobList recordNumList;
		HobList descriptionList;
		HobList graphicFileNameList;
		HobList overlayGraphicFileNameList;
#ifdef NEXT_FOR_CLARE
		HobList displayOrderList;
#endif

		int currentSerializeOffset;

		void init();
};

#endif //SOFTKEYLISTS_H
