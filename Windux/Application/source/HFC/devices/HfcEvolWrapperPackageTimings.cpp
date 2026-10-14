#include "HfcEvolWrapperPackageTimings.h"

#include "HfcEvolWrapperWrapBox.h"
#include "HfcEvolWrapperHeightBox.h"
#include "WrapperTags.h"
#include "CMReaderWriterEvolTag.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "Integer.h"
#include "Hex.h"

#include "util/e_string/utilstring.h"



#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperPackageTimings::_dataType_ = 0;
std::string HfcEvolWrapperPackageTimings::_dataTypeName_("HfcEvolWrapperPackageTimings"); 
#endif

std::list<HfcEvolWrapperPkgTimingsOffsets> HfcEvolWrapperPackageTimings::fieldList;


HfcEvolWrapperPackageTimings::HfcEvolWrapperPackageTimings()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	initFieldList();
}


HfcEvolWrapperPackageTimings::HfcEvolWrapperPackageTimings(const char* buffer)
	: EvolWrapperPackageTimings(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	initFieldList();
}


HfcEvolWrapperPackageTimings::HfcEvolWrapperPackageTimings(EvolWrapperPackageTimings* ewpt)
	: EvolWrapperPackageTimings(ewpt)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	initFieldList();
}


HfcEvolWrapperPackageTimings::HfcEvolWrapperPackageTimings(const char* data, int len, int formatType)
	: EvolWrapperPackageTimings()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	bool traceOn = Debug::isTracing("HfcEvolWrapperPackageTimings");
	if (traceOn)
	{
		HobString msgDebug;
		msgDebug.append("Constructor passing in const char* data, int len = ").append(Integer::toString(len));
		msgDebug.append(", int formatType = ").append(Integer::toString(formatType));
		msgDebug.append("    first 20 chars of data: ").append(Hex::toString(data, 20));
		Debug::trace("HfcEvolWrapperPackageTimings", msgDebug);
	}

	initFieldList();

	switch (formatType)
	{
		case EvolutionTaggedFormat:
		{
			WrapperTags wrapperTags;			

			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";
			unsigned int version = 0;			//	Assume no version tag in this data.

			//	Only Version 0 contains the WrapperGeneric object in the nibbilized data.
			const char *wgMsgBuf = WrapperGeneric::serialize();
			int wgMsgLength = WrapperGeneric::getSerializedLength();

			int offset = 0;
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
			if (token)
			{
				while (tok.next(token) != -1)
				{
					temp = token->getTag();
					offset = (temp.size()) ? wrapperTags.getOffset(temp) : 0;
					if ( traceOn )
						Debug::trace("HfcEvolWrapperPackageTimings", "tag = " + temp + "  offset = " + util_string::inttostring(offset));
					if (offset)
					{
						temp = token->getData();
						switch (offset)
						{
							case fieldOffsetEvolWrPkgTimingsVersion:			//	Version number
								if ( traceOn )
									Debug::trace("HfcEvolWrapperPackageTimings", "Version number");
								//version = stringtoint(temp); -- removed ats
								version = util_string::stringtoint(temp); 
								break;

							case fieldOffsetEvolWrPkgTimingsTimings:			//	Nibbilized wrapper timings data
							{
								if ( traceOn )
									Debug::trace("HfcEvolWrapperPackageTimings", "Wrapper timings data");

								HobString unnibbData("");
								HobString nibbData = token->getData();
								unsigned int nibbLength = (unsigned int) nibbData.size();
								unsigned int buffLength = nibbLength/2 + 1;

								if (version > 0)
								{
									//	We need to add the size of the WrapperGeneric object, which was not backed up.
									buffLength += wgMsgLength;
								}
								if ( traceOn )
								{
									HobString msgDebug = "nibbLength = ";
									msgDebug.append(util_string::inttostring(nibbLength));
									msgDebug.append("  unnibbBuffer length = ").append(util_string::inttostring(buffLength));
									Debug::trace("HfcEvolWrapperPackageTimings", msgDebug);
								}

								char *unnibbBuffer = new char[buffLength];
								char *unnibbPtr = unnibbBuffer;
								HobString oneByte;
								int unnibbDataLength = 0;

								if ((version > 0) && wgMsgLength)
								{
									//	Copy the WrapperGeneric::serialize() data to the unnibbBuffer.
									memcpy(unnibbBuffer, wgMsgBuf, wgMsgLength);
									//	Offset the pointer and counter to account for that data.
									unnibbPtr += wgMsgLength;
									unnibbDataLength += wgMsgLength;
								}

								for (unsigned int i = 0; i < nibbData.size(); i+=2)
								{
									oneByte = nibbData.substr(i,2);
									char val = (char)Hex::valueOf(oneByte.c_str());
									*(unnibbPtr++) = val;
									//unnibbData.append(&val, 1);
									unnibbDataLength++;
								}
								if ( traceOn )
								{
									Debug::trace("HfcEvolWrapperPackageTimings", "unnibbDataLength = " + util_string::inttostring(unnibbDataLength) + " calling update()");
								}
								update(unnibbBuffer, version);

								delete [] unnibbBuffer;
								break;
							}
						}
					}
				}
				delete token;
			}
			show("EvolWrapperPackageTimings.show");
			break;
		}

		default:
			Debug::trace("HfcEvolWrapperPackageTimings", "Constructor (Invalid format!!!)");
			// todo - throw exception("invalid format");
			break;
	}
}


HfcEvolWrapperPackageTimings::~HfcEvolWrapperPackageTimings()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


// static
void HfcEvolWrapperPackageTimings::initFieldList()
{
	if (fieldList.empty())
	{
		fieldList.push_back(fieldOffsetEvolWrPkgTimingsVersion);
		fieldList.push_back(fieldOffsetEvolWrPkgTimingsTimings);

		WrapperTags wrapperTags((FieldOffsetsList *)&fieldList);
	}
}

bool HfcEvolWrapperPackageTimings::updateTrayType(int precedence, const std::string& name, int exception)
{
	bool dataChanged = false;
	WrapBox* wrapBox = getWrapBox(precedence);
	if ( wrapBox )
	{
		bool debugging = Debug::isTracing("HfcEvolWrapperPackageTimings");

		if ( debugging )
		{
			Debug::trace("HfcEvolWrapperPackageTimings", "updateTrayType(): Updating wrapbox " + util_string::inttostring(precedence) +
					" tray type with name '" + name + "' and PLU exception " + util_string::inttostring(exception));
		}

		if (wrapBox->getName() != name)
		{
			wrapBox->setName(name);
			dataChanged = true;
		}
		if (wrapBox->getPLUException() != exception)
		{
			wrapBox->setPLUException(exception);
			dataChanged = true;
		}
	}
	else
		Debug::trace("HfcEvolWrapperPackageTimings", "updateTrayType: unable to find the wrap box");

	return dataChanged;
}


bool HfcEvolWrapperPackageTimings::updateWrapBox(const HfcEvolWrapperWrapBox& wrapBoxMessage)
{
	bool hasChanged = false;
	WrapBox* wrapBox = getWrapBox(wrapBoxMessage.getPrecedence());

	if ( wrapBox )
	{
		bool debugging = Debug::isTracing("HfcEvolWrapperPackageTimings");

		if ( debugging )
		{
			Debug::trace("HfcEvolWrapperPackageTimings", "updateWrapBox(): Updating wrapbox " +
					util_string::inttostring(wrapBoxMessage.getPrecedence()) + " with:");
			wrapBoxMessage.show();
		}

		hasChanged = wrapBoxMessage.update(*wrapBox);

		if ( debugging )
		{
			Debug::trace("HfcEvolWrapperPackageTimings", "updateWrapBox(): Wrapbox " +
					util_string::inttostring(wrapBoxMessage.getPrecedence()) + " was updated:");
			wrapBox->show("HfcEvolWrapperPackageTimings");
		}
	}
	else
		Debug::trace("HfcEvolWrapperPackageTimings", "updateWrapBox: unable to find the wrap box");

	return hasChanged;
}


bool HfcEvolWrapperPackageTimings::updateHeightBox(const HfcEvolWrapperHeightBox& heightBoxMessage)
{
	bool debugging = Debug::isTracing("HfcEvolWrapperPackageTimings");
	HobString msgDebug = "updateHeightBox() ";

	bool hasChanged = false;

	if (!heightBoxMessage.isReceived(HfcEvolWrapperHeightBox::fieldOffsetEvolWrHeightBoxPrecedence))
	{
		//if (debugging)
		//	Debug::trace("HfcEvolWrapperPackageTimings", msgDebug + "We did not receive a HeightBoxPrecedence. Exiting!");
		return hasChanged;
	}
	WrapBox* wrapBox = getWrapBox(heightBoxMessage.getPrecedence());

	if ( wrapBox )
	{
		if (!heightBoxMessage.isReceived(HfcEvolWrapperHeightBox::fieldOffsetEvolWrHeightBoxHeight))
		{
			//if (debugging)
			//	Debug::trace("HfcEvolWrapperPackageTimings", msgDebug + "We did not receive a HeightBoxHeight. Exiting!");
			return hasChanged;
		}
		WrapBox::HeightBox* heightBox = wrapBox->getHeightBox(heightBoxMessage.getHeight());
		if ( heightBox )
		{
			if ( Debug::isTracing("HfcEvolWrapperPackageTimings") )
			{
				Debug::trace("HfcEvolWrapperPackageTimings", "Updating wrapbox " +
						util_string::inttostring(heightBoxMessage.getPrecedence()) + "'s height box " + 
						util_string::inttostring(heightBoxMessage.getHeight()) + " with:");
				heightBoxMessage.show();
				Debug::trace("HfcEvolWrapperPackageTimings", "================");
			}

			hasChanged = heightBoxMessage.update(*heightBox);
		}
		else
			Debug::trace("HfcEvolWrapperPackageTimings", "updateWrapBox: unable to find the height box");
	}
	else
		Debug::trace("HfcEvolWrapperPackageTimings", "updateWrapBox: unable to find the wrap box");

	return hasChanged;
}

const char *HfcEvolWrapperPackageTimings::serialize(int format)const
{
	bool traceOn = Debug::isTracing("HfcEvolWrapperPackageTimings");
	if (traceOn)
		Debug::trace("HfcEvolWrapperPackageTimings", "serialize()");
	HobString msgDebug;
	HfcObject::serializedData = "";
	switch (format)
	{
		case InternalFormat:
		{
			if (traceOn)
				Debug::trace("HfcEvolWrapperPackageTimings", "serialize(InternalFormat)");
			EvolWrapperPackageTimings::serialize();
			EvolWrapperPackageTimings::getSerializedLength();
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("(InternalFormat) Total bytes to send = ").append(Integer::toString(EvolWrapperPackageTimings::getSerializedLength()));
				Debug::trace("HfcEvolWrapperPackageTimings", msgDebug2);
			}
			HfcObject::serializedData = EvolWrapperPackageTimings::serializedData;
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("(InternalFormat)2 Total bytes to send = ").append(Integer::toString((int) HfcObject::serializedData.length()));
				Debug::trace("HfcEvolWrapperPackageTimings", msgDebug2);
			}
		}
		break;

		case EvolutionTaggedFormat:
		{
			if (traceOn)
			{
				msgDebug = "serialize(EvolutionTaggedFormat):  ";
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("eventType = ").append(Integer::toString(getEventType()));
				Debug::trace("HfcEvolWrapperPackageTimings", msgDebug2);
			}
			WrapperTags wrapperTags;
			HobString tag = "";
			HobString fieldData = "";

			HfcObject::serializedData = CMReaderWriterEvolTag::EVOL_TAGGED_IDENTIFIER;
			
			char rtData[10];
			CMReaderWriterEvolTag::determineRecordType(rtData, getDeviceType(), WrEvolBackupPackageTimings);
			HfcObject::serializedData.append(rtData);
			HfcObject::serializedData.append(HobString(1, CMReaderWriterEvolTag::UNIT_SEPARATOR));

			//	We don't want to nibbilize the EvolMessage because it could eventually change and thus would mess up older data.
			WrapperGeneric::serialize();
			int evolMsgLength = WrapperGeneric::getSerializedLength();

			std::list<HfcEvolWrapperPkgTimingsOffsets>::iterator fieldIter = fieldList.begin();
			while (fieldIter != fieldList.end())
			{
				tag = wrapperTags.getTag(*fieldIter);
				switch (*fieldIter)
				{
					case fieldOffsetEvolWrPkgTimingsVersion:			//	Version number
						fieldData =	"1";
						break;

					case fieldOffsetEvolWrPkgTimingsTimings:			//	Nibbilized wrapper timings data
					{
						const char* serializedBase = EvolWrapperPackageTimings::serialize();
						int serializedLength = EvolWrapperPackageTimings::getSerializedLength();
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							//msgDebug2.append("Initially, serializedBase = ").append(Hex::toString((long) serializedBase));
							//msgDebug2.append(" and serializedLength = ").append(Integer::toString(serializedLength));
							Debug::trace("HfcEvolWrapperPackageTimings", msgDebug2);
						}
						serializedBase += evolMsgLength;
						serializedLength -= evolMsgLength;
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("After adjusting for the EvolMessage, serializedBase = ").append(Hex::toString((long long) serializedBase));
							msgDebug2.append(" and serializedLength = ").append(Integer::toString(serializedLength));
							Debug::trace("HfcEvolWrapperPackageTimings", msgDebug2);
						}

						Hex::nibblize(fieldData, serializedBase, serializedLength, Hex::DONT_APPEND);
						if (traceOn)
						{
							unsigned int origLen = (unsigned int) fieldData.length(), currPos = 0, maxLen, fixedBytes = 375;
							HobString msgDebug2 = msgDebug;

							msgDebug2.append("nibblized Length = ").append(Integer::toString(origLen));
							Debug::trace("HfcEvolWrapperPackageTimings", msgDebug2);
							while (currPos < origLen)
							{
								if ((currPos + fixedBytes) > origLen)
									maxLen = origLen - currPos;
								else
									maxLen = fixedBytes;

								HobString msgDebug3 = msgDebug;
								msgDebug3.append("bytes = ").append(Integer::toString(maxLen));
								msgDebug3.append(", nibblized data at ").append(Integer::toString(currPos));
								msgDebug3.append(" is:\n").append(Hex::toString(&fieldData[currPos], maxLen));
								Debug::trace("HfcEvolWrapperPackageTimings", msgDebug3);
								currPos += fixedBytes;
							}
						}
						break;
					}
					default:
					{
						tag = "";
						fieldData = "";
						break;
					}
				}
				if (tag.size())
				{
					HfcObject::serializedData.append(tag);
					HfcObject::serializedData.append(fieldData);
					HfcObject::serializedData.append(HobString(1, CMReaderWriterEvolTag::UNIT_SEPARATOR));
				}
				fieldIter++;
			}
			break;
		}

		default:
			Debug::trace("HfcEvolWrapperPackageTimings", "serialize(Invalid format!!!)");
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.c_str();
}

bool HfcEvolWrapperPackageTimings::changeTrayTypePrecedence(int precedence, int delta)
{
	bool hasChanged = false;
	int maxPrecedence = getNumberWrapBoxes() - 2;

	//	The default settings will always be the last 2 in the list.
	//	Do not allow their precedence to change!
	if ((precedence < maxPrecedence) && ((precedence + delta) < maxPrecedence))
		hasChanged = changePrecedence(precedence, delta);
 	return hasChanged;
}

bool HfcEvolWrapperPackageTimings::updateHeightBoxParam(const HfcEvolWrapperHeightBoxParam& heightBoxParam)
{
	bool debugging = Debug::isTracing("HfcEvolWrapperPackageTimings");
	HobString msgDebug;
	bool hasChanged = false;
	WrapBox* wrapBox = getWrapBox(heightBoxParam.getPrecedence());

	if (debugging)
		msgDebug = "updateHeightBoxParam():  ";

	if ( wrapBox )
	{
		if ( debugging )
		{
			Debug::trace("HfcEvolWrapperPackageTimings", msgDebug + "Showing the current wrap box:");
			wrapBox->show();
		}

		WrapBox::HeightBox* heightBox = wrapBox->getHeightBox(heightBoxParam.getHeight());
		if ( heightBox )
		{
			int oldValue = heightBox->getParameter(heightBoxParam.getAttribute());
			int newValue = heightBoxParam.getAttributeValue();

			if (debugging)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Attribute: ").append(Integer::toString(heightBoxParam.getAttribute()));
				msgDebug2.append("  oldValue = ").append(Integer::toString(oldValue));
				msgDebug2.append("  newValue = ").append(Integer::toString(newValue));
				Debug::trace("HfcEvolWrapperPackageTimings", msgDebug2);
			}
			if (oldValue == newValue)
			{
				if (debugging)
					Debug::trace("HfcEvolWrapperPackageTimings", msgDebug + "The values are the same!");
			}
			else
			{
				heightBox->setParameter(heightBoxParam.getAttribute(), newValue);
				hasChanged = true;
				if (debugging)
					Debug::trace("HfcEvolWrapperPackageTimings", msgDebug + "Updated the attribute with the new value.");
			}

		}
		else
			Debug::trace("HfcEvolWrapperPackageTimings", msgDebug + "Unable to find the heightBox!!!");

		if ( debugging )
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("Wrapbox: ").append(Integer::toString(heightBoxParam.getPrecedence())).append(" was updated.");
			Debug::trace("HfcEvolWrapperPackageTimings", msgDebug2);

			Debug::trace("HfcEvolWrapperPackageTimings", msgDebug + "Showing the updated wrap box:");
			wrapBox->show();
		}
	}
	else
	{
		if (debugging)
			Debug::trace("HfcEvolWrapperPackageTimings", msgDebug + "Unable to find the wrapBox!");
	}

	return hasChanged;
}
