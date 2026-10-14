/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcGCDDevError.h"
#include "CMReaderWriterGCD.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcGCDDevError::dataType = 0;
std::string HfcGCDDevError::dataTypeName("HfcGCDDevError"); 
#endif

HfcGCDDevError::HfcGCDDevError(const char *pData, int length) :
	m_code(static_cast< Code >(0))
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	int i = 0;
	int fieldIndex = 0;
	while (i < length)
	{
		int n;
		for (n = i; (n < length) && pData[n] != CMReaderWriterGCD::CH_DELIMETER; ++ n);
		int fieldLength = n - i;

		switch (fieldIndex)
		{
		case FIELDINDEX_CODE:
			m_code = static_cast< Code >(Integer::valueOf(&pData[i]));
			break;
		case FIELDINDEX_DESCRIPTION:
			m_description.clear();
			m_description.append(&pData[i], fieldLength);
			break;
		case FIELDINDEX_FILE_URL:
			m_url.clear();
			m_url.append(&pData[i], fieldLength);
			break;
		}

		i += fieldLength + 1;
		++ fieldIndex;
	}
}

HfcGCDDevError::~HfcGCDDevError()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}