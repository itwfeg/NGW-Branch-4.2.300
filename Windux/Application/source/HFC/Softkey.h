/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SOFTKEY_H
#define SOFTKEY_H

#include "evolution.h"
#include "HfcObject.h"

#include <list>

class DBAccess;
namespace edb
{
class DBSoftKey;
class DBProduct;
class DBCategory;
}
namespace util_filesystem
{
class Filename;
}

typedef enum SoftkeyFieldOffsets
{
	fieldOffsetSKeyFeatureNum = 1,
	fieldOffsetSKeyGroupNum,
	fieldOffsetSKeyHorizontalPos,
	fieldOffsetSKeyVerticalPos,
	fieldOffsetSKeyHorizontalSize,
	fieldOffsetSKeyVerticalSize,
	fieldOffsetSKeyBackgroundColor,
	fieldOffsetSKeyToggledBackgroundColor,
	fieldOffsetSKeyBackgroundImage,
	fieldOffsetSKeyToggledBackgroundImage,
	fieldOffsetSKeyTextColor,
	fieldOffsetSKeyName1,
	fieldOffsetSKeyName2,
	fieldOffsetSKeyAction,
	fieldOffsetSKeyActionIntegerParam,
	fieldOffsetSKeyActionStringParam,
	fieldOffsetSKeyActionCategoryNum,
	fieldOffsetSKeyActionOperatorNum,
	fieldOffsetSKeyActionProductNum,
	fieldOffsetSKeyActionFeatureNum,
	fieldOffsetSKeyActionParameter,
	fieldOffsetSKeyFlashkeyNum,			//	Used only when received from Quantum. Evolution will not write this one out.
	fieldOffsetSKeyForegroundImage,		//	Not part of the softkey, but we will need it for the softkey editor.

	fieldOffsetSKeyBgColorDefault,			//	Used for Browser only
	fieldOffsetSKeyBgImageDefault,			//	Used for Browser only
	fieldOffsetSKeyFgImageDefault,			//	Used for Browser only
	fieldOffsetSKeyTextColorDefault,		//	Used for Browser only
	fieldOffsetSKeyName1Default,			//	Used for Browser only
	fieldOffsetSKeyName2Default,			//	Used for Browser only
	fieldOffsetSKeyBgColorUseDefault,		//	Used for Browser only
	fieldOffsetSKeyBgImageUseDefault,		//	Used for Browser only
	fieldOffsetSKeyTextColorUseDefault,		//	Used for Browser only
	fieldOffsetSKeyName1UseDefault,			//	Used for Browser only
	fieldOffsetSKeyName2UseDefault,			//	Used for Browser only

	fieldOffsetSKeyForceNoBgColor,			//	Allows us to not look at the defaults for a color.
	fieldOffsetSKeyForceNoBgImage,			//	Allows us to not look at the defaults for an image.
	fieldOffsetSKeyForceNoText,				//	Allows us to not look at the defaults for text.
	fieldOffsetSKeyFlashkeyName,			//	Used only when received from Quantum. Evolution will not write this one out.
	fieldOffsetSKeyDeptNumber,				//  The department number the softkey is associated with
	fieldOffsetSKeyScriptName,
	fieldOffsetSKeyScriptInterval,
	endSoftkeyFieldOffsets
} SoftkeyFieldOffsets_t;

typedef enum SoftkeyActions
{
		//	Defined softkey actions (Parameter 1: actionIntParam, Parameter 2: actionStringParam in dbsoftkey.h)
	Action_NoAction,
	Action_PrintLabel,                             //  [No parameters.]
	Action_ChangeScreen,                           //  [Param 1: screen number.]
	Action_BrowseWeb,                              //  [Param 1: web page string. (optional)]
	Action_ChangeLanguage,                         //  [Param 1: language abbreviation (i.e., en=English, es=Spanish, etc.).]
	Action_LoginOperator,              //5         //  [Param 1: Operator ID.]
	Action_ResetPrice,                             //  [No parameters.]
	Action_SetPlatterTare,                         //  [No parameters.]
	Action_SelectProduct,                          //  [Param 1: product number.]
	Action_LoadSoftkeyGroup,                       //  [Param 1: feature number (0 means 'current feature').]
	Action_LoadCategory_First,         //10        //  [Param 1: category number. Param 2: feature number.]
	Action_LoadCategory_Next,                      //  [Param 1: category number. Param 2: feature number.]
	Action_LoadCategory_Previous,                  //  [Param 1: category number. Param 2: feature number.]
	Action_ApplyDiscount_CentsOff,                 //  [Param 1: discount amount.]
	Action_ApplyDiscount_PercentOff,               //  [Param 1: discount amount.]
	Action_VoidTransaction,            //15
	Action_LoadSpecials_First,                     //  [Param 2: feature number.]
	Action_LoadSpecials_Next,                      //  [Param 2: feature number.]
	Action_LoadSpecials_Previous,                  //  [Param 2: feature number.]
	Action_LoadSearch_First,                       //  [Param 1: string number. Param 2: feature number.]
	Action_LoadSearch_Next,            //20        //  [Param 1: string number. Param 2: feature number.]
	Action_LoadSearch_Previous,                    //  [Param 1: string number. Param 2: feature number.]
	Action_PrintExistingExpirationLabel,           //  [No parameters.]
	Action_FPP_URL,                                //  [Param 1: web page string. (optional)]
	Action_Application_URL,                        //  [Param 1: web page string. (optional)]
	Action_BeginCutTest,          	   //25        //  [No parameters.]
	Action_EndCutTest,                             //  [No parameters.]
	Action_SelectAnyProduct,                       //  [No parameters.]
	Action_PrintRunTotals,                         //  [No parameters.]
	Action_SwitchTotalType,                        //  [Param 1: total type number.]
	Action_PrintMultipleLabels,        //30
	Action_ToggleProductionEntryMode,
	Action_PrintNewExpirationLabel,                //  [No parameters.]
	Action_LoadTareSoftkey,						   //  [Param 1: tare value. ]
	Action_ChangeLabelPlacementAndPosition,		   //  [Param 1: placement. Param 2: rotation.]

    Action_ServeNextCustomer,          //35        //  [No parameters.]
    Action_NowServingQueueManagement,              //  [No parameters.]

	Action_ToggleLabelType,						   //  [actionParamStr = printer, labelType],	i.e.	"2,113" for second printer, label type # 113

	Action_CutTestHistory,                         // took 38 bc it was available

	Action_LoadAlphabeticSearch_First = 39,        //  [Param 1: Letter(s) of alphabet to search for. Param 2: feature number.]
	Action_LoadAlphabeticSearch_Next,              //  [Param 1: Letter(s) of alphabet to search for. Param 2: feature number.]
	Action_LoadAlphabeticSearch_Previous,          //  [Param 1: Letter(s) of alphabet to search for. Param 2: feature number.]

	Action_LoadMostCommon_First,       //42        //  [Param 1: hour number for prod stats query. Param 2: feature number.]
	Action_LoadMostCommon_Next,                    //  [Param 1: hour number for prod stats query. Param 2: feature number.]
	Action_LoadMostCommon_Previous,                //  [Param 1: hour number for prod stats query. Param 2: feature number.]

	Action_RedisplayCurrentGrid,       //45

	Action_ToggleAccumulateItemsMode,
	Action_AddItemToAccumulatedItems,
	Action_RemoveLastItemFromAccumulatedItems,
	Action_ShowAccumulatedItems,
	Action_RemoveAllAccumulatedItems,  //50
	Action_PrintAccumulatedItems,

	Action_LogoutOperator,
    Action_PlayVideo,
    Action_Portioning,
    Action_LaunchExternalApp,           //55

	Action_AddSideItemToMeal,
	Action_AddSideItemToMeatSpecSheet,

    Action_CutTestSuspend,
	Action_CutTestResume,
	Action_ToggleMode,                      // 60 SAME
	Action_OpenDocumentFolder,				// 61 SAME
	Action_TogglePick5Active,				// 62 SAME    02-05-2024: SOF-6183 - Flashkey action not -> -				
	Action_2ndLabelApplyToggle,             // 63 from 66 Forced the rearrangement of the actions -> moved back to 66 3-7-24
	Action_ForcedCameraOverride ,			// 64 from 63 pushed 1 renamed Action_OverrideWrapperCameraDimensions -> moved back to 63 3-7-24  
	Action_ToggleExtraShelfLifeDay,         // 65 from 65 Stayed the same
	Action_Swipe,	                        // 66 from 64 -> -> moved back to 64 3-7-24	
	Action_ChangeDepartment,				// 67 from 67 Stayed the same  
    Action_LastAction
	//	END - Defined softkey actions
} SoftkeyActions_t;

typedef enum SoftkeyDefaults
{
	SoftkeyDataOnly,						//	Only get what is in the Softkey record. (i.e. Backup/Restore)
	SoftkeyDataUsingDefaults,				//	Set up the softkey using default values if the softkey values do not exist. (i.e. Displaying a softkey)
	SoftkeyDataIncludingDefaultValues		//	Set up the softkey including any default values and specify which to use. (i.e. Editing a softkey)
} SoftkeyDefaults_t;

typedef enum labelPlacementAndRotation
{	//	Primary printer - only printer supported to date.
	p1_noChange_noChange,		//	 0	//	No change on label placement or label rotation
	p1_left_noChange,					//	Change label placement to left, no change on label rotation
	p1_right_noChange,					//	Change label placement to right, no change on label rotation
	p1_noChange_0,						//	No change on label placement, change label rotation to 0.
	p1_left_0,							//	Change label placement to left, change label rotation to 0.
	p1_right_0,					//	 5	//	Change label placement to right, change label rotation to 0.
	p1_noChange_45,
	p1_left_45,
	p1_right_45,
	p1_noChange_90,
	p1_left_90,					//	10
	p1_right_90,
	p1_noChange_135,
	p1_left_135,
	p1_right_135,
	p1_noChange_180,			//	15
	p1_left_180,
	p1_right_180,
	p1_noChange_225,
	p1_left_225,
	p1_right_225,				//	20
	p1_noChange_270,
	p1_left_270,
	p1_right_270,
	p1_noChange_315,
	p1_left_315,				//	25
	p1_right_315,
	//	Second printer - future use.
	p2_noChange_noChange,
	p2_left_noChange,
	p2_right_noChange,
	p2_noChange_0,				//	30
	p2_left_0,
	p2_right_0,
	p2_noChange_45,
	p2_left_45,
	p2_right_45,				//	35
	p2_noChange_90,
	p2_left_90,
	p2_right_90,
	p2_noChange_135,
	p2_left_135,				//	40
	p2_right_135,
	p2_noChange_180,
	p2_left_180,
	p2_right_180,
	p2_noChange_225,			//	45
	p2_left_225,
	p2_right_225,
	p2_noChange_270,
	p2_left_270,
	p2_right_270,				//	50
	p2_noChange_315,
	p2_left_315,
	p2_right_315,
	//	Third printer - future use.
	p3_noChange_noChange,
	p3_left_noChange,			//	55
	p3_right_noChange,
	p3_noChange_0,
	p3_left_0,
	p3_right_0,
	p3_noChange_45,				//	60
	p3_left_45,
	p3_right_45,
	p3_noChange_90,
	p3_left_90,
	p3_right_90,				//	65
	p3_noChange_135,
	p3_left_135,
	p3_right_135,
	p3_noChange_180,
	p3_left_180,				//	70
	p3_right_180,
	p3_noChange_225,
	p3_left_225,
	p3_right_225,
	p3_noChange_270,			//	75
	p3_left_270,
	p3_right_270,
	p3_noChange_315,
	p3_left_315,
	p3_right_315				//	80
} labelPlacementAndRotation_t;

class Softkey : public HfcObject
{
	public:

		//	Initial values for the Most common items softkeys
		static const short DataType_Unknown  = 0;
		static const short DataType_Product  = 1;
		static const short DataType_Category = 2;
		static const short DataType_Previous = 98;
		static const short DataType_Next     = 99;
		//	END - Initial values for the Most common items softkeys


		/*	Action subtypes (just throwing these out here for now):
			First, previous, next, last
			discountPerUnitPrice, packageDiscount, percentOff, buyXGetYFree, buyXPayYOffMore
		*/

		static const short FlashkeysPerGroup	= 16;	//	Quantum: 16 flashkeys per group
		static const short MaxFlashkeysWide		= 4;	//	Quantum: 4 flashkeys max across
		static const short MaxFlashkeysHigh		= 4;	//	Quantum: 4 flashkeys max down
		static const short magicFlashkeyWidth	= 50;	//	Temporary: X Pixels per flashkey
		static const short magicFlashkeyHeight	= 50;	//	Temporary: Y Pixels per flashkey

		//	These are 4 actions from the Quantum flashkeys. They will be converted to eVantage actions.
		static const short QuantumActionLoadProduct		= 2;
		static const short QuantumActionLoadOperator	= 20;
		static const short QuantumActionLoadMacro		= 182;
		static const short QuantumActionLoadGroup		= 214;

		Softkey(DBAccess *access = 0);
		Softkey(const char *data, int len, int formatType);
		Softkey(edb::DBSoftKey *dbSoftkeyRec, DBAccess *access = 0, short languageSelection = 1);

		~Softkey();

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum deptNum);

		static void initFieldLists();

		void setDBAccess(DBAccess *dbAccess);

		edb::DBSoftKey *getMyDbRecord();

		bool isRecordValid();

		HobRecordNum getGroupNumber() const;
		void setGroupNumber(HobRecordNum number);

		int getHorizontalPosition() const;
		void setHorizontalPosition(int position);

		int getVerticalPosition() const;
		void setVerticalPosition(int position);

		int getHorizontalSize() const;
		void setHorizontalSize(int size);

		int getVerticalSize() const;
		void setVerticalSize(int size);

		const HobString &getBackgroundColor() const;
		void setBackgroundColor(const HobString &color);

		const HobString &getToggledBackgroundColor() const;
		void setToggledBackgroundColor(const HobString &color);

		const HobString &getBackgroundImage() const;
		void setBackgroundImage(const HobString &graphicName);

		const HobString &getToggledBackgroundImage() const;
		void setToggledBackgroundImage(const HobString &graphicName);

		const HobString &getName1() const;
		void setName1(const HobString &name);

		const HobString &getName2() const;
		void setName2(const HobString &name);

		int getAction() const;
		void setAction(int actionValue);

		int getActionIntegerParam() const;
		void setActionIntegerParam(int param);

		const HobString &getActionStringParam() const;
		void setActionStringParam(const HobString &param);

		HobRecordNum getActionCategoryNumber() const;
		void setActionCategoryNumber(HobRecordNum number);

		const HobString &getActionOperatorNumber() const;
		void setActionOperatorNumber(const HobString &number);

		HobRecordNum getActionProductNumber() const;
		void setActionProductNumber(HobRecordNum number);

		HobRecordNum getActionFeatureNumber() const;
		void setActionFeatureNumber(HobRecordNum number);

		const HobString &getActionParameter() const;
		void setActionParameter(const HobString &param);
		void parseActionParameter();
		void determineActionParameter(HobString &result, int format) const;

		const HobString &getTextColor() const;
		void setTextColor(const HobString &color);

		bool getUseDefaultBackgroundColor() const;
		void setUseDefaultBackgroundColor(bool useDefault);

		bool getUseDefaultBackgroundImage() const;
		void setUseDefaultBackgroundImage(bool useDefault);

		bool getUseDefaultTextColor() const;
		void setUseDefaultTextColor(bool useDefault);

		bool getUseDefaultName1() const;
		void setUseDefaultName1(bool useDefault);

		bool getUseDefaultName2() const;
		void setUseDefaultName2(bool useDefault);

		bool isSoftkeyEnabled() const;
		void setSoftkeyEnabled(bool value);

		const HobString &getDefaultForegroundImage() const;
		void setDefaultForegroundImage(const HobString &image);

		const HobString &getScriptName() const;
		void setScriptName(const HobString &name);

		int getScriptInterval() const;
		void setScriptInterval(HobRecordNum interval);

		int updateField(int fieldCode, const HobString &token, int formatType = BrowserFormat);
		bool update(const Softkey *newSoftkey);
		void getFieldAsString(int fieldCode, HobString &result, int format) const;

		static Softkey *find(HobRecordNum groupNum, HobRecordNum deptNum, int horizPos, int verticalPos, DBAccess *dbAccess, short languageSelection = 1);

		int deleteRecord();

		void saveRecord();

		virtual const char *serialize(int format) const;

		void setFlashkeyNumber(HobRecordNum number);
		HobRecordNum setSoftkeyDataFromFlashkeyData();

		void show();

		static void verifyReservedRecords(DBAccess *dbAccess, int groupNumber = 0);

		static edb::DBSoftKey *findActionInSoftkeyGroup(HobRecordNum groupNum, HobRecordNum deptNum, int theAction, DBAccess *dbAccess = NULL);

		static void fitDescription(HobString &desc, unsigned int maxChars);

		void setupSoftkeyDefaultValues(short languageSelection, short defaultSetting, HobRecordNum featureNum = RECORD_NOT_FOUND, edb::DBProduct *dbProductRec = (edb::DBProduct *) NULL, edb::DBCategory *dbCategoryRec = (edb::DBCategory *) NULL);

		static HobString getProductSoftkeyName(edb::DBProduct *dbProduct, short languageSelection);
		static HobString getCategorySoftkeyName(edb::DBCategory *dbCategory, short languageSelection);

		static HobString getSpecialsImageFilename(HobRecordNum deptNum, HobRecordNum specialNum, short language, DBAccess *dbAccess);

		HobString validateSpecialsImage(HobString &graphicFileName, short language);
		HobString validateProductImage(HobString &graphicFileName, short language);
		HobString validateCategoryImage(HobString &graphicFileName, short language);
		HobString validateSoftkeyImage(HobString &graphicFileName, short language);
		static bool checkImageExists(HobString &graphicFileName, HobString rootPath, HobString imageDir, short language);

		HobRecordNum getFeatureNumber() const;
		void setFeatureNumber(HobRecordNum number);

		bool isFieldQuoted(int fieldOffset) const;

		bool getForceNoBackgroundColor() const;
		void setForceNoBackgroundColor(bool boolValue);
		bool getForceNoBackgroundImage() const;
		void setForceNoBackgroundImage(bool boolValue);
		bool getForceNoText() const;
		void setForceNoText(bool boolValue);

		bool isValidateBackgroundImageFile() const;
		void setValidateBackgroundImageFile(bool validate);
		
		int getPlacementAndPositionAction(HobString &parseStr);
		void getPlacementAndPositionActionString(HobString &result);

	private:
		HobRecordNum	groupNumber;
		HobRecordNum	flashkeyNumber;			//	Comes from Quantum only. We will use it to determine the groupNumber, horizontalPosition and verticalPosition.
		int				horizontalPosition;
		int				verticalPosition;
		int				horizontalSize;
		int				verticalSize;
		HobString		backgroundColor;
		HobString		toggledBackgroundColor;
		HobString		backgroundImage;		//	Include file extension???
		HobString		toggledBackgroundImage;		//	Include file extension???
		HobString		textColor;
		HobString		name1;					//	Primary language text
		HobString		name2;					//	Secondary language text
		int				action;
		int				actionSubtype;
		int				actionIntParam;
		HobString		actionStringParam;
		HobRecordNum	actionCategoryNumber;
		HobString		actionOperatorNumber;
		HobRecordNum	actionProductNumber;
		HobRecordNum	actionFeatureNumber;

		HobString		defaultBackgroundColor;
		HobString		defaultBackgroundImage;		//	Includes path & file extension
		HobString		defaultTextColor;
		HobString		defaultName1;					//	Primary language text
		HobString		defaultName2;					//	Secondary language text
		HobString		defaultForegroundImage;		//	Includes path & file extension

		bool			useDefaultBgColor;
		bool			useDefaultBgImage;
		bool			useDefaultTextColor;
		bool			useDefaultName1;
		bool			useDefaultName2;

		bool			forceNoBgColor;
		bool			forceNoBgImage;
		bool			forceNoText;

		bool			softkeyEnabled;
		HobRecordNum	featureNumber;
		HobRecordNum    deptNum;

		bool			bgImageExists;
		bool			fgImageExists;

		//	This variable will contain either the actionIntParam, actionStringParam, actionCategoryNumber or actionOperatorNumber.
		//	Where it really goes depends on the action.
		HobString		actionParameter;

		bool validateBackgroundImageFile;
		bool validateToggledBackgroundImageFile;

		HobString       scriptName;
		HobRecordNum    scriptInterval;

		DBAccess	*dbAccess;
		edb::DBSoftKey	*myDbRecord;

		void init();

		static std::list<SoftkeyFieldOffsets> evolFieldList, ultimaFieldList, browserFieldList, executeSoftkeyFieldList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //SOFTKEY_H
