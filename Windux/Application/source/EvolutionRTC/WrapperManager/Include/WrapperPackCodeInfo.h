#ifndef WRAPPERPACKCODEINFO_H
#define WRAPPERPACKCODEINFO_H

#include <string>

#include "Wrapper.h"
#include "WrapperPackSize.h"
#include "WrapperTiming.h"

class WrapperPackCodeInfo
{
	public:
		WrapperPackCodeInfo();
		WrapperPackCodeInfo(const char *buffer);
		virtual ~WrapperPackCodeInfo();

		const char *serialize() const;
		UINT32 getSerializedLength() const;

		void show();

		void getDescription(UINT8 *description) const;
		void setDescription(UINT8 *description);
		
		WrPackCode getPackCode() const;
		void setPackCode(WrPackCode packCode);

		// Pack Width Variants
		WrPackWidth getPackWidth() const;
		void setPackWidth(WrPackWidth packWidth);

		bool isNarrowPackage() const;
		bool isWidePackage() const;
		bool isFilmWidthIdle() const;
		bool isAutoWidthSelect() const;

		void setNarrowPackage();
		void setWidePackage();
		void setFilmWidthIdle();
		void setAutoWidthSelect();
		
		// Pack Length Variants
		WrPackLength getPackLength() const;
		void setPackLength(WrPackLength packLength);

		bool isShortPackage() const;
		bool isMediumPackage() const;
		bool isLongPackage() const;
		bool isFilmLengthIdle() const;
		bool isAutoLengthSelect() const;

		void setShortPackage();
		void setMediumPackage();
		void setLongPackage();
		void setFilmLengthIdle();
		void setAutoLengthSelect();

		// Pack Size Variants
		WrapperPackSize &getPackSize();
		void setPackSize(WrapperPackSize &packSize);

		void setPackSizeMinLength(UINT16 packSizeMinLength);
		UINT16 getPackSizeMinLength() const;
		void setPackSizeMaxLength(UINT16 packSizeMaxLength);
		UINT16 getPackSizeMaxLength() const;
		void setPackSizeMinWidth(UINT16 packSizeMinWidth);
		UINT16 getPackSizeMinWidth() const;
		void setPackSizeMaxWidth(UINT16 packSizeMaxWidth);
		UINT16 getPackSizeMaxWidth() const;

		// sideClamp Variants
		WrapperTiming &getSideClamp();
		void setSideClamp(WrapperTiming &sideClamp);

		void setSideClampOnMin(UINT8 sideClampOnMin);
		UINT8 getSideClampOnMin() const;
		void setSideClampOnValue(UINT8 sideClampOnValue);
		UINT8 getSideClampOnValue() const;
		void setSideClampOnMax(UINT8 sideClampOnMax);
		UINT8 getSideClampOnMax() const;
		void setSideClampOffMin(UINT8 sideClampOffMin);
		UINT8 getSideClampOffMin() const;
		void setSideClampOffValue(UINT8 sideClampOffValue);
		UINT8 getSideClampOffValue() const;
		void setSideClampOffMax(UINT8 sideClampOffMax);
		UINT8 getSideClampOffMax() const;

		// gripper Variants
		WrapperTiming &getGripper();
		void setGripper(WrapperTiming &gripper);

		void setGripperOnMin(UINT8 gripperOnMin);
		UINT8 getGripperOnMin() const;
		void setGripperOnValue(UINT8 gripperOnValue);
		UINT8 getGripperOnValue() const;
		void setGripperOnMax(UINT8 gripperOnMax);
		UINT8 getGripperOnMax() const;
		void setGripperOffMin(UINT8 gripperOffMin);
		UINT8 getGripperOffMin() const;
		void setGripperOffValue(UINT8 gripperOffValue);
		UINT8 getGripperOffValue() const;
		void setGripperOffMax(UINT8 gripperOffMax);
		UINT8 getGripperOffMax() const;

		// knife Variants
		WrapperTiming &getKnife();
		void setKnife(WrapperTiming &knife);

		void setKnifeOnMin(UINT8 knifeOnMin);
		UINT8 getKnifeOnMin() const;
		void setKnifeOnValue(UINT8 knifeOnValue);
		UINT8 getKnifeOnValue() const;
		void setKnifeOnMax(UINT8 knifeOnMax);
		UINT8 getKnifeOnMax() const;
		void setKnifeOffMin(UINT8 knifeOffMin);
		UINT8 getKnifeOffMin() const;
		void setKnifeOffValue(UINT8 knifeOffValue);
		UINT8 getKnifeOffValue() const;
		void setKnifeOffMax(UINT8 knifeOffMax);
		UINT8 getKnifeOffMax() const;

		// Booleans
		bool isSlideBlock() const;
		void setSlideBlock();
		void clearSlideBlock();

		bool isBooster() const;
		void setBooster();
		void clearBooster();

		// Totals
		void setPackageTotal(UINT32 packageTotal);
		UINT32 getPackageTotal() const;

		void setJamTotal(UINT32 jamTotal);
		UINT32 getJamTotal() const;

	private:
	    UINT8			description[PackCodeDescriptionLength]; 
	    WrPackCode		packCode;
	    WrPackWidth		packWidth;
	    WrPackLength	packLength;
	    WrapperPackSize	packSize;
	    WrapperTiming	sideClamp; 
	    WrapperTiming	gripper;    
	    WrapperTiming	knife;
		bool			slideBlock;
		bool			booster;
		UINT32			packageTotal;
	    UINT32			jamTotal;

		mutable std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};
#endif // WRAPPERPACKCODEINFO_H
