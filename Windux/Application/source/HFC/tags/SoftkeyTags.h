/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SOFTKEYTAGS_H
#define SOFTKEYTAGS_H

#include "Tags.h"

/******************************************************************************
*   The Softkey Tags declared                                                 *
******************************************************************************/
class SKTags
{
	public:
		SKTags();
		~SKTags();

		// from the Softkey Style table - sstyles.dbf
		static const std::string styleNum;
		static const std::string vAlign;
		static const std::string textColor;
		static const std::string disabledTextColor;
		static const std::string bgColor;
		static const std::string toggledBgColor;
		static const std::string bgImage;
		static const std::string toggledBgImage;
		static const std::string imageDir;
		static const std::string fgImageLocation;

		// from the Softkey Feature table - sfeatrs.dbf
		static const std::string featureNum;
		static const std::string hGridSize;
		static const std::string vGridSize;
		static const std::string hSpacing;
		static const std::string vSpacing;
		static const std::string groupNum;
		static const std::string fixedRowColMethod;
		static const std::string defaultButton;
		// styleNum comes from the Softkey Style tags

		// from the Softkey table - sftkeys.dbf
		// groupNum already defined
		static const std::string flashkeyNum;
		static const std::string flashkeyName;
		static const std::string hPosition;
		static const std::string vPosition;
		static const std::string hSize;
		static const std::string vSize;
		// bgColor already defined
		// bgImage already defined
		static const std::string name1;
		static const std::string name2;
		static const std::string action;
		static const std::string actionSubtype;
		static const std::string actionParam1Int;
		static const std::string actionParam2Str;

		// from the Softkey Group table - skgrps.dbf
		//	groupNum already defined
		//	hGridSize already defined
		//	vGridSize already defined
		//	hSpacing already defined
		//	vSpacing already defined
		
		static const std::string scriptName;
		static const std::string scriptInterval;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};


/******************************************************************************
*   SoftkeyStyleTags                                                          *
******************************************************************************/
class SoftkeyStyleTags : public Tags
{
	public:
		SoftkeyStyleTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~SoftkeyStyleTags();

	protected:
		const TagsList &getMyTagsList() const {return softkeyStyleTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList softkeyStyleTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


/******************************************************************************
*   SoftkeyFeatureTags                                                        *
******************************************************************************/
class SoftkeyFeatureTags : public Tags
{
	public:
		SoftkeyFeatureTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~SoftkeyFeatureTags();

	protected:
		const TagsList &getMyTagsList() const {return softkeyFeatureTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList softkeyFeatureTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


/******************************************************************************
*   SoftkeyTags                                                               *
******************************************************************************/
class SoftkeyTags : public Tags
{
	public:
		SoftkeyTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~SoftkeyTags();

	protected:
		const TagsList &getMyTagsList() const {return softkeyTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList softkeyTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


/******************************************************************************
*   SoftkeyGroupTags                                                        *
******************************************************************************/
class SoftkeyGroupTags : public Tags
{
	public:
		SoftkeyGroupTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~SoftkeyGroupTags();

	protected:
		const TagsList &getMyTagsList() const {return softkeyGroupTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList softkeyGroupTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//SOFTKEYTAGS_H
