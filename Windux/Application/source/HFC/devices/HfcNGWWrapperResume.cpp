#include "HfcNGWWrapperResume.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Long.h"

HfcNGWWrapperResume::HfcNGWWrapperResume( const char *data ) : NGWWrapperResume( data )
{

}
HfcNGWWrapperResume::HfcNGWWrapperResume(const char *data, int len, int formatType) : NGWWrapperResume()
{
	switch (formatType)
	{
		case InternalFormat:
			break;

		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short offset = 1;

			while (tok.next(&token) != -1)
			{
				switch (offset)
				{
					case fieldOffsetResumeId: {
						setPkgId( (unsigned char) Integer::valueOf( token.c_str() ) );
						break;
					}
					case fieldOffsetResume: {
						setState( (NGWState) Integer::valueOf( token.c_str() ) );
						break;
					}
				}
				offset++;
			}
			break;
		}
		default:
			break;
	}
}

HfcNGWWrapperResume::~HfcNGWWrapperResume()
{

}

const char *HfcNGWWrapperResume::serialize(int format) const
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case InternalFormat: {
			NGWWrapperResume::serialize();			
			HfcObject::serializedData = NGWWrapperResume::serializedData;
			break;
		}
		case BrowserFormat: {

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			unsigned int i = 1;
            while( i < endHfcNGWWrapperResumeFieldOffsets ) {
                HfcObject::serializedData.append(BrowserFormatSeparator);
                switch( i )
                {
					case fieldOffsetResumeId: {
						HfcObject::serializedData.append( Integer::toString( getPkgId() ) );
						break;
					}
                    case fieldOffsetResume: {
                        HfcObject::serializedData.append( Integer::toString( getState() ) );
                        break;
                    }                
                }
                i++;                
            }
            break;
		}
		default:
			Debug::trace("HfcNGWWrapperResume", "serialize() Invalid format!");
			break;
	}
    Debug::trace("HfcNGWWrapperResume", "serialize(): " + HfcObject::serializedData );
	return HfcObject::serializedData.data();
}