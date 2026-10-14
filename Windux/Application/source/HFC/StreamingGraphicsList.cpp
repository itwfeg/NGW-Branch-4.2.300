/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "evolution.h"
#include "StreamingGraphicsList.h"
#include "StreamingGraphic.h"
#include "Configuration.h"
#include "DBAccess.h"

#include "Integer.h"
#include "Long.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int StreamingGraphicsList::dataType = 0;
std::string StreamingGraphicsList::dataTypeName("StreamingGraphicsList"); 
#endif

StreamingGraphicsList::StreamingGraphicsList(DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	dbAccess = access;
}

// private
StreamingGraphicsList::~StreamingGraphicsList()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void StreamingGraphicsList::init()
{
	selectedOffset = -1;
	dbAccess = (DBAccess *)NULL;
}



std::list<StreamingGraphic*> &StreamingGraphicsList::getSlideShow()
{
	return slideShow;
}
const HobList &StreamingGraphicsList::getOrderList() const
{
	return recordNumList;
}
const HobList &StreamingGraphicsList::getDisplayTimeList() const
{
	return displayTimeList;
}
const HobList &StreamingGraphicsList::getGraphicList() const
{
	return graphicList;
}


void StreamingGraphicsList::add(StreamingGraphic *streamingGraphic)
{
	slideShow.push_back(streamingGraphic);
	recordNumList.push_back(HobString(Long::toString(streamingGraphic->getStreamingGraphicNum())));
	displayTimeList.push_back(HobString(Integer::toString(streamingGraphic->getDisplayTime())));

	HobString fileName = streamingGraphic->getImageFileName();
	Filename file1(fileName);
	if (!fileName.empty() && file1.getFileNameExtension().empty())
		fileName.append(Configuration::getInstance()->toStringCfgI(CfgInternal::STREAMINGGRAPHICSDEFAULTEXTENSION));
	graphicList.push_back(fileName);
}


const char *StreamingGraphicsList::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
			Debug::trace("StreamingGraphicsList", "serializing for format BrowserFormat: ");
			serializedData.append(recordNumList.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(displayTimeList.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(graphicList.serialize(format));
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}


int StreamingGraphicsList::getSelectedSlideOffset()
{
	return selectedOffset;
}
void StreamingGraphicsList::setSelectedSlideOffset(int offset)
{
	selectedOffset = offset;
}
void StreamingGraphicsList::incrementSelectedSlideOffset()
{
	if (++selectedOffset >= (int) slideShow.size())
		selectedOffset = 0;
	Debug::trace("StreamingGraphicsList.Clare", HobString("incrementSelectedSlideOffset():  selectedOffset = ") + HobString(Integer::toString(selectedOffset)) + HobString("   slideShow.size() = ") + HobString(Integer::toString((int)slideShow.size())));
}
void StreamingGraphicsList::decrementSelectedSlideOffset()
{
	if (--selectedOffset < 0)
		selectedOffset = (int)slideShow.size() - 1;
	Debug::trace("StreamingGraphicsList.Clare", HobString("decrementSelectedSlideOffset():  selectedOffset = ") + HobString(Integer::toString(selectedOffset)) + HobString("   slideShow.size() = ") + HobString(Integer::toString((int)slideShow.size())));
}

StreamingGraphic *StreamingGraphicsList::selectSlide(HobRecordNum slideNumber)
{
	StreamingGraphic *selectedSlide = (StreamingGraphic*) NULL;
	std::list<StreamingGraphic*>::iterator slideIter;

	displaySlideShowState(HobString("selectSlide():  This is the initial state of the list."));
	int offset = 0;

	slideIter = slideShow.begin();
	while (slideIter != slideShow.end() && !selectedSlide)
	{
		if ((*slideIter)->getStreamingGraphicNum() == slideNumber)
		{
			selectedSlide = *slideIter;
			selectedOffset = offset;
			Debug::trace("StreamingGraphicsList", HobString("selectSlide():  slide # = ") + HobString(Long::toString((long int) slideNumber)) + HobString("  selectedOffset = ") + HobString(Integer::toString(selectedOffset)));
		}
		++slideIter;
		++offset;
	}
	return selectedSlide;
}

StreamingGraphic *StreamingGraphicsList::moveSlideUp()
{
	StreamingGraphic *selectedSlide = (StreamingGraphic*) NULL;
	std::list<StreamingGraphic*>::iterator slideIter;

	displaySlideShowState(HobString("moveSlideUp():  This is the initial state of the list."));

	int slideShowLength = (int)slideShow.size();
	if (slideShowLength > 1)
	{
		StreamingGraphic *slideA = (StreamingGraphic*) NULL;			//	slideA has slideB as its next slide#
		StreamingGraphic *slideB = (StreamingGraphic*) NULL;			//	slideB has selectedSlide as its next slide#

		bool modifyNextValues = ((selectedOffset != 0) && (slideShowLength > 2)) ? true : false;

		int offsetA = selectedOffset - 2;
		int offsetB = selectedOffset - 1;

		if ((offsetA < 0) && (slideShowLength > 2) && (selectedOffset == 1))
		{
			slideA = slideShow.back();
		}

		//	First, find the slide to move and erase it from the list.
		slideIter = slideShow.begin();
		int offset = 0;
		while ((slideIter != slideShow.end()) && (!selectedSlide))
		{
			if (!slideA && (offsetA == offset))					//	Found first, if not the last element.
			{
				slideA = *slideIter;
			}
			if (!slideB && (offsetB == offset))					//	Found after slideA or found first if slideA is the last element.
			{
				slideB = *slideIter;
			}
			if (!selectedSlide && (selectedOffset == offset))	//	Found after slideB. This is the last one found.
			{
				selectedSlide = *slideIter;
				slideIter = slideShow.erase(slideIter);							//	slideIter now points to the next element.
				slideIter = slideShow.insert(--slideIter, selectedSlide);		//	Insert the slide above the previous element.
			}
			++offset;
			++slideIter;
		}

		if (slideA && selectedSlide && modifyNextValues)
		{
			slideA->setNextStreamingGraphicNum(selectedSlide->getStreamingGraphicNum());
			slideA->saveRecord();
			if (Debug::isTracing("StreamingGraphicsList"))
			{
				Debug::trace("StreamingGraphicsList", HobString("moveSlideUp():  slideA has been modified & saved."));
				Debug::trace("StreamingGraphicsList", HobString("moveSlideUp():  slideA:        slide# = ") + HobString(Long::toString(slideA->getStreamingGraphicNum())) + HobString(" nextSlide = ") + HobString(Long::toString(slideA->getNextStreamingGraphicNum())) + HobString("\n"));
			}
		}
		if (slideB && selectedSlide && modifyNextValues)
		{
			slideB->setNextStreamingGraphicNum(selectedSlide->getNextStreamingGraphicNum());
			slideB->saveRecord();
			if (Debug::isTracing("StreamingGraphicsList"))
			{
				Debug::trace("StreamingGraphicsList", HobString("moveSlideUp():  slideB has been modified & saved."));
				Debug::trace("StreamingGraphicsList", HobString("moveSlideUp():  slideB:        slide# = ") + HobString(Long::toString(slideB->getStreamingGraphicNum())) + HobString(" nextSlide = ") + HobString(Long::toString(slideB->getNextStreamingGraphicNum())) + HobString("\n"));
			}
		}
		if (selectedSlide)
		{
			//	If we need to change the starting slide # in Config, do this now before we start changing NEXT values.
			if (selectedOffset == 0)
				changeInitialStreamingGraphicNum(selectedSlide->getNextStreamingGraphicNum());
			else if (selectedOffset == 1)
				changeInitialStreamingGraphicNum(selectedSlide->getStreamingGraphicNum());

			if (slideB && modifyNextValues)
			{
				selectedSlide->setNextStreamingGraphicNum(slideB->getStreamingGraphicNum());
				selectedSlide->saveRecord();
				if (Debug::isTracing("StreamingGraphicsList"))
				{
					Debug::trace("StreamingGraphicsList", HobString("selectedSlide():  selectedSlide has been modified & saved."));
					Debug::trace("StreamingGraphicsList", HobString("selectedSlide():  selectedSlide: slide# = ") + HobString(Long::toString(selectedSlide->getStreamingGraphicNum())) + HobString(" nextSlide = ") + HobString(Long::toString(selectedSlide->getNextStreamingGraphicNum())) + HobString("\n"));
				}
			}
		}
		decrementSelectedSlideOffset();
	}
	displaySlideShowState(HobString("moveSlideUp():  This is the state of the list after moving the slide up."));

	return selectedSlide;
}


StreamingGraphic *StreamingGraphicsList::moveSlideDown()
{
	StreamingGraphic *selectedSlide = (StreamingGraphic*) NULL;
	std::list<StreamingGraphic*>::iterator slideIter;

	displaySlideShowState(HobString("moveSlideDown():  This is the initial state of the list."));

	int slideShowLength = (int)slideShow.size();
	if (slideShowLength > 1)
	{
		StreamingGraphic *slideA = (StreamingGraphic*) NULL;		//	slideA has selectedSlide as its next slide#
		StreamingGraphic *slideC = (StreamingGraphic*) NULL;		//	selectedSlide has slideC as its next slide#

		int lastOffset = slideShowLength - 1;
		bool modifyNextValues = ((selectedOffset != lastOffset) && (slideShowLength > 2)) ? true : false;

		int offsetA = selectedOffset - 1;
		int offsetC = selectedOffset + 1;

		if (slideShowLength > 2)
		{
			if (offsetA == -1)
			{
				slideA = slideShow.back();
			}
			if (selectedOffset == lastOffset)
			{
				slideC = slideShow.front();
			}
		}

		//	First, find the slide to move and erase it from the list.
		slideIter = slideShow.begin();
		int offset = 0;
		while ((slideIter != slideShow.end()) && (!slideA || !selectedSlide || !slideC))
		{
			if (!slideA && (offsetA == offset))			//	Found first, if not the last element.
			{
				slideA = *slideIter;
			}
			if (!selectedSlide && (selectedOffset == offset))	//	Found after slideA or found first if slideA is the last element.
			{
				selectedSlide = *slideIter;
				if (slideC)
				{
					//	This means the selectedSlide is the last slide in the list, so push it to the top.
					slideIter = slideShow.erase(slideIter);		//	Returns the position of the next element
					slideShow.push_front(selectedSlide);		//	Insert the slide
					--slideIter;								//	Decrement so we won't go past the end of the list.
				}
			}
			if (!slideC && (offsetC == offset))					//	Found after selectedSlide if selectedSlide is not the last slide.
			{
				slideC = *slideIter;
				slideIter = slideShow.erase(slideIter);			//	Returns the position of the next element
				slideIter = slideShow.insert(--slideIter, slideC);		//	Insert the slide above the previous element (selectedSlide).
			}
			++offset;
			++slideIter;
		}

		if (slideA && slideC && modifyNextValues)
		{
			slideA->setNextStreamingGraphicNum(slideC->getStreamingGraphicNum());
			slideA->saveRecord();
			if (Debug::isTracing("StreamingGraphicsList"))
			{
				Debug::trace("StreamingGraphicsList", HobString("moveSlideDown():  slideA has been modified & saved."));
				Debug::trace("StreamingGraphicsList", HobString("moveSlideDown():  slideA:        slide# = ") + HobString(Long::toString(slideA->getStreamingGraphicNum())) + HobString(" nextSlide = ") + HobString(Long::toString(slideA->getNextStreamingGraphicNum())) + HobString("\n"));
			}
		}
		if (selectedSlide)
		{
			//	If we need to change the starting slide # in Config, do this now before we start changing NEXT values.
			if (selectedOffset == 0)
				changeInitialStreamingGraphicNum(selectedSlide->getNextStreamingGraphicNum());
			else if (selectedOffset == lastOffset)
				changeInitialStreamingGraphicNum(selectedSlide->getStreamingGraphicNum());

			if (slideC && modifyNextValues)
			{
				selectedSlide->setNextStreamingGraphicNum(slideC->getNextStreamingGraphicNum());
				selectedSlide->saveRecord();
				if (Debug::isTracing("StreamingGraphicsList"))
				{
					Debug::trace("StreamingGraphicsList", HobString("moveSlideDown():  selectedSlide has been modified & saved.\n"));
					Debug::trace("StreamingGraphicsList", HobString("moveSlideDown():  selectedSlide: slide# = ") + HobString(Long::toString(selectedSlide->getStreamingGraphicNum())) + HobString(" nextSlide = ") + HobString(Long::toString(selectedSlide->getNextStreamingGraphicNum())) + HobString("\n"));
				}
			}
		}
		if (slideC && selectedSlide && modifyNextValues)
		{
			slideC->setNextStreamingGraphicNum(selectedSlide->getStreamingGraphicNum());
			slideC->saveRecord();
			if (Debug::isTracing("StreamingGraphicsList"))
			{
				Debug::trace("StreamingGraphicsList", HobString("moveSlideDown():  slideC has been modified & saved.\n"));
				Debug::trace("StreamingGraphicsList", HobString("moveSlideDown():  slideC:        slide# = ") + HobString(Long::toString(slideC->getStreamingGraphicNum())) + HobString(" nextSlide = ") + HobString(Long::toString(slideC->getNextStreamingGraphicNum())) + HobString("\n"));
			}
		}
		incrementSelectedSlideOffset();
	}
	displaySlideShowState(HobString("moveSlideDown():  This is the state of the list after moving the slide down."));
	return selectedSlide;
}


StreamingGraphic *StreamingGraphicsList::insertSlide(StreamingGraphic *newSlide)
{
	if (!newSlide)
		return newSlide;

	displaySlideShowState(HobString("insertSlide():  This is the initial state of the list."));

	int slideShowLength = (int)slideShow.size();
	if (slideShowLength == 0)
	{
		slideShow.push_front(newSlide);
		newSlide->setNextStreamingGraphicNum(newSlide->getStreamingGraphicNum());
		newSlide->saveRecord();
		if (Debug::isTracing("StreamingGraphicsList.Clare"))
		{
			Debug::trace("StreamingGraphicsList", HobString("insertSlide():  newSlide has been created & saved."));
			Debug::trace("StreamingGraphicsList", HobString("insertSlide():  newSlide: slide# = ") + HobString(Long::toString(newSlide->getStreamingGraphicNum())) + HobString(" nextSlide = ") + HobString(Long::toString(newSlide->getNextStreamingGraphicNum())) + HobString("\n"));
		}
		changeInitialStreamingGraphicNum(newSlide->getStreamingGraphicNum());
	}
	else
	{
		int offsetA = -1;
		StreamingGraphic *slideA = (StreamingGraphic*) NULL;
		StreamingGraphic *selectedSlide = (StreamingGraphic*) NULL;

		if ((selectedOffset == 0) || (selectedOffset == -1))
		{
			slideA = slideShow.back();
			if (selectedOffset == -1)
				selectedOffset = 0;
		}
		else
		{
			offsetA = selectedOffset - 1;
		}

		//	First, find the slide at the selected offset, then insert the new slide at that offset.
		std::list<StreamingGraphic*>::iterator slideIter = slideShow.begin();
		int offset = 0;
		while ((slideIter != slideShow.end()) && !selectedSlide)
		{
			if (!slideA && (offsetA == offset))					//	Found first, if not set for the last element.
			{
				slideA = *slideIter;
			}
			if (!selectedSlide && (selectedOffset == offset))	//	Found after slideA or found first if slideA was set for the last element.
			{
				selectedSlide = *slideIter;
				slideIter = slideShow.insert(slideIter, newSlide);		//	Insert the slide
			}
			++offset;
			++slideIter;
		}

		if (slideA && newSlide)
		{
			slideA->setNextStreamingGraphicNum(newSlide->getStreamingGraphicNum());
			slideA->saveRecord();
			if (Debug::isTracing("StreamingGraphicsList"))
			{
				Debug::trace("StreamingGraphicsList", HobString("insertSlide():  slideA has been modified & saved."));
				Debug::trace("StreamingGraphicsList", HobString("insertSlide():  slideA:        slide# = ") + HobString(Long::toString(slideA->getStreamingGraphicNum())) + HobString(" nextSlide = ") + HobString(Long::toString(slideA->getNextStreamingGraphicNum())) + HobString("\n"));
			}
		}
		if (newSlide)
		{
			if (selectedSlide)
			{
				newSlide->setNextStreamingGraphicNum(selectedSlide->getStreamingGraphicNum());
				newSlide->saveRecord();
				if (Debug::isTracing("StreamingGraphicsList.Clare"))
				{
					Debug::trace("StreamingGraphicsList", HobString("insertSlide():  newSlide has been created & saved."));
					Debug::trace("StreamingGraphicsList", HobString("insertSlide():  newSlide: slide# = ") + HobString(Long::toString(newSlide->getStreamingGraphicNum())) + HobString(" nextSlide = ") + HobString(Long::toString(newSlide->getNextStreamingGraphicNum())) + HobString("\n"));
				}
			}
			if (selectedOffset == 0)
			{
				//	Change the starting slide # in Config
				changeInitialStreamingGraphicNum(newSlide->getStreamingGraphicNum());
			}
		}
	}
	displaySlideShowState(HobString("insertSlide():  This is the state of the list after inserting the new slide."));

	return newSlide;
}

StreamingGraphic *StreamingGraphicsList::deleteSlide()
{
	StreamingGraphic *selectedSlide = (StreamingGraphic*) NULL;			//	selectedSlide has nextSlide as its next slide#
	StreamingGraphic *nextSlide = (StreamingGraphic*) NULL;
	std::list<StreamingGraphic*>::iterator slideIter;

	displaySlideShowState(HobString("deleteSlide():  This is the initial state of the list."));

	int slideShowLength = (int)slideShow.size();
	if ((slideShowLength > 0) && (selectedOffset >= 0) && (selectedOffset < slideShowLength))
	{
		StreamingGraphic *slideA = (StreamingGraphic*) NULL;			//	slideA has selectedSlide as its next slide#

		int offsetA = -1;

		if (selectedOffset == 0)
		{
			if (slideShowLength > 1)
			{
				slideA = slideShow.back();
			}
		}
		else
		{
			offsetA = selectedOffset - 1;
		}

		if ((slideShowLength > 1) && (selectedOffset == (slideShowLength - 1)))
		{
			nextSlide = slideShow.front();
		}

		slideIter = slideShow.begin();
		int offset = 0;
		while ((slideIter != slideShow.end()) && (!selectedSlide))
		{
			if (!slideA && (offsetA == offset))					//	Found first, if not the last element.
			{
				slideA = *slideIter;
			}
			if (!selectedSlide && (selectedOffset == offset))	//	Found after slideB. This is the last one found.
			{
				selectedSlide = *slideIter;
				slideIter = slideShow.erase(slideIter);			//	slideIter now points to the next element.
				selectedSlide->deleteRecord();					//	Delete the record from the database.
				Debug::trace("StreamingGraphicsList", HobString("deleteSlide():  selectedSlide has been deleted."));

				if ((slideIter != slideShow.end()))
					nextSlide = *slideIter;
				else if (!slideShow.empty())
				{
					slideIter = slideShow.begin();
					nextSlide = *slideIter;
					selectedOffset = 0;
				}
			}
			++offset;
			++slideIter;
		}

		if (slideA && selectedSlide)
		{
			slideA->setNextStreamingGraphicNum(selectedSlide->getNextStreamingGraphicNum());
			slideA->saveRecord();
			if (Debug::isTracing("StreamingGraphicsList"))
			{
				Debug::trace("StreamingGraphicsList", HobString("deleteSlide():  slideA has been modified & saved."));
				Debug::trace("StreamingGraphicsList", HobString("deleteSlide():  slideA:        slide# = ") + HobString(Long::toString(slideA->getStreamingGraphicNum())) + HobString(" nextSlide = ") + HobString(Long::toString(slideA->getNextStreamingGraphicNum())) + HobString("\n"));
			}
		}
		if (selectedSlide && (selectedOffset == 0))
		{
			//	If we need to change the starting slide # in Config, do this now before we start changing NEXT values.
			if (!slideShow.empty())
				changeInitialStreamingGraphicNum(selectedSlide->getNextStreamingGraphicNum());
			else
			{
				changeInitialStreamingGraphicNum(0);
				selectedOffset = -1;
			}
		}
	}
	displaySlideShowState(HobString("deleteSlide():  This is the state of the list after deleting the slide."));

	return nextSlide;
}

void StreamingGraphicsList::displaySlideShowState(HobString displayStr)
{
	if (Debug::isTracing("StreamingGraphicsList.ShowState"))
	{
		Debug::trace("StreamingGraphicsList.ShowState", displayStr);
		Debug::trace("StreamingGraphicsList.ShowState", HobString("    selectedOffset = ") + HobString(Integer::toString(selectedOffset)) + HobString("\n"));
		Debug::trace("StreamingGraphicsList.ShowState", HobString("    Config starting slide# = ") + HobString(Long::toString(Configuration::getInstance()->getInitialStreamingGraphicNum())) + HobString("\n"));

		displaySlideShowList();
	}
}

void StreamingGraphicsList::displaySlideShowList()
{
	if (Debug::isTracing("StreamingGraphicsList.ShowState"))
	{
		std::list<StreamingGraphic*>::iterator slideIter = slideShow.begin();

		StreamingGraphic *tempSlide = (StreamingGraphic*) NULL;
		int i = 0;
		while (slideIter != slideShow.end())
		{
			tempSlide = *slideIter;
			Debug::trace("StreamingGraphicsList.ShowState", HobString(" # ") + HobString(Integer::toString(i)) + HobString("    slide# = ") + HobString(Integer::toString(tempSlide->getStreamingGraphicNum())) + HobString("    nextSlide# = ") + HobString(Integer::toString(tempSlide->getNextStreamingGraphicNum())));
			++slideIter;
			++i;
		}
		Debug::trace("StreamingGraphicsList.ShowState", HobString("\n\n"));
	}
}

void StreamingGraphicsList::changeInitialStreamingGraphicNum(HobRecordNum slideNumber)
{
	Configuration::getInstance()->setInitialStreamingGraphicNum(slideNumber);
	Configuration::getInstance()->save(dbAccess);
}


void StreamingGraphicsList::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}
