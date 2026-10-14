/*
 * WeigherValueMaxFitParams.cpp
 *
 *  Created on: January 31, 2020
 *      Author: Doug Antonides
 */

#include "WeigherValueMaxFitParams.h"
#include "WeigherEvent.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

#endif

WeigherValueMaxFitParams::WeigherValueMaxFitParams( Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Weigher, WgValueMaxFitParams, pHobThread)
{
}


WeigherValueMaxFitParams::WeigherValueMaxFitParams(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &gravity;
	for (unsigned int i = 0; i < sizeof(gravity); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &m_LbsCountsConvFactor;
	for (unsigned int i = 0; i < sizeof(m_LbsCountsConvFactor); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &zeroValue;
	for (unsigned int i = 0; i < sizeof(zeroValue); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&spanValue;
	for (unsigned int i = 0; i < sizeof(spanValue); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&z0ElectronicOffset;
	for (unsigned int i = 0; i < sizeof(z0ElectronicOffset); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&deadWeight;
	for (unsigned int i = 0; i < sizeof(deadWeight); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&x2;
	for (unsigned int i = 0; i < sizeof(x2); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&x1;
	for (unsigned int i = 0; i < sizeof(x1); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&x0;
	for (unsigned int i = 0; i < sizeof(x0); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&y2;
	for (unsigned int i = 0; i < sizeof(y2); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&y1;
	for (unsigned int i = 0; i < sizeof(y1); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&y0;
	for (unsigned int i = 0; i < sizeof(y0); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&xRot_0;
	for (unsigned int i = 0; i < sizeof(xRot_0); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&xRot_1;
	for (unsigned int i = 0; i < sizeof(xRot_1); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&xRot_2;
	for (unsigned int i = 0; i < sizeof(xRot_2); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&yRot_0;
	for (unsigned int i = 0; i < sizeof(yRot_0); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&yRot_1;
	for (unsigned int i = 0; i < sizeof(yRot_1); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&yRot_2;
	for (unsigned int i = 0; i < sizeof(yRot_2); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&zRot_0;
	for (unsigned int i = 0; i < sizeof(zRot_0); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&zRot_1;
	for (unsigned int i = 0; i < sizeof(zRot_1); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&zRot_2;
	for (unsigned int i = 0; i < sizeof(zRot_2); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&xGain;
	for (unsigned int i = 0; i < sizeof(xGain); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&yGain;
	for (unsigned int i = 0; i < sizeof(yGain); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&gCoeff;
	for (unsigned int i = 0; i < sizeof(gCoeff); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&checkSum;
	for (unsigned int i = 0; i < sizeof(checkSum); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WeigherValueMaxFitParams::~WeigherValueMaxFitParams()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WeigherValueMaxFitParams::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &gravity;
	for (unsigned int i = 0; i < sizeof(gravity); i++) {
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &m_LbsCountsConvFactor;
	for (unsigned int i = 0; i < sizeof(m_LbsCountsConvFactor); i++) {
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &zeroValue;
	for (unsigned int i = 0; i < sizeof(zeroValue); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&spanValue;
	for (unsigned int i = 0; i < sizeof(spanValue); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&z0ElectronicOffset;
	for (unsigned int i = 0; i < sizeof(z0ElectronicOffset); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&deadWeight;
	for (unsigned int i = 0; i < sizeof(deadWeight); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&x2;
	for (unsigned int i = 0; i < sizeof(x2); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&x1;
	for (unsigned int i = 0; i < sizeof(x1); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&x0;
	for (unsigned int i = 0; i < sizeof(x0); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&y2;
	for (unsigned int i = 0; i < sizeof(y2); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&y1;
	for (unsigned int i = 0; i < sizeof(y1); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&y0;
	for (unsigned int i = 0; i < sizeof(y0); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&xRot_0;
	for (unsigned int i = 0; i < sizeof(xRot_0); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&xRot_1;
	for (unsigned int i = 0; i < sizeof(xRot_1); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&xRot_2;
	for (unsigned int i = 0; i < sizeof(xRot_2); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&yRot_0;
	for (unsigned int i = 0; i < sizeof(yRot_0); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&yRot_1;
	for (unsigned int i = 0; i < sizeof(yRot_1); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&yRot_2;
	for (unsigned int i = 0; i < sizeof(yRot_2); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&zRot_0;
	for (unsigned int i = 0; i < sizeof(zRot_0); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&zRot_1;
	for (unsigned int i = 0; i < sizeof(zRot_1); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&zRot_2;
	for (unsigned int i = 0; i < sizeof(zRot_2); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&xGain;
	for (unsigned int i = 0; i < sizeof(xGain); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&yGain;
	for (unsigned int i = 0; i < sizeof(yGain); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&gCoeff;
	for (unsigned int i = 0; i < sizeof(gCoeff); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&checkSum;
	for (unsigned int i = 0; i < sizeof(checkSum); i++) {
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WeigherValueMaxFitParams::show()
{
	bool isTracing = Debug::isTracing("WeigherValueMaxFitParams");

	if (isTracing == false)
	{
		return;
	}

	EvolMessage::show();

	Debug::trace("WeigherValueMaxFitParams", "WeigherValueMaxFitParams: ");

	Debug::trace("WeigherValueMaxFitParams", HobString("Gravity: ") + HobString(Integer::toString(getGravity())));
    Debug::trace("WeigherValueMaxFitParams", HobString("m_LbsCountsConvFactor: ") + HobString(Integer::toString(getConvFactor())));

	Debug::trace("WeigherValueMaxFitParams", HobString("zeroValue: ") + HobString(Integer::toString(getZeroValue())));
	Debug::trace("WeigherValueMaxFitParams", HobString("spanValue: ") + HobString(Integer::toString(getSpanValue())));

	Debug::trace("WeigherValueMaxFitParams", HobString("z0ElectronicOffset: ") + HobString(Integer::toString(getZ0ElectronicOffset())));
	Debug::trace("WeigherValueMaxFitParams", HobString("deadWeight: ") + HobString(Integer::toString(getDeadWeight())));

	Debug::trace("WeigherValueMaxFitParams", HobString("x2: ") + HobString(Integer::toString(getX2())));
	Debug::trace("WeigherValueMaxFitParams", HobString("x1: ") + HobString(Integer::toString(getX1())));

	Debug::trace("WeigherValueMaxFitParams", HobString("x0: ") + HobString(Integer::toString(getX0())));
	Debug::trace("WeigherValueMaxFitParams", HobString("y2: ") + HobString(Integer::toString(getY2())));

	Debug::trace("WeigherValueMaxFitParams", HobString("y1: ") + HobString(Integer::toString(getY1())));
	Debug::trace("WeigherValueMaxFitParams", HobString("y0: ") + HobString(Integer::toString(getY0())));

	Debug::trace("WeigherValueMaxFitParams", HobString("xRot_0: ") + HobString(Integer::toString(getX_Rot_0())));
	Debug::trace("WeigherValueMaxFitParams", HobString("xRot_1: ") + HobString(Integer::toString(getX_Rot_1())));

	Debug::trace("WeigherValueMaxFitParams", HobString("xRot_2: ") + HobString(Integer::toString(getX_Rot_2())));
	Debug::trace("WeigherValueMaxFitParams", HobString("yRot_0: ") + HobString(Integer::toString(getY_Rot_0())));

	Debug::trace("WeigherValueMaxFitParams", HobString("yRot_1: ") + HobString(Integer::toString(getY_Rot_1())));
	Debug::trace("WeigherValueMaxFitParams", HobString("yRot_2: ") + HobString(Integer::toString(getY_Rot_2())));

	Debug::trace("WeigherValueMaxFitParams", HobString("zRot_0: ") + HobString(Integer::toString(getZ_Rot_0())));
	Debug::trace("WeigherValueMaxFitParams", HobString("zRot_1: ") + HobString(Integer::toString(getZ_Rot_1())));

	Debug::trace("WeigherValueMaxFitParams", HobString("zRot_2: ") + HobString(Integer::toString(getZ_Rot_2())));
	Debug::trace("WeigherValueMaxFitParams", HobString("xGain: ") + HobString(Integer::toString(getX_Gain())));

	Debug::trace("WeigherValueMaxFitParams", HobString("yGain: ") + HobString(Integer::toString(getY_Gain())));
	Debug::trace("WeigherValueMaxFitParams", HobString("gCoeff: ") + HobString(Integer::toString(getG_Coeff())));

	Debug::trace("WeigherValueMaxFitParams", HobString("checkSum: ") + HobString(Integer::toString(getCheckSum())));
}