#include "HfcCLASysInfo.h"
#include "HobString.h"
#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcCLASysInfo::_dataType_ = 0;
std::string HfcCLASysInfo::_dataTypeName_("HfcCLASysInfo"); 
#endif


HfcCLASysInfo::HfcCLASysInfo(const char *data) : CLASystemInfo(data)
{
	Debug::trace("HfcCLASysInfo", "HfcCLASysInfo(const char *data)");
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


HfcCLASysInfo::~HfcCLASysInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcCLASysInfo::serialize(int format)const
{
	HfcObject::serializedData = "";
	HobString result;
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;
			Debug::trace("HfcCLASysInfo", "serialize(BrowserFormat)");
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while (loopCnt < fieldOffsetCLASystemInfoEnd)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					/*	EEP93C56Board	*/
					case fieldOffsetCLAPartNumber:
					{
						//HfcObject::serializedData.append(HobString((const char *) getPartNumber(), PART_NUMBER_SIZE));
						//Debug::trace("HfcCLASysInfo", "PartNumber: " + HobString((const char *) getPartNumber(), PART_NUMBER_SIZE));
                        std::string partNum = getPartNumber();
                        std::string printablePartNum = "";
                        for( int i = 0; i < PART_NUMBER_SIZE; ++i ) {
                            if( (partNum.at(i) >= 48 && partNum.at(i) <= 57) || partNum.at(i) == 45 )//0-9 or '-'
                            {
                                printablePartNum.push_back( partNum.at(i) );
                            }
                            else
                            {
                                printablePartNum.push_back(' ');//pad it with a space if it isn't printable
                            }
                        }
						//HfcObject::serializedData.append(getPartNumber());
						//Debug::trace("HfcCLASysInfo", "PartNumber: " + HobString(getPartNumber()));
                        HfcObject::serializedData.append(printablePartNum);
                        Debug::trace("HfcCLASysInfo", "PartNumber: " + HobString(printablePartNum));
						break;
					}
					case fieldOffsetCLABoardRev:
					{
						//HfcObject::serializedData.append(HobString((const char *) getBoardRev(), REVISION_SIZE));
						//Debug::trace("HfcCLASysInfo", "BoardRev: " + HobString((const char *) getBoardRev(), REVISION_SIZE));
						HfcObject::serializedData.append(getBoardRev());
						Debug::trace("HfcCLASysInfo", "BoardRev: " + HobString(getBoardRev()));
						break;
					}
					case fieldOffsetCLADateOfManufacture:
					{
						//HfcObject::serializedData.append(HobString((const char *) getDateOfManufacture(), DATE_SIZE));
						//Debug::trace("HfcCLASysInfo", "DateOfManufacture: " + HobString((const char *) getDateOfManufacture(), DATE_SIZE));
						HobString dateOfManufacture = getDateOfManufacture();
						if (dateOfManufacture.at(2) != '/' && dateOfManufacture.at(5) != '/')
						{
							dateOfManufacture.insert(2, "/");	
							dateOfManufacture.insert(5, "/");
						}
						//HfcObject::serializedData.append(getDateOfManufacture());
						//Debug::trace("HfcCLASysInfo", "DateOfManufacture: " + HobString(getDateOfManufacture()));
						HfcObject::serializedData.append(dateOfManufacture);
						Debug::trace("HfcCLASysInfo", "DateOfManufacture: " + HobString(dateOfManufacture));
						break;
					}
					case fieldOffsetCLASerialNumber:
					{
						//HfcObject::serializedData.append(HobString((const char *) getSerialNumber(), SERIAL_NUMBER_SIZE));
						//Debug::trace("HfcCLASysInfo", "SerialNumber: " + HobString((const char *) getSerialNumber(), SERIAL_NUMBER_SIZE));
						HfcObject::serializedData.append(getSerialNumber());
						Debug::trace("HfcCLASysInfo", "SerialNumber: " + HobString(getSerialNumber()));
						break;
					}
					case fieldOffsetCLABoardSectionChecksum:
					{
						HfcObject::serializedData.append(Long::toString(getBoardSectionChecksum()));
						Debug::trace("HfcCLASysInfo", "BoardSectionChecksum: " + HobString(Long::toString(getBoardSectionChecksum())));
						break;
					}
					/*	EEP93C56AWS	*/
					case fieldOffsetCLAAwsSerialNumber:
					{
						//HfcObject::serializedData.append(HobString((const char *) getAwsSerialNumber(), AWS_SERIAL_NUMBER_SIZE));
						//Debug::trace("HfcCLASysInfo", "AwsSerialNumber: " + HobString((const char *) getAwsSerialNumber(), AWS_SERIAL_NUMBER_SIZE));
						HobString awsSerialNumber = getAwsSerialNumber();
						bool flag = true;
						for(unsigned int i = 0; i < AWS_SERIAL_NUMBER_SIZE; i++)
						{
							if(awsSerialNumber.at(i) != '0')
							{
								flag = false;
							}
						}
						if(flag)
						{
							//Empty Serial Number
							awsSerialNumber.replace(2,1,"-");
							awsSerialNumber.replace(7,1,"-");
						}
						//We can only input 9 characters into the serial number field and 2 dashes, so remove the last byte since we know its going to be '0'
						awsSerialNumber.erase(AWS_SERIAL_NUMBER_SIZE-1,1);
						//HfcObject::serializedData.append(getAwsSerialNumber());
						//Debug::trace("HfcCLASysInfo", "AwsSerialNumber: " + HobString(getAwsSerialNumber()));
						HfcObject::serializedData.append(awsSerialNumber);
						Debug::trace("HfcCLASysInfo", "AwsSerialNumber: " + HobString(awsSerialNumber));
						break;
					}
					case fieldOffsetCLAAwsDateOfManufacture:
					{
						//HfcObject::serializedData.append(HobString((const char *) getAwsDateOfManufacture(), DATE_SIZE));
						//Debug::trace("HfcCLASysInfo", "AwsDateOfManufacture: " + HobString((const char *) getAwsDateOfManufacture(), DATE_SIZE));
						HobString dateOfManufacture = getAwsDateOfManufacture();
						dateOfManufacture.insert(2, "/");	
						dateOfManufacture.insert(5, "/");
						//HfcObject::serializedData.append(getAwsDateOfManufacture());
						//Debug::trace("HfcCLASysInfo", "AwsDateOfManufacture: " + HobString(getAwsDateOfManufacture()));
						HfcObject::serializedData.append(dateOfManufacture);
						Debug::trace("HfcCLASysInfo", "AwsDateOfManufacture: " + HobString(dateOfManufacture));
						break;
					}
					case fieldOffsetCLAAwsDateOfInstall:
					{
						//HfcObject::serializedData.append(HobString((const char *) getAwsDateOfInstall(), DATE_SIZE));
						//Debug::trace("HfcCLASysInfo", "AwsDateOfInstall: " + HobString((const char *) getAwsDateOfInstall(), DATE_SIZE));
						HobString dateOfInstall = getAwsDateOfInstall();
						dateOfInstall.insert(2, "/");
						dateOfInstall.insert(5, "/");
						//HfcObject::serializedData.append(getAwsDateOfInstall());
						//Debug::trace("HfcCLASysInfo", "AwsDateOfInstall: " + HobString(getAwsDateOfInstall()));
						HfcObject::serializedData.append(dateOfInstall);
						Debug::trace("HfcCLASysInfo", "AwsDateOfInstall: " + HobString(dateOfInstall));
						break;
					}
					case fieldOffsetCLAAwsModelNumber:
					{
						HfcObject::serializedData.append(Integer::toString(getModelNumber()));
						Debug::trace("HfcCLASysInfo", "AwsModelNumber: " + HobString(Integer::toString(getModelNumber())));
						break;
					}
					case fieldOffsetCLACycleCount:
					{
						HfcObject::serializedData.append(Long::toString(getCycleCount()));
						Debug::trace("HfcCLASysInfo", "CycleCount: " + HobString(Long::toString(getCycleCount())));
						break;
					}
					case fieldOffsetCLAAwsSectionChecksum:
					{
						HfcObject::serializedData.append(Long::toString(getAwsSectionChecksum()));
						Debug::trace("HfcCLASysInfo", "AwsSectionChecksum: " + HobString(Long::toString(getAwsSectionChecksum())));
						break;
					}
				}
				loopCnt++;
			}
			break;
		}
		default:
			// to do - throw exception("invalid format");
			break;
	}
	return 	HfcObject::serializedData.c_str();
}
