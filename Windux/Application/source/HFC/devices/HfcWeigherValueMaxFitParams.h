/**
 * @file HfcWeigherValueMaxFitParams.h
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Doug Antonides
 * @brief Value Max FIT Parameters
 * Created on: Jan 31, 2020
 *
 **/

#ifndef HFCWEIGHERVALUEMAXFITPARAMS_H
#define HFCWEIGHERVALUEMAXFITPARAMS_H

#include "evolution.h"
#include "HfcObject.h"
#include "WeigherValueMaxFitParams.h"

typedef enum HfcWeigherValueMaxFitParamsFieldOffsets
{
	fieldOffset_gravity = 1,
	fieldOffset_m_LbsCountsConvFactor,
	fieldOffset_zeroValue,
	fieldOffset_spanValue,
	fieldOffset_z0ElectronicOffset,
	fieldOffset_deadWeight,
	fieldOffset_x2,
	fieldOffset_x1,
	fieldOffset_x0,
	fieldOffset_y2,
	fieldOffset_y1,
	fieldOffset_y0,
	fieldOffset_xRot_0,
	fieldOffset_xRot_1,
	fieldOffset_xRot_2,
	fieldOffset_yRot_0,
	fieldOffset_yRot_1,
	fieldOffset_yRot_2,
	fieldOffset_zRot_0,
	fieldOffset_zRot_1,
	fieldOffset_zRot_2,
	fieldOffset_xGain,
	fieldOffset_yGain,
	fieldOffset_gCoeff,
	fieldOffset_checkSum,

	endHfcWeigherValueMaxFitParamsFieldOffsets
} HfcWeigherValueMaxFitParamsFieldOffsets_t;

class HfcWeigherValueMaxFitParams : public WeigherValueMaxFitParams, public HfcObject
{
	public:

		HfcWeigherValueMaxFitParams(const char *data);
		HfcWeigherValueMaxFitParams(const char *data, int len, int formatType);

		~HfcWeigherValueMaxFitParams();

		virtual const char *serialize(int format = InternalFormat) const;


private:
#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif

};

#endif	//	HFCWEIGHERVALUEMAXFITPARAMS_H
