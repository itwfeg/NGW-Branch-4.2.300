/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobList.h"
#include "HobTokenizerCSV.h"
#include "Debug.h"
#include "Integer.h"
#include "Bool.h"
#include <algorithm>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobList::dataType = 0;
std::string HobList::dataTypeName("HobList"); 
#endif


HobList::HobList()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

#if defined(LINUX_FIX)
HobList::HobList(const HobList& rhs)
    : HfcObject()
#else
HobList::HobList(const HobList& rhs)
#endif
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	myList = rhs.myList;
	spacingElements = rhs.spacingElements;
	quotingElements = rhs.quotingElements;
}


HobList::HobList(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			// format : totalCount, "item1,item2,item3, ..."

			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			long totalCount = 0L;

			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				if (loopCnt == 1)
				{
					totalCount = Integer::valueOf(token);
				}
				else if (loopCnt == 2 && !token.empty())
				{
					token.unQuoteForCSV(); // Remove the double quotes

					HobTokenizerCSV listTok(&token, BrowserFormatSeparator);
					HobString listEntry;
					long count = 0L;
					while (listTok.next(&listEntry) != -1)
					{
						listEntry.unQuoteForCSV();
						if (count < totalCount)
							myList.push_back(listEntry);
						count++;
					}
				}
				loopCnt++;
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HobList::~HobList()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	myList.clear();
}

// private
void HobList::init()
{
	myListIterator = myList.end();
	calledFirst = false;
	spacingElements = false;
	quotingElements = false;
}


long HobList::getListCount() const
{
	return (long)myList.size();
}

std::list<HobString>::size_type HobList::size() const
{
	return myList.size();
}

void HobList::push_front(const HobString& element)
{
	myList.push_front(element);
}

void HobList::push_back(const HobString& element)
{
	myList.push_back(element);
}

void HobList::insert(const HobString& element)
{
	// Insert the string where the iterator is currently located.
	// You must be sure you have navigated (using first(), next())
	// to where you want to insert the new string!!

	if (myListIterator != myList.end())
		myList.insert(myListIterator, element);
}

bool HobList::end()
{
	return (myListIterator == myList.end()) ? true : false;
}

HobList_type HobList::getMyList()
{
	return myList;
}


bool HobList::isSpacingElements() const
{
	return spacingElements;
}
void HobList::setSpacingElements(bool spacingElements)
{
	this->spacingElements = spacingElements;
}

bool HobList::isQuotingElements() const
{
	return quotingElements;
}
void HobList::setQuotingElements(bool quotingElements)
{
	this->quotingElements = quotingElements;
}


int HobList::first(HobString &returnedString)
{
	if (myList.size() == 0)
		return -1;

	myListIterator = myList.begin();
	if (myListIterator == myList.end())
		return -1;

	returnedString = *myListIterator;
	calledFirst = true;
	return 0;
}

int HobList::next(HobString &returnedString, bool shouldLoop)
{
	if (!calledFirst)
		return first(returnedString);

	if (myList.size() == 0)
		return -1;

	myListIterator++;
	if (myListIterator == myList.end())
	{
		if (shouldLoop)
			return first(returnedString);
		else
			return -1;
	}

	returnedString = *myListIterator;
	return 0;
}

int HobList::previous(HobString &returnedString, bool shouldLoop)
{
	if (!calledFirst)
		return last(returnedString);

	if (myList.size() == 0)
		return -1;

	if (myListIterator == myList.begin())
	{
		if (shouldLoop)
			return last(returnedString);
		else
			return -1;
	}

	--myListIterator;
	returnedString = *myListIterator;
	return 0;
}

int HobList::last(HobString &returnedString)
{
	if (myList.size() == 0)
		return -1;

	myListIterator = myList.end();
	--myListIterator;
	returnedString = *myListIterator;
	calledFirst = true;
	return 0;
}

bool HobList::exists(const HobString& element) const
{
	if (myList.size() == 0)
		return false;

	HobList_type::const_iterator tempIterator = find(myList.begin(), myList.end(), element);
	if (tempIterator == myList.end())
		return false;
	else
		return true;
}

int HobList::getElementPosition(const HobString& element) const
{
	if (myList.empty())
		return -1;

	HobList_type::const_iterator iter = myList.begin();
	int position = 0;
	while ( iter != myList.end() )
	{
		if ( *iter == element )
			return position;
		iter++;
		position++;
	}
	return -1;
}

HobString HobList::getElementAtPosition(int position) const
{
	if ( myList.empty() || position < 0 )
		return "";

	HobList_type::const_iterator iter = myList.begin();
	int offset = position;
	while ( offset && iter != myList.end() )
	{
		iter++;
		offset--;
	}
	if ( iter != myList.end() )
		return *iter;
	else
		return "";
}


void HobList::fillListFromCSVString(const std::string data)
{
	// The imput string is a comma separated list. Split the input on the commas and add each element to the list.
	HobTokenizerCSV dataTok(&data, BrowserFormatSeparator);
	HobString element;
	while (dataTok.next(&element) != -1)
	{
		element.unQuoteForCSV();
		myList.push_back(element);
	}
}


const char *HobList::serialize(int format) const
{
	return serialize(format, 0, -1);
}

/* startOffset is 0 based; range is from 0 to list.size() - 1.
 * If startOffset is beyond the end of the list, no items will be serialized.
 * The maxNumElements indicates the maximum number of elements to write out.
 * If the list contains less than maxNumElements (from the startOffset to the end of the list),
 * fewer list items will be serialized.
 * If numElements == -1, then write out the rest of the list, starting from startOffset.
 */
const char *HobList::serialize(int format, int startOffset, int maxNumElements) const
{
	serializedData = "";

	Debug::trace("HobList", HobString("serialize - startOffset:") + HobString(Integer::toString(startOffset)) + HobString("  maxNumElements:") + HobString(Integer::toString(maxNumElements)));

	if ( startOffset < 0 || (startOffset > 0 && (HobList_type::size_type)startOffset >= myList.size()) )
		return serializedData.c_str();

	if (getFormatType() != UnknownFormat)
		format = getFormatType();


	// Position the iterator at the first list element to be serialized.
	HobList_type::const_iterator iter = myList.begin();
	if (startOffset)
	{
		int offset = startOffset;
		while (offset--)
			iter++;
	}

	int elementsSerialized = 0;
	int elementsToBeSerialized = (maxNumElements == -1) ? (int)myList.size() : maxNumElements;

	switch (format)
	{
		case BrowserFormat:
		{
			HobString listData = "";
			HobString listElement = "";

			int loopCnt = 0;
			while (elementsToBeSerialized-- && iter != myList.end())
			{
				if (loopCnt++)
				{
					listData.append(BrowserFormatSeparator);
					if (spacingElements)
						listData.append(" ");
				}
				listElement = *iter;
				if (quotingElements)
					listElement.quoteForCSV();

				listData.append(listElement);
				elementsSerialized++;
				++iter;
			}
			listData.quoteForCSV();

			serializedData.append(Integer::toString(elementsSerialized));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(listData);

			Debug::trace("HobList", HobString("serialize(): ") + serializedData);
			break;
		}

		default:
			serializedData = HobString("HobList::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}
