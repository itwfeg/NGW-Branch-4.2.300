#include "EvolWrapperPackageTimings.h"
#include "EvolWrapperMessages.h"
#include "FpmManager.h"

#include "HobString.h"
#include "Debug.h"
#include "Integer.h"
#include "Hex.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperPackageTimings::_dataType_ = 0;
std::string EvolWrapperPackageTimings::_dataTypeName_("EvolWrapperPackageTimings"); 
#endif


EvolWrapperPackageTimings::EvolWrapperPackageTimings(Hobart::CEvolMsgReceiver *pHobThread)
	: WrapperGeneric(WrEvolPackageTimings, pHobThread), wrapBoxes_(), disposition(WRITE_TEMPORARY)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

//	if (Debug::isTracing("EvolWrapperPackageTimings"))
//	{
//		Debug::trace("EvolWrapperPackageTimings", "Constructor passing in a HobThread* pHobThread:  this = " + Hex::toString((long) this));
//	}
}


EvolWrapperPackageTimings::EvolWrapperPackageTimings(const char* buffer)
	: WrapperGeneric(buffer), wrapBoxes_(), disposition(WRITE_TEMPORARY)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

//	if (Debug::isTracing("EvolWrapperPackageTimings"))
//	{
//		Debug::trace("EvolWrapperPackageTimings", "Constructor passing in a const char* buffer:  this = " + Hex::toString((long) this));
//	}

	WrapperGeneric::serialize();
	int bufCounter = WrapperGeneric::getSerializedLength();

	UINT16 numWrapBoxes = 0, numHeightBoxes = 0;

	// Now, for the derived class items
	// Read the number of wrap boxes in the buffer
	char* pChar = (char*)&disposition;
	for (unsigned int i = 0; i < sizeof(disposition); i++)
	{
		*pChar++ = buffer[bufCounter++];
	}

	pChar = (char*)&numWrapBoxes;
	for (unsigned int i = 0; i < sizeof(numWrapBoxes); i++)
	{
		*pChar++ = buffer[bufCounter++];
	}

	// Read the number of height boxes for each wrap box
	std::vector<int> wrapBoxHeights(numWrapBoxes);
	for ( int i = 0; i < numWrapBoxes; ++i )
	{
		pChar = (char*)&numHeightBoxes;
		for ( unsigned int j = 0; j < sizeof(numHeightBoxes); ++j )
			*pChar++ = buffer[bufCounter++];

		wrapBoxHeights[i] = numHeightBoxes;
	}

	// Read each wrap box
	for ( int i = 0; i < numWrapBoxes; ++i )
	{
		char name[WrapBox::_MaxNameLength_ + 1];
		UINT32 pluException;
		UINT16 lengthLow, lengthHigh;
		UINT16 widthLow, widthHigh;
		UINT16 lip;

		// Read the wrap box's name
		pChar = name;
		for ( unsigned int j = 0; j < WrapBox::_MaxNameLength_ + 1; ++j )
			*pChar++ = buffer[bufCounter++];

		// Read the wrap box's PLU Exception
		pChar = (char*)&pluException;
		for ( unsigned j = 0; j < sizeof(pluException); ++j )
			*pChar++ = buffer[bufCounter++];

		// Read the wrap box's lengthLow
		pChar = (char*)&lengthLow;
		for ( unsigned j = 0; j < sizeof(lengthLow); ++j )
			*pChar++ = buffer[bufCounter++];

		// Read the wrap box's lengthHigh
		pChar = (char*)&lengthHigh;
		for ( unsigned j = 0; j < sizeof(lengthHigh); ++j )
			*pChar++ = buffer[bufCounter++];

		// Read the wrap box's widthLow
		pChar = (char*)&widthLow;
		for ( unsigned j = 0; j < sizeof(widthLow); ++j )
			*pChar++ = buffer[bufCounter++];

		// Read the wrap box's widthHigh
		pChar = (char*)&widthHigh;
		for ( unsigned j = 0; j < sizeof(widthHigh); ++j )
			*pChar++ = buffer[bufCounter++];

		// Read the wrap box's lip
		pChar = (char*)&lip;
		for ( unsigned j = 0; j < sizeof(lip); ++j )
			*pChar++ = buffer[bufCounter++];

		// Add the wrap box to the container
		wrapBoxes_.push_back(WrapBox(name, pluException, lengthLow, lengthHigh, widthLow, widthHigh, lip));
	}

	bool traceOn = Debug::isTracing("EvolWrapperPackageTimings.showHeightBox");

	// Read each wrap box
	iterator iter = wrapBoxes_.begin();
	for ( int i = 0; i < numWrapBoxes; ++i, ++iter )
	{
		// For each height box in each wrap box ...
		for ( int j = 0; j < wrapBoxHeights[i]; ++j )
		{
			UINT16 height;

			// Read the height box's height
			pChar = (char*)&height;
			for ( unsigned k = 0; k < sizeof(height); ++k )
				*pChar++ = buffer[bufCounter++];

			FpmManager* pSerialFlash = FpmManager::getInstance();
			int paramListLen = WrapBox::LAST_PARAMETER - 2;				// 1P by default with 19 params, but we carry 21 
			if (pSerialFlash->isWrapperTwoPrinter())
				paramListLen = WrapBox::LAST_PARAMETER;

			WrapBox::HeightBox heightBox(height);
			for ( int p = 0; p < paramListLen; ++p )
			{
				INT16 value;

				// Read each of the attributes
				pChar = (char*)&value;
#if 0	//	Keep out, unless it's really needed.
				if (traceOn && (p == 5 || p == 6))
				{
					char myBuf[sizeof(value) + 1];
					char* pBuffer = myBuf;
					for ( unsigned int b2 = 0; b2 < sizeof(value); ++b2 )
					{
						sprintf(pBuffer++, "%X", buffer[bufCounter + b2]);
					}
					*pBuffer = 0;
/*must port if used -- ats
					string msgDebug("Parameter ");
					msgDebug.append(inttostring(p));
					msgDebug.append(" = ").append(myBuf);
					Debug::trace("EvolWrapperPackageTimings.showHeightBox", msgDebug);
*/				
				}
#endif
				for ( unsigned int l = 0; l < sizeof(value); ++l )
				{
					*pChar++ = buffer[bufCounter++];
				}

				heightBox.setParameter(p, value);
			}

			if (traceOn)
			{
/*must port if used -- ats
				string msgDebug("*****  ");
				msgDebug.append("WrapBox ").append(inttostring(i));
				msgDebug.append(" -  Showing Height Box ").append(inttostring(j)).append("  *****");
				Debug::trace("EvolWrapperPackageTimings.showHeightBox", "Constructor passing in a const char* buffer:  " + msgDebug);
				heightBox.show("EvolWrapperPackageTimings.showHeightBox");
*/			
			}
			iter->insertHeightBox(heightBox);
		}
	}
}


EvolWrapperPackageTimings::EvolWrapperPackageTimings(EvolWrapperPackageTimings* ewpt)
	: WrapperGeneric(WrEvolPackageTimings), wrapBoxes_(), disposition(WRITE_TEMPORARY)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	if (!ewpt)
		return;

	if (Debug::isTracing("EvolWrapperPackageTimings"))
	{
/*must port if used -- ats
		HobString msgDebug = "Constructor passing in an EvolWrapperPackageTimings pointer to copy from: ";
		msgDebug.append(Hex::toString((long) ewpt));
		msgDebug.append("  this = ").append(Hex::toString((long) this));
		Debug::trace("EvolWrapperPackageTimings", msgDebug);
*/	
	}

	setDisposition(ewpt->getDisposition());

	int numWrapBoxes = ewpt->getNumberWrapBoxes();

	// Copy each wrap box
	for ( int i = 0; i < numWrapBoxes; ++i )
	{
		const WrapBox *myWB = ewpt->getWrapBox(i);
		if (myWB)
		{
			WrapBox newWrapBox(*myWB);
			insertWrapBox(newWrapBox, i);
		}
	}

	if (Debug::isTracing("EvolWrapperPackageTimings.show"))
	{
		Debug::trace("EvolWrapperPackageTimings.show", "START:  ****  Copy constructor  ****");
		show("EvolWrapperPackageTimings");
		Debug::trace("EvolWrapperPackageTimings.show", " END:   ****  Copy constructor  ****");
	}
}


EvolWrapperPackageTimings::~EvolWrapperPackageTimings()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
	Debug::trace("EvolWrapperPackageTimings", "Destructor:  this = " + Hex::toString((void*) this));
}

WrapBox* EvolWrapperPackageTimings::getWrapBox(int index)
{
	iterator iter = find_if(wrapBoxes_.begin(), wrapBoxes_.end(), bind2nd(isIndex(), index));
	if ( iter != wrapBoxes_.end() )
		return &*iter;

	return 0;
}


const WrapBox* EvolWrapperPackageTimings::getWrapBox(int index) const
{
	const_iterator iter = find_if(wrapBoxes_.begin(), wrapBoxes_.end(), bind2nd(isIndex(), index));
	if ( iter != wrapBoxes_.end() )
		return &*iter;

	return 0;
}

/*
const WrapBox* EvolWrapperPackageTimings::getWrapBox(const std::string& name) const
{
	int numWrapBoxes = wrapBoxes_.size();
	for ( int i = 0; i < numWrapBoxes; ++i )
	{
		if ( wrapBoxes_[i].getName() == name )
			return &wrapBoxes_[i];
	}

	return 0;
}
*/

void EvolWrapperPackageTimings::initDummy()
{
	static const char* names[18]                = { "10x14", "9S", "1S", "12S", "8N", "8W", "4S", "4D", "20S", "2S", "16S", "10S", "10L", "7S", "25", "OW", "ON1", "ON2" };
	static const unsigned int heightBoxes[18]   = {       4,    3,    8,     5,    6,    6,    6,    6,    10,   10,     7,     8,     5,    5,    3,    3,     3,     3 };
	static const unsigned int lengthLows[18]    = {      10,   20,   30,    40,   50,   60,   70,   80,    90,  100,   110,   120,   130,  140,  150,  160,   170,   180 };
	static const unsigned int lengthHighs[18]   = {      20,   30,   40,    50,   60,   70,   80,   90,   100,  110,   120,   130,   140,  150,  160,  170,   180,   190 };
	static const unsigned int widthLows[18]     = {      30,   40,   50,    60,   70,   80,   90,  100,   110,  120,   130,   140,   150,  160,  170,  180,   190,   200 };
	static const unsigned int widthHighs[18]    = {      40,   50,   60,    70,   80,   90,  100,  110,   120,  130,   140,   150,   160,  170,  180,  190,   200,   210 };
	static const unsigned int lipHeights[18]    = {      12,   15,   22,    25,   32,   35,   15,   22,    25,   32,    35,    42,    22,   25,   32,   35,    42,    45 };

	wrapBoxes_.clear();

	for ( int i = 0; i < 18; ++i )
	{
		int base = i * 1000;
		WrapBox wb(names[i], 0, lengthLows[i], lengthHighs[i],
				widthLows[i], widthHighs[i], lipHeights[i]);

		wb.initHeightBoxesDummy(heightBoxes[i], base);

		addWrapBox(wb);
	}
}


bool EvolWrapperPackageTimings::changePrecedence(int precedence, int delta)
{
	// Only do something if there is something to be done (delta != 0)
	if ( delta )
	{
		// Make sure the wrap box to be changed exists
		iterator iter = find_if(wrapBoxes_.begin(), wrapBoxes_.end(), bind2nd(isIndex(), precedence));
		if ( iter != wrapBoxes_.end() )
		{
			// Find the new position
			int i = 0;
			iterator iter2 = iter;
			if ( delta > 0 )
			{
				++iter2;
				for ( ; i < delta && iter2 != wrapBoxes_.end(); ++i, ++iter2 ) ;
			}
			else
				for ( ; i > delta && iter2 != wrapBoxes_.begin(); --i, --iter2 ) ;

			// If there a is a change to make, do it
			if ( i )
			{
				wrapBoxes_.insert(iter2, *iter);

				wrapBoxes_.erase(iter);

				return true;
			}
		}
	}

	return false;
}


bool EvolWrapperPackageTimings::insertWrapBox(const WrapBox& newWrapBox, int precedence)
{
	iterator iter = find_if(wrapBoxes_.begin(), wrapBoxes_.end(), bind2nd(isIndex(), precedence));
	if ( iter != wrapBoxes_.end() )
	{
		wrapBoxes_.insert(iter, newWrapBox);

		return true;
	}
	else
	{
		wrapBoxes_.push_back(newWrapBox);

		return true;
	}
}


bool EvolWrapperPackageTimings::updateWrapBox(const WrapBox& wrapBox, int precedence)
{
	iterator iter = find_if(wrapBoxes_.begin(), wrapBoxes_.end(), bind2nd(isIndex(), precedence));
	if ( iter != wrapBoxes_.end() )
	{
		*iter = wrapBox;

		return true;
	}

	return false;
}


void EvolWrapperPackageTimings::removeWrapBox(int precedence)
{
	iterator iter = find_if(wrapBoxes_.begin(), wrapBoxes_.end(), bind2nd(isIndex(), precedence));
	if ( iter != wrapBoxes_.end() )
		wrapBoxes_.erase(iter);
}


const char* EvolWrapperPackageTimings::serialize()const
{
	bool traceOn = Debug::isTracing("EvolWrapperPackageTimings.serialize");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "serialize()";
		Debug::trace("EvolWrapperPackageTimings.serialize", msgDebug);
		msgDebug.append(":   ");
	}

	WrapperGeneric::serialize();

	// Now, for the derived class items
	const char* pChar = (const char*)&disposition;
	for (unsigned int i = 0; i < sizeof(disposition); i++)
		serializedData.append(1, pChar[i]);

	// Write the number of wrap boxes to the buffer
	UINT16 numWrapBoxes = (UINT16)wrapBoxes_.size();
	if (traceOn)
	{
/*must port if used -- ats
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("numWrapBoxes = ").append(inttostring(numWrapBoxes));
		Debug::trace("EvolWrapperPackageTimings.serialize", msgDebug2);
*/	
	}
	pChar = (const char*)&numWrapBoxes;
	for (unsigned int i = 0; i < sizeof(numWrapBoxes); i++)
		serializedData.append(1, pChar[i]);

	if (traceOn)
	{
/*must port if used -- ats
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("current length = ").append(inttostring(EvolWrapperPackageTimings::getSerializedLength()));
		msgDebug2.append(". Begin serializing Wrap box Height box counts.");
		Debug::trace("EvolWrapperPackageTimings.serialize", msgDebug2);
	*/
	}
	// Write the number of height boxes for each wrap box
	const_iterator iter = wrapBoxes_.begin();
	while ( iter != wrapBoxes_.end() )
	{
		UINT16 numHeightBoxes = (UINT16)(iter++)->getNumberHeightBoxes();
		pChar = (const char*)&numHeightBoxes;
		for ( unsigned int j = 0; j < sizeof(numHeightBoxes); ++j )
			serializedData.append(1, pChar[j]);
	}
	if (traceOn)
	{
/*must port if used -- ats
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("current length = ").append(inttostring(EvolWrapperPackageTimings::getSerializedLength()));
		msgDebug2.append(". Begin serializing each Wrap box.");
		Debug::trace("EvolWrapperPackageTimings.serialize", msgDebug2);
*/	
	}

	//unsigned int boxNameLen = WrapBox::_MaxNameLength_ + 1;
	int count = 0;
	// Write each wrap box
	iter = wrapBoxes_.begin();
	while ( iter != wrapBoxes_.end() )
	{
		UINT16 dimension;
		UINT32 pluException;
		
		char boxName[(WrapBox::_MaxNameLength_ + 1)];
		const WrapBox& wrapBox = *(iter++);

		// Write the wrap box's name
		memset (boxName, 0, (WrapBox::_MaxNameLength_ + 1));
		strcpy(boxName, wrapBox.getName().c_str());
		pChar = boxName;
		for ( unsigned j = 0; j < (WrapBox::_MaxNameLength_ + 1); ++j )
			serializedData.append(1, pChar[j]);

		// Write the wrap box's PLU Exception
		pChar = (const char*)&pluException;
		pluException = wrapBox.getPLUException();
		for ( unsigned j = 0; j < sizeof(pluException); ++j )
			serializedData.append(1, pChar[j]);

		// Write the wrap box's lengthLow
		pChar = (const char*)&dimension;
		dimension = wrapBox.getLengthLow();
		for ( unsigned j = 0; j < sizeof(dimension); ++j )
			serializedData.append(1, pChar[j]);

		// Write the wrap box's lengthHigh
		dimension = wrapBox.getLengthHigh();
		for ( unsigned j = 0; j < sizeof(dimension); ++j )
			serializedData.append(1, pChar[j]);

		// Write the wrap box's widthLow
		dimension = wrapBox.getWidthLow();
		for ( unsigned j = 0; j < sizeof(dimension); ++j )
			serializedData.append(1, pChar[j]);

		// Write the wrap box's widthHigh
		dimension = wrapBox.getWidthHigh();
		for ( unsigned j = 0; j < sizeof(dimension); ++j )
			serializedData.append(1, pChar[j]);

		// Write the wrap box's lip
		dimension = wrapBox.getLip();
		for ( unsigned j = 0; j < sizeof(dimension); ++j )
			serializedData.append(1, pChar[j]);

		if (traceOn)
		{
/*must port if used -- ats
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("After wrap box # ").append(inttostring(++count));
			msgDebug2.append(", current length = ").append(inttostring(EvolWrapperPackageTimings::getSerializedLength()));
			Debug::trace("EvolWrapperPackageTimings.serialize", msgDebug2);
*/		
		}
	}
	if (traceOn)
	{
/*must port if used -- ats
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("current length = ").append(inttostring(EvolWrapperPackageTimings::getSerializedLength()));
		msgDebug2.append(". Begin serializing each Height box.");
		Debug::trace("EvolWrapperPackageTimings.serialize", msgDebug2);
*/	
	}

	// Write each height box
	count = 1;
	iter = wrapBoxes_.begin();
	while ( iter != wrapBoxes_.end() )
	{
		//int htCount = 0;
		const WrapBox& wrapBox = *(iter++);
		WrapBox::const_iterator iter = wrapBox.begin();

		// For each height box in each wrap box ...
		while ( iter != wrapBox.end() )
		{
			const WrapBox::HeightBox* heightBox = &*(iter++);
			UINT16 height = (UINT16)heightBox->getHeight();

			// Write the height box's height
			pChar = (const char*)&height;
			for ( unsigned k = 0; k < sizeof(height); ++k )
				serializedData.append(1, pChar[k]);

			FpmManager* pSerialFlash = FpmManager::getInstance();
			int paramListLen = WrapBox::LAST_PARAMETER - 2;				// 1P by default with 19 params, but we carry 21 
			if (pSerialFlash->isWrapperTwoPrinter())
				paramListLen = WrapBox::LAST_PARAMETER;

			INT16 value;
			pChar = (const char*)&value;
			for ( int p = 0; p < paramListLen; ++p )
			{
				value = (INT16)heightBox->getParameter(p);

				// Write each of the attributes
				for ( unsigned int l = 0; l < sizeof(value); ++l )
					serializedData.append(1, pChar[l]);
			}
			if (traceOn)
			{
/*must port if used -- ats
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("After wrap box # ").append(inttostring(count));
				msgDebug2.append(" and height box # ").append(inttostring(++htCount));
				msgDebug2.append(", current length = ").append(inttostring(EvolWrapperPackageTimings::getSerializedLength()));
				Debug::trace("EvolWrapperPackageTimings.serialize", msgDebug2);
*/			
			}
		}
		++count;
	}
	if (traceOn)
	{
/*must port if used -- ats
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("Serialize is done, current length = ").append(inttostring(EvolWrapperPackageTimings::getSerializedLength()));
		Debug::trace("EvolWrapperPackageTimings.serialize", msgDebug2);
*/	
	}
	return serializedData.data();
}

UINT32 EvolWrapperPackageTimings::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}


void EvolWrapperPackageTimings::show(const std::string& title) const
{
	if (Debug::isTracing("EvolWrapperPackageTimings.show"))
	{
		Debug::trace("EvolWrapperPackageTimings.show", "START:  " + title);
		Debug::trace("EvolWrapperPackageTimings.show", "disposition = " + HobString(Integer::toString(disposition)));
		Debug::trace("EvolWrapperPackageTimings.show", "wrap box count = " + HobString(Integer::toString((int)wrapBoxes_.size())));
		int i = 0;
		const_iterator iter = wrapBoxes_.begin();
		while ( iter != wrapBoxes_.end() )
		{
			Debug::trace("EvolWrapperPackageTimings.show", "Wrapbox " + HobString(Integer::toString(i++) )+ ":");
			(iter++)->show("EvolWrapperPackageTimings");
		}
		Debug::trace("EvolWrapperPackageTimings.show", " END:  " + title);
	}
}

UINT16 EvolWrapperPackageTimings::getDisposition() const
{
	return disposition;
}

void EvolWrapperPackageTimings::setDisposition(UINT16 value)
{
	disposition = value;
}

void EvolWrapperPackageTimings::update(const char* buffer, unsigned int version)
{
	unsigned short numWrapBoxes = 0, numHeightBoxes = 0;
	
	//unsigned short *pNumWrapBoxes = &numWrapBoxes;

	Debug::trace("EvolWrapperPackageTimings", "update()");
	bool traceOn2 = Debug::isTracing("EvolWrapperPackageTimings");

	if (traceOn2)
	{
/*must port if used -- ats
		HobString msgDebug;
		msgDebug.append("update:  first 20 chars of buffer: ").append(Hex::toString(buffer, 20));
		Debug::trace("EvolWrapperPackageTimings", msgDebug);
*/	
	}

	WrapperGeneric::serialize();
	int bufCounter = WrapperGeneric::getSerializedLength();

	//UINT16 numWrapBoxes = 0, numHeightBoxes = 0;

	// Now, for the derived class items
	// Read the number of wrap boxes in the buffer
	char* pChar = (char*)&disposition;
	for (unsigned int i = 0; i < sizeof(disposition); i++)
	{
		*pChar++ = buffer[bufCounter++];
	}
/*must port if used -- ats		

	if (traceOn2)
		Debug::trace("EvolWrapperPackageTimings", "disposition = " + inttostring(disposition));
*/
	pChar = (char*)&numWrapBoxes;
	
//	pChar = (char*)pNumWrapBoxes;
	for (unsigned int i = 0; i < sizeof(numWrapBoxes); i++)
	{
		*pChar++ = buffer[bufCounter++];
	}
/*must port if used -- ats	
	if (traceOn2)	
		Debug::trace("EvolWrapperPackageTimings", "numWrapBoxes = " + inttostring(numWrapBoxes));
*/
	// Read the number of height boxes for each wrap box
	std::vector<int> wrapBoxHeights(numWrapBoxes);
	for ( int i = 0; i < numWrapBoxes; ++i )
	{
		pChar = (char*)&numHeightBoxes;
		for ( unsigned int j = 0; j < sizeof(numHeightBoxes); ++j )
			*pChar++ = buffer[bufCounter++];

		wrapBoxHeights[i] = numHeightBoxes;
	}

	// Read each wrap box
	for ( int i = 0; i < numWrapBoxes; ++i )
	{
		char name[WrapBox::_MaxNameLength_ + 1];
		UINT32 pluException;
		UINT16 lengthLow, lengthHigh;
		UINT16 widthLow, widthHigh;
		UINT16 lip;

		// Read the wrap box's name
		pChar = name;
		for ( unsigned int j = 0; j < WrapBox::_MaxNameLength_ + 1; ++j )
			*pChar++ = buffer[bufCounter++];

		if (version == 0)
			pluException = 0;
		else
		{
			// Read the wrap box's PLU Exception
			pChar = (char*)&pluException;
			for ( unsigned j = 0; j < sizeof(pluException); ++j )
				*pChar++ = buffer[bufCounter++];
		}

		// Read the wrap box's lengthLow
		pChar = (char*)&lengthLow;
		for ( unsigned j = 0; j < sizeof(lengthLow); ++j )
			*pChar++ = buffer[bufCounter++];

		// Read the wrap box's lengthHigh
		pChar = (char*)&lengthHigh;
		for ( unsigned j = 0; j < sizeof(lengthHigh); ++j )
			*pChar++ = buffer[bufCounter++];

		// Read the wrap box's widthLow
		pChar = (char*)&widthLow;
		for ( unsigned j = 0; j < sizeof(widthLow); ++j )
			*pChar++ = buffer[bufCounter++];

		// Read the wrap box's widthHigh
		pChar = (char*)&widthHigh;
		for ( unsigned j = 0; j < sizeof(widthHigh); ++j )
			*pChar++ = buffer[bufCounter++];

		// Read the wrap box's lip
		pChar = (char*)&lip;
		for ( unsigned j = 0; j < sizeof(lip); ++j )
			*pChar++ = buffer[bufCounter++];

		// Add the wrap box to the container
		wrapBoxes_.push_back(WrapBox(name, pluException, lengthLow, lengthHigh, widthLow, widthHigh, lip));
	}

	bool traceOn = Debug::isTracing("EvolWrapperPackageTimings.showHeightBox");

	// Read each wrap box
	iterator iter = wrapBoxes_.begin();
	for ( int i = 0; i < numWrapBoxes; ++i, ++iter )
	{
		// For each height box in each wrap box ...
		for ( int j = 0; j < wrapBoxHeights[i]; ++j )
		{
			UINT16 height;

			// Read the height box's height
			pChar = (char*)&height;
			for ( unsigned k = 0; k < sizeof(height); ++k )
				*pChar++ = buffer[bufCounter++];

			FpmManager* pSerialFlash = FpmManager::getInstance();
			int paramListLen = WrapBox::LAST_PARAMETER - 2;				// 1P by default with 19 params, but we carry 21 
			if (pSerialFlash->isWrapperTwoPrinter())
				paramListLen = WrapBox::LAST_PARAMETER;

			WrapBox::HeightBox heightBox(height);
			for ( int p = 0; p < paramListLen; ++p )
			{
				INT16 value;

				// Read each of the attributes
				pChar = (char*)&value;
#if 0
				if (traceOn && (p == 5 || p == 6))
				{
					char myBuf[sizeof(value) + 1];
					char* pBuffer = myBuf;
					for ( unsigned int b2 = 0; b2 < sizeof(value); ++b2 )
					{
						sprintf(pBuffer++, "%X", buffer[bufCounter + b2]);
					}
					*pBuffer = 0;
/*must port if used -- ats
					string msgDebug("Parameter ");
					msgDebug.append(inttostring(p));
					msgDebug.append(" = ").append(myBuf);
					Debug::trace("EvolWrapperPackageTimings.showHeightBox", msgDebug);
*/				
				}
#endif
				for ( unsigned int l = 0; l < sizeof(value); ++l )
				{
					*pChar++ = buffer[bufCounter++];
				}

				heightBox.setParameter(p, value);
			}

			if (traceOn)
			{
/*must port if used -- ats				
				string msgDebug("*****  ");
				msgDebug.append("WrapBox ").append(inttostring(i));
				msgDebug.append(" -  Showing Height Box ").append(inttostring(j)).append("  *****");
				Debug::trace("EvolWrapperPackageTimings.showHeightBox", "Constructor passing in a const char* buffer:  " + msgDebug);
				heightBox.show("EvolWrapperPackageTimings.showHeightBox");
*/
			}
			iter->insertHeightBox(heightBox);
		}
	}
}
