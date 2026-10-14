/**
 * @file HfcNGWDimensionOverrides.h
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Overrides package dimensions from the camera board.
 * Created on: Oct 21, 2021
 */
#ifndef HFCNGWDIMENSIONOVERRIDES_H
#define HFCNGWDIMENSIONOVERRIDES_H
#include "HfcObject.h"
#include "NGWDimensionOverrides.h"

typedef enum HfcNGWDimensionOffsets
{
	fieldOffsetOverrideLength = 1,
	fieldOffsetOverrideWidth,
	fieldOffsetOverrideLipHeight,
	endNGWDimensionOffsets
} HfcNGWDimensionOffsets_t;

class HfcNGWDimensionOverrides : public NGWDimensionOverrides, public HfcObject
{
	public:
	HfcNGWDimensionOverrides(const char *data);
    HfcNGWDimensionOverrides(const char *data, int len, int formatType);
	~HfcNGWDimensionOverrides();

	virtual const char *serialize(int format = BrowserFormat) const;

	private:

};
#endif