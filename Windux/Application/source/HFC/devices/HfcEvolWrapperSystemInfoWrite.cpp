#include "HfcEvolWrapperSystemInfoWrite.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperSystemInfoWrite::_dataType_ = 0;
std::string HfcEvolWrapperSystemInfoWrite::_dataTypeName_("HfcEvolWrapperSystemInfoWrite"); 
#endif

HfcEvolWrapperSystemInfoWrite::HfcEvolWrapperSystemInfoWrite(const char *data, int len, int formatType) : EvolWrapperSystemInfoWrite()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			
			HobString token;
			short loopCnt = 1;
			
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetWriteAwsModelNumber:
						 setModelNumber((EvolWrapperSystemInfo::EvolAWSModel)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetWriteAwsSerialNumber:
					{
						//	The serial number is received without formatting so we must format it to XX-XXXX-XXX.
						//	Pad zeros at the end if it is not long enough.
						if (token.length() < EvolWrapperSystemInfo::_SerialNumbSize_)
							token.append((EvolWrapperSystemInfo::_SerialNumbSize_ - token.length()), '0');
						if (token.length() == EvolWrapperSystemInfo::_SerialNumbSize_)
						{
							//	The Serial number has not been formatted yet, so we will do so.
							token.insert(2, "-");
							token.insert(7, "-");
						}
						setAwsSerialNumber(token);
						break;
					}
					case fieldOffsetWriteAwsDateOfManufacture:
					{
						processDate(token);
						 setAwsDateOfManufacture(token);
						break;
					}
					case fieldOffsetWriteAwsDateOfInstall:
					{
						processDate(token);
						setAwsDateOfInstall(token);
						break;
					}
					case fieldOffsetWriteSystemWrapperCycleCount:
						setAwsCycleCount(Integer::valueOf(token.c_str()));
						break;

					default:
						break;
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

HfcEvolWrapperSystemInfoWrite::~HfcEvolWrapperSystemInfoWrite()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperSystemInfoWrite::serialize(int format)const 
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case InternalFormat:
		{
			EvolWrapperSystemInfoWrite::serialize();
			HfcObject::serializedData = EvolWrapperSystemInfoWrite::serializedData;
			break;
		}
		default:
		break;
	}
	return HfcObject::serializedData.data();
}

void HfcEvolWrapperSystemInfoWrite::processDate(HobString &theDate)
{
	Debug::trace("HfcEvolWrapperSystemInfo", "processDate START:   theDate = " + theDate);
	//	The aws needs the date in a mmddyy format, without any other characters.
	//	We will strip any out.
	if (theDate.at(5) == '/')
		theDate.erase(5, 1);	
	if (theDate.at(2) == '/')
		theDate.erase(2, 1);	
}