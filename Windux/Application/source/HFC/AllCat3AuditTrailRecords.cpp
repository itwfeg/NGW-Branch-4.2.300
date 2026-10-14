/* HT-Series Scale Project
 * COPYRIGHT (c) 2016.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AllCat3AuditTrailRecords.h"
#include "Cat3AuditTrailRecord.h"
#include "WeigherCat3PageRequest.h"
#include "WeigherWriteCat3Record.h"
#include "HobList.h"
#include "HobDateTime.h"

#include "MultiLanguageText.h"
#include "filesys/file.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

#include "Integer.h"
#include "ULong.h"
#include "util/e_string/utilstring.h"


using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllCat3AuditTrailRecords::dataType = 0;
std::string AllCat3AuditTrailRecords::dataTypeName("AllCat3AuditTrailRecords"); 
#endif



AllCat3AuditTrailRecords::AllCat3AuditTrailRecords()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

AllCat3AuditTrailRecords::~AllCat3AuditTrailRecords()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	Debug::trace("cat3.Delete", string("    AllCat3AuditTrailRecords: ") + util_string::ptrtostring(this));

	// Make sure the memory for each item in the list is deleted, destructor called for each object.
	const size_t cnt = cat3AuditTrailRecords.size();
	if ( cnt > 0 )
	{
		for (size_t i = 0; i < cnt; i++)
		{
			if (cat3AuditTrailRecords[i])
			{
				delete cat3AuditTrailRecords[i];
				cat3AuditTrailRecords[i] = 0;
			}
		}
		cat3AuditTrailRecords.clear();
	}
}


const char *AllCat3AuditTrailRecords::serialize(int format) const
{
	serializedData = "";

	HobList *auditEventList    = new HobList();
	HobList *auditTypeList     = new HobList();
	HobList *parameterIdList   = new HobList();
	HobList *dateTimeList      = new HobList();
	HobList *newValueList      = new HobList();
	HobList *previousValueList = new HobList();

	// Determine the starting offset into the cat3AuditTrailRecords vector to fill the lists to send to the UI.
	// The oldest entry is the first in the list.  If there are more than 1000 audit trail records, we only need
	// to display the most-recent 1000 records, so we have to find out where the starting record is in the cat3AuditTrailRecords vector.
	int recCount = getRecordCount();
	Debug::trace("cat3", string("serialize - Number of records in current audit trail list is: ") + Integer::toString(recCount) );
	int startingOffset = recCount - CAT3_MAX_VIEWABLE_AUDIT_RECORDS;
	if (startingOffset < 0)
		startingOffset = 0;
	if (recCount > CAT3_MAX_VIEWABLE_AUDIT_RECORDS)
		recCount = CAT3_MAX_VIEWABLE_AUDIT_RECORDS;
	int endingOffset = startingOffset + recCount;

	Debug::trace("cat3", string("serialize - starting offset: ") + Integer::toString(startingOffset) + string(" - ending offset: ") + Integer::toString(endingOffset) + string(" - recCount: ") + Integer::toString(recCount));
	// Fill the lists that we will send to the UI with the latest audit records, up to 1000 max.
	for (int i = startingOffset; i < endingOffset; i++)
	{
		auditEventList->push_back   ( Integer::toString( cat3AuditTrailRecords[i]->getAuditEvent() ) );
		auditTypeList->push_back    ( Integer::toString( cat3AuditTrailRecords[i]->getAuditType() ) );
		SealableParamID param = cat3AuditTrailRecords[i]->getParameterId();
		parameterIdList->push_back  ( Integer::toString( (int) param ) );

		time_t epochTime = cat3AuditTrailRecords[i]->getEpochTime();
		struct tm * timeinfo = localtime (&epochTime);
		char buffer [80];
		strftime (buffer,80,"%Y-%m-%d %H:%M",timeinfo);
		dateTimeList->push_back( buffer );

		string valStr;
		formatWeigherAuditValue( valStr, param, cat3AuditTrailRecords[i]->getNewValue() );
		newValueList->push_back( valStr );

		valStr = "";
		formatWeigherAuditValue( valStr, param, cat3AuditTrailRecords[i]->getPreviousValue() );
		previousValueList->push_back( valStr );
	}

	Debug::trace("cat3", string("serialize - done serializing records"));
	switch (format)
	{
		case BrowserFormat:
			if ( auditEventList->size() )
			{
				serializedData.append(auditEventList->serialize(format));
				serializedData.append(BrowserFormatSeparator);

				serializedData.append(auditTypeList->serialize(format));
				serializedData.append(BrowserFormatSeparator);

				serializedData.append(parameterIdList->serialize(format));
				serializedData.append(BrowserFormatSeparator);

				serializedData.append(dateTimeList->serialize(format));
				serializedData.append(BrowserFormatSeparator);

				serializedData.append(newValueList->serialize(format));
				serializedData.append(BrowserFormatSeparator);

				serializedData.append(previousValueList->serialize(format));
			}
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}

	if (auditEventList)
		delete auditEventList;
	if (auditTypeList)
		delete auditTypeList;
	if (parameterIdList)
		delete parameterIdList;
	if (dateTimeList)
		delete dateTimeList;
	if (newValueList)
		delete newValueList;
	if (previousValueList)
		delete previousValueList;

	return serializedData.c_str();
}

void AllCat3AuditTrailRecords::addCat3AuditTrailRecord(Cat3AuditTrailRecord *rec)
{
	if (rec)
	{
		SealableParamID param = rec->getParameterId();
		if ( param == FRACTIONAL_PRICING || param == POUNDS_FOR_MULTIPLIER )
			rec->setAuditevent(CONFIGURATION);

		Debug::trace("cat3", string(" Adding audit trail record to the set - time: ") + ULong::toString(rec->getEpochTime()) + string("  param: ") + Integer::toString(rec->getParameterId()) );
		rec->show();

		cat3AuditTrailRecords.push_back(rec);
	}
}

int AllCat3AuditTrailRecords::getRecordCount() const
{
	const size_t count = cat3AuditTrailRecords.size();
	Debug::trace("cat3", string(" Number of records in current audit trail list is: ") + Integer::toString(count) );
	return (int) count;
}

void AllCat3AuditTrailRecords::createReportString(std::string &reportString, bool reportEntireAuditTrail)
{
	int startingOffset = getRecordCount() - 1;
	int endingOffset = 0;
	if ( !reportEntireAuditTrail )
	{
		// Determine the starting offset into the cat3AuditTrailRecords vector.
		// The newest entry is the first in the list.  If there are more than 1000 audit trail records, we only need
		// to export the most-recent 1000 records, so we have to find out where the ending record is in the cat3AuditTrailRecords vector.

		// The starting offset for exporting is different from viewing the audit trail on the UI because we want to write out
		// the most recent log entry first, so the oldest entry is at the end of the file.
		endingOffset = startingOffset - CAT3_MAX_VIEWABLE_AUDIT_RECORDS + 1;
		if (endingOffset < 0)
			endingOffset = 0;
	}

	Debug::trace("cat3", string("createReportString - starting offset: ") + Integer::toString(startingOffset) + string(" - endingOffset: ") + Integer::toString(endingOffset));

	SealableParamID param = SERVICE_SWITCH_PRESSED;
	reportString = "";
	// Write out each audit record to the report.
	for (int i = startingOffset; i >= endingOffset; i--)
	{
		{
		std::ostringstream stringStream;
		stringStream.width(4);
		stringStream.flags ( std::ios::right | std::ios::dec );
		stringStream << ( i - endingOffset );
		reportString += stringStream.str() + "   ";
		}
		//reportString += Integer::toString( i - endingOffset ) + " ";

		param = cat3AuditTrailRecords[i]->getParameterId();
		{
		std::ostringstream stringStream;
		stringStream.width(2);
		stringStream.flags ( std::ios::right | std::ios::dec );
		stringStream << (int) cat3AuditTrailRecords[i]->getParameterId();
		reportString += stringStream.str() + "  ";
		}
		//reportString += Integer::toString( (int) cat3AuditTrailRecords[i]->getParameterId() ) + " ";

		time_t epochTime = cat3AuditTrailRecords[i]->getEpochTime();
		struct tm * timeinfo = localtime (&epochTime);
		char buffer [80];
		strftime (buffer,80,"%Y-%m-%d %H:%M",timeinfo);
		reportString += string(buffer) + " ";

		{
		std::ostringstream stringStream;
		stringStream.width(11);
		stringStream.flags ( std::ios::right );
		string valStr;
		formatWeigherAuditValue( valStr, param, cat3AuditTrailRecords[i]->getNewValue() );
		stringStream << valStr + '\n';
		reportString += stringStream.str();
		}
	}
}

void AllCat3AuditTrailRecords::saveAuditTrailToDisk(std::string filename, bool exportEntireAuditTrail)
{
	if ( filename.empty() )
		// No output filename specified, then leave.
		return;

	Debug::trace("cat3", string("saveAuditTrailToDisk - filename passed in: ") + filename);
	util_filesystem::File outputFile(filename);
	Debug::trace("cat3", string("saveAuditTrailToDisk - File object's full path: ") + outputFile.getFullPath());
	if (outputFile.exists())
	{
		Debug::trace("cat3", "saveAuditTrailToDisk - output file exists so deleting it up front ");
		outputFile.remove();
	}

	int startingOffset = getRecordCount() - 1;
	int endingOffset = 0;
	if ( !exportEntireAuditTrail )
	{
		// Determine the starting offset into the cat3AuditTrailRecords vector.
		// The newest entry is the first in the list.  If there are more than 1000 audit trail records, we only need
		// to export the most-recent 1000 records, so we have to find out where the ending record is in the cat3AuditTrailRecords vector.

		// The starting offset for exporting is different from viewing the audit trail on the UI because we want to write out
		// the most recent log entry first, so the oldest entry is at the end of the file.
		endingOffset = startingOffset - CAT3_MAX_VIEWABLE_AUDIT_RECORDS + 1;
		if (endingOffset < 0)
			endingOffset = 0;
	}

	Debug::trace("cat3", string("saveAuditTrailToDisk - starting offset: ") + Integer::toString(startingOffset) + string(" - endingOffset: ") + Integer::toString(endingOffset));

	// Note:  We will append to the file, in case this method is used to export multiple Cut Tests
	//File output(outputFilename);
	std::ofstream output(filename.c_str(), std::ios::out|std::ios::app);

	Debug::trace("cat3", "saveAuditTrailToDisk - after opening output file ");

	hfc::MultiLanguageText internationalize;

	std::string header = internationalize("WeigherAuditTrail.DiskHeader");
	output << header << endl;

	std::string auditTypeNameIndicator[] = { "local","remote","unknown" };
	int unknownType = 2;
	std::string auditEventNameIndicator[] = { "calibration","configuration","upgrade","unknown" };
	int unknownEvent = 3;
	std::string paramNameIndicator[]  = { "com","scaleFactor","maxWeight","gainFactor","zeroReference","calGainCoeff","calOffsetCoeff",
	                                      "filterSpeed","minWeightToPrint","model","divisionSize","software","gravityConstant",
	                                      "fractionEnabled","poundsForEnabled","switchPressed","mode",
										  "decimalSeparator","currencySymbolPlacement","totalPriceDigits","unitPriceDigits","roundingFactor",
										  "monetaryRoundingMethod","primaryMonetarySymbol","primaryPrecisionDigits","secondaryMonetarySymbol",
										  "secondaryPrecisoinDigits","unknown" };
	int unknownParameter = 27;


	int auditEvent = 0;
	int auditType  = 0;
	SealableParamID auditParam = SERVICE_SWITCH_PRESSED;
	// Write out each audit record to the file.
	string fileOutput;
	for (int i = startingOffset; i >= endingOffset; i--)
	{
		fileOutput += Integer::toString( i - endingOffset ) + ",";

//		auditType = cat3AuditTrailRecords[i]->getAuditType();
//		if ( auditType >= unknownType )
//			fileOutput += string("?") + Integer::toString(auditType) + string("?");
//		else
//		{
//			std::string eventName = internationalize( string("WeigherAuditTrail.").append( auditTypeNameIndicator[ cat3AuditTrailRecords[i]->getAuditType() ] ) );
//			fileOutput += eventName + ",";
//		}

		auditEvent = cat3AuditTrailRecords[i]->getAuditEvent();
		if ( auditEvent >= unknownEvent )
			fileOutput += string("?") + Integer::toString(auditEvent) + string("?");
		else
		{
			std::string eventName = internationalize( string("WeigherAuditTrail.").append( auditEventNameIndicator[ cat3AuditTrailRecords[i]->getAuditEvent() ] ) );
			fileOutput += eventName + ",";
		}

		auditParam = cat3AuditTrailRecords[i]->getParameterId();
		if ( auditParam >= unknownParameter )
			fileOutput += string("?") + Integer::toString(auditParam) + string("?");
		else
		{
			std::string paramName = internationalize( string("WeigherAuditTrail.").append( paramNameIndicator[ cat3AuditTrailRecords[i]->getParameterId() ] ) );
			fileOutput += paramName + ",";
		}

		time_t epochTime = cat3AuditTrailRecords[i]->getEpochTime();
		struct tm * timeinfo = localtime (&epochTime);
		char buffer [80];
		strftime (buffer,80,"%Y-%m-%d %H:%M",timeinfo);
		fileOutput += string(buffer) + ",";

		string valStr;
		formatWeigherAuditValue( valStr, auditParam, cat3AuditTrailRecords[i]->getNewValue() );
		fileOutput += valStr; // + ",";
//		formatWeigherAuditValue( valStr, auditParam, cat3AuditTrailRecords[i]->getPreviousValue() );
//		fileOutput += valStr;

		output << fileOutput << endl;
		fileOutput = "";
	}

	output.close();

#if defined ( __linux__ )
	system("sync");
#endif
}


void AllCat3AuditTrailRecords::formatWeigherAuditValue( std::string &valStr, SealableParamID param, unsigned long value ) const
{
	hfc::MultiLanguageText internationalize;

	switch (param)
	{
		case FILTER_SPEED:
			if (value == 0)
				valStr = internationalize( string("WeigherAuditTrail.").append("normal") );
			else if (value == 1)
				valStr = internationalize( string("WeigherAuditTrail.").append("slow") );
			else if (value == 2)
				valStr = internationalize( string("WeigherAuditTrail.").append("fast") );
			else
				valStr = ULong::toString( value );
			break;
		case MIN_WEIGHT_PRINT:
			// If the scale factor is 1, then whatever weigher model this is, it is being set in single-range mode
			// (and is avoir mode since scaleFactor 1 is only for avoir mode)
			// Since the min weight to print is stored in the eep with 2-implied decimal places when the scaleFactor is 1
			// and the application wants it to be 3-implied decimal places, we need to multiply by 10 to get the third decimal place
//			if ( getScaleFactor() == 1 )
//				value *= 10;

//			Configuration::getInstance()->formatWeight(value, valStr);
			valStr = ULong::toString( value );
			break;
		case WEIGHER_MODEL:
			if (value == 8)
				valStr = "HLX/HTi";
			else if (value == 16)
				valStr = "HTm";
			else if (value == 0)
				valStr = "None";							
			else 
				valStr = ULong::toString( value );
			break;
		case SCALE_FACTOR:
		case DIVISION_SIZE:
			if (value == 1)
				valStr = "0.01";
			else if (value == 2)
				valStr = "0.002";
			else if (value == 5)
				valStr = "0.005";
			else
				valStr = ULong::toString( value );
			break;
		case SOFTWARE_UPDATE:
		{
			string ver = ULong::toString( value );
			const size_t length = ver.length();
			if ( length < 7 )
				valStr = ULong::toString( value );
			else
			{
				int metrolVer = Integer::valueOf(ver.substr( length - 3, 3));       //  example: 4002100  len=7
				int minor = Integer::valueOf(ver.substr( length - 6, 3));
				int major = Integer::valueOf(ver.substr( 0, length - 6));
				valStr = Integer::toString(major).append(".");
				valStr.append(Integer::toString(minor)).append(".");
				valStr.append(Integer::toString(metrolVer));
			}
			break;
		}
		case WEIGHER_MODE:
		{
			if (value == 0)
				valStr = "Metric";
			else if (value == 1)
				valStr = "Avoir";
			else
				valStr = ULong::toString( value );
			break;
		}

		case PRIMARY_MONETARY_SYMBOL:
		{
			HobString myString;

			Debug::trace("cat3", string("Received: PRIMARY_MONETARY_SYMBOL, value =  ") + ULong::toString(value) );

			unsigned char 	*myCharPtr;
			
			myCharPtr 	= (unsigned char *)&value;
			
			if(*myCharPtr != (unsigned char)NULL)
			{
				myString.push_back(*myCharPtr);

				myCharPtr++;

				if(*myCharPtr != (unsigned char)NULL)
				{
					myString.push_back(*myCharPtr);

					myCharPtr++;
				}

				if(*myCharPtr != (unsigned char)NULL)
				{
					myString.push_back(*myCharPtr);

					myCharPtr++;
				}
			}
			
			myString.quoteForCSV();

			valStr = myString;
			
			Debug::trace("cat3", string("Received: PRIMARY_MONETARY_SYMBOL, valStr = ") + valStr) ;

			break;
		}

		case DECIMAL_SEPARATOR:
		{
			HobString myString;

			if(value == 46 )
				myString  = internationalize( string("WeigherAuditTrail.").append("period") );
				//myString = " . (Period)";
			else if(value == 44)
				//myString = " , (Comma)";
				myString  = internationalize( string("WeigherAuditTrail.").append("comma") );
			else
				myString = ULong::toString( value );

			myString.quoteForCSV();

			valStr = myString;
							
			break;
		}

		case ROUNDING_METHOD:
		{
			if (value == 0)
				valStr = internationalize( string("WeigherAuditTrail.").append("conventional") );
				//valStr = "Conventional";
			else if (value == 1)
				valStr = internationalize( string("WeigherAuditTrail.").append("roundDown") );
				//valStr = "Round Down";
			else if (value == 2)
				valStr = internationalize( string("WeigherAuditTrail.").append("roundUp") );
				//valStr = "Round Up";
			else if (value == 3)
				valStr = internationalize( string("WeigherAuditTrail.").append("unconventional") );
				//valStr = "Unconventional";
			else
				valStr = ULong::toString( value );

			break;
		}

		case MONETARY_RULE:
		{
			if (value == 1)
				valStr = internationalize( string("WeigherAuditTrail.").append("leadingPlacement") );
				//valStr = "Leading Placement";
			else if (value == 2)
				valStr = internationalize( string("WeigherAuditTrail.").append("trailingPlacement") );
				//valStr = "Trailing Placement";
			else
				valStr = ULong::toString( value );

			break;
		}

		default:
			valStr = ULong::toString( value );
			break;
	}
}

