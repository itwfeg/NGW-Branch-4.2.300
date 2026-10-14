/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLTAGS_H
#define COOLTAGS_H

#include "Tags.h"

/******************************************************************************
*   The COOL Tags declared                                                    *
******************************************************************************/
class COOLTags
{
	public:
		COOLTags();
		~COOLTags();

		// from the COOL Category table - colcats.dbf
		static const std::string coolCategoryNum;
		static const std::string coolCategoryName1;
		static const std::string coolCategoryName2;

		static const std::string belongsToAllCOOLCategories;

		// from the COOL Country table - colctrs.dbf
		static const std::string coolCountryNum;
		static const std::string coolCountryName1;
		static const std::string coolCountryName2;

		// from the COOL Production Steps table - colstps.dbf
		static const std::string coolProductionStepNum;
		static const std::string coolProductionStepName1;
		static const std::string coolProductionStepName2;

		// from the COOL Pretext table - colptxs.dbf
		static const std::string coolPretextNum;
		static const std::string coolPretextName1;
		static const std::string coolPretextName2;

		// from the COOL Text table - colptxs.dbf
		static const std::string coolTextNum;
		static const std::string coolTextText;
		static const std::string coolTextType;

		// from the COOL Shortlist and ShortlistText tables - collsts.dbf, colltxs.dbf
		static const std::string coolShortlistNum;
		static const std::string coolShortlistFirstLink;
		static const std::string coolShortlistOrderNum;
		static const std::string coolShortlistListLink;
		static const std::string coolShortlistListNextLink;

		// from the COOL Totals table - coltots.dbf
		static const std::string coolLogDateFirst;
		static const std::string coolLogTimeFirst;
		static const std::string coolLogDateLast;
		static const std::string coolLogTimeLast;

		// from the Product table - prods.dbf
		// see ProductTags.h

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


/******************************************************************************
*   COOLCategoryTags                                                          *
******************************************************************************/
class COOLCategoryTags : public Tags
{
	public:
		COOLCategoryTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~COOLCategoryTags();

	protected:
		const TagsList &getMyTagsList() const {return coolCategoryTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList coolCategoryTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


/******************************************************************************
*   COOLCountryTags                                                           *
******************************************************************************/
class COOLCountryTags : public Tags
{
	public:
		COOLCountryTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~COOLCountryTags();

	protected:
		const TagsList &getMyTagsList() const {return coolCountryTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList coolCountryTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


/******************************************************************************
*   COOLCategoryCountryTags                                                   *
******************************************************************************/
class COOLCategoryCountryTags : public Tags
{
	public:
		COOLCategoryCountryTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~COOLCategoryCountryTags();

	protected:
		const TagsList &getMyTagsList() const {return coolCategoryCountryTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList coolCategoryCountryTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


/******************************************************************************
*   COOLProductionStepTags                                                    *
******************************************************************************/
class COOLProductionStepTags : public Tags
{
	public:
		COOLProductionStepTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~COOLProductionStepTags();

	protected:
		const TagsList &getMyTagsList() const {return coolProductionStepTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList coolProductionStepTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


/******************************************************************************
*   COOLCategoryProductionStepTags                                            *
******************************************************************************/
class COOLCategoryProductionStepTags : public Tags
{
	public:
		COOLCategoryProductionStepTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~COOLCategoryProductionStepTags();

	protected:
		const TagsList &getMyTagsList() const {return coolCategoryProductionStepTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList coolCategoryProductionStepTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


/******************************************************************************
*   COOLPretextTags                                                           *
******************************************************************************/
class COOLPretextTags : public Tags
{
	public:
		COOLPretextTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~COOLPretextTags();

	protected:
		const TagsList &getMyTagsList() const {return coolPretextTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList coolPretextTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


/******************************************************************************
*   COOLCategoryPretextTags                                                   *
******************************************************************************/
class COOLCategoryPretextTags : public Tags
{
	public:
		COOLCategoryPretextTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~COOLCategoryPretextTags();

	protected:
		const TagsList &getMyTagsList() const {return coolCategoryPretextTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList coolCategoryPretextTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


/******************************************************************************
*   COOLTextTags                                                              *
******************************************************************************/
class COOLTextTags : public Tags
{
	public:
		COOLTextTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~COOLTextTags();

	protected:
		const TagsList &getMyTagsList() const {return coolTextTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList coolTextTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


/******************************************************************************
*   COOLHobShortlistTags                                                         *
******************************************************************************/
class COOLHobShortlistTags : public Tags
{
	public:
		COOLHobShortlistTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~COOLHobShortlistTags();

	protected:
		const TagsList &getMyTagsList() const {return coolHobShortlistTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList coolHobShortlistTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


/******************************************************************************
*   COOLShortlistTags                                                         *
******************************************************************************/
class COOLShortlistTags : public Tags
{
	public:
		COOLShortlistTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~COOLShortlistTags();

	protected:
		const TagsList &getMyTagsList() const {return coolShortlistTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList coolShortlistTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


/******************************************************************************
*   COOLListTextTags                                                         *
******************************************************************************/
class COOLListTextTags : public Tags
{
	public:
		COOLListTextTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~COOLListTextTags();

	protected:
		const TagsList &getMyTagsList() const {return coolListTextTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList coolListTextTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


/******************************************************************************
*   COOLListTags                                                         *
******************************************************************************/
class COOLListTags : public Tags
{
	public:
		COOLListTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~COOLListTags();

	protected:
		const TagsList &getMyTagsList() const {return coolListTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList coolListTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


/******************************************************************************
*   COOLShortlistTextTags                                                     *
******************************************************************************/
class COOLShortlistTextTags : public Tags
{
	public:
		COOLShortlistTextTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~COOLShortlistTextTags();

	protected:
		const TagsList &getMyTagsList() const {return coolShortlistTextTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList coolShortlistTextTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif	//COOLTAGS_H 
