#include "HfcEvolWrapperAllTrayTypes.h"



#include "HfcEvolWrapperPackageTimings.h"

#include "WrapBox.h"

#include "util/e_string/utilstring.h"
#include "Bool.h"



#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperAllTrayTypes::_dataType_ = 0;
std::string HfcEvolWrapperAllTrayTypes::_dataTypeName_("HfcEvolWrapperAllTrayTypes"); 
#endif


HfcEvolWrapperAllTrayTypes::HfcEvolWrapperAllTrayTypes(const HfcEvolWrapperPackageTimings& timings)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();

	int wrapBoxes = timings.getNumberWrapBoxes();
	names_.reserve(wrapBoxes);
	pluExceptions_.reserve(wrapBoxes);
	canChange_.reserve(wrapBoxes);
	for ( int i = 0; i < wrapBoxes; ++i )
	{
		const WrapBox* wrapBox = timings.getWrapBox(i);

		if ( wrapBox )
		{
			names_.push_back(wrapBox->getName());
			pluExceptions_.push_back(wrapBox->getPLUException());
		}
		else
		{
			names_.push_back("");
			pluExceptions_.push_back(0);
		}
		//	Can't change the name of or delete "Dflt Wide" or "Dflt Narr".
		//	You also can't change their order. I will also not allow them to assign PLU Exceptions either.
		if (i >= (wrapBoxes - 2))
			canChange_.push_back(false);
		else
			canChange_.push_back(true);
	}
}


HfcEvolWrapperAllTrayTypes::~HfcEvolWrapperAllTrayTypes()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


void HfcEvolWrapperAllTrayTypes::init()
{
	names_.clear();
	pluExceptions_.clear();
	canChange_.clear();
}


void HfcEvolWrapperAllTrayTypes::show() const
{
	bool isTracing = Debug::isTracing("HfcEvolWrapperAllTrayTypes");
	std::string debugString;

	if ( !isTracing )
		Debug::addTraceCategory("HfcEvolWrapperAllTrayTypes");

	
	for ( int i = 1; i < endHfcEvolWrTrayTypeFieldOffsets; ++i )
	{
		switch (i)
		{
			case fieldOffsetEvolWrAllTrayTypeNames:
			{
				int names = getNumberNames();
				debugString.append("# Names = ").append(util_string::inttostring(names));

				debugString.append(", Names: ");
				for ( int i = 0; i < names; ++i )
					debugString.append(names_[i]).append(", ");

				break;
			}
			case fieldOffsetEvolWrAllTrayTypePLUExceptions:
			{
				int pluExceptions = getNumberPLUExceptions();
				debugString.append("\n# PLU Exceptions ").append(util_string::inttostring(pluExceptions));

				debugString.append(", PLU Exceptions: ");
				for ( int i = 0; i < pluExceptions; ++i )
					debugString.append(util_string::inttostring(pluExceptions_[i])).append(", ");

				break;
			}
			case fieldOffsetEvolWrAllTrayTypeCanChange:
			{
				int canChangeCount = getNumberCanChanges();
				debugString.append("\n# of Can change ").append(util_string::inttostring(canChangeCount));

				debugString.append(", Can change: ");
				for ( int i = 0; i < canChangeCount; ++i )
					debugString.append(util_string::booltostring(canChange_[i])).append(", ");

				break;
			}
		}
	}

	Debug::trace("HfcEvolWrapperAllTrayTypes", debugString);

	if ( !isTracing )
		Debug::removeTraceCategory("HfcEvolWrapperAllTrayTypes");
}


const char* HfcEvolWrapperAllTrayTypes::serialize(int format)const
{
	Debug::trace("HfcEvolWrapperAllTrayTypes", "serialize()");

	serializedData.erase();
	switch ( format )
	{
		case BrowserFormat:
		{
			for ( int i = 1; i < endHfcEvolWrTrayTypeFieldOffsets; ++i )
			{
				if ( i > 1 )
					serializedData.append(BrowserFormatSeparator);

				switch (i)
				{
					case fieldOffsetEvolWrAllTrayTypeNames:
					{
						// Put out first the number of names
						int numNames = getNumberNames();
						serializedData.append(util_string::inttostring(numNames));
						HobString listContents;

						// Then, put out the names themselves
						for ( int i = 0; i < numNames; ++i )
						{
							if ( i )
								listContents.append(BrowserFormatSeparator);

							// The names probably need to be surrounded by quotes since there
							// may be quotes inside the names themselves
							HobString temp(names_[i]);
							temp.quoteForCSV();
							listContents.append(temp);
						}

						listContents.quoteForCSV();
						serializedData.append(BrowserFormatSeparator);
						serializedData.append(listContents);

						break;
					}
					case fieldOffsetEvolWrAllTrayTypePLUExceptions:
					{
						// Put out the number of PLU Exceptions
						int numPLUExceptions = getNumberPLUExceptions();
						serializedData.append(util_string::inttostring(numPLUExceptions));
						HobString listContents;

						// Then, put out the PLU exceptions themselves
						for ( int i = 0; i < numPLUExceptions; ++i )
						{
							if ( i )
								listContents.append(BrowserFormatSeparator);

							listContents.append(util_string::inttostring(pluExceptions_[i]));
						}

						listContents.quoteForCSV();
						serializedData.append(BrowserFormatSeparator);
						serializedData.append(listContents);

						break;
					}
					case fieldOffsetEvolWrAllTrayTypeCanChange:
					{
						// Put out the number of CanChange values
						int canChangeCount = getNumberCanChanges();
						serializedData.append(util_string::inttostring(canChangeCount));
						HobString listContents;

						// Then, put out the CanChange values themselves
						for ( int i = 0; i < canChangeCount; ++i )
						{
							if ( i )
								listContents.append(BrowserFormatSeparator);

							listContents.append(Bool::toString(canChange_[i]));
						}

						listContents.quoteForCSV();
						serializedData.append(BrowserFormatSeparator);
						serializedData.append(listContents);

						break;
					}
				}
			}

			break;
		}
		default:
			// todo - throw exception("invalid format");
			break;
	}

	Debug::trace("HfcEvolWrapperAllTrayTypes", "serialize():  " + serializedData);

	return serializedData.c_str();
}
