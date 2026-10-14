/**
 * @file databasejson.h
 * @brief JSON Strings for Database Records.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DATABASEJSON_H
#define DATABASEJSON_H

#define JSON_DEPTNUM_FIELD                                  "deptNum"
#define JSON_NOTE_FIELD                                     "note"
#define JSON_NUMBER_FIELD                                   "number"
#define JSON_TEXT_FIELD                                     "text"

// ============================================================================
// Department
// ============================================================================

#define JSON_DEPARTMENT_DEPTNUM_FIELD                       JSON_DEPTNUM_FIELD
#define JSON_DEPARTMENT_NAME1_FIELD                         "name1"
#define JSON_DEPARTMENT_NAME2_FIELD                         "name2"

// ============================================================================
// Product
// ============================================================================

#define JSON_PRODUCT_PLU_FIELD                              "plu"
#define JSON_PRODUCT_TYPE_FIELD                             "productType"
#define JSON_PRODUCT_DESCRIPTION_FIELD                      "productDesc"
#define JSON_PRODUCT_DEPTNUM_FIELD                          JSON_DEPTNUM_FIELD

#define JSON_PRODUCT_REPORTINGCLASS_FIELD                   "reportingClassNum"
#define JSON_PRODUCT_CATEGORY_FIELD                         "categoryNum"
#define JSON_PRODUCT_SPECIALNUMBER_FIELD                    "specialNum"

#define JSON_PRODUCT_PRICEFACTOR_FIELD                      "priceFactor"
#define JSON_PRODUCT_PORTIONS_FIELD                         "portions"

#define JSON_PRODUCT_BARCODEPREFIX_FIELD                    "barcodePrefix"
#define JSON_PRODUCT_RAWBARCODENUMBER_FIELD                 "rawBarcodeNumber"
#define JSON_PRODUCT_BARCODEPRODUCTNUMBER_FIELD             "barcodeProductNumber"
#define JSON_PRODUCT_BARCODEVENDORNUMBER_FIELD              "barcodeVendorNumber"
#define JSON_PRODUCT_BARCODEEANCODE_FIELD                   "barcodeEAN"

#define JSON_PRODUCT_BYCOUNT_FIELD                          "byCount"
#define JSON_PRODUCT_FORCEBYCOUNT_FIELD                     "forceByCount"
#define JSON_PRODUCT_EXCEPTIONPRICE_FIELD                   "exceptionPrice"
#define JSON_PRODUCT_PRICE_FIELD                            "price"
#define JSON_PRODUCT_FORCEPRICE_FIELD                       "forcePrice"

#define JSON_PRODUCT_DISCOUNTMETHOD_FIELD                   "discountMethod"
#define JSON_PRODUCT_DISCOUNTPRICE1_FIELD                   "discountPrice1"
#define JSON_PRODUCT_DISCOUNTPRICE2_FIELD                   "discountPrice2"
#define JSON_PRODUCT_DISCOUNTPRICE3_FIELD                   "discountPrice3"
#define JSON_PRODUCT_BONUSPOINTS_FIELD                      "bonusPoints"

#define JSON_PRODUCT_TARE_FIELD                             "tare"
#define JSON_PRODUCT_TAREB_FIELD                            "tareB"
#define JSON_PRODUCT_FORCETARE_FIELD                        "forceTare"
#define JSON_PRODUCT_PROPORTIONALTARE_FIELD                 "proportionalTare"
#define JSON_PRODUCT_NETWEIGHT_FIELD                        "netWeight"

#define JSON_PRODUCT_SHELFLIFEDAYS_FIELD                    "shelfLifeDays"
#define JSON_PRODUCT_SHELFLIFEHOURS_FIELD                   "shelfLifeHours"
#define JSON_PRODUCT_DONTPRINTSHELFLIFE_FIELD               "doNotPriceShelfLife"
#define JSON_PRODUCT_PRODUCTLIFEDAYS_FIELD                  "productLifeDays"
#define JSON_PRODUCT_PRODUCTLIFEHOURS_FIELD                 "productLifeHours"
#define JSON_PRODUCT_DONTPRINTPRODUCTLIFE_FIELD             "doNotPrintProductLife"
#define JSON_PRODUCT_DONTPRINTPACKEDON_FIELD                "doNotPrintPackedOn"

#define JSON_PRODUCT_GRAPHIC1_FIELD                         "graphic1"
#define JSON_PRODUCT_GRAPHIC2_FIELD                         "graphic2"
#define JSON_PRODUCT_GRAPHIC3_FIELD                         "graphic3"
#define JSON_PRODUCT_GRAPHIC4_FIELD                         "graphic4"
#define JSON_PRODUCT_IMAGE_FIELD                            "image"

#define JSON_PRODUCT_NAME1_FIELD                            "name1"
#define JSON_PRODUCT_NAME2_FIELD                            "name2"

#define JSON_PRODUCT_SPECIAL_NUM_FIELD                      "specialNum"
#define JSON_PRODUCT_SPECIALSTARTDATE_FIELD                 "specialStartDate"

#define JSON_PRODUCT_MEALSIDEITEMSGROUPNUM_FIELD            "mealSideItemsGroupNum"
#define JSON_PRODUCT_MEALSIDEITEMSAREFREE_FIELD             "mealSideItemsAreFree"
#define JSON_PRODUCT_MEALNUMSIDESINCLUDED_FIELD             "mealNumSidesIncluded"

#define JSON_PRODUCT_CUTTESTMEATSPECSHEETGROUPNUM_FIELD     "cutTestMeatSpecSheetGroupNum"
#define JSON_PRODUCT_PRODUCTIONPLANNINGTYPE_FIELD           "prodPlanningType"

#define JSON_PRODUCT_LABEL1_FIELD                           "label1Num"
#define JSON_PRODUCT_LABEL2_FIELD                           "label2Num"
#define JSON_PRODUCT_LABEL3_FIELD                           "label3Num"

#define JSON_PRODUCT_APPLYSECURITYLABEL_FIELD               "applySecurityLabel"
#define JSON_PRODUCT_INTAKEMODE_FIELD                       "intakeMode"
#define JSON_PRODUCT_LABEL1ROTATION_FIELD                   "label1Rotation"
#define JSON_PRODUCT_LABEL1PLACEMENT_FIELD                  "label1Placement"
#define JSON_PRODUCT_LABEL2ROTATION_FIELD                   "label2Rotation"
#define JSON_PRODUCT_LABEL2PLACEMENT_FIELD                  "label2Placement"
#define JSON_PRODUCT_LABEL3ROTATION_FIELD                   "label3Rotation"
#define JSON_PRODUCT_LABEL3PLACEMENT_FIELD                  "label3Placement"

// COOL
#define JSON_PRODUCT_DEFAULTCOOLTEXT_FIELD                  "defaultCoolTextNum"
#define JSON_PRODUCT_CURRENTCOOLTEXT_FIELD                  "currentCoolTextNum"
#define JSON_PRODUCT_VERIFYCOOL_FIELD                       "verifyCool"
#define JSON_PRODUCT_COOLREQUIRED_FIELD                     "coolRequired"
#define JSON_PRODUCT_COOLTRACKINGTEXT_FIELD                 "coolTrackingText"

#define JSON_PRODUCT_COOLCATEGORY_FIELD                     "coolCategoryNum"
#define JSON_PRODUCT_COOLPRETEXT_FIELD                      "coolPreTextNum"
#define JSON_PRODUCT_COOLLISTKEY_FIELD                      "coolListId"

#define JSON_PRODUCT_EXP_TEXT1_FIELD                        "text1Num"
#define JSON_PRODUCT_EXP_TEXT2_FIELD                        "text2Num"                          // 7
#define JSON_PRODUCT_SPECIALMESSAGE_FIELD                   "specialMessageNum"
#define JSON_PRODUCT_NOTE_FIELD                             "noteNum"
#define JSON_PRODUCT_MARQUEE_FIELD                          "marqueeNum"
#define JSON_PRODUCT_COOKING_INST_FIELD                     "cookingInstructionsNum"            // 5
#define JSON_PRODUCT_RECIPE_FIELD                           "recipeNum"                         // 6
#define JSON_PRODUCT_SUGGEST_SELL_FIELD                     "suggestiveSellNum"                 // 8
#define JSON_PRODUCT_ALLERGENS_FIELD                        "allergensNum"                      // 9
#define JSON_PRODUCT_NUTRITION_FIELD                        "nutritionNum"

// Pick-5
#define JSON_PRODUCT_WEIGHTMIN_FIELD                        "weightMin"
#define JSON_PRODUCT_WEIGHTMAX_FIELD                        "weightMax"
#define JSON_PRODUCT_TOTALPRICEMIN_FIELD                    "totalPriceMin"
#define JSON_PRODUCT_TOTALPRICEMAX_FIELD                    "totalPriceMax"

// Automatic Mode Labels
#define JSON_PRODUCT_MANUALLABELTYPENUM1_FIELD              "manualLabel1"
#define JSON_PRODUCT_MANUALLABELTYPENUM2_FIELD              "manualLabel2"
#define JSON_PRODUCT_MANUALLABELTYPENUM3_FIELD              "manualLabel3"
#define JSON_PRODUCT_PREPACKLABELTYPENUM1_FIELD             "prepackLabel1"
#define JSON_PRODUCT_PREPACKLABELTYPENUM2_FIELD             "prepackLabel2"
#define JSON_PRODUCT_PREPACKLABELTYPENUM3_FIELD             "prepackLabel3"
#define JSON_PRODUCT_PRODENTRYLABELTYPENUM1_FIELD           "prodEntryLabel1"
#define JSON_PRODUCT_PRODENTRYLABELTYPENUM2_FIELD           "prodEntryLabel2"
#define JSON_PRODUCT_PRODENTRYLABELTYPENUM3_FIELD           "prodEntryLabel3"
#define JSON_PRODUCT_SELFSERVLABELTYPENUM1_FIELD            "selfServLabel1"
#define JSON_PRODUCT_SELFSERVLABELTYPENUM2_FIELD            "selfServLabel2"
#define JSON_PRODUCT_SELFSERVLABELTYPENUM3_FIELD            "selfServLabel3"
#define JSON_PRODUCT_PICK5LABELTYPENUM1_FIELD               "pick5Label1"
#define JSON_PRODUCT_PICK5LABELTYPENUM2_FIELD               "pick5Label2"
#define JSON_PRODUCT_PICK5LABELTYPENUM3_FIELD               "pick5Label3"

// ============================================================================
// Operator
// ============================================================================

#define JSON_OPERATOR_NUMBER_FIELD                          JSON_NUMBER_FIELD
#define JSON_OPERATOR_RFID_FIELD                            "RFID"
#define JSON_OPERATOR_NAME_FIELD                            "name"
#define JSON_OPERATOR_LANGUAGE_FIELD                        "language"
#define JSON_OPERATOR_CLASSIFICATION_FIELD                  "accessLevel"

#define JSON_OPERATOR_NOTE_FIELD                            JSON_NOTE_FIELD
#define JSON_OPERATOR_SAWNOTE_FIELD                         "sawNote"

// ============================================================================
// Operator Notes
// ============================================================================

#define JSON_OPERATOR_NOTES_NOTE_FIELD                      JSON_NOTE_FIELD

// ============================================================================
// Category
// ============================================================================

#define JSON_CATEGORY_NUMBER_FIELD                          JSON_NUMBER_FIELD
#define JSON_CATEGORY_PARENTCATEGORY_FIELD                  "parentCategoryNum"
#define JSON_CATEGORY_DEPTNUM_FIELD                         JSON_DEPTNUM_FIELD
#define JSON_CATEGORY_NAME1_FIELD                           "name1"
#define JSON_CATEGORY_NAME2_FIELD                           "name2"
#define JSON_CATEGORY_BACKGROUNDCOLOR_FIELD                 "backgroundColor"
#define JSON_CATEGORY_TEXTCOLOR_FIELD                       "textColor"
#define JSON_CATEGORY_IMAGE_FIELD                           "image"
#define JSON_CATEGORY_PLAYLIST_FIELD                        "playlist"

// ============================================================================
// Product Texts
// ============================================================================

#define JSON_PRODUCT_TEXT_TYPE_FIELD                        "type"
#define JSON_PRODUCT_TEXT_NUMBER_FIELD                      JSON_NUMBER_FIELD
#define JSON_PRODUCT_TEXT_DEPTNUM_FIELD                     JSON_DEPTNUM_FIELD
#define JSON_PRODUCT_TEXT_TEXT_FIELD                        JSON_TEXT_FIELD

// ============================================================================
// Nutrifacts
// ============================================================================

#define JSON_NUTRIFACTS_NUMBER_FIELD                        JSON_NUMBER_FIELD
#define JSON_NUTRIFACTS_DEPTNUM_FIELD                       JSON_DEPTNUM_FIELD
#define JSON_NUTRIFACTS_COL1_DATA_FIELD                     "column1"
#define JSON_NUTRIFACTS_COL2_DATA_FIELD                     "column2"
#define JSON_NUTRIFACTS_COL3_DATA_FIELD                     "column3"
#define JSON_NUTRIFACTS_COL4_DATA_FIELD                     "column4"

// ============================================================================
// Resource
// ============================================================================

#define JSON_RESOURCE_FILE_NAME                             "filename"
#define JSON_RESOURCE_DATA                                  "data"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DATABASEJSON_H
