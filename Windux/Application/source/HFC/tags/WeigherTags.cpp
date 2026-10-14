/* HTSeries Scale Project
 * COPYRIGHT (c) 2016.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWeigherConfig.h"
#include "WeigherTags.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherTags::_dataType_ = 0;
std::string WeigherTags::_dataTypeName_("WeigherTags"); 
#endif

const std::string WeigherTags::weigherModel            = "WM";
const std::string WeigherTags::weigherFilterSpeed      = "WF";
const std::string WeigherTags::weigherScaleFactor      = "WD";
const std::string WeigherTags::weigherMaxWeight        = "WX";
const std::string WeigherTags::weigherMinWeightToPrint = "WP";
const std::string WeigherTags::weigherMode             = "WO";

TagsList WeigherTags::weigherTags(endHfcWeigherConfigFieldOffsets);
TagsMap  WeigherTags::fieldMap;	//example pair: r#,fieldOffsetTextNum


WeigherTags::WeigherTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	if (pFieldList)
	{
		//initialize the the table of tags for the Statistics class
		weigherTags[0]                            = emptyTag;

		weigherTags[fieldOffsetFilterSpeed]       = weigherFilterSpeed;
		weigherTags[fieldOffsetScaleWeighMode]    = weigherMode;
		weigherTags[fieldOffsetWeigherModel]      = weigherModel;
		weigherTags[fieldOffsetScaleFactor]       = weigherScaleFactor;
		weigherTags[fieldOffsetMaxScaleWeight]    = weigherMaxWeight;
		weigherTags[fieldOffsetMinWeightToPrint]  = weigherMinWeightToPrint;

		putTagsInMap(pFieldList);
	}
}

WeigherTags::~WeigherTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

}

const TagsList &WeigherTags::getMyTagsList() const
{
	return weigherTags;
}

TagsMap &WeigherTags::getMyTagsMap() const
{
	return fieldMap;
}
