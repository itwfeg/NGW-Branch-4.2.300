#include "GCDDevDataMsg.h"
#include "CMReaderWriterGCD.h"

GCDDevDataMsg::GCDDevDataMsg(GCDDevDataMsgProvider* pProvider) :
	EvolMessage(GCDDev, GcdData)
{
	//build the data stream to send
	serializedData.reserve(512);
	CMReaderWriterGCD::setDataHeader(serializedData, eventType);
	CMReaderWriterGCD::concatToData(serializedData, "1", 0);
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_TareDisp));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_WgtDisp));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_UPriceDisp));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_TPriceDisp));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_CZAnnunc));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_NetAnnunc));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_WtForAnnunc));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_PrepackAnnunc));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_FreqShopUnitPrice));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_FreqShopTotalPrice));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_FreqShopSavings));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_PluNum));
	HobString desc = pProvider->GetData(GCDDevDataMsgProvider::Field_ProdDescription);
	desc.stripFontSequences();
	CMReaderWriterGCD::concatToData(serializedData, desc);
	HobString marq = (pProvider->GetData(GCDDevDataMsgProvider::Field_MarqueeString).length() > 0) ?
		pProvider->GetData(GCDDevDataMsgProvider::Field_MarqueeString) :
	pProvider->GetData(GCDDevDataMsgProvider::Field_StoreMarqueeString);
	CMReaderWriterGCD::concatToData(serializedData, marq);
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_PluType));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_DualRangeEnabled));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_DualRangeDirection));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_WeigherAttached));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_FractionalAnnunc));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_AdvertType));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_AdvertData));
	CMReaderWriterGCD::concatToData(serializedData, pProvider->GetData(GCDDevDataMsgProvider::Field_UnitPriceAnnunc));
	CMReaderWriterGCD::concatToData(serializedData, "-");
	CMReaderWriterGCD::resetDataHeaderLength(serializedData);
}

UINT32 GCDDevDataMsg::getSerializedLength() const
{
	return static_cast< UINT >(serializedData.size());
}

const char *GCDDevDataMsg::serialize() const
{
	return serializedData.data();
}