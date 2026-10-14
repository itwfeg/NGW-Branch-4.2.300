#ifndef GCDDATAMSG_H
#define GCDDATAMSG_H

#include "EvolMessage.h"
#include "HobString.h"
#include "MessageFormat.h"

class GCDDevDataMsgProvider
{
public:

	enum Field
	{
		Field_TareDisp,
		Field_WgtDisp,
		Field_UPriceDisp,
		Field_TPriceDisp,
		Field_CZAnnunc,
		Field_NetAnnunc,
		Field_WtForAnnunc,
		Field_PrepackAnnunc,
		Field_FreqShopUnitPrice,
		Field_FreqShopTotalPrice,
		Field_FreqShopSavings,
		Field_PluNum,
		Field_ProdDescription,
		Field_MarqueeString,
		Field_StoreMarqueeString,
		Field_PluType,
		Field_DualRangeEnabled,
		Field_DualRangeDirection,
		Field_WeigherAttached,
		Field_FractionalAnnunc,
		Field_AdvertType,
		Field_AdvertData,
		Field_UnitPriceAnnunc,

		// new fields above this marker
		Field_Count
	};

	virtual HobString GetData(const Field field) const = 0;
};

class GCDDevDataMsg : public EvolMessage
{
public:
	GCDDevDataMsg(GCDDevDataMsgProvider* pProvider);

	UINT32 getSerializedLength() const;

	const char *serialize() const;
};
#endif