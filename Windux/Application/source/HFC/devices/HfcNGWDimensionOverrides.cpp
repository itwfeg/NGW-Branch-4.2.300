#include "HfcNGWDimensionOverrides.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"



HfcNGWDimensionOverrides::HfcNGWDimensionOverrides(const char *data) : NGWDimensionOverrides(data){}

HfcNGWDimensionOverrides::HfcNGWDimensionOverrides(const char *data, int len, int formatType) : NGWDimensionOverrides()
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
					case fieldOffsetOverrideLength:
						setOverrideLength( Integer::valueOf( token.c_str() ) );
						break;
                    case fieldOffsetOverrideWidth:
                        setOverrideWidth( Integer::valueOf( token.c_str() ) );
                        break;
                    case fieldOffsetOverrideLipHeight: 
                        setOverrideLipHeight( Integer::valueOf( token.c_str() ) );   
                        break;
					default:
						break;
				}
				loopCnt++;
			}
            break;
		}

		default:		
			break;
	}
}


HfcNGWDimensionOverrides::~HfcNGWDimensionOverrides()
{
}

const char *HfcNGWDimensionOverrides::serialize(int format)const
{
    switch (format)
    {
        case InternalFormat:
            {
                Debug::trace("HfcNGWDimensionOverrides", "serialize(InternalFormat)");
                NGWDimensionOverrides::serialize();
                NGWDimensionOverrides::getSerializedLength();
                HfcObject::serializedData = NGWDimensionOverrides::serializedData;
            }
            break;

        case BrowserFormat:
            {
                Debug::trace("HfcNGWDimensionOverrides", "serialize(BrowserFormat)");
                short loopCnt = 1;

                HfcObject::serializedData = Integer::toString((int)getDeviceType());
                HfcObject::serializedData.append(BrowserFormatSeparator);
                HfcObject::serializedData.append(Integer::toString((int)getEventType()));

                while( loopCnt < endNGWDimensionOffsets )
                {
                    HfcObject::serializedData.append(BrowserFormatSeparator);
                    switch (loopCnt)
                    {
                        case fieldOffsetOverrideLength:
                            HfcObject::serializedData.append( Integer::toString( getOverrideLength() ) );
                            break;
                        case fieldOffsetOverrideWidth:
                            HfcObject::serializedData.append( Integer::toString( getOverrideWidth() ) );
                            break;
                        case fieldOffsetOverrideLipHeight:
                            HfcObject::serializedData.append( Integer::toString( getOverrideLipHeight() ) );
                            break;
                        default:
                            break;
                    }
                    loopCnt++;
                }

                HfcObject::serializedData.data();
            }
            break;
        default:
            break;
    }

    return HfcObject::serializedData.data();
}

