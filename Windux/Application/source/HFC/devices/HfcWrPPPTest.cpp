#include "HfcWrPPPTest.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"


HfcWrPPPTest::HfcWrPPPTest( const char *data ) : PPPTest( data )
{

}

HfcWrPPPTest::HfcWrPPPTest( const char *data, int len, int formatType ) : PPPTest()
{
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
                    case fieldOffsetStart: {
                        setStartTest( Bool::valueOf(token.c_str()) );
                        break;
                    }
                    loopCnt++;
                }
            }

        }
        default:
            break;
    }
}

HfcWrPPPTest::~HfcWrPPPTest()
{

}
const char *HfcWrPPPTest::serialize(int format) const
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat: {

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			unsigned int i = 1;
            while( i < endHfcPPPTstFieldOffsets ) {
                HfcObject::serializedData.append(BrowserFormatSeparator);
                switch( i )
                {
                    case fieldOffsetStart: {
                        HfcObject::serializedData.append( Bool::toString( getStartTest() ) );
                        break;
                    }
                }
                i++;                
            }
            break;
		}
        case InternalFormat:
		{
 			PPPTest::serialize();
			HfcObject::serializedData = PPPTest::serializedData;
            break;
		}
		default:
			Debug::trace("HfcWrPPPTest", "serialize() Invalid format!");
			break;
	}
    Debug::trace("HfcWrPPPTest", "serialize(): " + HfcObject::serializedData );
	return HfcObject::serializedData.data();
}