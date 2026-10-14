/**
 * @file WeigherValueMaxFitParams.h
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Doug Antonides
 * @brief Value Max FIT Parameters
 * Created on: Jan 31, 2020
 *
 **/

#ifndef WEIGHERVALUEMAXFITPARAMS_H
#define WEIGHERVALUEMAXFITPARAMS_H
#include "Weigher.h"
#include "EvolMessage.h"


class WeigherValueMaxFitParams : public EvolMessage
{
	public:

		WeigherValueMaxFitParams(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		WeigherValueMaxFitParams(const char *buffer);
		~WeigherValueMaxFitParams();

		const char *serialize() const;

		const UINT32 getGravity() const { return gravity; }
		void setGravity(UINT32 gravity) { this->gravity = gravity; }

		const UINT32 getConvFactor() const { return m_LbsCountsConvFactor; }
		void setConvFactor(UINT32 m_LbsCountsConvFactor) { this->m_LbsCountsConvFactor = m_LbsCountsConvFactor; }

		const UINT32 getZeroValue() const { return zeroValue; }
		void setZeroValue(UINT32 zeroValue) { this->zeroValue = zeroValue; }

		const UINT32 getSpanValue() const { return spanValue; }
		void setSpanValue(UINT32 spanValue) { this->spanValue = spanValue; }

		const UINT32 getZ0ElectronicOffset() const { return z0ElectronicOffset; }
		void setZ0ElectronicOffset(UINT32 z0ElectronicOffset) { this->z0ElectronicOffset = z0ElectronicOffset; }

		const UINT32 getDeadWeight() const { return deadWeight; }
		void setDeadWeight(UINT32 deadWeight) { this->deadWeight = deadWeight; }

		const UINT32 getX2() const { return x2; }
		void setX2(UINT32 x2) { this->x2 = x2; }

		const UINT32 getX1() const { return x1; }
		void setX1(UINT32 x1) { this->x1 = x1; }

		const UINT32 getX0() const { return x0; }
		void setX0(UINT32 x0) { this->x0 = x0; }

		const UINT32 getY2() const { return y2; }
		void setY2(UINT32 y2) { this->y2 = y2; }

		const UINT32 getY1() const { return y1; }
		void setY1(UINT32 y1) { this->y1 = y1; }

		const UINT32 getY0() const { return y0; }
		void setY0(UINT32 y0) { this->y0 = y0; }

		const UINT32 getX_Rot_0() const { return xRot_0; }
		void setX_Rot_0(UINT32 xRot_0) { this->xRot_0 = xRot_0; }

		const UINT32 getX_Rot_1() const { return xRot_1; }
		void setX_Rot_1(UINT32 xRot_1) { this->xRot_1 = xRot_1; }

		const UINT32 getX_Rot_2() const { return xRot_2; }
		void setX_Rot_2(UINT32 xRot_2) { this->xRot_2 = xRot_2; }

		const UINT32 getY_Rot_0() const { return yRot_0; }
		void setY_Rot_0(UINT32 yRot_0) { this->yRot_0 = yRot_0; }

		const UINT32 getY_Rot_1() const { return yRot_1; }
		void setY_Rot_1(UINT32 yRot_1) { this->yRot_1 = yRot_1; }

		const UINT32 getY_Rot_2() const { return yRot_2; }
		void setY_Rot_2(UINT32 yRot_2) { this->yRot_2 = yRot_2; }

		const UINT32 getZ_Rot_0() const { return zRot_0; }
		void setZ_Rot_0(UINT32 zRot_0) { this->zRot_0 = zRot_0; }

		const UINT32 getZ_Rot_1() const { return zRot_1; }
		void setZ_Rot_1(UINT32 zRot_1) { this->zRot_1 = zRot_1; }

		const UINT32 getZ_Rot_2() const { return zRot_2; }
		void setZ_Rot_2(UINT32 zRot_2) { this->zRot_2 = zRot_2; }

		const UINT32 getX_Gain() const { return xGain; }
		void setX_Gain(UINT32 xGain) { this->xGain = xGain; }

		const UINT32 getY_Gain() const { return yGain; }
		void setY_Gain(UINT32 yGain) { this->yGain = yGain; }

		const UINT32 getG_Coeff() const { return gCoeff; }
		void setG_Coeff(UINT32 gCoeff) { this->gCoeff = gCoeff; }

		const UINT32 getCheckSum() const { return checkSum; }
		void setCheckSum(UINT32 checkSum) { this->checkSum = checkSum; }


        void show();

	protected:

		// CAN ID: 0x430
		UINT32	gravity;
		UINT32	m_LbsCountsConvFactor;

		// CAN ID: 0x431
		UINT32	zeroValue;
		UINT32	spanValue;

		// CAN ID: 0x432
		UINT32	z0ElectronicOffset;
		UINT32	deadWeight;

		// CAN ID: 0x433
		UINT32	x2;
		UINT32	x1;

		// CAN ID: 0x434
		UINT32	x0;
		UINT32	y2;

		// CAN ID: 0x4305
		UINT32	y1;
		UINT32	y0;

		// CAN ID: 0x436
		UINT32	xRot_0;
		UINT32	xRot_1;

		// CAN ID: 0x437
		UINT32	xRot_2;
		UINT32	yRot_0;

		// CAN ID: 0x438
		UINT32	yRot_1;
		UINT32	yRot_2;

		// CAN ID: 0x439
		UINT32	zRot_0;
		UINT32	zRot_1;

		// CAN ID: 0x43A
		UINT32	zRot_2;
		UINT32	xGain;

		// CAN ID: 0x43B
		UINT32	yGain;
		UINT32	gCoeff;

		// CAN ID: 0x43C
		UINT32	checkSum;

};


#endif //WEIGHERVALUEMAXFITPARAMS_H