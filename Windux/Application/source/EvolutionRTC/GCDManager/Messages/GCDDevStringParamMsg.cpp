#include "GCDDevStringParamMsg.h"
#include "CMReaderWriterGCD.h"

GCDDevStringParamMsg::GCDDevStringParamMsg( GCDEvent event, const std::string& str ) :
	EvolMessage( GCDDev, event )
{
	serializedData.reserve( 16 * 1024 );
	CMReaderWriterGCD::setDataHeader( serializedData, eventType );
	CMReaderWriterGCD::concatToData( serializedData, "1", 0 );
	CMReaderWriterGCD::concatToData( serializedData, str.c_str() );
	CMReaderWriterGCD::resetDataHeaderLength( serializedData );
}

UINT32 GCDDevStringParamMsg::getSerializedLength() const
{
	return static_cast< UINT >( serializedData.size() );
}

const char *GCDDevStringParamMsg::serialize() const
{
	return serializedData.data();
}