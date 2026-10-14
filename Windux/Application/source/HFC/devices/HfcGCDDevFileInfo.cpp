/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcGCDDevFileInfo.h"
#include "UInteger.h"
#include "CMReaderWriterGCD.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcGCDDevFileInfo::dataType = 0;
std::string HfcGCDDevFileInfo::dataTypeName("HfcGCDDevFileInfo"); 
#endif

HfcGCDDevFileInfo::HfcGCDDevFileInfo(const char *pData, int length)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	memset(m_szPath, 0, sizeof(m_szPath));

	int i = 0;
	int fieldIndex = 0;
	while (i < length)
	{
		int n;
		for (n = i; (n < length) && pData[n] != CMReaderWriterGCD::CH_DELIMETER; ++ n);
		int fieldLength = n - i;

		switch (fieldIndex)
		{
		case FIELDINDEX_FILEINFO_PATH:
			strncpy(m_szPath, &pData[i], fieldLength);
			break;
		case FIELDINDEX_FILEINFO_SIZE:
			m_dwFileSize = UInteger::valueOf(&pData[i]);
			break;
		case FIELDINDEX_FILEINFO_LASTWRITETIME:
			m_dwLastWriteTime = UInteger::valueOf(&pData[i]);
			break;
		}

		i += fieldLength + 1;
		++ fieldIndex;
	}
}

HfcGCDDevFileInfo::~HfcGCDDevFileInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
