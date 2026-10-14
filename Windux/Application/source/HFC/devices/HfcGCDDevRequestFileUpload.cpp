/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcGCDDevRequestFileUpload.h"
#include "UInteger.h"
#include "CMReaderWriterGCD.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcGCDDevRequestFileUpload::dataType = 0;
std::string HfcGCDDevRequestFileUpload::dataTypeName("HfcGCDDevRequestFileUpload"); 
#endif

HfcGCDDevRequestFileUpload::HfcGCDDevRequestFileUpload(const char *pData, int length)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	memset(m_szFilename, 0, sizeof(m_szFilename));
	memset(m_szUploadPath, 0, sizeof(m_szUploadPath));

	int i = 0;
	int fieldIndex = 0;
	while (i < length)
	{
		int n;
		for (n = i; (n < length) && pData[n] != CMReaderWriterGCD::CH_DELIMETER; ++ n);
		int fieldLength = n - i;

		switch (fieldIndex)
		{
		case FIELDINDEX_REQUESTFILEUPLOAD_FILENAME:
			strncpy(m_szFilename, &pData[i], fieldLength);
			break;
		case FIELDINDEX_REQUESTFILEUPLOAD_UPLOADPATH:
			strncpy(m_szUploadPath, &pData[i], fieldLength);
			break;
		}

		i += fieldLength + 1;
		++ fieldIndex;
	}
}

HfcGCDDevRequestFileUpload::~HfcGCDDevRequestFileUpload()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}