#ifndef SQLITEPRODUCTDEFAULT_H
#define SQLITEPRODUCTDEFAULT_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"
#include "Bool.h"

#include DBPRODUCTDEFAULTI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteProductDefault : public DBProductDefaultI
	{
		public:

			// Types and subtypes
			typedef DBProductDefaultI interface_type;

			
			// Constructors and destructor
			SqliteProductDefault(SqliteRecord* r, DBProductDefaultI* d = 0);
			~SqliteProductDefault();


			// Think about replacing this method with two friend functions from the
			// SqliteProductDefaultsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBProductDefaultI
			virtual DBProductDefaultI* clone() const { return new SqliteProductDefault(new SqliteRecord(*rec_)); }
			virtual const DBProductDefaultI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getDeptNumber() const { return rec_->getField(DEPTNUM_FIELD); }
			virtual void setDeptNumber(const std::string& number) { rec_->setField(DEPTNUM_FIELD, number); }
			virtual bool isDeptNumberNull() const { return rec_->isFieldNull(DEPTNUM_FIELD); }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual int getType() const { return rec_->getField(TYPE_FIELD); }
			virtual void setType(int type) { rec_->setField(TYPE_FIELD, type); }
			virtual bool isTypeNull() const { return rec_->isFieldNull(TYPE_FIELD); }
			virtual bool isTypeWritable() const { return true; }

			virtual int getPrice() const { return rec_->getField(PRICE_FIELD); }
			virtual void setPrice(int price) { rec_->setField(PRICE_FIELD, price); }
			virtual bool isPriceNull() const { return rec_->isFieldNull(PRICE_FIELD); }
			virtual void setPriceNull() { rec_->setFieldNull(PRICE_FIELD); }
			virtual bool isPriceWritable() const { return true; }

			virtual int getPriceFactor() const { return rec_->getField(PRICEFACTOR_FIELD); }
			virtual void setPriceFactor(int pf) { rec_->setField(PRICEFACTOR_FIELD, pf); }
			virtual bool isPriceFactorNull() const { return rec_->isFieldNull(PRICEFACTOR_FIELD); }
			virtual void setPriceFactorNull() { rec_->setFieldNull(PRICEFACTOR_FIELD); }
			virtual bool isPriceFactorWritable() const { return true; }

			virtual bool getForcePrice() const { return util_string::stringtobool(rec_->getField(FORCEPRICE_FIELD)); }
			virtual void setForcePrice(bool fp) { rec_->setField(FORCEPRICE_FIELD, Bool::toString(fp)); }
			virtual bool isForcePriceNull() const { return rec_->isFieldNull(FORCEPRICE_FIELD); }
			virtual void setForcePriceNull() { rec_->setFieldNull(FORCEPRICE_FIELD); }
			virtual bool isForcePriceWritable() const { return true; }

			virtual int getExceptionPrice() const { return rec_->getField(EXCEPTIONPRICE_FIELD); }
			virtual void setExceptionPrice(int xp) { rec_->setField(EXCEPTIONPRICE_FIELD, xp); }
			virtual bool isExceptionPriceNull() const { return rec_->isFieldNull(EXCEPTIONPRICE_FIELD); }
			virtual void setExceptionPriceNull() { rec_->setFieldNull(EXCEPTIONPRICE_FIELD); }
			virtual bool isExceptionPriceWritable() const { return true; }

			virtual int getDiscountMethod() const { return rec_->getField(DISCOUNTMETHOD_FIELD); }
			virtual void setDiscountMethod(int dm) { rec_->setField(DISCOUNTMETHOD_FIELD, dm); }
			virtual bool isDiscountMethodNull() const { return rec_->isFieldNull(DISCOUNTMETHOD_FIELD); }
			virtual void setDiscountMethodNull() { rec_->setFieldNull(DISCOUNTMETHOD_FIELD); }
			virtual bool isDiscountMethodWritable() const { return true; }

			virtual int getDiscountPrice1() const { return rec_->getField(DISCOUNTPRICE1_FIELD); }
			virtual void setDiscountPrice1(int dp1) { rec_->setField(DISCOUNTPRICE1_FIELD, dp1); }
			virtual bool isDiscountPrice1Null() const { return rec_->isFieldNull(DISCOUNTPRICE1_FIELD); }
			virtual void setDiscountPrice1Null() { rec_->setFieldNull(DISCOUNTPRICE1_FIELD); }
			virtual bool isDiscountPrice1Writable() const { return true; }

			virtual int getDiscountPrice2() const { return rec_->getField(DISCOUNTPRICE2_FIELD); }
			virtual void setDiscountPrice2(int dp2) { rec_->setField(DISCOUNTPRICE2_FIELD, dp2); }
			virtual bool isDiscountPrice2Null() const { return rec_->isFieldNull(DISCOUNTPRICE2_FIELD); }
			virtual void setDiscountPrice2Null() { rec_->setFieldNull(DISCOUNTPRICE2_FIELD); }
			virtual bool isDiscountPrice2Writable() const { return true; }

			virtual int getDiscountPrice3() const { return rec_->getField(DISCOUNTPRICE3_FIELD); }
			virtual void setDiscountPrice3(int dp3) { rec_->setField(DISCOUNTPRICE3_FIELD, dp3); }
			virtual bool isDiscountPrice3Null() const { return rec_->isFieldNull(DISCOUNTPRICE3_FIELD); }
			virtual void setDiscountPrice3Null() { rec_->setFieldNull(DISCOUNTPRICE3_FIELD); }
			virtual bool isDiscountPrice3Writable() const { return true; }

			virtual int getTare() const { return rec_->getField(TARE_FIELD); }
			virtual void setTare(int t) { rec_->setField(TARE_FIELD, t); }
			virtual bool isTareNull() const { return rec_->isFieldNull(TARE_FIELD); }
			virtual void setTareNull() { rec_->setFieldNull(TARE_FIELD); }
			virtual bool isTareWritable() const { return true; }

			virtual bool getForceTare() const { return util_string::stringtobool(rec_->getField(FORCETARE_FIELD)); }
			virtual void setForceTare(bool ft) { rec_->setField(FORCETARE_FIELD, Bool::toString(ft)); }
			virtual bool isForceTareNull() const { return rec_->isFieldNull(FORCETARE_FIELD); }
			virtual void setForceTareNull() { rec_->setFieldNull(FORCETARE_FIELD); }
			virtual bool isForceTareWritable() const { return true; }

			virtual int getProportionalTare() const { return rec_->getField(PROPORTIONALTARE_FIELD); }
			virtual void setProportionalTare(int pt) { rec_->setField(PROPORTIONALTARE_FIELD, pt); }
			virtual bool isProportionalTareNull() const { return rec_->isFieldNull(PROPORTIONALTARE_FIELD); }
			virtual void setProportionalTareNull() { rec_->setFieldNull(PROPORTIONALTARE_FIELD); }
			virtual bool isProportionalTareWritable() const { return true; }

			virtual int getNetWeight() const { return rec_->getField(NETWEIGHT_FIELD); }
			virtual void setNetWeight(int nw) { rec_->setField(NETWEIGHT_FIELD, nw); }
			virtual bool isNetWeightNull() const { return rec_->isFieldNull(NETWEIGHT_FIELD); }
			virtual void setNetWeightNull() { rec_->setFieldNull(NETWEIGHT_FIELD); }
			virtual bool isNetWeightWritable() const { return true; }

			virtual int getByCount() const { return rec_->getField(BYCOUNT_FIELD); }
			virtual void setByCount(int bc) { rec_->setField(BYCOUNT_FIELD, bc); }
			virtual bool isByCountNull() const { return rec_->isFieldNull(BYCOUNT_FIELD); }
			virtual void setByCountNull() { rec_->setFieldNull(BYCOUNT_FIELD); }
			virtual bool isByCountWritable() const { return true; }

			virtual bool getForceByCount() const { return util_string::stringtobool(rec_->getField(FORCEBYCOUNT_FIELD)); }
			virtual void setForceByCount(bool fbc) { rec_->setField(FORCEBYCOUNT_FIELD, Bool::toString(fbc)); }
			virtual bool isForceByCountNull() const { return rec_->isFieldNull(FORCEBYCOUNT_FIELD); }
			virtual void setForceByCountNull() { rec_->setFieldNull(FORCEBYCOUNT_FIELD); }
			virtual bool isForceByCountWritable() const { return true; }

			virtual int getShelfLifeDays() const { return rec_->getField(SHELFLIFEDAYS_FIELD); }
			virtual void setShelfLifeDays(int sld) { rec_->setField(SHELFLIFEDAYS_FIELD, sld); }
			virtual bool isShelfLifeDaysNull() const { return rec_->isFieldNull(SHELFLIFEDAYS_FIELD); }
			virtual void setShelfLifeDaysNull() { rec_->setFieldNull(SHELFLIFEDAYS_FIELD); }
			virtual bool isShelfLifeDaysWritable() const { return true; }

			virtual int getShelfLifeHours() const { return rec_->getField(SHELFLIFEHOURS_FIELD); }
			virtual void setShelfLifeHours(int slh) { rec_->setField(SHELFLIFEHOURS_FIELD, slh); }
			virtual bool isShelfLifeHoursNull() const { return rec_->isFieldNull(SHELFLIFEHOURS_FIELD); }
			virtual void setShelfLifeHoursNull() { rec_->setFieldNull(SHELFLIFEHOURS_FIELD); }
			virtual bool isShelfLifeHoursWritable() const { return true; }

			virtual int getShelfLifeMinutes() const { return rec_->getField(SHELFLIFEMINUTES_FIELD); }
			virtual void setShelfLifeMinutes(int slm) { rec_->setField(SHELFLIFEMINUTES_FIELD, slm); }
			virtual bool isShelfLifeMinutesNull() const { return rec_->isFieldNull(SHELFLIFEMINUTES_FIELD); }
			virtual void setShelfLifeMinutesNull() { rec_->setFieldNull(SHELFLIFEMINUTES_FIELD); }
			virtual bool isShelfLifeMinutesWritable() const { return true; }

			virtual int getProductLifeDays() const { return rec_->getField(PRODUCTLIFEDAYS_FIELD); }
			virtual void setProductLifeDays(int pld) { rec_->setField(PRODUCTLIFEDAYS_FIELD, pld); }
			virtual bool isProductLifeDaysNull() const { return rec_->isFieldNull(PRODUCTLIFEDAYS_FIELD); }
			virtual void setProductLifeDaysNull() { rec_->setFieldNull(PRODUCTLIFEDAYS_FIELD); }
			virtual bool isProductLifeDaysWritable() const { return true; }

			virtual int getProductLifeHours() const { return rec_->getField(PRODUCTLIFEHOURS_FIELD); }
			virtual void setProductLifeHours(int plh) { rec_->setField(PRODUCTLIFEHOURS_FIELD, plh); }
			virtual bool isProductLifeHoursNull() const { return rec_->isFieldNull(PRODUCTLIFEHOURS_FIELD); }
			virtual void setProductLifeHoursNull() { rec_->setFieldNull(PRODUCTLIFEHOURS_FIELD); }
			virtual bool isProductLifeHoursWritable() const { return true; }

			virtual int getProductLifeMinutes() const { return rec_->getField(PRODUCTLIFEMINUTES_FIELD); }
			virtual void setProductLifeMinutes(int plm) { rec_->setField(PRODUCTLIFEMINUTES_FIELD, plm); }
			virtual bool isProductLifeMinutesNull() const { return rec_->isFieldNull(PRODUCTLIFEMINUTES_FIELD); }
			virtual void setProductLifeMinutesNull() { rec_->setFieldNull(PRODUCTLIFEMINUTES_FIELD); }
			virtual bool isProductLifeMinutesWritable() const { return true; }

			virtual bool getDontPrintShelfLife() const { return util_string::stringtobool(rec_->getField(DONTPRINTSHELFLIFE_FIELD)); }
			virtual void setDontPrintShelfLife(bool dpsl) { rec_->setField(DONTPRINTSHELFLIFE_FIELD, Bool::toString(dpsl)); }
			virtual bool isDontPrintShelfLifeNull() const { return rec_->isFieldNull(DONTPRINTSHELFLIFE_FIELD); }
			virtual void setDontPrintShelfLifeNull() { rec_->setFieldNull(DONTPRINTSHELFLIFE_FIELD); }
			virtual bool isDontPrintShelfLifeWritable() const { return true; }

			virtual bool getDontPrintProductLife() const { return util_string::stringtobool(rec_->getField(DONTPRINTPRODUCTLIFE_FIELD)); }
			virtual void setDontPrintProductLife(bool dppl) { rec_->setField(DONTPRINTPRODUCTLIFE_FIELD, Bool::toString(dppl)); }
			virtual bool isDontPrintProductLifeNull() const { return rec_->isFieldNull(DONTPRINTPRODUCTLIFE_FIELD); }
			virtual void setDontPrintProductLifeNull() { rec_->setFieldNull(DONTPRINTPRODUCTLIFE_FIELD); }
			virtual bool isDontPrintProductLifeWritable() const { return true; }

			virtual bool getDontPrintPackedOn() const { return util_string::stringtobool(rec_->getField(DONTPRINTPACKEDON_FIELD)); }
			virtual void setDontPrintPackedOn(bool dppo) { rec_->setField(DONTPRINTPACKEDON_FIELD, Bool::toString(dppo)); }
			virtual bool isDontPrintPackedOnNull() const { return rec_->isFieldNull(DONTPRINTPACKEDON_FIELD); }
			virtual void setDontPrintPackedOnNull() { rec_->setFieldNull(DONTPRINTPACKEDON_FIELD); }
			virtual bool isDontPrintPackedOnWritable() const { return true; }

			virtual int getBonusPoints() const { return rec_->getField(BONUSPOINTS_FIELD); }
			virtual void setBonusPoints(int bp) { rec_->setField(BONUSPOINTS_FIELD, bp); }
			virtual bool isBonusPointsNull() const { return rec_->isFieldNull(BONUSPOINTS_FIELD); }
			virtual void setBonusPointsNull() { rec_->setFieldNull(BONUSPOINTS_FIELD); }
			virtual bool isBonusPointsWritable() const { return true; }

			virtual int getPortions() const { return rec_->getField(PORTIONS_FIELD); }
			virtual void setPortions(int p) { rec_->setField(PORTIONS_FIELD, p); }
			virtual bool isPortionsNull() const { return rec_->isFieldNull(PORTIONS_FIELD); }
			virtual void setPortionsNull() { rec_->setFieldNull(PORTIONS_FIELD); }
			virtual bool isPortionsWritable() const { return true; }

			virtual int getProductionPlanningType() const { return rec_->getField(PRODUCTIONPLANNINGTYPE_FIELD); }
			virtual void setProductionPlanningType(int p) { rec_->setField(PRODUCTIONPLANNINGTYPE_FIELD, p); }
			virtual bool isProductionPlanningTypeNull() const { return rec_->isFieldNull(PRODUCTIONPLANNINGTYPE_FIELD); }
			virtual void setProductionPlanningTypeNull() { rec_->setFieldNull(PRODUCTIONPLANNINGTYPE_FIELD); }
			virtual bool isProductionPlanningTypeWritable() const { return true; }

			virtual bool getApplySecurityLabel() const { return util_string::stringtobool(rec_->getField(APPLYSECURITYLABEL_FIELD)); }
			virtual void setApplySecurityLabel(bool asl) { rec_->setField(APPLYSECURITYLABEL_FIELD, Bool::toString(asl)); }
			virtual bool isApplySecurityLabelNull() const { return rec_->isFieldNull(APPLYSECURITYLABEL_FIELD); }
			virtual void setApplySecurityLabelNull() { rec_->setFieldNull(APPLYSECURITYLABEL_FIELD); }
			virtual bool isApplySecurityLabelWritable() const { return true; }

			virtual int getIntakeMode() const { return rec_->getField(INTAKEMODE_FIELD); }
			virtual void setIntakeMode(int mode) { rec_->setField(INTAKEMODE_FIELD, mode); }
			virtual bool isIntakeModeNull() const { return rec_->isFieldNull(INTAKEMODE_FIELD); }
			virtual void setIntakeModeNull() { rec_->setFieldNull(INTAKEMODE_FIELD); }
			virtual bool isIntakeModeWritable() const { return true; }

			virtual int getLabel1Rotation() const { return rec_->getField(LABEL1ROTATION_FIELD); }
			virtual void setLabel1Rotation(int l1r) { rec_->setField(LABEL1ROTATION_FIELD, l1r); }
			virtual bool isLabel1RotationNull() const { return rec_->isFieldNull(LABEL1ROTATION_FIELD); }
			virtual void setLabel1RotationNull() { rec_->setFieldNull(LABEL1ROTATION_FIELD); }
			virtual bool isLabel1RotationWritable() const { return true; }

			virtual int getLabel1Placement() const { return rec_->getField(LABEL1PLACEMENT_FIELD); }
			virtual void setLabel1Placement(int l1p) { rec_->setField(LABEL1PLACEMENT_FIELD, l1p); }
			virtual bool isLabel1PlacementNull() const { return rec_->isFieldNull(LABEL1PLACEMENT_FIELD); }
			virtual void setLabel1PlacementNull() { rec_->setFieldNull(LABEL1PLACEMENT_FIELD); }
			virtual bool isLabel1PlacementWritable() const { return true; }

			virtual int getLabel2Rotation() const { return rec_->getField(LABEL2ROTATION_FIELD); }
			virtual void setLabel2Rotation(int l2r) { rec_->setField(LABEL2ROTATION_FIELD, l2r); }
			virtual bool isLabel2RotationNull() const { return rec_->isFieldNull(LABEL2ROTATION_FIELD); }
			virtual void setLabel2RotationNull() { rec_->setFieldNull(LABEL2ROTATION_FIELD); }
			virtual bool isLabel2RotationWritable() const { return true; }

			virtual int getLabel2Placement() const { return rec_->getField(LABEL2PLACEMENT_FIELD); }
			virtual void setLabel2Placement(int l2p) { rec_->setField(LABEL2PLACEMENT_FIELD, l2p); }
			virtual bool isLabel2PlacementNull() const { return rec_->isFieldNull(LABEL2PLACEMENT_FIELD); }
			virtual void setLabel2PlacementNull() { rec_->setFieldNull(LABEL2PLACEMENT_FIELD); }
			virtual bool isLabel2PlacementWritable() const { return true; }

			virtual int getLabel3Rotation() const { return rec_->getField(LABEL3ROTATION_FIELD); }
			virtual void setLabel3Rotation(int l3r) { rec_->setField(LABEL3ROTATION_FIELD, l3r); }
			virtual bool isLabel3RotationNull() const { return rec_->isFieldNull(LABEL3ROTATION_FIELD); }
			virtual void setLabel3RotationNull() { rec_->setFieldNull(LABEL3ROTATION_FIELD); }
			virtual bool isLabel3RotationWritable() const { return true; }

			virtual int getLabel3Placement() const { return rec_->getField(LABEL3PLACEMENT_FIELD); }
			virtual void setLabel3Placement(int l3p) { rec_->setField(LABEL3PLACEMENT_FIELD, l3p); }
			virtual bool isLabel3PlacementNull() const { return rec_->isFieldNull(LABEL3PLACEMENT_FIELD); }
			virtual void setLabel3PlacementNull() { rec_->setFieldNull(LABEL3PLACEMENT_FIELD); }
			virtual bool isLabel3PlacementWritable() const { return true; }

			virtual std::string getName1() const { return rec_->getField(NAME1_FIELD); }
			virtual void setName1(const std::string& name) { rec_->setField(NAME1_FIELD, name); }
			virtual bool isName1Null() const { return rec_->isFieldNull(NAME1_FIELD); }
			virtual void setName1Null() { rec_->setFieldNull(NAME1_FIELD); }
			virtual bool isName1Writable() const { return true; }

			virtual std::string getName2() const { return rec_->getField(NAME2_FIELD); }
			virtual void setName2(const std::string& name) { rec_->setField(NAME2_FIELD, name); }
			virtual bool isName2Null() const { return rec_->isFieldNull(NAME2_FIELD); }
			virtual void setName2Null() { rec_->setFieldNull(NAME2_FIELD); }
			virtual bool isName2Writable() const { return true; }

			//VERIFYCOOL_FIELD

			//COOLREQUIRED_FIELD

			//PRODUCTIONPLANNINGTYPE_FIELD

			virtual std::string getReportingClass() const { return rec_->getField(REPORTINGCLASS_FIELD); }
			virtual void setReportingClass(const std::string& rc) { rec_->setField(REPORTINGCLASS_FIELD, rc); }
			virtual bool isReportingClassNull() const { return rec_->isFieldNull(REPORTINGCLASS_FIELD); }
			virtual void setReportingClassNull() { rec_->setFieldNull(REPORTINGCLASS_FIELD); }
			virtual bool isReportingClassWritable() const { return true; }

			virtual std::string getText() const { return rec_->getField(TEXT_FIELD); }
			virtual void setText(const std::string& t) { rec_->setField(TEXT_FIELD, t); }
			virtual bool isTextNull() const { return rec_->isFieldNull(TEXT_FIELD); }
			virtual void setTextNull() { rec_->setFieldNull(TEXT_FIELD); }
			virtual bool isTextWritable() const { return true; }

			virtual std::string getSpecialMessage() const { return rec_->getField(SPECIALMESSAGE_FIELD); }
			virtual void setSpecialMessage(const std::string& sm) { rec_->setField(SPECIALMESSAGE_FIELD, sm); }
			virtual bool isSpecialMessageNull() const { return rec_->isFieldNull(SPECIALMESSAGE_FIELD); }
			virtual void setSpecialMessageNull() { rec_->setFieldNull(SPECIALMESSAGE_FIELD); }
			virtual bool isSpecialMessageWritable() const { return true; }

			virtual std::string getNutrition() const { return rec_->getField(NUTRITION_FIELD); }
			virtual void setNutrition(const std::string& n) { rec_->setField(NUTRITION_FIELD, n); }
			virtual bool isNutritionNull() const { return rec_->isFieldNull(NUTRITION_FIELD); }
			virtual void setNutritionNull() { rec_->setFieldNull(NUTRITION_FIELD); }
			virtual bool isNutritionWritable() const { return true; }

			virtual std::string getMarquee() const { return rec_->getField(MARQUEE_FIELD); }
			virtual void setMarquee(const std::string& t) { rec_->setField(MARQUEE_FIELD, t); }
			virtual bool isMarqueeNull() const { return rec_->isFieldNull(MARQUEE_FIELD); }
			virtual void setMarqueeNull() { rec_->setFieldNull(MARQUEE_FIELD); }
			virtual bool isMarqueeWritable() const { return true; }

			virtual std::string getNote() const { return rec_->getField(NOTE_FIELD); }
			virtual void setNote(const std::string& n) { rec_->setField(NOTE_FIELD, n); }
			virtual bool isNoteNull() const { return rec_->isFieldNull(NOTE_FIELD); }
			virtual void setNoteNull() { rec_->setFieldNull(NOTE_FIELD); }
			virtual bool isNoteWritable() const { return true; }

			virtual std::string getCategory() const { return rec_->getField(CATEGORY_FIELD); }
			virtual void setCategory(const std::string& cat) { rec_->setField(CATEGORY_FIELD, cat); }
			virtual bool isCategoryNull() const { return rec_->isFieldNull(CATEGORY_FIELD); }
			virtual void setCategoryNull() { rec_->setFieldNull(CATEGORY_FIELD); }
			virtual bool isCategoryWritable() const { return true; }

			virtual std::string getLabel1() const { return rec_->getField(LABEL1_FIELD); }
			virtual void setLabel1(const std::string& l1) { rec_->setField(LABEL1_FIELD, l1); }
			virtual bool isLabel1Null() const { return rec_->isFieldNull(LABEL1_FIELD); }
			virtual void setLabel1Null() { rec_->setFieldNull(LABEL1_FIELD); }
			virtual bool isLabel1Writable() const { return true; }

			virtual std::string getLabel2() const { return rec_->getField(LABEL2_FIELD); }
			virtual void setLabel2(const std::string& l2) { rec_->setField(LABEL2_FIELD, l2); }
			virtual bool isLabel2Null() const { return rec_->isFieldNull(LABEL2_FIELD); }
			virtual void setLabel2Null() { rec_->setFieldNull(LABEL2_FIELD); }
			virtual bool isLabel2Writable() const { return true; }

			virtual std::string getLabel3() const { return rec_->getField(LABEL3_FIELD); }
			virtual void setLabel3(const std::string& l3) { rec_->setField(LABEL3_FIELD, l3); }
			virtual bool isLabel3Null() const { return rec_->isFieldNull(LABEL3_FIELD); }
			virtual void setLabel3Null() { rec_->setFieldNull(LABEL3_FIELD); }
			virtual bool isLabel3Writable() const { return true; }

			virtual int getSpecialType() const { return rec_->getField(SPECIALTYPE_FIELD); }
			virtual void setSpecialType(int st) { rec_->setField(SPECIALTYPE_FIELD, st); }
			virtual bool isSpecialTypeNull() const { return rec_->isFieldNull(SPECIALTYPE_FIELD); }
			virtual void setSpecialTypeNull() { rec_->setFieldNull(SPECIALTYPE_FIELD); }
			virtual bool isSpecialTypeWritable() const { return true; }

			// COOLCATEGORYNUMBER_FIELD

			// DEFAULTCOOLTEXTNUMBER_FIELD

			// COOLPRETEXTNUMBER_FIELD

			// COOLLISTNUMBER_FIELD

			virtual int getUPCNumberSystem() const { return rec_->getField(UPCNUMBERSYSTEM_FIELD); }
			virtual void setUPCNumberSystem(int upcns) { rec_->setField(UPCNUMBERSYSTEM_FIELD, upcns); }
			virtual bool isUPCNumberSystemNull() const { return rec_->isFieldNull(UPCNUMBERSYSTEM_FIELD); }
			virtual void setUPCNumberSystemNull() { rec_->setFieldNull(UPCNUMBERSYSTEM_FIELD); }
			virtual bool isUPCNumberSystemWritable() const { return true; }

			virtual std::string getEANFormatString() const { return rec_->getField(EANFORMATSTRING_FIELD); }
			virtual void setEANFormatString(const std::string& eanfs) { rec_->setField(EANFORMATSTRING_FIELD, eanfs); }
			virtual bool isEANFormatStringNull() const { return rec_->isFieldNull(EANFORMATSTRING_FIELD); }
			virtual void setEANFormatStringNull() { rec_->setFieldNull(EANFORMATSTRING_FIELD); }
			virtual bool isEANFormatStringWritable() const { return true; }

			virtual std::string getEAN128CFormatString() const { return rec_->getField(EAN128CFORMATSTRING_FIELD); }
			virtual void setEAN128CFormatString(const std::string& eanfs) { rec_->setField(EAN128CFORMATSTRING_FIELD, eanfs); }
			virtual bool isEAN128CFormatStringNull() const { return rec_->isFieldNull(EAN128CFORMATSTRING_FIELD); }
			virtual void setEAN128CFormatStringNull() { rec_->setFieldNull(EAN128CFORMATSTRING_FIELD); }
			virtual bool isEAN128CFormatStringWritable() const { return true; }

			virtual std::string getGS1GTINFormatString() const { return rec_->getField(GS1GTINFORMATSTRING_FIELD); }
			virtual void setGS1GTINFormatString(const std::string& gs1GTINfs) { rec_->setField(GS1GTINFORMATSTRING_FIELD, gs1GTINfs); }
			virtual bool isGS1GTINFormatStringNull() const { return rec_->isFieldNull(GS1GTINFORMATSTRING_FIELD); }
			virtual void setGS1GTINFormatStringNull() { rec_->setFieldNull(GS1GTINFORMATSTRING_FIELD); }
			virtual bool isGS1GTINFormatStringWritable() const { return true; }

			virtual std::string getGS1AIFormatString() const { return rec_->getField(GS1AIFORMATSTRING_FIELD); }
			virtual void setGS1AIFormatString(const std::string& gs1AIfs) { rec_->setField(GS1AIFORMATSTRING_FIELD, gs1AIfs); }
			virtual bool isGS1AIFormatStringNull() const { return rec_->isFieldNull(GS1AIFORMATSTRING_FIELD); }
			virtual void setGS1AIFormatStringNull() { rec_->setFieldNull(GS1AIFORMATSTRING_FIELD); }
			virtual bool isGS1AIFormatStringWritable() const { return true; }
			
			virtual std::string getQRCodeFormatString() const { return rec_->getField(QRCODEFORMATSTRING_FIELD); }
			virtual void setQRCodeFormatString(const std::string& qrcodefs) { rec_->setField(QRCODEFORMATSTRING_FIELD, qrcodefs); }
			virtual bool isQRCodeFormatStringNull() const { return rec_->isFieldNull(QRCODEFORMATSTRING_FIELD); }
			virtual void setQRCodeFormatStringNull() { rec_->setFieldNull(QRCODEFORMATSTRING_FIELD); }
			virtual bool isQRCodeFormatStringWritable() const { return true; }

			virtual std::string getDataMatrixFormatString() const { return rec_->getField(DATAMATRIXFORMATSTRING_FIELD); }
			virtual void setDataMatrixFormatString(const std::string& datamatrixfs) { rec_->setField(DATAMATRIXFORMATSTRING_FIELD, datamatrixfs); }
			virtual bool isDataMatrixFormatStringNull() const { return rec_->isFieldNull(DATAMATRIXFORMATSTRING_FIELD); }
			virtual void setDataMatrixFormatStringNull() { rec_->setFieldNull(DATAMATRIXFORMATSTRING_FIELD); }
			virtual bool isDataMatrixFormatStringWritable() const { return true; }
			
			virtual int getEANPrefix() const { return rec_->getField(EANPREFIX_FIELD); }
			virtual void setEANPrefix(int eanPrefix) { rec_->setField(EANPREFIX_FIELD, eanPrefix); }
			virtual bool isEANPrefixNull() const { return rec_->isFieldNull(EANPREFIX_FIELD); }
			virtual void setEANPrefixNull() { rec_->setFieldNull(EANPREFIX_FIELD); }
			virtual bool isEANPrefixWritable() const { return true; }

			virtual std::string getDescription() const { return rec_->getField(DESCRIPTION_FIELD); }
			virtual void setDescription(const std::string& desc) { rec_->setField(DESCRIPTION_FIELD, desc); }
			virtual bool isDescriptionNull() const { return rec_->isFieldNull(DESCRIPTION_FIELD); }
			virtual void setDescriptionNull() { rec_->setFieldNull(DESCRIPTION_FIELD); }
			virtual bool isDescriptionWritable() const { return true; }

			virtual std::string getGraphic1() const { return rec_->getField(GRAPHIC1_FIELD); }
			virtual void setGraphic1(const std::string& g1) { rec_->setField(GRAPHIC1_FIELD, g1); }
			virtual bool isGraphic1Null() const { return rec_->isFieldNull(GRAPHIC1_FIELD); }
			virtual void setGraphic1Null() { rec_->setFieldNull(GRAPHIC1_FIELD); }
			virtual bool isGraphic1Writable() const { return true; }

			virtual std::string getGraphic2() const { return rec_->getField(GRAPHIC2_FIELD); }
			virtual void setGraphic2(const std::string& g2) { rec_->setField(GRAPHIC2_FIELD, g2); }
			virtual bool isGraphic2Null() const { return rec_->isFieldNull(GRAPHIC2_FIELD); }
			virtual void setGraphic2Null() { rec_->setFieldNull(GRAPHIC2_FIELD); }
			virtual bool isGraphic2Writable() const { return true; }

			virtual std::string getGraphic3() const { return rec_->getField(GRAPHIC3_FIELD); }
			virtual void setGraphic3(const std::string& g3) { rec_->setField(GRAPHIC3_FIELD, g3); }
			virtual bool isGraphic3Null() const { return rec_->isFieldNull(GRAPHIC3_FIELD); }
			virtual void setGraphic3Null() { rec_->setFieldNull(GRAPHIC3_FIELD); }
			virtual bool isGraphic3Writable() const { return true; }

			virtual std::string getGraphic4() const { return rec_->getField(GRAPHIC4_FIELD); }
			virtual void setGraphic4(const std::string& g4) { rec_->setField(GRAPHIC4_FIELD, g4); }
			virtual bool isGraphic4Null() const { return rec_->isFieldNull(GRAPHIC4_FIELD); }
			virtual void setGraphic4Null() { rec_->setFieldNull(GRAPHIC4_FIELD); }
			virtual bool isGraphic4Writable() const { return true; }

			virtual std::string getImage() const { return rec_->getField(IMAGE_FIELD); }
			virtual void setImage(const std::string& img) { rec_->setField(IMAGE_FIELD, img); }
			virtual bool isImageNull() const { return rec_->isFieldNull(IMAGE_FIELD); }
			virtual void setImageNull() { rec_->setFieldNull(IMAGE_FIELD); }
			virtual bool isImageWritable() const { return true; }

			// COOLTRACKINGNUMBER_FIELD

			virtual int getMealSideItemsGroupNum() const { return rec_->getField(MEALSIDEITEMSGROUPNUM_FIELD); }
			virtual void setMealSideItemsGroupNum(int p) { rec_->setField(MEALSIDEITEMSGROUPNUM_FIELD, p); }
			virtual bool isMealSideItemsGroupNumNull() const { return rec_->isFieldNull(MEALSIDEITEMSGROUPNUM_FIELD); }
			virtual void setMealSideItemsGroupNumNull() { rec_->setFieldNull(MEALSIDEITEMSGROUPNUM_FIELD); }
			virtual bool isMealSideItemsGroupNumWritable() const { return true; }

			virtual bool getMealSideItemsAreFree() const { return util_string::stringtobool(rec_->getField(MEALSIDEITEMSAREFREE_FIELD)); }
			virtual void setMealSideItemsAreFree(int f) { rec_->setField(MEALSIDEITEMSAREFREE_FIELD, Bool::toString(f)); }
			virtual bool isMealSideItemsAreFreeNull() const { return rec_->isFieldNull(MEALSIDEITEMSAREFREE_FIELD); }
			virtual void setMealSideItemsAreFreeNull() { rec_->setFieldNull(MEALSIDEITEMSAREFREE_FIELD); }
			virtual bool isMealSideItemsAreFreeWritable() const { return true; }

			virtual int getMealNumSidesIncluded() const { return rec_->getField(MEALNUMSIDESINCLUDED_FIELD); }
			virtual void setMealNumSidesIncluded(int p) { rec_->setField(MEALNUMSIDESINCLUDED_FIELD, p); }
			virtual bool isMealNumSidesIncludedNull() const { return rec_->isFieldNull(MEALNUMSIDESINCLUDED_FIELD); }
			virtual void setMealNumSidesIncludedNull() { rec_->setFieldNull(MEALNUMSIDESINCLUDED_FIELD); }
			virtual bool isMealNumSidesIncludedWritable() const { return true; }

			virtual int getCutTestMeatSpecSheetGroupNum() const { return rec_->getField(CUTTESTMEATSPECSHEETGROUPNUM_FIELD); }
			virtual void setCutTestMeatSpecSheetGroupNum(int p) { rec_->setField(CUTTESTMEATSPECSHEETGROUPNUM_FIELD, p); }
			virtual bool isCutTestMeatSpecSheetGroupNumNull() const { return rec_->isFieldNull(CUTTESTMEATSPECSHEETGROUPNUM_FIELD); }
			virtual void setCutTestMeatSpecSheetGroupNumNull() { rec_->setFieldNull(CUTTESTMEATSPECSHEETGROUPNUM_FIELD); }
			virtual bool isCutTestMeatSpecSheetGroupNumWritable() const { return true; }
			virtual int getTareB() const { return rec_->getField(TAREB_FIELD); }
			virtual void setTareB(int t) { rec_->setField(TAREB_FIELD, t); }
			virtual bool isTareBNull() const { return rec_->isFieldNull(TAREB_FIELD); }
			virtual void setTareBNull() { rec_->setFieldNull(TAREB_FIELD); }
			virtual bool isTareBWritable() const { return true; }

			// Automatic mode labels
			// manual
			virtual std::string getManualLabelTypeNum1() const { return rec_->getField(MANUALLABELTYPENUM1_FIELD); }
			virtual void setManualLabelTypeNum1(const std::string& l1) { rec_->setField(MANUALLABELTYPENUM1_FIELD, l1); }
			virtual bool isManualLabelTypeNum1Null() const { return rec_->isFieldNull(MANUALLABELTYPENUM1_FIELD); }
			virtual void setManualLabelTypeNum1Null() { rec_->setFieldNull(MANUALLABELTYPENUM1_FIELD); }
			virtual bool isManualLabelTypeNum1Writable() const { return true; }

			virtual std::string getManualLabelTypeNum2() const { return rec_->getField(MANUALLABELTYPENUM2_FIELD); }
			virtual void setManualLabelTypeNum2(const std::string& l2) { rec_->setField(MANUALLABELTYPENUM2_FIELD, l2); }
			virtual bool isManualLabelTypeNum2Null() const { return rec_->isFieldNull(MANUALLABELTYPENUM2_FIELD); }
			virtual void setManualLabelTypeNum2Null() { rec_->setFieldNull(MANUALLABELTYPENUM2_FIELD); }
			virtual bool isManualLabelTypeNum2Writable() const { return true; }

			virtual std::string getManualLabelTypeNum3() const { return rec_->getField(MANUALLABELTYPENUM3_FIELD); }
			virtual void setManualLabelTypeNum3(const std::string& l3) { rec_->setField(MANUALLABELTYPENUM3_FIELD, l3); }
			virtual bool isManualLabelTypeNum3Null() const { return rec_->isFieldNull(MANUALLABELTYPENUM3_FIELD); }
			virtual void setManualLabelTypeNum3Null() { rec_->setFieldNull(MANUALLABELTYPENUM3_FIELD); }
			virtual bool isManualLabelTypeNum3Writable() const { return true; }
			// prepack
			virtual std::string getPrepackLabelTypeNum1() const { return rec_->getField(PREPACKLABELTYPENUM1_FIELD); }
			virtual void setPrepackLabelTypeNum1(const std::string& l1) { rec_->setField(PREPACKLABELTYPENUM1_FIELD, l1); }
			virtual bool isPrepackLabelTypeNum1Null() const { return rec_->isFieldNull(PREPACKLABELTYPENUM1_FIELD); }
			virtual void setPrepackLabelTypeNum1Null() { rec_->setFieldNull(PREPACKLABELTYPENUM1_FIELD); }
			virtual bool isPrepackLabelTypeNum1Writable() const { return true; }

			virtual std::string getPrepackLabelTypeNum2() const { return rec_->getField(PREPACKLABELTYPENUM2_FIELD); }
			virtual void setPrepackLabelTypeNum2(const std::string& l2) { rec_->setField(PREPACKLABELTYPENUM2_FIELD, l2); }
			virtual bool isPrepackLabelTypeNum2Null() const { return rec_->isFieldNull(PREPACKLABELTYPENUM2_FIELD); }
			virtual void setPrepackLabelTypeNum2Null() { rec_->setFieldNull(PREPACKLABELTYPENUM2_FIELD); }
			virtual bool isPrepackLabelTypeNum2Writable() const { return true; }

			virtual std::string getPrepackLabelTypeNum3() const { return rec_->getField(PREPACKLABELTYPENUM3_FIELD); }
			virtual void setPrepackLabelTypeNum3(const std::string& l3) { rec_->setField(PREPACKLABELTYPENUM3_FIELD, l3); }
			virtual bool isPrepackLabelTypeNum3Null() const { return rec_->isFieldNull(PREPACKLABELTYPENUM3_FIELD); }
			virtual void setPrepackLabelTypeNum3Null() { rec_->setFieldNull(PREPACKLABELTYPENUM3_FIELD); }
			virtual bool isPrepackLabelTypeNum3Writable() const { return true; }
			// prodentry
			virtual std::string getProdEntryLabelTypeNum1() const { return rec_->getField(PRODENTRYLABELTYPENUM1_FIELD); }
			virtual void setProdEntryLabelTypeNum1(const std::string& l1) { rec_->setField(PRODENTRYLABELTYPENUM1_FIELD, l1); }
			virtual bool isProdEntryLabelTypeNum1Null() const { return rec_->isFieldNull(PRODENTRYLABELTYPENUM1_FIELD); }
			virtual void setProdEntryLabelTypeNum1Null() { rec_->setFieldNull(PRODENTRYLABELTYPENUM1_FIELD); }
			virtual bool isProdEntryLabelTypeNum1Writable() const { return true; }

			virtual std::string getProdEntryLabelTypeNum2() const { return rec_->getField(PRODENTRYLABELTYPENUM2_FIELD); }
			virtual void setProdEntryLabelTypeNum2(const std::string& l2) { rec_->setField(PRODENTRYLABELTYPENUM2_FIELD, l2); }
			virtual bool isProdEntryLabelTypeNum2Null() const { return rec_->isFieldNull(PRODENTRYLABELTYPENUM2_FIELD); }
			virtual void setProdEntryLabelTypeNum2Null() { rec_->setFieldNull(PRODENTRYLABELTYPENUM2_FIELD); }
			virtual bool isProdEntryLabelTypeNum2Writable() const { return true; }

			virtual std::string getProdEntryLabelTypeNum3() const { return rec_->getField(PRODENTRYLABELTYPENUM3_FIELD); }
			virtual void setProdEntryLabelTypeNum3(const std::string& l3) { rec_->setField(PRODENTRYLABELTYPENUM3_FIELD, l3); }
			virtual bool isProdEntryLabelTypeNum3Null() const { return rec_->isFieldNull(PRODENTRYLABELTYPENUM3_FIELD); }
			virtual void setProdEntryLabelTypeNum3Null() { rec_->setFieldNull(PRODENTRYLABELTYPENUM3_FIELD); }
			virtual bool isProdEntryLabelTypeNum3Writable() const { return true; }
			// selfserv
			virtual std::string getSelfServLabelTypeNum1() const { return rec_->getField(SELFSERVLABELTYPENUM1_FIELD); }
			virtual void setSelfServLabelTypeNum1(const std::string& l1) { rec_->setField(SELFSERVLABELTYPENUM1_FIELD, l1); }
			virtual bool isSelfServLabelTypeNum1Null() const { return rec_->isFieldNull(SELFSERVLABELTYPENUM1_FIELD); }
			virtual void setSelfServLabelTypeNum1Null() { rec_->setFieldNull(SELFSERVLABELTYPENUM1_FIELD); }
			virtual bool isSelfServLabelTypeNum1Writable() const { return true; }

			virtual std::string getSelfServLabelTypeNum2() const { return rec_->getField(SELFSERVLABELTYPENUM2_FIELD); }
			virtual void setSelfServLabelTypeNum2(const std::string& l2) { rec_->setField(SELFSERVLABELTYPENUM2_FIELD, l2); }
			virtual bool isSelfServLabelTypeNum2Null() const { return rec_->isFieldNull(SELFSERVLABELTYPENUM2_FIELD); }
			virtual void setSelfServLabelTypeNum2Null() { rec_->setFieldNull(SELFSERVLABELTYPENUM2_FIELD); }
			virtual bool isSelfServLabelTypeNum2Writable() const { return true; }

			virtual std::string getSelfServLabelTypeNum3() const { return rec_->getField(SELFSERVLABELTYPENUM3_FIELD); }
			virtual void setSelfServLabelTypeNum3(const std::string& l3) { rec_->setField(SELFSERVLABELTYPENUM3_FIELD, l3); }
			virtual bool isSelfServLabelTypeNum3Null() const { return rec_->isFieldNull(SELFSERVLABELTYPENUM3_FIELD); }
			virtual void setSelfServLabelTypeNum3Null() { rec_->setFieldNull(SELFSERVLABELTYPENUM3_FIELD); }
			virtual bool isSelfServLabelTypeNum3Writable() const { return true; }
			// pick5
			virtual std::string getPick5LabelTypeNum1() const { return rec_->getField(PICK5LABELTYPENUM1_FIELD); }
			virtual void setPick5LabelTypeNum1(const std::string& l1) { rec_->setField(PICK5LABELTYPENUM1_FIELD, l1); }
			virtual bool isPick5LabelTypeNum1Null() const { return rec_->isFieldNull(PICK5LABELTYPENUM1_FIELD); }
			virtual void setPick5LabelTypeNum1Null() { rec_->setFieldNull(PICK5LABELTYPENUM1_FIELD); }
			virtual bool isPick5LabelTypeNum1Writable() const { return true; }

			virtual std::string getPick5LabelTypeNum2() const { return rec_->getField(PICK5LABELTYPENUM2_FIELD); }
			virtual void setPick5LabelTypeNum2(const std::string& l2) { rec_->setField(PICK5LABELTYPENUM2_FIELD, l2); }
			virtual bool isPick5LabelTypeNum2Null() const { return rec_->isFieldNull(PICK5LABELTYPENUM2_FIELD); }
			virtual void setPick5LabelTypeNum2Null() { rec_->setFieldNull(PICK5LABELTYPENUM2_FIELD); }
			virtual bool isPick5LabelTypeNum2Writable() const { return true; }

			virtual std::string getPick5LabelTypeNum3() const { return rec_->getField(PICK5LABELTYPENUM3_FIELD); }
			virtual void setPick5LabelTypeNum3(const std::string& l3) { rec_->setField(PICK5LABELTYPENUM3_FIELD, l3); }
			virtual bool isPick5LabelTypeNum3Null() const { return rec_->isFieldNull(PICK5LABELTYPENUM3_FIELD); }
			virtual void setPick5LabelTypeNum3Null() { rec_->setFieldNull(PICK5LABELTYPENUM3_FIELD); }
			virtual bool isPick5LabelTypeNum3Writable() const { return true; }

			// OnlineOrdering
			virtual std::string getOnlineOrderingLabelTypeNum1() const { return rec_->getField(ONLINEORDERINGLABELTYPENUM1_FIELD); }
			virtual void setOnlineOrderingLabelTypeNum1(const std::string& l1) { rec_->setField(ONLINEORDERINGLABELTYPENUM1_FIELD, l1); }
			virtual bool isOnlineOrderingLabelTypeNum1Null() const { return rec_->isFieldNull(ONLINEORDERINGLABELTYPENUM1_FIELD); }
			virtual void setOnlineOrderingLabelTypeNum1Null() { rec_->setFieldNull(ONLINEORDERINGLABELTYPENUM1_FIELD); }
			virtual bool isOnlineOrderingLabelTypeNum1Writable() const { return true; }

			virtual std::string getOnlineOrderingLabelTypeNum2() const { return rec_->getField(ONLINEORDERINGLABELTYPENUM2_FIELD); }
			virtual void setOnlineOrderingLabelTypeNum2(const std::string& l2) { rec_->setField(ONLINEORDERINGLABELTYPENUM2_FIELD, l2); }
			virtual bool isOnlineOrderingLabelTypeNum2Null() const { return rec_->isFieldNull(ONLINEORDERINGLABELTYPENUM2_FIELD); }
			virtual void setOnlineOrderingLabelTypeNum2Null() { rec_->setFieldNull(ONLINEORDERINGLABELTYPENUM2_FIELD); }
			virtual bool isOnlineOrderingLabelTypeNum2Writable() const { return true; }

			virtual std::string getOnlineOrderingLabelTypeNum3() const { return rec_->getField(ONLINEORDERINGLABELTYPENUM3_FIELD); }
			virtual void setOnlineOrderingLabelTypeNum3(const std::string& l3) { rec_->setField(ONLINEORDERINGLABELTYPENUM3_FIELD, l3); }
			virtual bool isOnlineOrderingLabelTypeNum3Null() const { return rec_->isFieldNull(ONLINEORDERINGLABELTYPENUM3_FIELD); }
			virtual void setOnlineOrderingLabelTypeNum3Null() { rec_->setFieldNull(ONLINEORDERINGLABELTYPENUM3_FIELD); }
			virtual bool isOnlineOrderingLabelTypeNum3Writable() const { return true; }

			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string DEPTNUM_FIELD;
			static const std::string TYPE_FIELD;
			static const std::string PRICE_FIELD;
			static const std::string PRICEFACTOR_FIELD;
			static const std::string FORCEPRICE_FIELD;
			static const std::string EXCEPTIONPRICE_FIELD;
			static const std::string DISCOUNTMETHOD_FIELD;
			static const std::string DISCOUNTPRICE1_FIELD;
			static const std::string DISCOUNTPRICE2_FIELD;
			static const std::string DISCOUNTPRICE3_FIELD;
			static const std::string TARE_FIELD;
			static const std::string FORCETARE_FIELD;
			static const std::string PROPORTIONALTARE_FIELD;
			static const std::string NETWEIGHT_FIELD;
			static const std::string BYCOUNT_FIELD;
			static const std::string FORCEBYCOUNT_FIELD;
			static const std::string SHELFLIFEDAYS_FIELD;
			static const std::string SHELFLIFEHOURS_FIELD;
			static const std::string SHELFLIFEMINUTES_FIELD;
			static const std::string PRODUCTLIFEDAYS_FIELD;
			static const std::string PRODUCTLIFEHOURS_FIELD;
			static const std::string PRODUCTLIFEMINUTES_FIELD;
			static const std::string DONTPRINTSHELFLIFE_FIELD;
			static const std::string DONTPRINTPRODUCTLIFE_FIELD;
			static const std::string DONTPRINTPACKEDON_FIELD;
			static const std::string BONUSPOINTS_FIELD;
			static const std::string PORTIONS_FIELD;
			static const std::string APPLYSECURITYLABEL_FIELD;
			static const std::string INTAKEMODE_FIELD;
			static const std::string LABEL1ROTATION_FIELD;
			static const std::string LABEL1PLACEMENT_FIELD;
			static const std::string LABEL2ROTATION_FIELD;
			static const std::string LABEL2PLACEMENT_FIELD;
			static const std::string LABEL3ROTATION_FIELD;
			static const std::string LABEL3PLACEMENT_FIELD;
			static const std::string NAME1_FIELD;
			static const std::string NAME2_FIELD;
			static const std::string VERIFYCOOL_FIELD;
			static const std::string COOLREQUIRED_FIELD;
			static const std::string PRODUCTIONPLANNINGTYPE_FIELD;
			static const std::string REPORTINGCLASS_FIELD;
			static const std::string TEXT_FIELD;
			static const std::string SPECIALMESSAGE_FIELD;
			static const std::string NUTRITION_FIELD;
			static const std::string MARQUEE_FIELD;
			static const std::string NOTE_FIELD;
			static const std::string CATEGORY_FIELD;
			static const std::string LABEL1_FIELD;
			static const std::string LABEL2_FIELD;
			static const std::string LABEL3_FIELD;
			static const std::string SPECIALTYPE_FIELD;
			static const std::string COOLCATEGORYNUMBER_FIELD;
			static const std::string DEFAULTCOOLTEXTNUMBER_FIELD;
			static const std::string COOLPRETEXTNUMBER_FIELD;
			static const std::string COOLLISTNUMBER_FIELD;
			static const std::string UPCNUMBERSYSTEM_FIELD;
			static const std::string EANFORMATSTRING_FIELD;
			static const std::string EAN128CFORMATSTRING_FIELD;
			static const std::string GS1GTINFORMATSTRING_FIELD;
			static const std::string GS1AIFORMATSTRING_FIELD;
			static const std::string QRCODEFORMATSTRING_FIELD;
			static const std::string DATAMATRIXFORMATSTRING_FIELD;
			static const std::string EANPREFIX_FIELD;
			static const std::string DESCRIPTION_FIELD;
			static const std::string GRAPHIC1_FIELD;
			static const std::string GRAPHIC2_FIELD;
			static const std::string GRAPHIC3_FIELD;
			static const std::string GRAPHIC4_FIELD;
			static const std::string IMAGE_FIELD;
			static const std::string COOLTRACKINGNUMBER_FIELD;
			//static const std::string FORCEPORTIONS_FIELD;
			static const std::string MEALSIDEITEMSGROUPNUM_FIELD;
			static const std::string MEALSIDEITEMSAREFREE_FIELD;
			static const std::string MEALNUMSIDESINCLUDED_FIELD;
			static const std::string CUTTESTMEATSPECSHEETGROUPNUM_FIELD;
			static const std::string TAREB_FIELD;
			static const std::string MANUALLABELTYPENUM1_FIELD;
			static const std::string MANUALLABELTYPENUM2_FIELD;
			static const std::string MANUALLABELTYPENUM3_FIELD;
			static const std::string PREPACKLABELTYPENUM1_FIELD;
			static const std::string PREPACKLABELTYPENUM2_FIELD;
			static const std::string PREPACKLABELTYPENUM3_FIELD;
			static const std::string PRODENTRYLABELTYPENUM1_FIELD;
			static const std::string PRODENTRYLABELTYPENUM2_FIELD;
			static const std::string PRODENTRYLABELTYPENUM3_FIELD;
			static const std::string SELFSERVLABELTYPENUM1_FIELD;
			static const std::string SELFSERVLABELTYPENUM2_FIELD;
			static const std::string SELFSERVLABELTYPENUM3_FIELD;
			static const std::string PICK5LABELTYPENUM1_FIELD;
			static const std::string PICK5LABELTYPENUM2_FIELD;
			static const std::string PICK5LABELTYPENUM3_FIELD;
			static const std::string ONLINEORDERINGLABELTYPENUM1_FIELD;
			static const std::string ONLINEORDERINGLABELTYPENUM2_FIELD;
			static const std::string ONLINEORDERINGLABELTYPENUM3_FIELD;

		private:

			SqliteRecord* rec_;
			DBProductDefaultI* decorated_;

			// Do not allow the default copy constructor.
			SqliteProductDefault(const SqliteProductDefault& rec);
			// Do not allow the default assignment operator.
			SqliteProductDefault operator=(const SqliteProductDefault& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRODUCTDEFAULT_H
