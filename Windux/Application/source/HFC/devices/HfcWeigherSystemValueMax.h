/**
 * @file HfcWeigherSystemValueMax.h
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Doug Antonides
 * @brief Value Max FIT Parameters
 * Created on: Jan 31, 2020
 *
 **/

#ifndef HFCWEIGHERSYSTEMVALUEMAX_H_
#define HFCWEIGHERSYSTEMVALUEMAX_H_

#include "evolution.h"
#include "HfcObject.h"
#include "WeigherSystemRequestValueMax.h"

typedef enum HfcWeigherSystemRequestValueMaxFieldOffsets
{
	fieldOffRequestFITParams = 1,

	endHfcWeigherSystemRequestValueMaxFieldOffsets
} HfcWeigherSystemRequestValueMaxFieldOffsets_t;

class HfcWeigherSystemValueMax : public WeigherSystemRequestValueMax, public HfcObject
{
	public:

		HfcWeigherSystemValueMax(const char *data);
		HfcWeigherSystemValueMax(const char *data, int len, int formatType);

		~HfcWeigherSystemValueMax();

		virtual const char *serialize(int format = InternalFormat) const;


private:

#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif

};

#endif /* HFCWEIGHERSYSTEMVALUEMAX_H_ */


