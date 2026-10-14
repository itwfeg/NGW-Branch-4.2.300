/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef STREAMINGGRAPHICSLIST_H
#define STREAMINGGRAPHICSLIST_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"

class StreamingGraphic;
class DBAccess;


class StreamingGraphicsList : public HfcObject
{
	public:

		StreamingGraphicsList(DBAccess *access = (DBAccess *)NULL);
		~StreamingGraphicsList();

		std::list<StreamingGraphic*> &getSlideShow();
		const HobList &getOrderList() const;
		const HobList &getDisplayTimeList() const;
		const HobList &getGraphicList() const;

		void add(StreamingGraphic *streamingGraphic);

		const char *serialize(int format = BrowserFormat) const;

		int getSelectedSlideOffset();
		void setSelectedSlideOffset(int offset);
		void incrementSelectedSlideOffset();
		void decrementSelectedSlideOffset();

		StreamingGraphic *selectSlide(HobRecordNum slideNumber);
		StreamingGraphic *moveSlideUp();
		StreamingGraphic *moveSlideDown();
		StreamingGraphic *insertSlide(StreamingGraphic *newSlide);
		StreamingGraphic *deleteSlide();

		void displaySlideShowState(HobString displayStr);
		void displaySlideShowList();
		void changeInitialStreamingGraphicNum(HobRecordNum slideNumber);

		void setDBAccess(DBAccess *access);


	private:

		std::list<StreamingGraphic*>	slideShow;
		HobList recordNumList;
		HobList displayTimeList;
		HobList graphicList;
		int		selectedOffset;

		DBAccess *dbAccess;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //STREAMINGGRAPHICSLIST_H
