/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

/* This file contains the record type definitions for record types not
 * in the Quantum or Ultima.  They are new to the Evolution.
 * This file is similar to scalemaster.h, regarding the record type definitions.
 */

#ifndef _SCALEMASTEREVOL_H
#define _SCALEMASTEREVOL_H


/******************************************************************************
*                                                                             *
*           E V O L U T I O N       F U N C T I O N   C O D E S               *
*                                                                             *
******************************************************************************/

#define COMM_START   0x3031     /*  01  */
#define COMM_END     0x4646     /*  FF  */

#define DEL_ALL_FILES						0x3241	   /*  2A  */
#define DEL_ALL_COOL_LISTTEXTS				0x3242     /*  2B  */
#define DEL_ALL_SUPER_OP_CHANGES            0x3244     /*  2D  */
#define DEL_ALL_CUTTING_TESTS				0x3250	   /*  2P  */
#define DEL_ALL_CUT_TEST_TOTALS				0x3251	   /*  2Q  */
#define DEL_ALL_COOL_TEXTS					0x3252     /*  2R  */	
#define DEL_ALL_COOL_CATEGORIES				0x3253     /*  2S  */
#define DEL_ALL_COOL_COUNTRIES				0x3254     /*  2T  */
#define DEL_ALL_COOL_CATEGORY_COUNTRIES		0x3255     /*  2U  */
#define DEL_ALL_COOL_PRETEXTS				0x3256     /*  2V  */
#define DEL_ALL_COOL_CATEGORY_PRETEXTS		0x3257     /*  2W  */
#define DEL_ALL_COOL_PROD_STEPS				0x3258     /*  2X  */	
#define DEL_ALL_COOL_CATEGORY_PROD_STEPS	0x3259     /*  2Y  */
#define DEL_ALL_COOL_SHORTLIST_INFO			0x325A     /*  2Z  */

#define DEL_ALL_PRODUCT_DEFAULTS       0x334A     /*  3J  */
#define DEL_ALL_PRODUCT_NOTES          0x334B     /*  3K  */
#define DEL_ALL_OPERATOR_NOTES         0x334C     /*  3L  */
#define DEL_ALL_CATEGORIES             0x334D     /*  3M  */
#define DEL_ALL_PRESENTATION_METHODS   0x334E     /*  3N  */
#define DEL_ALL_SPECIALS               0x334F     /*  3O  */
#define DEL_ALL_STREAMING_GRAPHIC_INFO 0x3350     /*  3P  */
#define DEL_ALL_PRODUCT_STATISTICS     0x3351     /*  3Q  */

#define DEL_ALL_EXPIRATION_LABELS		0x3352		/*  3R  */

#define DEL_ALL_PRODUCT_GRAPHICS       0x3353     /*  3S  */
#define DEL_ALL_CATEGORY_GRAPHICS      0x3354     /*  3T  */
#define DEL_ALL_SPECIALS_GRAPHICS      0x3355     /*  3U  */
#define DEL_ALL_STREAMING_GRAPHICS     0x3356     /*  3V  */
#define DEL_ALL_USER_GRAPHICS          0x3357     /*  3W  */
#define DEL_ALL_EMAIL_ALERTS           0x3358     /*  3X  */
#define DEL_ALL_USER_FONTS             0x3359	  /*  3Y  */
#define DEL_ALL_SOFTKEY_GRAPHICS	   0x335A	  /*  3Z  */

#define CLEAR_ALL_TOTALS               0x344F     /*  4O  */
#define DEL_ALL_SOFTKEYS			   0x3453     /*  4S  */
#define DEL_ALL_SOFTKEY_STYLES		   0x3454     /*  4T  */
#define DEL_ALL_SOFTKEY_FEATURES       0x3455     /*  4U  */
#define DEL_ALL_SOFTKEY_GROUPS		   0x3456     /*  4V  */

/*  POLL COMMANDS  */

#define READ_ALL_CUTTING_TESTS		 0x3550	    /*  5P  */
#define READ_ALL_CUT_TEST_TOTALS	 0x3551	    /*  5Q  */
#define READ_EXPIRATION_LABELS       0x3552		/*  5R  */

#define READ_PRINTER_SETTINGS     	 0x355A     /*  5Z  */

//	60 - 6B in scalemaster.h
#define READ_SUPER_OP_CHANGES        0x3244     /*  6D  */
#define READ_WRAPPER_TIMINGS         0x3649     /*  6I  */
#define READ_PRODUCT_DEFAULTS        0x364A     /*  6J  */
#define READ_PRODUCT_NOTES           0x364B     /*  6K  */
#define READ_OPERATOR_NOTES          0x364C     /*  6L  */
#define READ_CATEGORIES              0x364D     /*  6M  */
#define READ_PRESENTATION_METHODS    0x364E     /*  6N  */
#define READ_SPECIALS                0x364F     /*  6O  */
#define READ_STREAMING_GRAPHIC_INFO  0x3650     /*  6P  */
#define READ_PRODUCT_STATISTICS      0x3651     /*  6Q  */
#define READ_ROM_GRAPHICS            0x3652     /*  6R  */
#define READ_PRODUCT_GRAPHICS        0x3653     /*  6S  */
#define READ_CATEGORY_GRAPHICS       0x3654     /*  6T  */
#define READ_SPECIALS_GRAPHICS       0x3655     /*  6U  */
#define READ_STREAMING_GRAPHICS      0x3656     /*  6V  */
#define READ_USER_GRAPHICS           0x3657     /*  6W  */
#define READ_EMAIL_ALERTS            0x3658     /*  6X  */
#define READ_USER_FONTS              0x3659		/*  6Y  */
#define READ_SOFTKEY_GRAPHICS        0x365A		/*  6Z  */

#define READ_SOFTKEYS				 0x374E		/*  7N  */
#define READ_SOFTKEY_STYLES			 0x374F		/*  7O  */
#define READ_SOFTKEY_FEATURES		 0x3750		/*  7P  */
#define READ_SOFTKEY_GROUPS			 0x3751		/*  7Q  */

#define READ_COOL_TEXTS					0x3752     /*  7R  */	
#define READ_COOL_CATEGORIES			0x3753     /*  7S  */
#define READ_COOL_COUNTRIES				0x3754     /*  7T  */
#define READ_COOL_CATEGORY_COUNTRIES	0x3755     /*  7U  */
#define READ_COOL_PRETEXTS				0x3756     /*  7V  */
#define READ_COOL_CATEGORY_PRETEXTS		0x3757     /*  7W  */
#define READ_COOL_PROD_STEPS			0x3758     /*  7X  */	
#define READ_COOL_CATEGORY_PROD_STEPS	0x3759     /*  7Y  */
#define READ_COOL_SHORTLIST_INFO		0x375A     /*  7Z  */

//	80 - 8V in scalemaster.h
#define READ_CONFIG_SYNCHRONIZATION     0x3858     /*  8X  */
#define WRITE_CONFIG_SYNCHRONIZATION    0x3859     /*  8Y  */
#define WRITE_PRINTER_SETTINGS			0x385A     /*  8Z  */

/*  SELECT COMMANDS  */

#define WRITE_WRAPPER_TIMINGS        0x3949     /*  9I  */
#define WRITE_PRODUCT_DEFAULTS       0x394A     /*  9J  */
#define WRITE_PRODUCT_NOTES          0x394B     /*  9K  */
#define WRITE_OPERATOR_NOTES         0x394C     /*  9L  */
#define WRITE_CATEGORIES             0x394D     /*  9M  */
#define WRITE_PRESENTATION_METHODS   0x394E     /*  9N  */
#define WRITE_SPECIALS               0x394F     /*  9O  */
#define WRITE_STREAMING_GRAPHIC_INFO 0x3950     /*  9P  */
#define WRITE_PRODUCT_STATISTICS     0x3951     /*  9Q  */
#define WRITE_ROM_GRAPHICS           0x3953     /*  9R  */
#define WRITE_PRODUCT_GRAPHICS       0x3953     /*  9S  */
#define WRITE_CATEGORY_GRAPHICS      0x3954     /*  9T  */
#define WRITE_SPECIALS_GRAPHICS      0x3955     /*  9U  */
#define WRITE_STREAMING_GRAPHICS     0x3956     /*  9V  */
#define WRITE_USER_GRAPHICS		  	 0x3957     /*  9W  */
#define WRITE_EMAIL_ALERTS           0x3958     /*  9X  */
#define WRITE_USER_FONTS             0x3959		/*  9Y  */
#define WRITE_SOFTKEY_GRAPHICS       0x395A		/*  9Z  */

#define WRITE_ERROR					 0x4552		/*  ER  */

#define WRITE_SOFTKEY				 0x4147		/*  AG  */
#define WRITE_SOFTKEY_STYLE			 0x4148		/*  AH  */
#define WRITE_SOFTKEY_FEATURE		 0x4149		/*  AI  */
#define DEL_PRODUCT_DEFAULT          0x414A     /*  AJ  */
#define DEL_PRODUCT_NOTE             0x414B     /*  AK  */
#define WRITE_SOFTKEY_GROUP			 0x414C		/*  AL  */
#define DEL_CATEGORY                 0x414D     /*  AM  */
#define DEL_PRESENTATION_METHOD      0x414E     /*  AN  */
#define DEL_SPECIAL                  0x414F     /*  AO  */
#define DEL_STREAMING_GRAPHIC_INFO   0x4150     /*  AP  */
#define DEL_PRODUCT_STATISTIC        0x4151     /*  AQ  */

#define WRITE_EXPIRATION_LABEL       0x4152		/*  AR  */

#define DEL_PRODUCT_GRAPHIC          0x4153     /*  AS  */
#define DEL_CATEGORY_GRAPHIC         0x4154     /*  AT  */
#define DEL_SPECIALS_GRAPHIC         0x4155     /*  AU  */
#define DEL_STREAMING_GRAPHIC        0x4156     /*  AV  */
#define DEL_USER_GRAPHIC             0x4157     /*  AW  */
#define DEL_EMAIL_ALERT              0x4158     /*  AX  */
#define DEL_USER_FONT                0x4159		/*  AY  */
#define DEL_SOFTKEY_GRAPHIC          0x415A		/*  AZ  */

#define DEL_SOFTKEY					 0x4246		/*  BF */
#define DEL_SOFTKEY_STYLE			 0x4247		/*  BG */
#define DEL_SOFTKEY_FEATURE			 0x4248		/*  BH */
#define DEL_SOFTKEY_GROUP			 0x4249		/*  BI */

#define DEL_EXPIRATION_LABELS		0x4250		/*  BP  */

#define DEL_TOT_NAME                0x4251   /*  BQ  */

#define WRITE_COOL_TEXTS				0x4252     /*  BR  */	
#define WRITE_COOL_CATEGORIES			0x4253     /*  BS  */
#define WRITE_COOL_COUNTRIES			0x4254     /*  BT  */
#define WRITE_COOL_CATEGORY_COUNTRIES	0x4255     /*  BU  */
#define WRITE_COOL_PRETEXTS				0x4256     /*  BV  */
#define WRITE_COOL_CATEGORY_PRETEXTS	0x4257     /*  BW  */
#define WRITE_COOL_PROD_STEPS			0x4258     /*  BX  */	
#define WRITE_COOL_CATEGORY_PROD_STEPS	0x4259     /*  BY  */
#define WRITE_COOL_SHORTLIST_INFO		0x425A     /*  BZ  */


#define WRITE_CUTTING_TEST				0x4350	   /*  CP  */
#define WRITE_CUT_TEST_TOTAL			0x4351	   /*  CQ  */
#define DEL_COOL_TEXTS					0x4352     /*  CR  */	
#define DEL_COOL_CATEGORIES				0x4353     /*  CS  */
#define DEL_COOL_COUNTRIES				0x4354     /*  CT  */
#define DEL_COOL_CATEGORY_COUNTRIES		0x4355     /*  CU  */
#define DEL_COOL_PRETEXTS				0x4356     /*  CV  */
#define DEL_COOL_CATEGORY_PRETEXTS		0x4357     /*  CW  */
#define DEL_COOL_PROD_STEPS				0x4358     /*  CX  */	
#define DEL_COOL_CATEGORY_PROD_STEPS	0x4359     /*  CY  */
#define DEL_COOL_SHORTLIST_INFO			0x435A     /*  CZ  */


/* SYNCHRONIZATION COMMANDS */
#define REGISTER_SECONDARY_SCALE        0x4430     /*  D0  */
#define NOTIFY_LOCAL_BATCHES            0x4431     /*  D1  */
#define NOTIFY_ONLINE                   0x4432     /*  D2  */
#define WRITE_SYNCHRONIZATION_CONFIG    0x4439     /*  D9  */
#define READ_BATCHES                    0x4441     /*  DA  */
#define WRITE_BATCH                     0x4442     /*  DB  */
#define WRITE_SYNCHRONIZATION_CHANGE    0x4443     /*  DC  */
#define WRITE_SCALE                     0x4444     /*  DD  */

#define READ_PRODTEXT5          		0x3553      /*  5S  */
#define READ_PRODTEXT6          		0x3554      /*  5T  */
#define READ_PRODTEXT7          		0x3555      /*  5U  */
#define READ_PRODTEXT8          		0x3556      /*  5V  */
#define READ_PRODTEXT9          		0x3557      /*  5W  */

#define WRITE_SUPER_OP_CHANGES          0x4544      /*  ED  */
#define WRITE_PRODTEXT5         		0x4553      /*  ES  */
#define WRITE_PRODTEXT6         		0x4554      /*  ET  */
#define WRITE_PRODTEXT7         		0x4555      /*  EU  */
#define WRITE_PRODTEXT8         		0x4556      /*  EV  */
#define WRITE_PRODTEXT9         		0x4557      /*  EW  */

#define DEL_PRODTEXT5					0x4453      /*  DS  */
#define DEL_PRODTEXT6         			0x4454      /*  DT  */
#define DEL_PRODTEXT7          			0x4455      /*  DU  */
#define DEL_PRODTEXT8          			0x4456      /*  DV  */
#define DEL_PRODTEXT9          			0x4457      /*  DW  */

#define DEL_ALL_PRODTEXT5				0x3249      /*  2I  */
#define DEL_ALL_PRODTEXT6      			0x324A      /*  2J  */
#define DEL_ALL_PRODTEXT7      			0x324B      /*  2K  */
#define DEL_ALL_PRODTEXT8      			0x324C      /*  2L  */
#define DEL_ALL_PRODTEXT9      			0x324D      /*  2M  */


#define READ_RECALLS                    0x3647      /*  6G  */
#define READ_PLANOGRAM_GRAPHICS         0x3648      /*  6H  */

#define WRITE_RECALLS                   0x3947      /*  9G  */
#define WRITE_PLANOGRAM_GRAPHICS        0x3948      /*  9H  */

#define DEL_RECALLS                     0x4447      /*  DG  */
#define DEL_PLANOGRAM_GRAPHICS          0x4448      /*  DH  */

#define DEL_ALL_RECALLS                 0x3247      /*  2G  */
#define DEL_ALL_PLANOGRAM_GRAPHICS      0x3248      /*  2H  */


#define READ_PRODUCT_TEXT_CFGS          0x354C     /*  5L  */
#define READ_DEVICES                    0x354D     /*  5M  */

#define WRITE_PRODUCT_TEXT_CFGS         0x424C     /*  BL  */
#define WRITE_DEVICES                   0x424D     /*  BM  */

#define DEL_PRODUCT_TEXT_CFG            0x444C     /*  DL  */
#define DEL_DEVICE                      0x444D     /*  DM  */

#define DEL_ALL_PRODUCT_TEXT_CFGS       0x3245     /*  2E  */
#define DEL_ALL_DEVICES                 0x3246     /*  2F  */

#define READ_BARCODE_SCANNERS           0x4433     /*  D3  */
#define WRITE_BARCODE_SCANNERS          0x4434     /*  D4  */

#define DEL_BARCODE_SCANNER             0x4435     /*  D5  */
#define DEL_ALL_BARCODE_SCANNERS        0x4436     /*  D6  */


#define WRITE_USB_KEYS_PRESENT          0x4541      /*  EA  */

#define EXECUTE_TASK                    0x444F      /*  DO  */

#define READ_WEIGHER_SETTINGS           0x3559      /*  5Y  */
#define WRITE_WEIGHER_SETTINGS          0x3857      /*  8W  */

#define READ_CAT3_AUDIT_TRAIL			0x3558      /*  5X  */
#define WRITE_CAT3_AUDIT_TRAIL     		0x4558      /*  EX  */

#define REMOTE_PRINT_TEMPLATE			0x5A59     /*  ZY  */
#define REMOTE_PRINT_LABEL				0x5A5A     /*  ZZ  */

#endif  // _SCALEMASTEREVOL_H
