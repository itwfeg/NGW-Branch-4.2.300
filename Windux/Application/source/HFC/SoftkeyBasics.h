/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SOFTKEYBASICS_H
#define SOFTKEYBASICS_H


typedef enum
{
    VA_Top,			//	0
    VA_Center,		//	1
    VA_Bottom		//	2
} VerticalAlignment;

typedef enum
{
    HA_Left,		//	0
    HA_Center,		//	1
    HA_Right		//	2
} HorizontalAlignment;

typedef enum
{
    AP_UpperLeft,		//	0
    AP_UpperCenter,		//	1
    AP_UpperRight,		//	2
	AP_CenterLeft,		//	3
	AP_Center,			//	4
	AP_CenterRight,		//	5
	AP_LowerLeft,		//	6
	AP_LowerCenter,		//	7
	AP_LowerRight		//	8
} AreaPosition;

typedef enum
{
	FRCM_None,         // 0
	FRCM_TopRow,       // 1
	FRCM_FirstColumn,  // 2
	FRCM_BottomRow,    // 3
	FRCM_LastColumn    // 4
} FixedRowColMethod;



//	Reserved for reference to a softkey Feature, Group or Style
static const int Reserved_ProductGrid						= 999001;
static const int Reserved_StartScreenButtons				= 999002;		//	ProgrammableButtons for the Main screen.
static const int Reserved_RunScreenButtons					= 999003;		//	ProgrammableButtons for the Run screen.
static const int Reserved_PresetTareGrid					= 999004;
static const int Reserved_CategoryNavigation				= 999005;
static const int Reserved_CategoryButtons					= 999006;
static const int Reserved_MostCommonlyUsedItems				= 999007;

static const int ReservedSK_SelfservCategoryButtons			= 999008;
static const int Reserved_AccumulateItemsStartScreenButtons	= 999010;	//	ProgrammableButtons for the start of run screen
static const int Reserved_AccumulateItemsRunScreenButtons	= 999011;	//  ProgrammableButtons for the run screens

static const int Reserved_StartScreenCutTestOff				= 999012;		//	ProgrammableButtons for the Start a run screen when Cutting test is off.
static const int Reserved_RunScreenCutTestOff				= 999013;		//	ProgrammableButtons for the Run screen when Cutting test is off.
static const int Reserved_StartScreenCutTestOn				= 999014;		//	ProgrammableButtons for the Start a run screen when Cutting test is on.
static const int Reserved_RunScreenCutTestOn				= 999015;		//	ProgrammableButtons for the Run screen when Cutting test is on.

//NOTE: THESE NUMBERS HAVE CHANGED FROM THE ORIGINAL ONES PRE-PORT: (were 999012-999013)
static const int ReservedSK_SelfservAZCategoryButtons		= 999016;
static const int ReservedSK_SelfservCategoryResultsButtons	= 999017;

static const int ReservedSK_SelfservProductGrid             = 999018;
static const int ReservedSK_SelfservAZCategoryResults       = 999019;       // Results for when a A-Z Category is pressed

static const int Reserved_FixedRowColGrid                   = 999020;
static const int Reserved_ForcedCameraOverride              = 999025;

static const int Reserved_RunScreenNumericKeypadButtons		= 999030;		//	ProgrammableButtons for the Run screen when the numeric keypad is hidden (Fastrack).

static const int Reserved_BuildAMealScreenButtons			= 999031;		//	ProgrammableButtons for the Build-A-Meal screen (Fastrack).
static const int Reserved_BuildAMealSideItemButtons			= 999032;		//	ProgrammableButtons for the Build-A-Meal Side Item selection (Fastrack).

static const int Reserved_MeatSpecSheetScreeenButtons       = 999033;       //  Programmable Buttons for Meat Specification Sheets screen (Fastrack)
static const int Reserved_MeatSpecSheetSideItemButtons      = 999034;       //  Programmable Buttons for Meat Specification Sheets Side Items (Fastrack)
static const int Reserved_RunScreenCutTestOnOptionOne       = 999035;       //  Programmable Buttons for the Run Screen when the New Cut Test Option 1 is on

static const int Reserved_CameraSystemPredictions           = 999040;       //  Edgify and Tiliter Camera Predictions for Self Service CAMERA

// 4.2.100 and 5.2.100 will start moving towards Global 888 Flashkey values
static const int Reserved_FreshServeButtons                 = 888050;    	// Left the early ones open for consistency although not all of them will be used 
																			
//	END - Reserved for reference to a softkey Feature, Group or Style

#endif //SOFTKEYBASICS_H
