#include "HfcWrPPPRequest.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"


HfcWrPPPReq::HfcWrPPPReq( const char *data ) : PPPRequest( data )
{

}

HfcWrPPPReq::HfcWrPPPReq( const char *data, int len, int formatType ) : PPPRequest()
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
                    case fieldOffsetRequest: {
                        setRequest( (PPPRequest::PPPReq)Integer::valueOf(token.c_str()) );
                        break;
                    }
                    case fieldOffsetData: {
                        setData( Integer::valueOf(token.c_str()) );
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

HfcWrPPPReq::~HfcWrPPPReq()
{

}
const char *HfcWrPPPReq::serialize(int format) const
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat: {

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			unsigned int i = 1;
            while( i < endHfcPPPReqFieldOffsets ) {
                HfcObject::serializedData.append(BrowserFormatSeparator);
                switch( i )
                {
                    case fieldOffsetRequest: {
                        HfcObject::serializedData.append( Integer::toString( getRequest() ) );
                        break;
                    }
                    case fieldOffsetData: {
                        HfcObject::serializedData.append( Integer::toString( getData() ) );
                        break;
                    }
                }
                i++;                
            }
            break;
		}
        case InternalFormat:
		{
 			PPPRequest::serialize();
			HfcObject::serializedData = PPPRequest::serializedData;
            break;
		}

		default:
			Debug::trace("HfcWrPPPReq", "serialize() Invalid format!");
			break;
	}
    Debug::trace("HfcWrPPPReq", "serialize(): " + HfcObject::serializedData );
	return HfcObject::serializedData.data();
}