/**
 * @file HfcWeigherDefaultsComplete.h
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for signaling the completion of saving weigher configuration defaults.
 * Created on: May 9, 2018
 */

#ifndef HFCWEIGHERDEFAULTSCOMPLETE_H
#define HFCWEIGHERDEFAULTSCOMPLETE_H
#include "evolution.h"
#include "HfcObject.h"
#include "WeigherFactoryDefaultsComplete.h"

typedef enum HfcWeigherDefaultsFieldOffsets
{
	fieldOffsetWeigherStatus = 1,

	endHfcWeigherDefaultsFieldOffsets
} HfcWeigherDefaultsFieldOffsets_t;


class HfcWeigherDefaultsComplete : public WeigherFactoryDefaultsComplete, public HfcObject
{
	public:
		HfcWeigherDefaultsComplete(const char *data);
		HfcWeigherDefaultsComplete(const char *data, int len, int formatType);

		~HfcWeigherDefaultsComplete();

		virtual const char *serialize(int format = BrowserFormat) const;


	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif
