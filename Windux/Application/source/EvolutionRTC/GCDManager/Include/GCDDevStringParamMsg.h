#ifndef GCDDEVSTRINGMSG_H
#define GCDDEVSTRINGMSG_H

#include "EvolMessage.h"
#include "GCDEvent.h"

class GCDDevStringParamMsg : public EvolMessage
{
public:
	GCDDevStringParamMsg( GCDEvent event, const std::string& str );

	UINT32 getSerializedLength() const;

	const char *serialize() const;

protected:
	virtual GCDEvent getGCDEvent() const = 0;

};

template <GCDEvent _Event>
class GCDDevStringParamMsgT : public GCDDevStringParamMsg
{
public:
	GCDDevStringParamMsgT( const std::string& str ) : GCDDevStringParamMsg( _Event, str ) {}

protected:
	virtual GCDEvent getGCDEvent() const { return _Event; }
};

typedef GCDDevStringParamMsgT<GcdGetFileInfo> GCDDevGetFileInfoMsg;
typedef GCDDevStringParamMsgT<GcdStartFirmwareUpdate> GCDDevStartFirmwareUpdateMsg;
typedef GCDDevStringParamMsgT<GcdStartPlaylistUpdate> GCDDevStartPlaylistUpdateMsg;
typedef GCDDevStringParamMsgT<GcdSetUnitSeparator> GCDDevSetUnitSeparatorMsg; // FT-841
typedef GCDDevStringParamMsgT<GcdSetCurrencySymbol> GCDDevSetCurrencySymbolMsg; // FT-841
typedef GCDDevStringParamMsgT<GcdNowServingCustomerNumber> GCDDevNowServingCustomerNumberMsg; // FT-1458
typedef GCDDevStringParamMsgT<GcdDismissNowServingCustomerNumber> GCDDevDismissNowServingCustomerNumberMsg; // FT-1488

#endif