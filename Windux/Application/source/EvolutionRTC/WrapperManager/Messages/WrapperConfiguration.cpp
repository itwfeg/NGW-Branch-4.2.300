//#include "vxWorks.h"

#include "WrapperConfiguration.h"
#include "WrapperTime.h"
#include "WrapperTiming.h"
#include "WrapperGripperTiming.h"
#include "WrapperPackCodeInfo.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperConfiguration::dataType = 0;
std::string WrapperConfiguration::dataTypeName("WrapperConfiguration"); 
#endif

WrapperConfiguration::WrapperConfiguration(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Wrapper, WrConfig, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
    /***********************************************
    * Set the Wrapper default configuration block. *
    ***********************************************/

	configType = WrDefault;

	deviceSignature[0] = 'W';
	deviceSignature[1] = 'R';
	deviceSignature[2] = '\0';
	deviceSignature[3] = '\0';
	deviceSignature[4] = '\0';
	deviceSignature[5] = '\0';

	wideFilmTestEnable = true;
	narrowFilmTestEnable = true;
	celsiusEnable = false;
	heaterEnable = false;
	heaterSetting = 300;      

	packageLengthAdjustment = 0;    
	packageWidthAdjustment = 0;        

	pauseTimeout = 15; // seconds
	idleTimeout = 1; // machine cycles
	filmTension = MediumFilmTension;
	packWidth = AutoWidthSelect;

    /********************************************
	* Removed to disable setting totals during 	*
	* default eep configuration 				*
	********************************************/
	// packageGrandTotal = 0;
    // wrapperCycleTotal = 0;

	elevatorCollapseTotal = 0;    
	intakeJamTotal = 0;    
	lengthJamTotal = 0;    
	widthJamTotal = 0;    
	coverOpenTotal = 0;    
	sealerBeltOpenTotal = 0;    
	packageTooTallTotal = 0;    
	pusherHomeErrorTotal = 0;    
	filmPullErrorTotal = 0;    

	errorTotalsDate = 0;    
	packCodeTotalsDate = 0;    
    
    /*******************************************
     * Set the default heartbeat timing values. *
     *******************************************/

	heartbeat = WrapperTiming(0, 0, 255, 0, 0, 255);

    /*******************************
    * Default Gripper PWM settings *
    *******************************/

	WrapperTime x(0,60,255);
	gripperSetback0 = WrapperGripperTiming(x, Pwm038Gripper);

	x = WrapperTime(0,100,255);
	gripperSetback1 = WrapperGripperTiming(x, IgnoreGripperPwm);

	x = WrapperTime(0,140,255);
	gripperSetback2 = WrapperGripperTiming(x, Pwm074Gripper);

	x = WrapperTime(0,180,255);
	gripperSetback3 = WrapperGripperTiming(x, IgnoreGripperPwm);
    
    /***********************************************************************************
     * Set the default pack code 0 (formerly pack code 0 narrow) specific timing values.*
     ************************************************************************************/

    // WrPackCodeInfo *pack_code = &(config->pack_code[PACK_CODE_0]);
    
	packCode[PackCode0].setDescription((UINT8 *) "1S,2S,5S,10S,17S");

	packCode[PackCode0].setPackCode(PackCode0);
	packCode[PackCode0].setPackWidth(NarrowPackage);
	packCode[PackCode0].setPackLength(ShortPackage);

	packCode[PackCode0].clearSlideBlock();
	packCode[PackCode0].clearBooster();

	packCode[PackCode0].setPackSizeMinLength(93);
	packCode[PackCode0].setPackSizeMaxLength(130);
	packCode[PackCode0].setPackSizeMinWidth(121);
	packCode[PackCode0].setPackSizeMaxWidth(255);

	packCode[PackCode0].setSideClampOnMin(110);
	packCode[PackCode0].setSideClampOnValue(117);
	packCode[PackCode0].setSideClampOnMax(135);
	packCode[PackCode0].setSideClampOffMin(175);
	packCode[PackCode0].setSideClampOffValue(192);
	packCode[PackCode0].setSideClampOffMax(200);

	packCode[PackCode0].setGripperOnMin(30);
	packCode[PackCode0].setGripperOnValue(40);
	packCode[PackCode0].setGripperOnMax(45);
	packCode[PackCode0].setGripperOffMin(220);
	packCode[PackCode0].setGripperOffValue(231);
	packCode[PackCode0].setGripperOffMax(240);

	packCode[PackCode0].setKnifeOnMin(160);
	packCode[PackCode0].setKnifeOnValue(192);
	packCode[PackCode0].setKnifeOnMax(215);
	packCode[PackCode0].setKnifeOffMin(250);
	packCode[PackCode0].setKnifeOffValue(250);
	packCode[PackCode0].setKnifeOffMax(250);

	packCode[PackCode0].setPackageTotal(0);
	packCode[PackCode0].setJamTotal(0);
        
	/**********************************************************************************
	 * Set the default pack code 1 (formerly pack code 0 wide) specific timing values.*
	 **********************************************************************************/
    
	// pack_code = &(config->pack_code[PACK_CODE_1]);

	packCode[PackCode1].setDescription((UINT8 *) "7S");

	packCode[PackCode1].setPackCode(PackCode1);
	packCode[PackCode1].setPackWidth(WidePackage);
	packCode[PackCode1].setPackLength(ShortPackage);

	packCode[PackCode1].setSlideBlock();
	packCode[PackCode1].clearBooster();

	packCode[PackCode1].setPackSizeMinLength(93);
	packCode[PackCode1].setPackSizeMaxLength(130);
	packCode[PackCode1].setPackSizeMinWidth(20);
	packCode[PackCode1].setPackSizeMaxWidth(120);

	packCode[PackCode1].setSideClampOnMin(110);
	packCode[PackCode1].setSideClampOnValue(130);
	packCode[PackCode1].setSideClampOnMax(130);
	packCode[PackCode1].setSideClampOffMin(180);
	packCode[PackCode1].setSideClampOffValue(186);
	packCode[PackCode1].setSideClampOffMax(200);

	packCode[PackCode1].setGripperOnMin(30);
	packCode[PackCode1].setGripperOnValue(40);
	packCode[PackCode1].setGripperOnMax(45);
	packCode[PackCode1].setGripperOffMin(220);
	packCode[PackCode1].setGripperOffValue(230);
	packCode[PackCode1].setGripperOffMax(240);

	packCode[PackCode1].setKnifeOnMin(170);
	packCode[PackCode1].setKnifeOnValue(194);
	packCode[PackCode1].setKnifeOnMax(215);
	packCode[PackCode1].setKnifeOffMin(250);
	packCode[PackCode1].setKnifeOffValue(250);
	packCode[PackCode1].setKnifeOffMax(250);

	packCode[PackCode1].setPackageTotal(0);
	packCode[PackCode1].setJamTotal(0);

	/*************************************************************************************
	 * Set the default pack code 2 (formerly pack code 1 narrow) specific timing values. *
	 *************************************************************************************/

    // pack_code = &(config->pack_code[PACK_CODE_2]);
    
	packCode[PackCode2].setDescription((UINT8 *) "20S,4S");

	packCode[PackCode2].setPackCode(PackCode2);
	packCode[PackCode2].setPackWidth(NarrowPackage);
	packCode[PackCode2].setPackLength(MediumPackage);

	packCode[PackCode2].setSlideBlock();
	packCode[PackCode2].clearBooster();

	packCode[PackCode2].setPackSizeMinLength(78);
	packCode[PackCode2].setPackSizeMaxLength(92);
	packCode[PackCode2].setPackSizeMinWidth(121);
	packCode[PackCode2].setPackSizeMaxWidth(255);

	packCode[PackCode2].setSideClampOnMin(110);
	packCode[PackCode2].setSideClampOnValue(117);
	packCode[PackCode2].setSideClampOnMax(130);
	packCode[PackCode2].setSideClampOffMin(180);
	packCode[PackCode2].setSideClampOffValue(190);
	packCode[PackCode2].setSideClampOffMax(200);

	packCode[PackCode2].setGripperOnMin(30);
	packCode[PackCode2].setGripperOnValue(40);
	packCode[PackCode2].setGripperOnMax(45);
	packCode[PackCode2].setGripperOffMin(220);
	packCode[PackCode2].setGripperOffValue(232);
	packCode[PackCode2].setGripperOffMax(240);

	packCode[PackCode2].setKnifeOnMin(170);
	packCode[PackCode2].setKnifeOnValue(185);
	packCode[PackCode2].setKnifeOnMax(215);
	packCode[PackCode2].setKnifeOffMin(250);
	packCode[PackCode2].setKnifeOffValue(250);
	packCode[PackCode2].setKnifeOffMax(250);

	packCode[PackCode2].setPackageTotal(0);
	packCode[PackCode2].setJamTotal(0);

	/***********************************************************************************
	 * Set the default pack code 3 (formerly pack code 1 wide) specific timing values. *
     ***********************************************************************************/
    
    // pack_code = &(config->pack_code[PACK_CODE_3]);

	packCode[PackCode3].setDescription((UINT8 *) "16S");

	packCode[PackCode3].setPackCode(PackCode3);
	packCode[PackCode3].setPackWidth(WidePackage);
	packCode[PackCode3].setPackLength(MediumPackage);

	packCode[PackCode3].setSlideBlock();
	packCode[PackCode3].clearBooster();

	packCode[PackCode3].setPackSizeMinLength(78);
	packCode[PackCode3].setPackSizeMaxLength(92);
	packCode[PackCode3].setPackSizeMinWidth(20);
	packCode[PackCode3].setPackSizeMaxWidth(120);

	packCode[PackCode3].setSideClampOnMin(110);
	packCode[PackCode3].setSideClampOnValue(125);
	packCode[PackCode3].setSideClampOnMax(130);
	packCode[PackCode3].setSideClampOffMin(180);
	packCode[PackCode3].setSideClampOffValue(185);
	packCode[PackCode3].setSideClampOffMax(200);

	packCode[PackCode3].setGripperOnMin(30);
	packCode[PackCode3].setGripperOnValue(40);
	packCode[PackCode3].setGripperOnMax(45);
	packCode[PackCode3].setGripperOffMin(220);
	packCode[PackCode3].setGripperOffValue(227);
	packCode[PackCode3].setGripperOffMax(240);

	packCode[PackCode3].setKnifeOnMin(170);
	packCode[PackCode3].setKnifeOnValue(185);
	packCode[PackCode3].setKnifeOnMax(215);
	packCode[PackCode3].setKnifeOffMin(250);
	packCode[PackCode3].setKnifeOffValue(250);
	packCode[PackCode3].setKnifeOffMax(250);

	packCode[PackCode3].setPackageTotal(0);
	packCode[PackCode3].setJamTotal(0);
 
	/*************************************************************************************
	 * Set the default pack code 4 (formerly pack code 2 narrow) specific timing values. *
	 *************************************************************************************/
    
    // pack_code = &(config->pack_code[PACK_CODE_4]);

	packCode[PackCode4].setDescription((UINT8 *) "8S");

	packCode[PackCode4].setPackCode(PackCode4);
	packCode[PackCode4].setPackWidth(NarrowPackage);
	packCode[PackCode4].setPackLength(LongPackage);

	packCode[PackCode4].setSlideBlock();
	packCode[PackCode4].clearBooster();

	packCode[PackCode4].setPackSizeMinLength(46);
	packCode[PackCode4].setPackSizeMaxLength(77);
	packCode[PackCode4].setPackSizeMinWidth(121);
	packCode[PackCode4].setPackSizeMaxWidth(255);

	packCode[PackCode4].setSideClampOnMin(110);
	packCode[PackCode4].setSideClampOnValue(117);
	packCode[PackCode4].setSideClampOnMax(130);
	packCode[PackCode4].setSideClampOffMin(180);
	packCode[PackCode4].setSideClampOffValue(193);
	packCode[PackCode4].setSideClampOffMax(200);

	packCode[PackCode4].setGripperOnMin(30);
	packCode[PackCode4].setGripperOnValue(40);
	packCode[PackCode4].setGripperOnMax(45);
	packCode[PackCode4].setGripperOffMin(220);
	packCode[PackCode4].setGripperOffValue(228);
	packCode[PackCode4].setGripperOffMax(240);

	packCode[PackCode4].setKnifeOnMin(170);
	packCode[PackCode4].setKnifeOnValue(185);
	packCode[PackCode4].setKnifeOnMax(215);
	packCode[PackCode4].setKnifeOffMin(250);
	packCode[PackCode4].setKnifeOffValue(250);
	packCode[PackCode4].setKnifeOffMax(250);

	packCode[PackCode4].setPackageTotal(0);
	packCode[PackCode4].setJamTotal(0);
     
	/***********************************************************************************
	 * Set the default pack code 5 (formerly pack code 2 wide) specific timing values. *
	 ***********************************************************************************/
    
	// pack_code = &(config->pack_code[PACK_CODE_5]);

	packCode[PackCode5].setDescription((UINT8 *) "9S,12S,10x14");

	packCode[PackCode5].setPackCode(PackCode5);
	packCode[PackCode5].setPackWidth(WidePackage);
	packCode[PackCode5].setPackLength(LongPackage);

	packCode[PackCode5].setSlideBlock();
	packCode[PackCode5].clearBooster();

	packCode[PackCode5].setPackSizeMinLength(46);
	packCode[PackCode5].setPackSizeMaxLength(77);
	packCode[PackCode5].setPackSizeMinWidth(20);
	packCode[PackCode5].setPackSizeMaxWidth(120);

	packCode[PackCode5].setSideClampOnMin(110);
	packCode[PackCode5].setSideClampOnValue(122);
	packCode[PackCode5].setSideClampOnMax(145);
	packCode[PackCode5].setSideClampOffMin(165);
	packCode[PackCode5].setSideClampOffValue(188);
	packCode[PackCode5].setSideClampOffMax(200);

	packCode[PackCode5].setGripperOnMin(30);
	packCode[PackCode5].setGripperOnValue(40);
	packCode[PackCode5].setGripperOnMax(45);
	packCode[PackCode5].setGripperOffMin(220);
	packCode[PackCode5].setGripperOffValue(229);
	packCode[PackCode5].setGripperOffMax(240);

	packCode[PackCode5].setKnifeOnMin(170);
	packCode[PackCode5].setKnifeOnValue(185);
	packCode[PackCode5].setKnifeOnMax(215);
	packCode[PackCode5].setKnifeOffMin(250);
	packCode[PackCode5].setKnifeOffValue(250);
	packCode[PackCode5].setKnifeOffMax(250);

	packCode[PackCode5].setPackageTotal(0);
	packCode[PackCode5].setJamTotal(0);

	/********************************************************
	 * Set the default pack code 6 specific timing values . *
	 ********************************************************/

    // pack_code = &(config->pack_code[PACK_CODE_6]);
    
	packCode[PackCode6].setDescription((UINT8 *) "25");

	packCode[PackCode6].setPackCode(PackCode6);
	packCode[PackCode6].setPackWidth(WidePackage);
	packCode[PackCode6].setPackLength(LongPackage);

	packCode[PackCode6].setSlideBlock();
	packCode[PackCode6].clearBooster();

	packCode[PackCode6].setPackSizeMinLength(68);
	packCode[PackCode6].setPackSizeMaxLength(81);
	packCode[PackCode6].setPackSizeMinWidth(60);
	packCode[PackCode6].setPackSizeMaxWidth(78);

	packCode[PackCode6].setSideClampOnMin(110);
	packCode[PackCode6].setSideClampOnValue(134);
	packCode[PackCode6].setSideClampOnMax(145);
	packCode[PackCode6].setSideClampOffMin(165);
	packCode[PackCode6].setSideClampOffValue(184);
	packCode[PackCode6].setSideClampOffMax(200);

	packCode[PackCode6].setGripperOnMin(30);
	packCode[PackCode6].setGripperOnValue(40);
	packCode[PackCode6].setGripperOnMax(45);
	packCode[PackCode6].setGripperOffMin(220);
	packCode[PackCode6].setGripperOffValue(226);
	packCode[PackCode6].setGripperOffMax(240);

	packCode[PackCode6].setKnifeOnMin(170);
	packCode[PackCode6].setKnifeOnValue(182);
	packCode[PackCode6].setKnifeOnMax(215);
	packCode[PackCode6].setKnifeOffMin(250);
	packCode[PackCode6].setKnifeOffValue(250);
	packCode[PackCode6].setKnifeOffMax(250);

	packCode[PackCode6].setPackageTotal(0);
	packCode[PackCode6].setJamTotal(0);
        
	/***********************************************************
	 * Initialize the pack code 7 user programmable pack code .*
	 ***********************************************************/
	// pack_code = &(config->pack_code[PACK_CODE_7]);
    
	packCode[PackCode7].setDescription((UINT8 *) "User Defined 8");

	packCode[PackCode7].setPackCode(PackCode7);
	packCode[PackCode7].setPackWidth(NarrowPackage);
	packCode[PackCode7].setPackLength(ShortPackage);

	packCode[PackCode7].setSlideBlock();
	packCode[PackCode7].clearBooster();

	packCode[PackCode7].setPackSizeMinLength(0);
	packCode[PackCode7].setPackSizeMaxLength(0);
	packCode[PackCode7].setPackSizeMinWidth(0);
	packCode[PackCode7].setPackSizeMaxWidth(0);

	packCode[PackCode7].setSideClampOnMin(110);
	packCode[PackCode7].setSideClampOnValue(117);
	packCode[PackCode7].setSideClampOnMax(135);
	packCode[PackCode7].setSideClampOffMin(170);
	packCode[PackCode7].setSideClampOffValue(195);
	packCode[PackCode7].setSideClampOffMax(205);

	packCode[PackCode7].setGripperOnMin(30);
	packCode[PackCode7].setGripperOnValue(40);
	packCode[PackCode7].setGripperOnMax(45);
	packCode[PackCode7].setGripperOffMin(220);
	packCode[PackCode7].setGripperOffValue(235);
	packCode[PackCode7].setGripperOffMax(240);

	packCode[PackCode7].setKnifeOnMin(160);
	packCode[PackCode7].setKnifeOnValue(195);
	packCode[PackCode7].setKnifeOnMax(215);
	packCode[PackCode7].setKnifeOffMin(250);
	packCode[PackCode7].setKnifeOffValue(250);
	packCode[PackCode7].setKnifeOffMax(250);

	packCode[PackCode7].setPackageTotal(0);
	packCode[PackCode7].setJamTotal(0);

	/***********************************************************
	 * Initialize the pack code 8 user programmable pack code .*
	 ***********************************************************/
    // pack_code = &(config->pack_code[PACK_CODE_8]);
    
	packCode[PackCode8].setDescription((UINT8 *) "User Defined 9");

	packCode[PackCode8].setPackCode(PackCode8);
	packCode[PackCode8].setPackWidth(NarrowPackage);
	packCode[PackCode8].setPackLength(ShortPackage);

	packCode[PackCode8].setSlideBlock();
	packCode[PackCode8].clearBooster();

	packCode[PackCode8].setPackSizeMinLength(0);
	packCode[PackCode8].setPackSizeMaxLength(0);
	packCode[PackCode8].setPackSizeMinWidth(0);
	packCode[PackCode8].setPackSizeMaxWidth(0);

	packCode[PackCode8].setSideClampOnMin(110);
	packCode[PackCode8].setSideClampOnValue(117);
	packCode[PackCode8].setSideClampOnMax(135);
	packCode[PackCode8].setSideClampOffMin(170);
	packCode[PackCode8].setSideClampOffValue(195);
	packCode[PackCode8].setSideClampOffMax(205);

	packCode[PackCode8].setGripperOnMin(30);
	packCode[PackCode8].setGripperOnValue(40);
	packCode[PackCode8].setGripperOnMax(45);
	packCode[PackCode8].setGripperOffMin(220);
	packCode[PackCode8].setGripperOffValue(235);
	packCode[PackCode8].setGripperOffMax(240);

	packCode[PackCode8].setKnifeOnMin(160);
	packCode[PackCode8].setKnifeOnValue(195);
	packCode[PackCode8].setKnifeOnMax(215);
	packCode[PackCode8].setKnifeOffMin(250);
	packCode[PackCode8].setKnifeOffValue(250);
	packCode[PackCode8].setKnifeOffMax(250);

	packCode[PackCode8].setPackageTotal(0);
	packCode[PackCode8].setJamTotal(0);

	/***********************************************************
	 * Initialize the pack code 9 user programmable pack code .*
	 ***********************************************************/

    // pack_code = &(config->pack_code[PACK_CODE_9]);
    
	packCode[PackCode9].setDescription((UINT8 *) "User Defined 10");

	packCode[PackCode9].setPackCode(PackCode9);
	packCode[PackCode9].setPackWidth(NarrowPackage);
	packCode[PackCode9].setPackLength(ShortPackage);

	packCode[PackCode9].setSlideBlock();
	packCode[PackCode9].clearBooster();

	packCode[PackCode9].setPackSizeMinLength(0);
	packCode[PackCode9].setPackSizeMaxLength(0);
	packCode[PackCode9].setPackSizeMinWidth(0);
	packCode[PackCode9].setPackSizeMaxWidth(0);

	packCode[PackCode9].setSideClampOnMin(110);
	packCode[PackCode9].setSideClampOnValue(117);
	packCode[PackCode9].setSideClampOnMax(135);
	packCode[PackCode9].setSideClampOffMin(170);
	packCode[PackCode9].setSideClampOffValue(195);
	packCode[PackCode9].setSideClampOffMax(205);

	packCode[PackCode9].setGripperOnMin(30);
	packCode[PackCode9].setGripperOnValue(40);
	packCode[PackCode9].setGripperOnMax(45);
	packCode[PackCode9].setGripperOffMin(220);
	packCode[PackCode9].setGripperOffValue(235);
	packCode[PackCode9].setGripperOffMax(240);

	packCode[PackCode9].setKnifeOnMin(160);
	packCode[PackCode9].setKnifeOnValue(195);
	packCode[PackCode9].setKnifeOnMax(215);
	packCode[PackCode9].setKnifeOffMin(250);
	packCode[PackCode9].setKnifeOffValue(250);
	packCode[PackCode9].setKnifeOffMax(250);

	packCode[PackCode9].setPackageTotal(0);
	packCode[PackCode9].setJamTotal(0);

	/************************************************************
	 * Initialize the pack code 10 user programmable pack code .*
	 ************************************************************/

	// pack_code = &(config->pack_code[PACK_CODE_10]);
    
	packCode[PackCode10].setDescription((UINT8 *) "User Defined 11");

	packCode[PackCode10].setPackCode(PackCode10);
	packCode[PackCode10].setPackWidth(NarrowPackage);
	packCode[PackCode10].setPackLength(ShortPackage);

	packCode[PackCode10].setSlideBlock();
	packCode[PackCode10].clearBooster();

	packCode[PackCode10].setPackSizeMinLength(0);
	packCode[PackCode10].setPackSizeMaxLength(0);
	packCode[PackCode10].setPackSizeMinWidth(0);
	packCode[PackCode10].setPackSizeMaxWidth(0);

	packCode[PackCode10].setSideClampOnMin(110);
	packCode[PackCode10].setSideClampOnValue(117);
	packCode[PackCode10].setSideClampOnMax(135);
	packCode[PackCode10].setSideClampOffMin(170);
	packCode[PackCode10].setSideClampOffValue(195);
	packCode[PackCode10].setSideClampOffMax(205);

	packCode[PackCode10].setGripperOnMin(30);
	packCode[PackCode10].setGripperOnValue(40);
	packCode[PackCode10].setGripperOnMax(45);
	packCode[PackCode10].setGripperOffMin(220);
	packCode[PackCode10].setGripperOffValue(235);
	packCode[PackCode10].setGripperOffMax(240);

	packCode[PackCode10].setKnifeOnMin(160);
	packCode[PackCode10].setKnifeOnValue(195);
	packCode[PackCode10].setKnifeOnMax(215);
	packCode[PackCode10].setKnifeOffMin(250);
	packCode[PackCode10].setKnifeOffValue(250);
	packCode[PackCode10].setKnifeOffMax(250);

	packCode[PackCode10].setPackageTotal(0);
	packCode[PackCode10].setJamTotal(0);

	/************************************************************
	 * Initialize the pack code 11 user programmable pack code .*
	 ************************************************************/

	// pack_code = &(config->pack_code[PACK_CODE_11]);
    
	packCode[PackCode11].setDescription((UINT8 *) "User Defined 12");

	packCode[PackCode11].setPackCode(PackCode11);
	packCode[PackCode11].setPackWidth(NarrowPackage);
	packCode[PackCode11].setPackLength(ShortPackage);

	packCode[PackCode11].setSlideBlock();
	packCode[PackCode11].clearBooster();

	packCode[PackCode11].setPackSizeMinLength(0);
	packCode[PackCode11].setPackSizeMaxLength(0);
	packCode[PackCode11].setPackSizeMinWidth(0);
	packCode[PackCode11].setPackSizeMaxWidth(0);

	packCode[PackCode11].setSideClampOnMin(110);
	packCode[PackCode11].setSideClampOnValue(117);
	packCode[PackCode11].setSideClampOnMax(135);
	packCode[PackCode11].setSideClampOffMin(170);
	packCode[PackCode11].setSideClampOffValue(195);
	packCode[PackCode11].setSideClampOffMax(205);

	packCode[PackCode11].setGripperOnMin(30);
	packCode[PackCode11].setGripperOnValue(40);
	packCode[PackCode11].setGripperOnMax(45);
	packCode[PackCode11].setGripperOffMin(220);
	packCode[PackCode11].setGripperOffValue(235);
	packCode[PackCode11].setGripperOffMax(240);

	packCode[PackCode11].setKnifeOnMin(160);
	packCode[PackCode11].setKnifeOnValue(195);
	packCode[PackCode11].setKnifeOnMax(215);
	packCode[PackCode11].setKnifeOffMin(250);
	packCode[PackCode11].setKnifeOffValue(250);
	packCode[PackCode11].setKnifeOffMax(250);

	packCode[PackCode11].setPackageTotal(0);
	packCode[PackCode11].setJamTotal(0);
}

WrapperConfiguration::WrapperConfiguration(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &configType;
	for (unsigned int i = 0; i < sizeof(configType); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &deviceSignature;
	for (unsigned int i = 0; i < sizeof(deviceSignature); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pauseTimeout;
	for (unsigned int i = 0; i < sizeof(pauseTimeout); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &idleTimeout;
	for (unsigned int i = 0; i < sizeof(idleTimeout); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &filmTension;
	for (unsigned int i = 0; i < sizeof(filmTension); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packWidth;
	for (unsigned int i = 0; i < sizeof(packWidth); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageGrandTotal;
	for (unsigned int i = 0; i < sizeof(packageGrandTotal); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &wrapperCycleTotal;
	for (unsigned int i = 0; i < sizeof(wrapperCycleTotal); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &elevatorCollapseTotal;
	for (unsigned int i = 0; i < sizeof(elevatorCollapseTotal); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &intakeJamTotal;
	for (unsigned int i = 0; i < sizeof(intakeJamTotal); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &lengthJamTotal;
	for (unsigned int i = 0; i < sizeof(lengthJamTotal); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &widthJamTotal;
	for (unsigned int i = 0; i < sizeof(widthJamTotal); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &coverOpenTotal;
	for (unsigned int i = 0; i < sizeof(coverOpenTotal); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sealerBeltOpenTotal;
	for (unsigned int i = 0; i < sizeof(sealerBeltOpenTotal); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageTooTallTotal;
	for (unsigned int i = 0; i < sizeof(packageTooTallTotal); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pusherHomeErrorTotal;
	for (unsigned int i = 0; i < sizeof(pusherHomeErrorTotal); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &filmPullErrorTotal;
	for (unsigned int i = 0; i < sizeof(filmPullErrorTotal); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &celsiusEnable;
	for (unsigned int i = 0; i < sizeof(celsiusEnable); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &wideFilmTestEnable;
	for (unsigned int i = 0; i < sizeof(wideFilmTestEnable); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &narrowFilmTestEnable;
	for (unsigned int i = 0; i < sizeof(narrowFilmTestEnable); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &heaterEnable;
	for (unsigned int i = 0; i < sizeof(heaterEnable); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &heaterSetting;
	for (unsigned int i = 0; i < sizeof(heaterSetting); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageLengthAdjustment;
	for (unsigned int i = 0; i < sizeof(packageLengthAdjustment); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageWidthAdjustment;
	for (unsigned int i = 0; i < sizeof(packageWidthAdjustment); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &errorTotalsDate;
	for (unsigned int i = 0; i < sizeof(errorTotalsDate); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packCodeTotalsDate;
	for (unsigned int i = 0; i < sizeof(packCodeTotalsDate); i++)
	{
		*pChar++ = buffer[j++];
	}

	// fills in the heartbeat!!
	heartbeat = WrapperTiming(&buffer[j]);

	heartbeat.serialize();
	j += heartbeat.getSerializedLength();

	// fills in the gripper Setbacks!!
	gripperSetback0 = WrapperGripperTiming(&buffer[j]);

	gripperSetback0.serialize();
	j += gripperSetback0.getSerializedLength();

	gripperSetback1 = WrapperGripperTiming(&buffer[j]);

	gripperSetback1.serialize();
	j += gripperSetback1.getSerializedLength();

	gripperSetback2 = WrapperGripperTiming(&buffer[j]);

	gripperSetback2.serialize();
	j += gripperSetback2.getSerializedLength();

	gripperSetback3 = WrapperGripperTiming(&buffer[j]);

	gripperSetback3.serialize();
	j += gripperSetback3.getSerializedLength();

	for (int i = 0; i < NumberPackCodes; i++)
	{
		packCode[i] = WrapperPackCodeInfo(&buffer[j]);

		packCode[i].serialize();
		j += packCode[i].getSerializedLength();
	}
}

WrapperConfiguration::~WrapperConfiguration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *WrapperConfiguration::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &configType;
	for (unsigned int i = 0; i < sizeof(configType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &deviceSignature;
	for (unsigned int i = 0; i < sizeof(deviceSignature); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pauseTimeout;
	for (unsigned int i = 0; i < sizeof(pauseTimeout); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &idleTimeout;
	for (unsigned int i = 0; i < sizeof(idleTimeout); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmTension;
	for (unsigned int i = 0; i < sizeof(filmTension); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packWidth;
	for (unsigned int i = 0; i < sizeof(packWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageGrandTotal;
	for (unsigned int i = 0; i < sizeof(packageGrandTotal); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &wrapperCycleTotal;
	for (unsigned int i = 0; i < sizeof(wrapperCycleTotal); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &elevatorCollapseTotal;
	for (unsigned int i = 0; i < sizeof(elevatorCollapseTotal); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &intakeJamTotal;
	for (unsigned int i = 0; i < sizeof(intakeJamTotal); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &lengthJamTotal;
	for (unsigned int i = 0; i < sizeof(lengthJamTotal); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &widthJamTotal;
	for (unsigned int i = 0; i < sizeof(widthJamTotal); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &coverOpenTotal;
	for (unsigned int i = 0; i < sizeof(coverOpenTotal); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sealerBeltOpenTotal;
	for (unsigned int i = 0; i < sizeof(sealerBeltOpenTotal); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageTooTallTotal;
	for (unsigned int i = 0; i < sizeof(packageTooTallTotal); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pusherHomeErrorTotal;
	for (unsigned int i = 0; i < sizeof(pusherHomeErrorTotal); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmPullErrorTotal;
	for (unsigned int i = 0; i < sizeof(filmPullErrorTotal); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &celsiusEnable;
	for (unsigned int i = 0; i < sizeof(celsiusEnable); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &wideFilmTestEnable;
	for (unsigned int i = 0; i < sizeof(wideFilmTestEnable); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &narrowFilmTestEnable;
	for (unsigned int i = 0; i < sizeof(narrowFilmTestEnable); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &heaterEnable;
	for (unsigned int i = 0; i < sizeof(heaterEnable); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &heaterSetting;
	for (unsigned int i = 0; i < sizeof(heaterSetting); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageLengthAdjustment;
	for (unsigned int i = 0; i < sizeof(packageLengthAdjustment); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageWidthAdjustment;
	for (unsigned int i = 0; i < sizeof(packageWidthAdjustment); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &errorTotalsDate;
	for (unsigned int i = 0; i < sizeof(errorTotalsDate); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packCodeTotalsDate;
	for (unsigned int i = 0; i < sizeof(packCodeTotalsDate); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	const char *pCC = heartbeat.serialize();
	int mLength = heartbeat.getSerializedLength();

	serializedData.append(pCC, mLength);
	
	pCC = gripperSetback0.serialize();
	mLength = gripperSetback0.getSerializedLength();

	serializedData.append(pCC, mLength);

	pCC = gripperSetback1.serialize();
	mLength = gripperSetback1.getSerializedLength();

	serializedData.append(pCC, mLength);

	pCC = gripperSetback2.serialize();
	mLength = gripperSetback2.getSerializedLength();

	serializedData.append(pCC, mLength);

	pCC = gripperSetback3.serialize();
	mLength = gripperSetback3.getSerializedLength();

	serializedData.append(pCC, mLength);
	
	for (int i = 0; i < NumberPackCodes; i++)
	{
		pCC = packCode[i].serialize();
		mLength = packCode[i].getSerializedLength();

		serializedData.append(pCC, mLength);
	}
	return serializedData.data();
}

void WrapperConfiguration::show()
{
	Debug::addTraceCategory("WrapperConfiguration");
	Debug::trace("WrapperConfiguration", "WrapperConfiguration: ");

	EvolMessage::show();

	if (configType == WrPermanent)
	{
		Debug::trace("WrapperConfiguration", "configType: Permanent");
	}
	else if (configType == WrTemporary)
	{
		Debug::trace("WrapperConfiguration", "configType: WrTemporary");
	}
	else if (configType == WrDefault)
	{
		Debug::trace("WrapperConfiguration", "configType: WrDefault");
	}
	else if ((configType & WrCalibInProgress) == WrCalibInProgress)
	{
		Debug::trace("WrapperConfiguration", "configType: WrCalibInProgress");
	}
	else
	{
		Debug::trace("WrapperConfiguration", HobString("configType: Unknown ") +
											 HobString(Integer::toString((UINT16) configType)));
	}

	Debug::trace("WrapperConfiguration", Hex::toString((const char *) &deviceSignature[0], SystemDevSignatureSize));

	Debug::trace("WrapperConfiguration", HobString("pauseTimeout: ") +
										 HobString(Integer::toString((UINT16) pauseTimeout)));
	Debug::trace("WrapperConfiguration", HobString("idleTimeout: ") +
										 HobString(Integer::toString((UINT16) idleTimeout)));

	if (filmTension == LowFilmTension)
	{
		Debug::trace("WrapperConfiguration", "filmTension: LowFilmTension!");
	}
	else if (filmTension == MediumFilmTension)
	{
		Debug::trace("WrapperConfiguration", "filmTension: MediumFilmTension!");
	}
	else if (filmTension == HighFilmTension)
	{
		Debug::trace("WrapperConfiguration", "filmTension: HighFilmTension!");
	}
	else
	{
		Debug::trace("WrapperConfiguration", HobString("filmTension: Unknown ") +
											 HobString(Integer::toString((UINT16) filmTension)));
	}
	
	if (packWidth == NarrowPackage)
	{
		Debug::trace("WrapperConfiguration", "packWidth: NarrowPackage!");
	}
	else if (packWidth == WidePackage)
	{
		Debug::trace("WrapperConfiguration", "packWidth: WidePackage!");
	}
	else if (packWidth == FilmWidthIdle)
	{
		Debug::trace("WrapperConfiguration", "packWidth: FilmWidthIdle!");
	}
	else if (packWidth == AutoWidthSelect)
	{
		Debug::trace("WrapperConfiguration", "packWidth: AutoWidthSelect!");
	}
	else
	{
		Debug::trace("WrapperConfiguration", HobString("packWidth: Unknown ") +
											 HobString(Integer::toString((UINT16) packWidth)));
	}
	
	Debug::trace("WrapperConfiguration", HobString("packageGrandTotal: ") +
										 HobString(Integer::toString((UINT16) packageGrandTotal)));
	Debug::trace("WrapperConfiguration", HobString("wrapperCycleTotal: ") +
										 HobString(Integer::toString((UINT16) wrapperCycleTotal)));
	Debug::trace("WrapperConfiguration", HobString("elevatorCollapseTotal: ") +
										 HobString(Integer::toString((UINT16) elevatorCollapseTotal)));
	Debug::trace("WrapperConfiguration", HobString("intakeJamTotal: ") +
										 HobString(Integer::toString((UINT16) intakeJamTotal)));
	Debug::trace("WrapperConfiguration", HobString("lengthJamTotal: ") +
										 HobString(Integer::toString((UINT16) lengthJamTotal)));
	Debug::trace("WrapperConfiguration", HobString("widthJamTotal: ") +
										 HobString(Integer::toString((UINT16) widthJamTotal)));
	Debug::trace("WrapperConfiguration", HobString("coverOpenTotal: ") +
										 HobString(Integer::toString((UINT16) coverOpenTotal)));
	Debug::trace("WrapperConfiguration", HobString("sealerBeltOpenTotal: ") +
										 HobString(Integer::toString((UINT16) sealerBeltOpenTotal)));
	Debug::trace("WrapperConfiguration", HobString("packageTooTallTotal: ") +
										 HobString(Integer::toString((UINT16) packageTooTallTotal)));
	Debug::trace("WrapperConfiguration", HobString("pusherHomeErrorTotal: ") +
										 HobString(Integer::toString((UINT16) pusherHomeErrorTotal)));
	Debug::trace("WrapperConfiguration", HobString("filmPullErrorTotal: ") +
										 HobString(Integer::toString((UINT16) filmPullErrorTotal)));

	if (celsiusEnable == true)
	{
		Debug::trace("WrapperConfiguration", "Tempatures in Celsius");
	}
	else
	{
		Debug::trace("WrapperConfiguration", "Tempatures in Fahrenheit");
	}

	if (wideFilmTestEnable == true)
	{
		Debug::trace("WrapperConfiguration", "Wide Film Test Enabled");
	}
	else
	{
		Debug::trace("WrapperConfiguration", "Wide Film Test NOT Enabled");
	}

	if (narrowFilmTestEnable == true)
	{
		Debug::trace("WrapperConfiguration", "Narrow Film Test Enabled");
	}
	else
	{
		Debug::trace("WrapperConfiguration", "Narrow Film Test NOT Enabled");
	}

	if (heaterEnable == true)
	{
		Debug::trace("WrapperConfiguration", "Heater Enabled");
	}
	else
	{
		Debug::trace("WrapperConfiguration", "Heater NOT Enabled");
	}

	Debug::trace("WrapperConfiguration", HobString("heaterSetting: ") +
										 HobString(Integer::toString((UINT16) heaterSetting)));
	Debug::trace("WrapperConfiguration", HobString("packageLengthAdjustment: ") +
										 HobString(Integer::toString((UINT16) packageLengthAdjustment)));
	Debug::trace("WrapperConfiguration", HobString("packageWidthAdjustment: ") +
										 HobString(Integer::toString((UINT16) packageWidthAdjustment)));
	Debug::trace("WrapperConfiguration", HobString("errorTotalsDate: ") +
										 HobString(Integer::toString((UINT16) errorTotalsDate)));
	Debug::trace("WrapperConfiguration", HobString("packCodeTotalsDate: ") +
										 HobString(Integer::toString((UINT16) packCodeTotalsDate)));

	
	Debug::trace("WrapperConfiguration", "heartbeat:");
	heartbeat.show();

	Debug::trace("WrapperConfiguration", "gripperSetback0:");
	gripperSetback0.show();

	Debug::trace("WrapperConfiguration", "gripperSetback1:");
	gripperSetback1.show();

	Debug::trace("WrapperConfiguration", "gripperSetback2:");
	gripperSetback2.show();

	Debug::trace("WrapperConfiguration", "gripperSetback3:");
	gripperSetback3.show();

	for (int i = 0; i < NumberPackCodes; i++)
	{
		packCode[i].show();
	}

	Debug::removeTraceCategory("WrapperConfiguration");
}

void WrapperConfiguration::clear()
{
	configType = WrPermanent;

	for (int i = 0; i < SystemDevSignatureSize; i++)
	{
		deviceSignature[i] = '\0';
	}

	wideFilmTestEnable = false;
	narrowFilmTestEnable = false;
	celsiusEnable = false;
	heaterEnable = false;
	heaterSetting = 0;      

	packageLengthAdjustment = 0;    
	packageWidthAdjustment = 0;        

	pauseTimeout = 0; // seconds
	idleTimeout = 0; // machine cycles
	filmTension = LowFilmTension;
	packWidth = NarrowPackage;

	packageGrandTotal = 0;
    wrapperCycleTotal = 0;

	elevatorCollapseTotal = 0;    
	intakeJamTotal = 0;    
	lengthJamTotal = 0;    
	widthJamTotal = 0;    
	coverOpenTotal = 0;    
	sealerBeltOpenTotal = 0;    
	packageTooTallTotal = 0;    
	pusherHomeErrorTotal = 0;    
	filmPullErrorTotal = 0;    

	errorTotalsDate = 0;    
	packCodeTotalsDate = 0;    

	heartbeat = WrapperTiming();

	gripperSetback0 = WrapperGripperTiming();
	gripperSetback1 = WrapperGripperTiming();
	gripperSetback2 = WrapperGripperTiming();
	gripperSetback3 = WrapperGripperTiming();

	for (int i = 0; i < NumberPackCodes; i++)
	{
		packCode[i] = WrapperPackCodeInfo();
	}
}

WrConfigType WrapperConfiguration::getConfigType() const
{
	return configType;
}

void WrapperConfiguration::setConfigType(WrConfigType configType)
{
	this->configType = configType;
}

void WrapperConfiguration::setDeviceSignature(UINT8 *deviceSignature)
{
	for (int i = 0; i < SystemDevSignatureSize; i++)
	{
		this->deviceSignature[i] = deviceSignature[i];
	}
}

void WrapperConfiguration::getDeviceSignature(UINT8 *deviceSignature) const
{
	for (int i = 0; i < SystemDevSignatureSize; i++)
	{
		deviceSignature[i] = this->deviceSignature[i];
	}
}

UINT8 WrapperConfiguration::getPauseTimeout() const
{
	return pauseTimeout;
}

void WrapperConfiguration::setPauseTimeout(UINT8 pauseTimeout)
{
	this->pauseTimeout = pauseTimeout;
}

UINT8 WrapperConfiguration::getIdleTimeout() const
{
	return idleTimeout;
}

void WrapperConfiguration::setIdleTimeout(UINT8 idleTimeout)
{
	this->idleTimeout = idleTimeout;
}

// Film Tension Variants
WrFilmTension WrapperConfiguration::getFilmTension() const
{
	return filmTension;
}

void WrapperConfiguration::setFilmTension(WrFilmTension filmTension)
{
	this->filmTension = filmTension;
}

bool WrapperConfiguration::isLowFilmTension() const
{
	return (filmTension == LowFilmTension);
}

bool WrapperConfiguration::isMediumFilmTension() const
{
	return (filmTension == MediumFilmTension);
}

bool WrapperConfiguration::isHighFilmTension() const
{
	return (filmTension == HighFilmTension);
}

void WrapperConfiguration::setLowFilmTension()
{
	filmTension = LowFilmTension;
}

void WrapperConfiguration::setMediumTension()
{
	filmTension = MediumFilmTension;
}

void WrapperConfiguration::setHighFilmTension()
{
	filmTension = HighFilmTension;
}

// Pack Width Variants
WrPackWidth WrapperConfiguration::getPackWidth() const
{
	return packWidth;
}

void WrapperConfiguration::setPackWidth(WrPackWidth packWidth)
{
	this->packWidth = packWidth;
}

bool WrapperConfiguration::isNarrowPackage() const
{
	return (packWidth == NarrowPackage);
}

bool WrapperConfiguration::isWidePackage() const
{
	return (packWidth == WidePackage);
}

bool WrapperConfiguration::isFilmWidthIdle() const
{
	return (packWidth == FilmWidthIdle);
}

bool WrapperConfiguration::isAutoWidthSelect() const
{
	return (packWidth == AutoWidthSelect);
}

void WrapperConfiguration::setNarrowPackage()
{
	packWidth = NarrowPackage;
}

void WrapperConfiguration::setWidePackage()
{
	packWidth = WidePackage;
}

void WrapperConfiguration::setFilmWidthIdle()
{
	packWidth = FilmWidthIdle;
}

void WrapperConfiguration::setAutoWidthSelect()
{
	packWidth = AutoWidthSelect;
}

UINT32 WrapperConfiguration::getPackageGrandTotal() const
{
	return packageGrandTotal;
}

void WrapperConfiguration::setPackageGrandTotal(UINT32 packageGrandTotal)
{
	this->packageGrandTotal = packageGrandTotal;
}

UINT32 WrapperConfiguration::getWrapperCycleTotal() const
{
	return wrapperCycleTotal;
}

void WrapperConfiguration::setWrapperCycleTotal(UINT32 wrapperCycleTotal)
{
	this->wrapperCycleTotal = wrapperCycleTotal;
}

UINT16 WrapperConfiguration::getElevatorCollapseTotal() const
{
	return elevatorCollapseTotal;
}

void WrapperConfiguration::setElevatorCollapseTotal(UINT16 elevatorCollapseTotal)
{
	this->elevatorCollapseTotal = elevatorCollapseTotal;
}

UINT16 WrapperConfiguration::getIntakeJamTotal() const
{
	return intakeJamTotal;
}

void WrapperConfiguration::setIntakeJamTotal(UINT16 intakeJamTotal)
{
	this->intakeJamTotal = intakeJamTotal;
}

UINT16 WrapperConfiguration::getLengthJamTotal() const
{
	return lengthJamTotal;
}

void WrapperConfiguration::setLengthJamTotal(UINT16 lengthJamTotal)
{
	this->lengthJamTotal = lengthJamTotal;
}

UINT16 WrapperConfiguration::getWidthJamTotal() const
{
	return widthJamTotal;
}

void WrapperConfiguration::setWidthJamTotal(UINT16 widthJamTotal)
{
	this->widthJamTotal = widthJamTotal;
}

UINT16 WrapperConfiguration::getCoverOpenTotal() const
{
	return coverOpenTotal;
}

void WrapperConfiguration::setCoverOpenTotal(UINT16 coverOpenTotal)
{
	this->coverOpenTotal = coverOpenTotal;
}

UINT16 WrapperConfiguration::getSealerBeltOpenTotal() const
{
	return sealerBeltOpenTotal;
}

void WrapperConfiguration::setSealerBeltOpenTotal(UINT16 sealerBeltOpenTotal)
{
	this->sealerBeltOpenTotal = sealerBeltOpenTotal;
}

UINT16 WrapperConfiguration::getPackageTooTallTotal() const
{
	return packageTooTallTotal;
}

void WrapperConfiguration::setPackageTooTallTotal(UINT16 packageTooTallTotal)
{
	this->packageTooTallTotal = packageTooTallTotal;
}

UINT16 WrapperConfiguration::getPusherHomeErrorTotal() const
{
	return pusherHomeErrorTotal;
}

void WrapperConfiguration::setPusherHomeErrorTotal(UINT16 pusherHomeErrorTotal)
{
	this->pusherHomeErrorTotal = pusherHomeErrorTotal;
}

UINT16 WrapperConfiguration::getFilmPullErrorTotal() const
{
	return filmPullErrorTotal;
}

void WrapperConfiguration::setFilmPullErrorTotal(UINT16 filmPullErrorTotal)
{
	this->filmPullErrorTotal = filmPullErrorTotal;
}

bool WrapperConfiguration::isCelsiusEnable() const
{
	return celsiusEnable;
}

void WrapperConfiguration::setCelsiusEnable(bool value)
{
	celsiusEnable = value;
}

bool WrapperConfiguration::getCelsiusEnable() const
{
	return celsiusEnable;
}

void WrapperConfiguration::clearCelsiusEnable()
{
	celsiusEnable = false;
}

bool WrapperConfiguration::isWideFilmTestEnable() const
{
	return wideFilmTestEnable;
}

void WrapperConfiguration::setWideFilmTestEnable(bool value)
{
	wideFilmTestEnable = value;
}

bool WrapperConfiguration::getWideFilmTestEnable() const
{
	return wideFilmTestEnable;
}

void WrapperConfiguration::clearWideFilmTestEnable()
{
	wideFilmTestEnable = false;
}

bool WrapperConfiguration::isNarrowFilmTestEnable() const
{
	return narrowFilmTestEnable;
}

void WrapperConfiguration::setNarrowFilmTestEnable(bool value)
{
	narrowFilmTestEnable = value;
}

bool WrapperConfiguration::getNarrowFilmTestEnable() const
{
	return narrowFilmTestEnable;
}

void WrapperConfiguration::clearNarrowFilmTestEnable()
{
	narrowFilmTestEnable = false;
}

bool WrapperConfiguration::isHeaterEnable() const
{
	return heaterEnable;
}

void WrapperConfiguration::setHeaterEnable(bool value)
{
	heaterEnable = value;
}

bool WrapperConfiguration::getHeaterEnable() const
{
	return heaterEnable;
}

void WrapperConfiguration::clearHeaterEnable()
{
	heaterEnable = false;
}

UINT16 WrapperConfiguration::getHeaterSetting() const
{
	return heaterSetting;
}

void WrapperConfiguration::setHeaterSetting(UINT16 heaterSetting)
{
	this->heaterSetting = heaterSetting;
}

UINT16 WrapperConfiguration::getPackageLengthAdjustment() const
{
	return packageLengthAdjustment;
}

void WrapperConfiguration::setPackageLengthAdjustment(UINT16 packageLengthAdjustment)
{
	this->packageLengthAdjustment = packageLengthAdjustment;
}

UINT16 WrapperConfiguration::getPackageWidthAdjustment() const
{
	return packageWidthAdjustment;
}

void WrapperConfiguration::setPackageWidthAdjustment(UINT16 packageWidthAdjustment)
{
	this->packageWidthAdjustment = packageWidthAdjustment;
}

UINT32 WrapperConfiguration::getErrorTotalsDate() const
{
	return errorTotalsDate;
}

void WrapperConfiguration::setErrorTotalsDate(UINT32 errorTotalsDate)
{
	this->errorTotalsDate = errorTotalsDate;
}

UINT32 WrapperConfiguration::getPackCodeTotalsDate() const
{
	return packCodeTotalsDate;
}

void WrapperConfiguration::setPackCodeTotalsDate(UINT32 packCodeTotalsDate)
{
	this->packCodeTotalsDate = packCodeTotalsDate;
}

WrapperTiming &WrapperConfiguration::getHeartbeat()
{
	return heartbeat;
}

void WrapperConfiguration::setHeartbeat(WrapperTiming *heartbeat)
{
	this->heartbeat = WrapperTiming(heartbeat->getOnMin(),
								   heartbeat->getOnValue(),
								   heartbeat->getOnMax(),
								   heartbeat->getOffMin(),
								   heartbeat->getOffValue(),
								   heartbeat->getOffMax());
}

void WrapperConfiguration::setHeartbeat(WrapperTiming &heartbeat)
{
	this->heartbeat = WrapperTiming(heartbeat);
}

WrapperGripperTiming &WrapperConfiguration::getWrapperGripperTiming0()
{
	return gripperSetback0;
}

void WrapperConfiguration::setWrapperGripperTiming0(WrapperGripperTiming *gripperSetback0)
{
	this->gripperSetback0 = WrapperGripperTiming(gripperSetback0->getSetback(),
												 gripperSetback0->getPwm());
}

void WrapperConfiguration::setWrapperGripperTiming0(WrapperGripperTiming &gripperSetback0)
{
	this->gripperSetback0 = WrapperGripperTiming(gripperSetback0);
}

WrapperGripperTiming &WrapperConfiguration::getWrapperGripperTiming1()
{
	return gripperSetback1;
}

void WrapperConfiguration::setWrapperGripperTiming1(WrapperGripperTiming *gripperSetback1)
{
	this->gripperSetback1 = WrapperGripperTiming(gripperSetback1->getSetback(),
												 gripperSetback1->getPwm());
}

void WrapperConfiguration::setWrapperGripperTiming1(WrapperGripperTiming &gripperSetback1)
{
	this->gripperSetback1 = WrapperGripperTiming(gripperSetback1);
}

WrapperGripperTiming &WrapperConfiguration::getWrapperGripperTiming2()
{
	return gripperSetback2;
}

void WrapperConfiguration::setWrapperGripperTiming2(WrapperGripperTiming *gripperSetback2)
{
	this->gripperSetback2 = WrapperGripperTiming(gripperSetback2->getSetback(),
												 gripperSetback2->getPwm());
}

void WrapperConfiguration::setWrapperGripperTiming2(WrapperGripperTiming &gripperSetback2)
{
	this->gripperSetback2 = WrapperGripperTiming(gripperSetback2);
}

WrapperGripperTiming &WrapperConfiguration::getWrapperGripperTiming3()
{
	return gripperSetback3;
}

void WrapperConfiguration::setWrapperGripperTiming3(WrapperGripperTiming *gripperSetback3)
{
	this->gripperSetback3 = WrapperGripperTiming(gripperSetback3->getSetback(),
												 gripperSetback3->getPwm());
}

void WrapperConfiguration::setWrapperGripperTiming3(WrapperGripperTiming &gripperSetback3)
{
	this->gripperSetback3 = WrapperGripperTiming(gripperSetback3);
}

void WrapperConfiguration::setWrapperPackCodeInfo(WrapperPackCodeInfo &packCodeInfo)
{
	int index = packCodeInfo.getPackCode();

	if ((0 <= index) && (index < NumberPackCodes))
	{
		packCode[index] = packCodeInfo;
	}
}

const WrapperPackCodeInfo &WrapperConfiguration::getWrapperPackCodeInfo(WrPackCode packCodeEntry) const
{
	return packCode[packCodeEntry];
}
