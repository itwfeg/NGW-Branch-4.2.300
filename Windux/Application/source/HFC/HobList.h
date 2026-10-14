/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HOBLIST_H
#define HOBLIST_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>
#include <iterator>

typedef std::list<HobString> HobList_type;


class HobList : public HfcObject
{
	public:

		HobList();
		HobList(const char *data, int len, int formatType);
		HobList(const HobList& rhs);
		~HobList();

		long getListCount() const;
		HobList_type::size_type size() const;
		void push_front(const HobString& element);
		void push_back(const HobString& element);
		void insert(const HobString& element);
		bool end();
		HobList_type getMyList();

		bool isSpacingElements() const;
		void setSpacingElements(bool spacingElements);

		bool isQuotingElements() const;
		void setQuotingElements(bool quotingElements);

		int first(HobString &returnedString);
		int next(HobString &returnedString, bool shouldLoop = false);
		int previous(HobString &returnedString, bool shouldLoop = false);
		int last(HobString &returnedString);
		
		bool exists(const HobString& element) const;
		int getElementPosition(const HobString& element) const;
		HobString getElementAtPosition(int position) const;

		void fillListFromCSVString(const std::string data);

		virtual const char *serialize(int format) const;
		virtual const char *serialize(int format, int startOffset, int maxNumElements) const;

	protected:

		HobList_type myList;
		HobList_type::iterator myListIterator;

		bool calledFirst;

		bool spacingElements;

		bool quotingElements;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HOBLIST_H
