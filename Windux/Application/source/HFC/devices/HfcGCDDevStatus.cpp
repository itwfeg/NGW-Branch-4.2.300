/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcGCDDevStatus.h"
#include "CMReaderWriterGCD.h"
#include "UInteger.h"

LPCSTR HfcGCDDevStatus::SZ_MEMTSTPASS_TRUE = "MemTstPass";

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcGCDDevStatus::dataType = 0;
std::string HfcGCDDevStatus::dataTypeName("HfcGCDDevStatus"); 
#endif

HfcGCDDevStatus::HfcGCDDevStatus(const char *pData, int length) :
	m_ddrTestPassed(FALSE)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	m_boardVersion.setVersionType(HobVersion::GCDHW);
	m_gcdMgrVersion.setVersionType(HobVersion::GCDAppSW);

	int i = 0;
	int fieldIndex = 0;
	while (i < length)
	{
		int n;
		for (n = i; (n < length) && pData[n] != CMReaderWriterGCD::CH_DELIMETER; ++ n);
		int fieldLength = n - i;

		switch (fieldIndex)
		{
		case FIELDINDEX_BOARD_MAJOR_VERSION:
			m_boardVersion.setMajorRevision(static_cast< UINT16 >(UInteger::valueOf(&pData[i])));
			break;
		case FIELDINDEX_BOARD_MINOR_VERSION:
			m_boardVersion.setMinorRevision(static_cast< UINT16 >(UInteger::valueOf(&pData[i])));
			break;
		case FIELDINDEX_WINCE_MAJOR_VERSION:
			m_winCEVersion.setMajorRevision(static_cast< UINT16 >(UInteger::valueOf(&pData[i])));
			break;
		case FIELDINDEX_WINCE_MINOR_VERSION:
			m_winCEVersion.setMinorRevision(static_cast< UINT16 >(UInteger::valueOf(&pData[i])));
			break;
		case FIELDINDEX_GCDDLL_MAJOR_VERSION:
			m_gcdDllVersion.setMajorRevision(static_cast< UINT16 >(UInteger::valueOf(&pData[i])));
			break;
		case FIELDINDEX_GCDDLL_MINOR_VERSION:
			m_gcdDllVersion.setMinorRevision(static_cast< UINT16 >(UInteger::valueOf(&pData[i])));
			break;
		case FIELDINDEX_GCDMGR_MAJOR_VERSION:
			m_gcdMgrVersion.setMajorRevision(static_cast< UINT16 >(UInteger::valueOf(&pData[i]))); 
			break;
		case FIELDINDEX_GCDMGR_MINOR_VERSION:
			m_gcdMgrVersion.setMinorRevision(static_cast< UINT16 >(UInteger::valueOf(&pData[i])));
			break;
		case FIELDINDEX_MEMORY_TEST_RESULT:
			m_ddrTestPassed = (0 == util_string::strnicmp(SZ_MEMTSTPASS_TRUE, &pData[i], fieldLength));
			break;
		}

		i += fieldLength + 1;
		++ fieldIndex;
	}
}

HfcGCDDevStatus::~HfcGCDDevStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
