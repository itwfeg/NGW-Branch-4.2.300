#include "HfcEVMMemoryStatistics.h"


#include "HobTokenizerCSV.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEVMMemoryStatistics::dataType = 0;
std::string HfcEVMMemoryStatistics::dataTypeName("HfcEVMMemoryStatistics"); 
#endif


HfcEVMMemoryStatistics::HfcEVMMemoryStatistics()
		: EVMMemoryStatistics()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}


HfcEVMMemoryStatistics::HfcEVMMemoryStatistics(const char *data)
		: EVMMemoryStatistics(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}


HfcEVMMemoryStatistics::HfcEVMMemoryStatistics(const char *data, int len, int formatType)
		: EVMMemoryStatistics()
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
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while ( tok.next(&token) != -1 )
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetUsedMemory:
						setUsedMemory(Integer::valueOf(token));
						break;

					case fieldOffsetFreeMemory:
						setFreeMemory(Integer::valueOf(token));
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


HfcEVMMemoryStatistics::~HfcEVMMemoryStatistics()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


void HfcEVMMemoryStatistics::init()
{
}


const char* HfcEVMMemoryStatistics::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			EVMMemoryStatistics::serialize();

			HfcObject::serializedData = EVMMemoryStatistics::serializedData;

			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}

	return HfcObject::serializedData.data();
}


