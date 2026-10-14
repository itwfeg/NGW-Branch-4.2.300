/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

/* This file contains the record type definitions used in reading to/writing from
 * external communications (Intranet, DataGateweigh).  These were used in the Ultima
 * and Quantum to define the record types for the classic ScaleMaster Interface.
 * the following definitions are duplicates of those found in ninclude\scalemaster.h.
 * This file was actually created from a subset of ninclude\scalemaster.h (Quantum).
 */

#ifndef _SCALEMASTER_H
#define _SCALEMASTER_H


/******************************************************************************
*                                                                             *
*           S C A L E M A S T E R   F U N C T I O N   C O D E S               *
*                                                                             *
******************************************************************************/

/************************************************************/
/*	RETAIN THESE FIRST GENERATION FUNCTION CODES, 			*/
/*	SO AS TO NOT CONFLICT WITH SOME LEGACY CODE OUT THERE.  */
/************************************************************/
#define FIRST_GEN_FC_00		0x3030
#define FIRST_GEN_FC_02		0x3032
#define FIRST_GEN_FC_05		0x3035
#define FIRST_GEN_FC_06		0x3036
#define FIRST_GEN_FC_20		0x3230
#define FIRST_GEN_FC_21		0x3231
#define FIRST_GEN_FC_22		0x3232
#define FIRST_GEN_FC_23		0x3233
#define FIRST_GEN_FC_26		0x3236
#define FIRST_GEN_FC_27		0x3237
#define FIRST_GEN_FC_28		0x3238

#define CANCEL_LAST_COMMAND 0x3239      /*  29  */

/*  EXECUTE COMMANDS  */

#define MIN_EXECUTE_COMMAND 				DEL_ALL_GEN_TEXT	/*  MIN_EXECUTE_COMMAND  */
#define DEL_ALL_GEN_TEXT					0x3330      /*  30  */
#define CLEAR_HOURLY_TOTALS					0x3331      /*  31  */
#define CLEAR_OP_TOTALS						0x3332      /*  32  */
#define CLEAR_PLU_CLASS_TOTALS				0x3333      /*  33  */
#define DEL_ALL_CLASSES						0x3334      /*  34  */
#define DEL_ALL_COMMODITIES					0x3335      /*  35  */
#define DEL_ALL_CDTY_CLASS					0x3336      /*  36  */
#define DEL_ALL_MARQUEES					0x3337      /*  37  */
#define DEL_ALL_NU_DATA						0x3338      /*  38  */
#define DEL_ALL_FONTS						0x3339      /*  39  */

#define DEL_ALL_SHOP_ADR					0x3341      /*  3A  EUROPE  */
#define DEL_ALL_SANI_STAMP					0x3342      /*  3B  EUROPE  */
#define DEL_ALL_INGREDIENTS					0x3343      /*  3C  EUROPE  */
#define DEL_ALL_PLU_TEXT1					0x3344      /*  3D  EUROPE  */
#define DEL_ALL_PLU_TEXT2					0x3345      /*  3E  EUROPE  */
#define DEL_ALL_PLU_TEXT3					0x3346      /*  3F  EUROPE  */
#define DEL_ALL_MONETARY					0x3347      /*  3G  EUROPE  */
#define CLEAR_DAILY_TOTALS					0x3348      /*  3H  EUROPE  */
#define CLEAR_LOTNUMBER_TOTALS				0x3349      /*  3I  EUROPE  */

#define DEL_ALL_PICTS						0x3430      /*  40  */
#define DEL_ALL_OP_IDS						0x3431      /*  41  */
#define DEL_ALL_EXTEXT						0x3432      /*  42  */
#define DEL_ALL_SMSGS						0x3433      /*  43  */
#define DEL_ALL_SPEEDKEYS					0x3434      /*  44  */
#define DEL_ALL_MACROS						0x3435      /*  45  */
#define DEL_ALL_LAB_FORMS					0x3436      /*  46  */
#define DEL_ALL_TOT_NAMES					0x3437      /*  47  */
#define DEL_ALL_DEPT_NAMES					0x3438      /*  48  */
#define DEL_ALL_FLASHKEYS					0x3439      /*  49  */
#define DEL_ALL_MARQ_TIMES					0x3441      /*  4A  */
#define DEL_ALL_OP_CHANGES					0x3442      /*  4B  */
#define DEL_ALL_IP_ADDRESS					0x3443      /*  4C  */
#define DEL_ALL_ITEM_TRANS					0x3444      /*  4D  */
#define DEL_ALL_SYSTEM_TEXT					0x3445      /*  4E  */
#define RESET_SCALE							0x3446	    /*  4F  */ 	/* Q */
#define REMOTE_FLASH_UPGRADE				0x3447      /*  4G  */ 	/* Q */
#define TFTP_DB_DATE						0x3448      /*  4H  */ 	/* Q */
#define DEL_ALL_FS_GRAPHICS					0x3449		/*  4I  */ 	/* Q */
#define DEL_ALL_FS_OPERATE					0x344A		/*  4J	*/ 	/* Q */
#define SM_DEL_ALL_COOL_TEXT				0x344B		/*  4K  */ 	/* Q */
#define SM_DEL_ALL_CUSTOM_COOL_TEXT			0x344C		/*  4L  */ 	/* Q */
#define SM_DEL_ALL_COUNTRY_TEXT				0x344D		/*  4M	*/ 	/* Q */
#define SM_DEL_ALL_SHORTLIST				0x344E		/*  4N  */ 	/* Q */
#define SM_CLEAR_COOL_TOTALS				0x3450		/*  4P  */ 	/* Q */
#define SM_DEL_ALL_TRACKING_TEXT			0x3451		/*  4Q  */ 	/* Q */
#define SM_DEL_ALL_PRE_TEXT					0x3452		/*  4R  */ 	/* Q */
#define MAX_EXECUTE_COMMAND 	SM_DEL_ALL_PRE_TEXT	/*  MAX_EXECUTE_COMMAND  */

/**/
/*  POLL COMMANDS  */

#define MIN_POLL_COMMAND					READ_CLASS_TOTALS	/*  MIN_POLL_COMMAND  */
#define READ_CLASS_TOTALS					0x3531      /*  51  */
#define READ_CLASSES						0x3532      /*  52  */
#define READ_COMMODITIES					0x3533      /*  53  */
#define READ_COMMODITY_TOTALS				0x3534      /*  54  */
#define READ_CONFIGURATION					0x3535      /*  55  */
#define READ_DATE_TIME						0x3536      /*  56  */
#define READ_HOURLY_TOTALS					0x3537      /*  57  */
#define SM_UNUSED_58						0x3538      /*  58  */
#define READ_MARQUEES						0x3539      /*  59  */
#define READ_MARQ_TIMES						0x3541      /*  5A  */
#define READ_IP_ADDRESS						0x3542      /*  5B  */

#define READ_DAILY_TOTALS					0x3543		/*	5C  EUROPE  */
#define READ_LOTNUMBER_TOTALS				0x3544		/*	5D  EUROPE  */
#define READ_SHOP_ADR						0x3545		/*	5E  EUROPE  */
#define READ_SANI_STAMP						0x3546		/*	5F  EUROPE  */
#define READ_INGREDIENTS					0x3547		/*	5G  EUROPE  */
#define READ_PLU_TEXT1						0x3548		/*	5H  EUROPE  */
#define READ_PLU_TEXT2						0x3549		/*	5I  EUROPE  */
#define READ_PLU_TEXT3						0x354A		/*	5J  EUROPE  */
#define READ_MONETARY						0x354B		/*	5K  EUROPE  */
	
#define READ_NU_DATA						0x3630      /*  60  */
#define READ_NU_TEMPLATES					0x3631      /*  61  */
#define READ_OP_IDS							0x3632      /*  62  */
#define READ_OP_TOTALS						0x3633      /*  63  */
#define READ_EXTEXT							0x3634      /*  64  */
#define READ_SCALE_STATUS					0x3635      /*  65  */
#define READ_SPECIAL_MSGS					0x3636  	/*  66  */
#define READ_SPEED_KEYS						0x3637  	/*  67  */
#define READ_STATISTICS						0x3638  	/*  68  */
#define READ_PICTURE						0x3639  	/*  69  */
#define READ_OP_CHANGES						0x3641  	/*  6A  */
#define READ_FLASH_KEYS						0x3642  	/*  6B  */
#define READ_FONT							0x3730  	/*  70  */
#define READ_LABEL_FORM						0x3731  	/*  71  */
#define READ_MACRO							0x3732  	/*  72  */
#define READ_STORE_NAME						0x3733  	/*  73  */
#define READ_TOT_NAME						0x3734  	/*  74  */
#define READ_DEPT_NAME						0x3735  	/*  75  */
#define READ_GEN_TEXT						0x3736  	/*  76  */
#define READ_CLEAR_ITEM_TRANS				0x3737  	/*  77  */
#define READ_SYSTEM_TEXT					0x3738  	/*  78  */
#define READ_NXT_OP_TRANS					0x3739  	/*  79  */
#define READ_ALL_OP_TRANS					0x3741  	/*  7A  */
#define READ_NXT_OP_SBC_TRANS				0x3742  	/*  7B  */
#define READ_ALL_OP_SBC_TRANS				0x3743  	/*  7C  */
#define READ_CONFIG_ITEMS					0x3744  	/*  7D  */
#define READ_FS_GRAPHIC_ID					0x3745		/*  7E  */	/* Q */
#define READ_FS_OPERATE_ID					0x3746    	/*  7F  */	/* Q */
#define SM_READ_COOL_TEXT_RECORD			0x3747		/*  7G  */	/* Q */
#define SM_READ_CUSTOM_COOL_TEXT_RECORD		0x3748		/* 	7H  */	/* Q */
#define SM_READ_COUNTRY_TEXT_RECORD			0x3749		/*  7I  */	/* Q */
#define SM_READ_SHORTLIST_RECORD			0x374A		/*  7J  */	/* Q */
#define SM_READ_COOL_TOTAL_RECORD			0x374B		/*  7K  */	/* Q */
#define SM_READ_TRACKING_TEXT_RECORD		0x374C		/*  7L	*/	/* Q */
#define SM_READ_PRE_TEXT_RECORD				0x374D		/*  7M	*/	/* Q */
#define MAX_POLL_COMMAND    	SM_READ_PRE_TEXT_RECORD	/*  MAX_POLL_COMMAND  */

/**/
/*  SELECT COMMANDS  */

#define MIN_SELECT_COMMAND  				DEL_PICT			/*	MIN_SELECT_COMMAND  */
#define DEL_PICT                			0x3830      /*  80  */
#define DEL_CLASS               			0x3831      /*  81  */
#define DEL_COMMODITY           			0x3832      /*  82  */
#define DEL_FONT                			0x3833      /*  83  */
#define DEL_MARQUEE             			0x3834      /*  84  */
#define DEL_NU_DATA             			0x3835      /*  85  */
#define DEL_NU_TEMPLATE         			0x3836      /*  86  */
#define DEL_EXTEXT              			0x3837      /*  87  */
#define WRITE_CLASSES           			0x3838      /*  88  */
#define WRITE_COMMODITIES       			0x3839      /*  89  */
#define WRITE_STORE_NAME        			0x3841      /*  8A  */
#define DEL_SPCL_MSG            			0x3842      /*  8B  */
#define WRITE_MARQ_TIMES        			0x3843      /*  8C  */
#define WRITE_OP_CHANGES        			0x3844      /*  8D  */
#define DEL_OP_ID               			0x3845      /*  8E  */
#define WRITE_SEQUENCE          			0x3846      /*  8F  */

#define WRITE_SHOP_ADR		    			0x3847		/*	8G  EUROPE  */
#define WRITE_SANI_STAMP	    			0x3848		/*	8H  EUROPE  */
#define WRITE_INGREDIENTS	    			0x3849		/*	8I  EUROPE  */
#define WRITE_PLU_TEXT1		    			0x384A		/*	8J  EUROPE  */
#define WRITE_PLU_TEXT2		    			0x384B		/*	8K  EUROPE  */
#define WRITE_PLU_TEXT3		    			0x384C		/*	8L  EUROPE  */
#define WRITE_MONETARY		    			0x384D		/*	8M  EUROPE  */
#define WRITE_DTOTALS		    			0x384E		/*	8N  EUROPE  */
#define WRITE_LTOTALS		    			0x384F		/*	8O  EUROPE  */
#define DEL_SMSG			    			0x3850		/*	8P  EUROPE  */
#define DEL_SHOP_ADR		    			0x3851		/*	8Q  EUROPE  */
#define DEL_SANI_STAMP     	    			0x3852		/*	8R  EUROPE  */
#define DEL_INGREDIENTS		    			0x3853		/*	8S  EUROPE  */
#define DEL_PLU_TEXT1		    			0x3854		/*	8T  EUROPE  */
#define DEL_PLU_TEXT2		    			0x3855		/*	8U  EUROPE  */
#define DEL_PLU_TEXT3		    			0x3856		/*	8V  EUROPE  */

#define WRITE_CONFIGURATION     			0x3930      /*  90  */
#define WRITE_DATE_TIME         			0x3931      /*  91  */
#define WRITE_FONT              			0x3932      /*  92  */
#define WRITE_MARQUEES          			0x3933      /*  93  */
#define WRITE_NU_DATA           			0x3934      /*  94  */
#define WRITE_NU_TEMPLATES      			0x3935      /*  95  */
#define WRITE_OP_IDS            			0x3936      /*  96  */
#define WRITE_EXTEXT            			0x3937      /*  97  */
#define WRITE_SPECIAL_MSGS      			0x3938      /*  98  */
#define WRITE_SPEED_KEYS        			0x3939      /*  99  */
#define WRITE_PICT              			0x3941      /*  9A  */
#define WRITE_MACRO             			0x3942      /*  9B  */
#define WRITE_LAB_FORM          			0x3943      /*  9C  */
#define WRITE_TOT_NAME          			0x3944      /*  9D  */
#define WRITE_DEPT_NAME         			0x3945      /*  9E  */
#define WRITE_GEN_TEXT          			0x3946      /*  9F  */
#define WRITE_PTOTALS           			0x4130      /*  A0  */
#define WRITE_CTOTALS           			0x4131      /*  A1  */
#define WRITE_HTOTALS           			0x4132      /*  A2  */
#define WRITE_OTOTALS           			0x4133      /*  A3  */
#define WRITE_COMTOTALS         			0x4134      /*  A4  */
#define WRITE_FLASH_KEYS        			0x4135      /*  A5  */
#define DEL_MACRO               			0x4136      /*  A6  */
#define DEL_LABEL_FORM          			0x4137      /*  A7  */
#define DEL_TOTAL_TYPE          			0x4138      /*  A8  */
#define DEL_STORE_NAME          			0x4139      /*  A9  */
#define DEL_GENERIC_TEXT        			0x4141      /*  AA  */
#define DEL_MARQUEE_TIME        			0x4142      /*  AB  */
#define DEL_FLASHKEY            			0x4143      /*  AC  */
#define WRITE_SCALE_STATUS      			0x4144      /*  AD  */
#define WRITE_STATISTICS        			0x4145      /*  AE  */
#define WRITE_IP_ADDRESS        			0x4146      /*  AF  */
#define DEL_IP_ADDRESS						0x4230      /*  B0  */
#define RESET_ITEM_TRANS_NUM				0x4231      /*  B1  */

#define WRITE_SYSTEM_TEXT   				0x4232		/*  B2  */
#define DEL_SYSTEM_TEXT     				0x4233		/*  B3  */
#define WRITE_NXT_OP_TRANS  				0x4234		/*  B4  */
#define WRITE_OP_TRANS      				0x4235		/*  B5  */
#define WRITE_NXT_OP_SBC_TRANS  			0x4236		/*  B6  */
#define WRITE_OP_SBC_TRANS      			0x4237		/*  B7  */
#define DELETE_BY_OP_TRANS      			0x4238		/*  B8  */
#define WRITE_CONFIG_ITEMS      			0x4239		/*  B9  */
#define WRITE_ITEM_TRANS        			0x4241		/*  BA  */

#define DEL_FS_GRAPHIC_ID					0x4242		/*  BB */
#define WRITE_FS_GRAPHIC_ID					0x4243		/*  BC */
#define DEL_FS_OPERATE_ID					0x4244		/*  BD */
#define WRITE_FS_OPERATE_ID					0x4245		/*  BE  */
#define SM_WRITE_COOL_TEXT_RECORD			0x4333		/*  C3  */	/* Q */
#define SM_WRITE_CUSTOM_COOL_TEXT_RECORD	0x4334		/*	C4  */	/* Q */
#define SM_WRITE_COUNTRY_TEXT_RECORD		0x4335		/*  C5  */	/* Q */
#define SM_WRITE_SHORTLIST_RECORD			0x4336		/*  C6  */	/* Q */
#define SM_DEL_COOL_TEXT_RECORD   			0x4337		/*  C7	*/	/* Q */
#define SM_DEL_CUSTOM_COOL_TEXT_RECORD		0x4338		/*  C8  */	/* Q */
#define SM_DEL_COUNTRY_TEXT_RECORD			0x4339		/*  C9  */	/* Q */
#define SM_DEL_SHORTLIST_RECORD				0x4341		/*  CA  */	/* Q */
#define SM_WRITE_COOL_TOTAL					0x4342		/*  CB	*/	/* Q */
#define SM_WRITE_TRACKING_TEXT_RECORD		0x4343		/*  CC  */	/* Q */
#define SM_DEL_TRACKING_TEXT_RECORD			0x4344		/*  CD  */	/* Q */
#define SM_WRITE_PRE_TEXT_RECORD			0x4345		/*  CE  */	/* Q */
#define SM_DEL_PRE_TEXT_RECORD				0x4346		/*  CF  */	/* Q */
#define MAX_SELECT_COMMAND	    SM_DEL_PRE_TEXT_RECORD	/* MAX_SELECT_COMMAND */

#endif
