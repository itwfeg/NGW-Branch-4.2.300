#ifndef DBPRODUCTDEFAULT_H
#define DBPRODUCTDEFAULT_H


#include <string>


#include "edb.h"
#include DBPRODUCTDEFAULTI_INC
#if defined( __linux__ )
    #include DBPRODUCTDEFAULTSRM_INC
#endif

#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;
	

	class DBProductDefault
	{
			// DBProductDefaultsRM class is a friend of DBProductDefault because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBProductDefaultsRM;
			friend class DBQuery<DBProductDefault>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBProductDefaultI interface_type;
			typedef DBProductDefaultsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBProductDefault(const DBProductDefault& rec); 
			~DBProductDefault();


			// Methods associated with DBProductDefaultI
			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }

			int getType() const { return decorated_->getType(); }
			void setType(int type) { decorated_->setType(type); }
			bool isTypeNull() const { return decorated_->isTypeNull(); }
			bool isTypeWritable() const { return decorated_->isTypeWritable(); }

			std::string getReportingClass() const { return decorated_->getReportingClass(); }
			void setReportingClass(const std::string& rc) { decorated_->setReportingClass(rc); }
			bool isReportingClassNull() const { return decorated_->isReportingClassNull(); }
			void setReportingClassNull() { decorated_->setReportingClassNull(); }
			bool isReportingClassWritable() const { return decorated_->isReportingClassWritable(); }

			int getByCount() const { return decorated_->getByCount(); }
			void setByCount(int bc) { decorated_->setByCount(bc); }
			bool isByCountNull() const { return decorated_->isByCountNull(); }
			void setByCountNull() { decorated_->setByCountNull(); }
			bool isByCountWritable() const { return decorated_->isByCountWritable(); }

			bool getForceByCount() const { return decorated_->getForceByCount(); }
			void setForceByCount(bool fbc) { decorated_->setForceByCount(fbc); }
			bool isForceByCountNull() const { return decorated_->isForceByCountNull(); }
			void setForceByCountNull() { decorated_->setForceByCountNull(); }
			bool isForceByCountWritable() const { return decorated_->isForceByCountWritable(); }

			int getPrice() const { return decorated_->getPrice(); }
			void setPrice(int price) { decorated_->setPrice(price); }
			bool isPriceNull() const { return decorated_->isPriceNull(); }
			void setPriceNull() { decorated_->setPriceNull(); }
			bool isPriceWritable() const { return decorated_->isPriceWritable(); }

			int getPriceFactor() const { return decorated_->getPriceFactor(); }
			void setPriceFactor(int pf) { decorated_->setPriceFactor(pf); }
			bool isPriceFactorNull() const { return decorated_->isPriceFactorNull(); }
			void setPriceFactorNull() { decorated_->setPriceFactorNull(); }
			bool isPriceFactorWritable() const { return decorated_->isPriceFactorWritable(); }

			bool getForcePrice() const { return decorated_->getForcePrice(); }
			void setForcePrice(bool fp) { decorated_->setForcePrice(fp); }
			bool isForcePriceNull() const { return decorated_->isForcePriceNull(); }
			void setForcePriceNull() { decorated_->setForcePriceNull(); }
			bool isForcePriceWritable() const { return decorated_->isForcePriceWritable(); }

			int getDiscountPrice1() const { return decorated_->getDiscountPrice1(); }
			void setDiscountPrice1(int dp1) { decorated_->setDiscountPrice1(dp1); }
			bool isDiscountPrice1Null() const { return decorated_->isDiscountPrice1Null(); }
			void setDiscountPrice1Null() { decorated_->setDiscountPrice1Null(); }
			bool isDiscountPrice1Writable() const { return decorated_->isDiscountPrice1Writable(); }

			int getDiscountPrice2() const { return decorated_->getDiscountPrice2(); }
			void setDiscountPrice2(int dp2) { decorated_->setDiscountPrice2(dp2); }
			bool isDiscountPrice2Null() const { return decorated_->isDiscountPrice2Null(); }
			void setDiscountPrice2Null() { decorated_->setDiscountPrice2Null(); }
			bool isDiscountPrice2Writable() const { return decorated_->isDiscountPrice2Writable(); }

			int getDiscountPrice3() const { return decorated_->getDiscountPrice3(); }
			void setDiscountPrice3(int dp3) { decorated_->setDiscountPrice3(dp3); }
			bool isDiscountPrice3Null() const { return decorated_->isDiscountPrice3Null(); }
			void setDiscountPrice3Null() { decorated_->setDiscountPrice3Null(); }
			bool isDiscountPrice3Writable() const { return decorated_->isDiscountPrice3Writable(); }

			int getDiscountMethod() const { return decorated_->getDiscountMethod(); }
			void setDiscountMethod(int dm) { decorated_->setDiscountMethod(dm); }
			bool isDiscountMethodNull() const { return decorated_->isDiscountMethodNull(); }
			void setDiscountMethodNull() { decorated_->setDiscountMethodNull(); }
			bool isDiscountMethodWritable() const { return decorated_->isDiscountMethodWritable(); }

			int getExceptionPrice() const { return decorated_->getExceptionPrice(); }
			void setExceptionPrice(int xp) { decorated_->setExceptionPrice(xp); }
			bool isExceptionPriceNull() const { return decorated_->isExceptionPriceNull(); }
			void setExceptionPriceNull() { decorated_->setExceptionPriceNull(); }
			bool isExceptionPriceWritable() const { return decorated_->isExceptionPriceWritable(); }

			std::string getDescription() const { return decorated_->getDescription(); }
			void setDescription(const std::string& desc) { decorated_->setDescription(desc); }
			bool isDescriptionNull() const { return decorated_->isDescriptionNull(); }
			void setDescriptionNull() { decorated_->setDescriptionNull(); }
			bool isDescriptionWritable() const { return decorated_->isDescriptionWritable(); }

			std::string getName1() const { return decorated_->getName1(); }
			void setName1(const std::string& name) { decorated_->setName1(name); }
			bool isName1Null() const { return decorated_->isName1Null(); }
			void setName1Null() { decorated_->setName1Null(); }
			bool isName1Writable() const { return decorated_->isName1Writable(); }

			std::string getName2() const { return decorated_->getName2(); }
			void setName2(const std::string& name) { decorated_->setName2(name); }
			bool isName2Null() const { return decorated_->isName2Null(); }
			void setName2Null() { decorated_->setName2Null(); }
			bool isName2Writable() const { return decorated_->isName2Writable(); }

			int getUPCNumberSystem() const { return decorated_->getUPCNumberSystem(); }
			void setUPCNumberSystem(int upcns) { decorated_->setUPCNumberSystem(upcns); }
			bool isUPCNumberSystemNull() const { return decorated_->isUPCNumberSystemNull(); }
			void setUPCNumberSystemNull() { decorated_->setUPCNumberSystemNull(); }
			bool isUPCNumberSystemWritable() const { return decorated_->isUPCNumberSystemWritable(); }

			std::string getEANFormatString() const { return decorated_->getEANFormatString(); }
			void setEANFormatString(const std::string& eanfs) { decorated_->setEANFormatString(eanfs); }
			bool isEANFormatStringNull() const { return decorated_->isEANFormatStringNull(); }
			void setEANFormatStringNull() { decorated_->setEANFormatStringNull(); }
			bool isEANFormatStringWritable() const { return decorated_->isEANFormatStringWritable(); }

			std::string getEAN128CFormatString() const { return decorated_->getEAN128CFormatString(); }
			void setEAN128CFormatString(const std::string& eanfs) { decorated_->setEAN128CFormatString(eanfs); }
			bool isEAN128CFormatStringNull() const { return decorated_->isEAN128CFormatStringNull(); }
			void setEAN128CFormatStringNull() { decorated_->setEAN128CFormatStringNull(); }
			bool isEAN128CFormatStringWritable() const { return decorated_->isEAN128CFormatStringWritable(); }

			std::string getGS1GTINFormatString() const { return decorated_->getGS1GTINFormatString(); }
			void setGS1GTINFormatString(const std::string& gs1GTINfs) { decorated_->setGS1GTINFormatString(gs1GTINfs); }
			bool isGS1GTINFormatStringNull() const { return decorated_->isGS1GTINFormatStringNull(); }
			void setGS1GTINFormatStringNull() { decorated_->setGS1GTINFormatStringNull(); }
			bool isGS1GTINFormatStringWritable() const { return decorated_->isGS1GTINFormatStringWritable(); }

			std::string getGS1AIFormatString() const { return decorated_->getGS1AIFormatString(); }
			void setGS1AIFormatString(const std::string& gs1AIfs) { decorated_->setGS1AIFormatString(gs1AIfs); }
			bool isGS1AIFormatStringNull() const { return decorated_->isGS1AIFormatStringNull(); }
			void setGS1AIFormatStringNull() { decorated_->setGS1AIFormatStringNull(); }
			bool isGS1AIFormatStringWritable() const { return decorated_->isGS1AIFormatStringWritable(); }

			std::string getQRCodeFormatString() const { return decorated_->getQRCodeFormatString(); }
			void setQRCodeFormatString(const std::string& qrcodefs) { decorated_->setQRCodeFormatString(qrcodefs); }
			bool isQRCodeFormatStringNull() const { return decorated_->isQRCodeFormatStringNull(); }
			void setQRCodeFormatStringNull() { decorated_->setQRCodeFormatStringNull(); }
			bool isQRCodeFormatStringWritable() const { return decorated_->isQRCodeFormatStringWritable(); }

			std::string getDataMatrixFormatString() const { return decorated_->getDataMatrixFormatString(); }
			void setDataMatrixFormatString(const std::string& datamatrixfs) { decorated_->setDataMatrixFormatString(datamatrixfs); }
			bool isDataMatrixFormatStringNull() const { return decorated_->isDataMatrixFormatStringNull(); }
			void setDataMatrixFormatStringNull() { decorated_->setDataMatrixFormatStringNull(); }
			bool isDataMatrixFormatStringWritable() const { return decorated_->isDataMatrixFormatStringWritable(); }

			int getEANPrefix() const { return decorated_->getEANPrefix(); }
			void setEANPrefix(int eanPrefix) { decorated_->setEANPrefix(eanPrefix); }
			bool isEANPrefixNull() const { return decorated_->isEANPrefixNull(); }
			void setEANPrefixNull() { decorated_->setEANPrefixNull(); }
			bool isEANPrefixWritable() const { return decorated_->isEANPrefixWritable(); }

			int getNetWeight() const { return decorated_->getNetWeight(); }
			void setNetWeight(int nw) { decorated_->setNetWeight(nw); }
			bool isNetWeightNull() const { return decorated_->isNetWeightNull(); }
			void setNetWeightNull() { decorated_->setNetWeightNull(); }
			bool isNetWeightWritable() const { return decorated_->isNetWeightWritable(); }

			int getShelfLifeDays() const { return decorated_->getShelfLifeDays(); }
			void setShelfLifeDays(int sld) { decorated_->setShelfLifeDays(sld); }
			bool isShelfLifeDaysNull() const { return decorated_->isShelfLifeDaysNull(); }
			void setShelfLifeDaysNull() { decorated_->setShelfLifeDaysNull(); }
			bool isShelfLifeDaysWritable() const { return decorated_->isShelfLifeDaysWritable(); }

			int getShelfLifeHours() const { return decorated_->getShelfLifeHours(); }
			void setShelfLifeHours(int slh) { decorated_->setShelfLifeHours(slh); }
			bool isShelfLifeHoursNull() const { return decorated_->isShelfLifeHoursNull(); }
			void setShelfLifeHoursNull() { decorated_->setShelfLifeHoursNull(); }
			bool isShelfLifeHoursWritable() const { return decorated_->isShelfLifeHoursWritable(); }

			int getShelfLifeMinutes() const { return decorated_->getShelfLifeMinutes(); }
			void setShelfLifeMinutes(int slm) { decorated_->setShelfLifeMinutes(slm); }
			bool isShelfLifeMinutesNull() const { return decorated_->isShelfLifeMinutesNull(); }
			void setShelfLifeMinutesNull() { decorated_->setShelfLifeMinutesNull(); }
			bool isShelfLifeMinutesWritable() const { return decorated_->isShelfLifeMinutesWritable(); }

			bool getDontPrintShelfLife() const { return decorated_->getDontPrintShelfLife(); }
			void setDontPrintShelfLife(bool dpsl) { decorated_->setDontPrintShelfLife(dpsl); }
			bool isDontPrintShelfLifeNull() const { return decorated_->isDontPrintShelfLifeNull(); }
			void setDontPrintShelfLifeNull() { decorated_->setDontPrintShelfLifeNull(); }
			bool isDontPrintShelfLifeWritable() const { return decorated_->isDontPrintShelfLifeWritable(); }

			int getProductLifeDays() const { return decorated_->getProductLifeDays(); }
			void setProductLifeDays(int pld) { decorated_->setProductLifeDays(pld); }
			bool isProductLifeDaysNull() const { return decorated_->isProductLifeDaysNull(); }
			void setProductLifeDaysNull() { decorated_->setProductLifeDaysNull(); }
			bool isProductLifeDaysWritable() const { return decorated_->isProductLifeDaysWritable(); }

			int getProductLifeHours() const { return decorated_->getProductLifeHours(); }
			void setProductLifeHours(int plh) { decorated_->setProductLifeHours(plh); }
			bool isProductLifeHoursNull() const { return decorated_->isProductLifeHoursNull(); }
			void setProductLifeHoursNull() { decorated_->setProductLifeHoursNull(); }
			bool isProductLifeHoursWritable() const { return decorated_->isProductLifeHoursWritable(); }

			int getProductLifeMinutes() const { return decorated_->getProductLifeMinutes(); }
			void setProductLifeMinutes(int plh) { decorated_->setProductLifeMinutes(plh); }
			bool isProductLifeMinutesNull() const { return decorated_->isProductLifeMinutesNull(); }
			void setProductLifeMinutesNull() { decorated_->setProductLifeMinutesNull(); }
			bool isProductLifeMinutesWritable() const { return decorated_->isProductLifeMinutesWritable(); }

			bool getDontPrintProductLife() const { return decorated_->getDontPrintProductLife(); }
			void setDontPrintProductLife(bool dppl) { decorated_->setDontPrintProductLife(dppl); }
			bool isDontPrintProductLifeNull() const { return decorated_->isDontPrintProductLifeNull(); }
			void setDontPrintProductLifeNull() { decorated_->setDontPrintProductLifeNull(); }
			bool isDontPrintProductLifeWritable() const { return decorated_->isDontPrintProductLifeWritable(); }

			bool getDontPrintPackedOn() const { return decorated_->getDontPrintPackedOn(); }
			void setDontPrintPackedOn(bool dppo) { decorated_->setDontPrintPackedOn(dppo); }
			bool isDontPrintPackedOnNull() const { return decorated_->isDontPrintPackedOnNull(); }
			void setDontPrintPackedOnNull() { decorated_->setDontPrintPackedOnNull(); }
			bool isDontPrintPackedOnWritable() const { return decorated_->isDontPrintPackedOnWritable(); }

			std::string getSpecialMessage() const { return decorated_->getSpecialMessage(); }
			void setSpecialMessage(const std::string& sm) { decorated_->setSpecialMessage(sm); }
			bool isSpecialMessageNull() const { return decorated_->isSpecialMessageNull(); }
			void setSpecialMessageNull() { decorated_->setSpecialMessageNull(); }
			bool isSpecialMessageWritable() const { return decorated_->isSpecialMessageWritable(); }

			std::string getText() const { return decorated_->getText(); }
			void setText(const std::string& t) { decorated_->setText(t); }
			bool isTextNull() const { return decorated_->isTextNull(); }
			void setTextNull() { decorated_->setTextNull(); }
			bool isTextWritable() const { return decorated_->isTextWritable(); }

			std::string getNutrition() const { return decorated_->getNutrition(); }
			void setNutrition(const std::string& n) { decorated_->setNutrition(n); }
			bool isNutritionNull() const { return decorated_->isNutritionNull(); }
			void setNutritionNull() { decorated_->setNutritionNull(); }
			bool isNutritionWritable() const { return decorated_->isNutritionWritable(); }

			std::string getMarquee() const { return decorated_->getMarquee(); }
			void setMarquee(const std::string& t) { decorated_->setMarquee(t); }
			bool isMarqueeNull() const { return decorated_->isMarqueeNull(); }
			void setMarqueeNull() { decorated_->setMarqueeNull(); }
			bool isMarqueeWritable() const { return decorated_->isMarqueeWritable(); }

			std::string getNote() const { return decorated_->getNote(); }
			void setNote(const std::string& n) { decorated_->setNote(n); }
			bool isNoteNull() const { return decorated_->isNoteNull(); }
			void setNoteNull() { decorated_->setNoteNull(); }
			bool isNoteWritable() const { return decorated_->isNoteWritable(); }

			std::string getCategory() const { return decorated_->getCategory(); }
			void setCategory(const std::string& cat) { decorated_->setCategory(cat); }
			bool isCategoryNull() const { return decorated_->isCategoryNull(); }
			void setCategoryNull() { decorated_->setCategoryNull(); }
			bool isCategoryWritable() const { return decorated_->isCategoryWritable(); }

			std::string getGraphic1() const { return decorated_->getGraphic1(); }
			void setGraphic1(const std::string& g1) { decorated_->setGraphic1(g1); }
			bool isGraphic1Null() const { return decorated_->isGraphic1Null(); }
			void setGraphic1Null() { decorated_->setGraphic1Null(); }
			bool isGraphic1Writable() const { return decorated_->isGraphic1Writable(); }

			std::string getGraphic2() const { return decorated_->getGraphic2(); }
			void setGraphic2(const std::string& g2) { decorated_->setGraphic2(g2); }
			bool isGraphic2Null() const { return decorated_->isGraphic2Null(); }
			void setGraphic2Null() { decorated_->setGraphic2Null(); }
			bool isGraphic2Writable() const { return decorated_->isGraphic2Writable(); }

			std::string getGraphic3() const { return decorated_->getGraphic3(); }
			void setGraphic3(const std::string& g3) { decorated_->setGraphic3(g3); }
			bool isGraphic3Null() const { return decorated_->isGraphic3Null(); }
			void setGraphic3Null() { decorated_->setGraphic3Null(); }
			bool isGraphic3Writable() const { return decorated_->isGraphic3Writable(); }

			std::string getGraphic4() const { return decorated_->getGraphic4(); }
			void setGraphic4(const std::string& g4) { decorated_->setGraphic4(g4); }
			bool isGraphic4Null() const { return decorated_->isGraphic4Null(); }
			void setGraphic4Null() { decorated_->setGraphic4Null(); }
			bool isGraphic4Writable() const { return decorated_->isGraphic4Writable(); }

			std::string getImage() const { return decorated_->getImage(); }
			void setImage(const std::string& img) { decorated_->setImage(img); }
			bool isImageNull() const { return decorated_->isImageNull(); }
			void setImageNull() { decorated_->setImageNull(); }
			bool isImageWritable() const { return decorated_->isImageWritable(); }

			std::string getLabel1() const { return decorated_->getLabel1(); }
			void setLabel1(const std::string& l1) { decorated_->setLabel1(l1); }
			bool isLabel1Null() const { return decorated_->isLabel1Null(); }
			void setLabel1Null() { decorated_->setLabel1Null(); }
			bool isLabel1Writable() const { return decorated_->isLabel1Writable(); }

			int getLabel1Rotation() const { return decorated_->getLabel1Rotation(); }
			void setLabel1Rotation(int l1r) { decorated_->setLabel1Rotation(l1r); }
			bool isLabel1RotationNull() const { return decorated_->isLabel1RotationNull(); }
			void setLabel1RotationNull() { decorated_->setLabel1RotationNull(); }
			bool isLabel1RotationWritable() const { return decorated_->isLabel1RotationWritable(); }

			int getLabel1Placement() const { return decorated_->getLabel1Placement(); }
			void setLabel1Placement(int l1p) { decorated_->setLabel1Placement(l1p); }
			bool isLabel1PlacementNull() const { return decorated_->isLabel1PlacementNull(); }
			void setLabel1PlacementNull() { decorated_->setLabel1PlacementNull(); }
			bool isLabel1PlacementWritable() const { return decorated_->isLabel1PlacementWritable(); }

			std::string getLabel2() const { return decorated_->getLabel2(); }
			void setLabel2(const std::string& l2) { decorated_->setLabel2(l2); }
			bool isLabel2Null() const { return decorated_->isLabel2Null(); }
			void setLabel2Null() { decorated_->setLabel2Null(); }
			bool isLabel2Writable() const { return decorated_->isLabel2Writable(); }

			int getLabel2Rotation() const { return decorated_->getLabel2Rotation(); }
			void setLabel2Rotation(int l2r) { decorated_->setLabel2Rotation(l2r); }
			bool isLabel2RotationNull() const { return decorated_->isLabel2RotationNull(); }
			void setLabel2RotationNull() { decorated_->setLabel2RotationNull(); }
			bool isLabel2RotationWritable() const { return decorated_->isLabel2RotationWritable(); }

			int getLabel2Placement() const { return decorated_->getLabel2Placement(); }
			void setLabel2Placement(int l2p) { decorated_->setLabel2Placement(l2p); }
			bool isLabel2PlacementNull() const { return decorated_->isLabel2PlacementNull(); }
			void setLabel2PlacementNull() { decorated_->setLabel2PlacementNull(); }
			bool isLabel2PlacementWritable() const { return decorated_->isLabel2PlacementWritable(); }

			std::string getLabel3() const { return decorated_->getLabel3(); }
			void setLabel3(const std::string& l3) { decorated_->setLabel3(l3); }
			bool isLabel3Null() const { return decorated_->isLabel3Null(); }
			void setLabel3Null() { decorated_->setLabel3Null(); }
			bool isLabel3Writable() const { return decorated_->isLabel3Writable(); }

			int getLabel3Rotation() const { return decorated_->getLabel3Rotation(); }
			void setLabel3Rotation(int l3r) { decorated_->setLabel3Rotation(l3r); }
			bool isLabel3RotationNull() const { return decorated_->isLabel3RotationNull(); }
			void setLabel3RotationNull() { decorated_->setLabel3RotationNull(); }
			bool isLabel3RotationWritable() const { return decorated_->isLabel3RotationWritable(); }

			int getLabel3Placement() const { return decorated_->getLabel3Placement(); }
			void setLabel3Placement(int l3p) { decorated_->setLabel3Placement(l3p); }
			bool isLabel3PlacementNull() const { return decorated_->isLabel3PlacementNull(); }
			void setLabel3PlacementNull() { decorated_->setLabel3PlacementNull(); }
			bool isLabel3PlacementWritable() const { return decorated_->isLabel3PlacementWritable(); }

			int getProportionalTare() const { return decorated_->getProportionalTare(); }
			void setProportionalTare(int pt) { decorated_->setProportionalTare(pt); }
			bool isProportionalTareNull() const { return decorated_->isProportionalTareNull(); }
			void setProportionalTareNull() { return decorated_->setProportionalTareNull(); }
			bool isProportionalTareWritable() const { return decorated_->isProportionalTareWritable(); }

			int getTare() const { return decorated_->getTare(); }
			void setTare(int t) { decorated_->setTare(t); }
			bool isTareNull() const { return decorated_->isTareNull(); }
			void setTareNull() { return decorated_->setTareNull(); }
			bool isTareWritable() const { return decorated_->isTareWritable(); }

			bool getForceTare() const { return decorated_->getForceTare(); }
			void setForceTare(bool ft) { decorated_->setForceTare(ft); }
			bool isForceTareNull() const { return decorated_->isForceTareNull(); }
			void setForceTareNull() { return decorated_->setForceTareNull(); }
			bool isForceTareWritable() const { return decorated_->isForceTareWritable(); }

			int getBonusPoints() const { return decorated_->getBonusPoints(); }
			void setBonusPoints(int bp) { decorated_->setBonusPoints(bp); }
			bool isBonusPointsNull() const { return decorated_->isBonusPointsNull(); }
			void setBonusPointsNull() { return decorated_->setBonusPointsNull(); }
			bool isBonusPointsWritable() const { return decorated_->isBonusPointsWritable(); }

			int getIntakeMode() const { return decorated_->getIntakeMode(); }
			void setIntakeMode(int mode) { decorated_->setIntakeMode(mode); }
			bool isIntakeModeNull() const { return decorated_->isIntakeModeNull(); }
			void setIntakeModeNull() { return decorated_->setIntakeModeNull(); }
			bool isIntakeModeWritable() const { return decorated_->isIntakeModeWritable(); }

			bool getApplySecurityLabel() const { return decorated_->getApplySecurityLabel(); }
			void setApplySecurityLabel(bool asl) { decorated_->setApplySecurityLabel(asl); }
			bool isApplySecurityLabelNull() const { return decorated_->isApplySecurityLabelNull(); }
			void setApplySecurityLabelNull() { return decorated_->setApplySecurityLabelNull(); }
			bool isApplySecurityLabelWritable() const { return decorated_->isApplySecurityLabelWritable(); }

			int getSpecialType() const { return decorated_->getSpecialType(); }
			void setSpecialType(int st) { decorated_->setSpecialType(st); }
			bool isSpecialTypeNull() const { return decorated_->isSpecialTypeNull(); }
			void setSpecialTypeNull() { return decorated_->setSpecialTypeNull(); }
			bool isSpecialTypeWritable() const { return decorated_->isSpecialTypeWritable(); }

			int getPortions() const { return decorated_->getPortions(); }
			void setPortions(int p) { decorated_->setPortions(p); }
			bool isPortionsNull() const { return decorated_->isPortionsNull(); }
			void setPortionsNull() { decorated_->setPortionsNull(); }
			bool isPortionsWritable() const { return decorated_->isPortionsWritable(); }

			int getProductionPlanningType() const { return decorated_->getProductionPlanningType(); }
			void setProductionPlanningType(int p) { decorated_->setProductionPlanningType(p); }
			bool isProductionPlanningTypeNull() const { return decorated_->isProductionPlanningTypeNull(); }
			void setProductionPlanningTypeNull() { decorated_->setProductionPlanningTypeNull(); }
			bool isProductionPlanningTypeWritable() const { return decorated_->isProductionPlanningTypeWritable(); }

			int getMealSideItemsGroupNum() const { return decorated_->getMealSideItemsGroupNum(); }
			void setMealSideItemsGroupNum(int p) { decorated_->setMealSideItemsGroupNum(p); }
			bool isMealSideItemsGroupNumNull() const { return decorated_->isMealSideItemsGroupNumNull(); }
			void setMealSideItemsGroupNumNull() { decorated_->setMealSideItemsGroupNumNull(); }
			bool isMealSideItemsGroupNumWritable() const { return decorated_->isMealSideItemsGroupNumWritable(); }

			bool getMealSideItemsAreFree() const { return decorated_->getMealSideItemsAreFree(); }
			void setMealSideItemsAreFree(int f) { decorated_->setMealSideItemsAreFree(f); }
			bool isMealSideItemsAreFreeNull() const { return decorated_->isMealSideItemsAreFreeNull(); }
			void setMealSideItemsAreFreeNull() { decorated_->setMealSideItemsAreFreeNull(); }
			bool isMealSideItemsAreFreeWritable() const { return decorated_->isMealSideItemsAreFreeWritable(); }

			int getMealNumSidesIncluded() const { return decorated_->getMealNumSidesIncluded(); }
			void setMealNumSidesIncluded(int p) { decorated_->setMealNumSidesIncluded(p); }
			bool isMealNumSidesIncludedNull() const { return decorated_->isMealNumSidesIncludedNull(); }
			void setMealNumSidesIncludedNull() { decorated_->setMealNumSidesIncludedNull(); }
			bool isMealNumSidesIncludedWritable() const { return decorated_->isMealNumSidesIncludedWritable(); }

			int getCutTestMeatSpecSheetGroupNum() const { return decorated_->getCutTestMeatSpecSheetGroupNum(); }
			void setCutTestMeatSpecSheetGroupNum(int p) { decorated_->setCutTestMeatSpecSheetGroupNum(p); }
			bool isCutTestMeatSpecSheetGroupNumNull() const { return decorated_->isCutTestMeatSpecSheetGroupNumNull(); }
			void setCutTestMeatSpecSheetGroupNumNull() { decorated_->setCutTestMeatSpecSheetGroupNumNull(); }
			bool isCutTestMeatSpecSheetGroupNumWritable() const { return decorated_->isCutTestMeatSpecSheetGroupNumWritable(); }
			int getTareB() const { return decorated_->getTareB(); }
			void setTareB(int t) { decorated_->setTareB(t); }
			bool isTareBNull() const { return decorated_->isTareBNull(); }
			void setTareBNull() { return decorated_->setTareBNull(); }
			bool isTareBWritable() const { return decorated_->isTareBWritable(); }
			// manual
			std::string getManualLabelTypeNum1() const { return decorated_->getManualLabelTypeNum1(); }
			void setManualLabelTypeNum1(const std::string& l1) { decorated_->setManualLabelTypeNum1(l1); }
			bool isManualLabelTypeNum1Null() const { return decorated_->isManualLabelTypeNum1Null(); }
			void setManualLabelTypeNum1Null() { decorated_->setManualLabelTypeNum1Null(); }
			bool isManualLabelTypeNum1Writable() const { return decorated_->isManualLabelTypeNum1Writable(); }

			std::string getManualLabelTypeNum2() const { return decorated_->getManualLabelTypeNum2(); }
			void setManualLabelTypeNum2(const std::string& l2) { decorated_->setManualLabelTypeNum2(l2); }
			bool isManualLabelTypeNum2Null() const { return decorated_->isManualLabelTypeNum2Null(); }
			void setManualLabelTypeNum2Null() { decorated_->setManualLabelTypeNum2Null(); }
			bool isManualLabelTypeNum2Writable() const { return decorated_->isManualLabelTypeNum2Writable(); }

			std::string getManualLabelTypeNum3() const { return decorated_->getManualLabelTypeNum3(); }
			void setManualLabelTypeNum3(const std::string& l3) { decorated_->setManualLabelTypeNum3(l3); }
			bool isManualLabelTypeNum3Null() const { return decorated_->isManualLabelTypeNum3Null(); }
			void setManualLabelTypeNum3Null() { decorated_->setManualLabelTypeNum3Null(); }
			bool isManualLabelTypeNum3Writable() const { return decorated_->isManualLabelTypeNum3Writable(); }
			// prepack
			std::string getPrepackLabelTypeNum1() const { return decorated_->getPrepackLabelTypeNum1(); }
			void setPrepackLabelTypeNum1(const std::string& l1) { decorated_->setPrepackLabelTypeNum1(l1); }
			bool isPrepackLabelTypeNum1Null() const { return decorated_->isPrepackLabelTypeNum1Null(); }
			void setPrepackLabelTypeNum1Null() { decorated_->setPrepackLabelTypeNum1Null(); }
			bool isPrepackLabelTypeNum1Writable() const { return decorated_->isPrepackLabelTypeNum1Writable(); }

			std::string getPrepackLabelTypeNum2() const { return decorated_->getPrepackLabelTypeNum2(); }
			void setPrepackLabelTypeNum2(const std::string& l2) { decorated_->setPrepackLabelTypeNum2(l2); }
			bool isPrepackLabelTypeNum2Null() const { return decorated_->isPrepackLabelTypeNum2Null(); }
			void setPrepackLabelTypeNum2Null() { decorated_->setPrepackLabelTypeNum2Null(); }
			bool isPrepackLabelTypeNum2Writable() const { return decorated_->isPrepackLabelTypeNum2Writable(); }

			std::string getPrepackLabelTypeNum3() const { return decorated_->getPrepackLabelTypeNum3(); }
			void setPrepackLabelTypeNum3(const std::string& l3) { decorated_->setPrepackLabelTypeNum3(l3); }
			bool isPrepackLabelTypeNum3Null() const { return decorated_->isPrepackLabelTypeNum3Null(); }
			void setPrepackLabelTypeNum3Null() { decorated_->setPrepackLabelTypeNum3Null(); }
			bool isPrepackLabelTypeNum3Writable() const { return decorated_->isPrepackLabelTypeNum3Writable(); }
			// prodentry
			std::string getProdEntryLabelTypeNum1() const { return decorated_->getProdEntryLabelTypeNum1(); }
			void setProdEntryLabelTypeNum1(const std::string& l1) { decorated_->setProdEntryLabelTypeNum1(l1); }
			bool isProdEntryLabelTypeNum1Null() const { return decorated_->isProdEntryLabelTypeNum1Null(); }
			void setProdEntryLabelTypeNum1Null() { decorated_->setProdEntryLabelTypeNum1Null(); }
			bool isProdEntryLabelTypeNum1Writable() const { return decorated_->isProdEntryLabelTypeNum1Writable(); }

			std::string getProdEntryLabelTypeNum2() const { return decorated_->getProdEntryLabelTypeNum2(); }
			void setProdEntryLabelTypeNum2(const std::string& l2) { decorated_->setProdEntryLabelTypeNum2(l2); }
			bool isProdEntryLabelTypeNum2Null() const { return decorated_->isProdEntryLabelTypeNum2Null(); }
			void setProdEntryLabelTypeNum2Null() { decorated_->setProdEntryLabelTypeNum2Null(); }
			bool isProdEntryLabelTypeNum2Writable() const { return decorated_->isProdEntryLabelTypeNum2Writable(); }

			std::string getProdEntryLabelTypeNum3() const { return decorated_->getProdEntryLabelTypeNum3(); }
			void setProdEntryLabelTypeNum3(const std::string& l3) { decorated_->setProdEntryLabelTypeNum3(l3); }
			bool isProdEntryLabelTypeNum3Null() const { return decorated_->isProdEntryLabelTypeNum3Null(); }
			void setProdEntryLabelTypeNum3Null() { decorated_->setProdEntryLabelTypeNum3Null(); }
			bool isProdEntryLabelTypeNum3Writable() const { return decorated_->isProdEntryLabelTypeNum3Writable(); }
			// selfserv
			std::string getSelfServLabelTypeNum1() const { return decorated_->getSelfServLabelTypeNum1(); }
			void setSelfServLabelTypeNum1(const std::string& l1) { decorated_->setSelfServLabelTypeNum1(l1); }
			bool isSelfServLabelTypeNum1Null() const { return decorated_->isSelfServLabelTypeNum1Null(); }
			void setSelfServLabelTypeNum1Null() { decorated_->setSelfServLabelTypeNum1Null(); }
			bool isSelfServLabelTypeNum1Writable() const { return decorated_->isSelfServLabelTypeNum1Writable(); }

			std::string getSelfServLabelTypeNum2() const { return decorated_->getSelfServLabelTypeNum2(); }
			void setSelfServLabelTypeNum2(const std::string& l2) { decorated_->setSelfServLabelTypeNum2(l2); }
			bool isSelfServLabelTypeNum2Null() const { return decorated_->isSelfServLabelTypeNum2Null(); }
			void setSelfServLabelTypeNum2Null() { decorated_->setSelfServLabelTypeNum2Null(); }
			bool isSelfServLabelTypeNum2Writable() const { return decorated_->isSelfServLabelTypeNum2Writable(); }

			std::string getSelfServLabelTypeNum3() const { return decorated_->getSelfServLabelTypeNum3(); }
			void setSelfServLabelTypeNum3(const std::string& l3) { decorated_->setSelfServLabelTypeNum3(l3); }
			bool isSelfServLabelTypeNum3Null() const { return decorated_->isSelfServLabelTypeNum3Null(); }
			void setSelfServLabelTypeNum3Null() { decorated_->setSelfServLabelTypeNum3Null(); }
			bool isSelfServLabelTypeNum3Writable() const { return decorated_->isSelfServLabelTypeNum3Writable(); }
			// pick5
			std::string getPick5LabelTypeNum1() const { return decorated_->getPick5LabelTypeNum1(); }
			void setPick5LabelTypeNum1(const std::string& l1) { decorated_->setPick5LabelTypeNum1(l1); }
			bool isPick5LabelTypeNum1Null() const { return decorated_->isPick5LabelTypeNum1Null(); }
			void setPick5LabelTypeNum1Null() { decorated_->setPick5LabelTypeNum1Null(); }
			bool isPick5LabelTypeNum1Writable() const { return decorated_->isPick5LabelTypeNum1Writable(); }

			std::string getPick5LabelTypeNum2() const { return decorated_->getPick5LabelTypeNum2(); }
			void setPick5LabelTypeNum2(const std::string& l2) { decorated_->setPick5LabelTypeNum2(l2); }
			bool isPick5LabelTypeNum2Null() const { return decorated_->isPick5LabelTypeNum2Null(); }
			void setPick5LabelTypeNum2Null() { decorated_->setPick5LabelTypeNum2Null(); }
			bool isPick5LabelTypeNum2Writable() const { return decorated_->isPick5LabelTypeNum2Writable(); }

			std::string getPick5LabelTypeNum3() const { return decorated_->getPick5LabelTypeNum3(); }
			void setPick5LabelTypeNum3(const std::string& l3) { decorated_->setPick5LabelTypeNum3(l3); }
			bool isPick5LabelTypeNum3Null() const { return decorated_->isPick5LabelTypeNum3Null(); }
			void setPick5LabelTypeNum3Null() { decorated_->setPick5LabelTypeNum3Null(); }
			bool isPick5LabelTypeNum3Writable() const { return decorated_->isPick5LabelTypeNum3Writable(); }

			// OnlineOrdering
			std::string getOnlineOrderingLabelTypeNum1() const { return decorated_->getOnlineOrderingLabelTypeNum1(); }
			void setOnlineOrderingLabelTypeNum1(const std::string& l1) { decorated_->setOnlineOrderingLabelTypeNum1(l1); }
			bool isOnlineOrderingLabelTypeNum1Null() const { return decorated_->isOnlineOrderingLabelTypeNum1Null(); }
			void setOnlineOrderingLabelTypeNum1Null() { decorated_->setOnlineOrderingLabelTypeNum1Null(); }
			bool isOnlineOrderingLabelTypeNum1Writable() const { return decorated_->isOnlineOrderingLabelTypeNum1Writable(); }

			std::string getOnlineOrderingLabelTypeNum2() const { return decorated_->getOnlineOrderingLabelTypeNum2(); }
			void setOnlineOrderingLabelTypeNum2(const std::string& l2) { decorated_->setOnlineOrderingLabelTypeNum2(l2); }
			bool isOnlineOrderingLabelTypeNum2Null() const { return decorated_->isOnlineOrderingLabelTypeNum2Null(); }
			void setOnlineOrderingLabelTypeNum2Null() { decorated_->setOnlineOrderingLabelTypeNum2Null(); }
			bool isOnlineOrderingLabelTypeNum2Writable() const { return decorated_->isOnlineOrderingLabelTypeNum2Writable(); }

			std::string getOnlineOrderingLabelTypeNum3() const { return decorated_->getOnlineOrderingLabelTypeNum3(); }
			void setOnlineOrderingLabelTypeNum3(const std::string& l3) { decorated_->setOnlineOrderingLabelTypeNum3(l3); }
			bool isOnlineOrderingLabelTypeNum3Null() const { return decorated_->isOnlineOrderingLabelTypeNum3Null(); }
			void setOnlineOrderingLabelTypeNum3Null() { decorated_->setOnlineOrderingLabelTypeNum3Null(); }
			bool isOnlineOrderingLabelTypeNum3Writable() const { return decorated_->isOnlineOrderingLabelTypeNum3Writable(); }
			
			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return util_string::inttostring(decorated_->getType()); }
			void setKey(const std::string& key) { decorated_->setType(util_string::stringtoint(key)); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBProductDefault& rec);


		private:

			edb_adapter::DBProductDefaultI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBProductDefault(edb_adapter::DBProductDefaultI* d);

			// Do not allow the default assignment operator.
			DBProductDefault operator=(const DBProductDefault& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRODUCTDEFAULT_H
