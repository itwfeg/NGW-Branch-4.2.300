/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcLabelApplierTest.h"
#include "LabelApplier.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcLabelApplierTest::dataType = 0;
std::string HfcLabelApplierTest::dataTypeName("HfcLabelApplierTest"); 
#endif

HfcLabelApplierTest::HfcLabelApplierTest(const char *data) : LabelApplierTest(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcLabelApplierTest::HfcLabelApplierTest(const char *data, int len, int formatType) : LabelApplierTest()
{
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
				switch (loopCnt)
				{
					case fieldOffsetTestApplierStationId:
						setStationId((LaStationID) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetTestApplierTestId:
						setTestId((LaTestID) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetTestApplierParm1:
						setParm1(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetTestApplierParm2:
						setParm2(Integer::valueOf(token.c_str()));
						break;
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcLabelApplierTest::~HfcLabelApplierTest()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcLabelApplierTest::serialize(int format) const
{
	const char *msgdata = NULL;
	int msglen = 0;

	switch (format)
	{
		case InternalFormat:
		{
			msgdata = LabelApplierTest::serialize();
			msglen = LabelApplierTest::getSerializedLength();

			HfcObject::serializedData = LabelApplierTest::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}

	return HfcObject::serializedData.data();
}
