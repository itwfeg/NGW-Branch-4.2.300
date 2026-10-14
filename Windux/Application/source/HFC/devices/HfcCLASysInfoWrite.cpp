/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcCLASysInfoWrite.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcCLASysInfoWrite::dataType = 0;
std::string HfcCLASysInfoWrite::dataTypeName("HfcCLASysInfoWrite"); 
#endif

HfcCLASysInfoWrite::HfcCLASysInfoWrite(const char *data, int len, int formatType)	: CLASystemInfoWrite()
{
	Debug::trace("HfcCLASysInfoWrite", "HfcCLASysInfoWrite::HfcCLASysInfoWrite(const char *data, int len, int formatType)");

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
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
					case fieldOffsetLASerialNumber:
					{
						//If the serial number is too short, pad the end with 0's
						if (token.length() < CLASystemInfoWrite::_serial_number_size_)
						{
							token.append((CLASystemInfoWrite::_serial_number_size_ - token.length()), '0');
						}
						//If the serial number is not formatted, insert dashes
						if(token.length() == CLASystemInfoWrite::_serial_number_size_)
						{
							token.insert(2,"-");
							token.insert(7,"-");
						}
						if(token.length() < AWS_SERIAL_NUMBER_SIZE)
						{
							token.append((AWS_SERIAL_NUMBER_SIZE - token.length()), '0');
						}
						setAwsSerialNumber(token.c_str());
						break;
					}
					case fieldOffsetDateOfManufacture:
						processDate(token);
						setAwsDateOfManufacture(token.c_str());
						break;
					case fieldOffsetDateOfInstall:
						processDate(token);
						setAwsDateOfInstall(token.c_str());
						break;
					case fieldOffsetModelNumber:
						setModelNumber((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetCycleCount:
						setCycleCount((ULONG)Long::valueOf(token.c_str()));
						break;
					case fieldOffsetSectionChecksum:
						setAwsSectionChecksum((ULONG)Long::valueOf(token.c_str()));
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

HfcCLASysInfoWrite::~HfcCLASysInfoWrite()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *HfcCLASysInfoWrite::serialize(int format)const 
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case InternalFormat:
		{
			CLASystemInfoWrite::serialize();
			HfcObject::serializedData = CLASystemInfoWrite::serializedData;
			break;
		}
		default:
		break;
	}
	return HfcObject::serializedData.data();
}

void HfcCLASysInfoWrite::processDate(HobString & date)
{
	Debug::trace("HfcCLASysInfoWrite", "processDate START:   date = " + date);
	//Remove the '/' from the date since we only have 6 bytes for mmddyy
	if (date.at(5) == '/')
		date.erase(5, 1);	
	if (date.at(2) == '/')
		date.erase(2, 1);	
}
