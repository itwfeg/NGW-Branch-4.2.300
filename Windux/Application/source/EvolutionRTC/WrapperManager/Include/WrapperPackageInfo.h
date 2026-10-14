#ifndef WRAPPERPACKAGEINFO_H
#define WRAPPERPACKAGEINFO_H

#include <string>

#include "Wrapper.h"
#include "WrapperIntakeMode.h"

class WrapperPackageInfo
{
	public:
		WrapperPackageInfo();
		WrapperPackageInfo(const char *buffer);
		~WrapperPackageInfo();

		const char *serialize() const;
		UINT32 getSerializedLength() const;

		void show() const;

		// Booleans
		bool isPresent() const;
		void setPresent();
		void clearPresent();

		bool isPullFilm() const;
		void setPullFilm();
		void clearPullFilm();

		bool isGhostPackage() const;
		void setGhostPackage();
		void clearGhostPackage();

		bool isSlowDelay() const;
		void setSlowDelay();
		void clearSlowDelay();

		bool isQuickDelay() const;
		void setQuickDelay();
		void clearQuickDelay();

		void setIntakeMode(WrapperIntakeMode::IntakeMode intakeMode);
		WrapperIntakeMode::IntakeMode getIntakeMode() const;
		bool isIntakeModeNormal() const;
		bool isIntakeModeGentle() const;

		bool isSlideBlock() const;
		void setSlideBlock();
		void clearSlideBlock();

		bool isBooster() const;
		void setBooster();
		void clearBooster();

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

		void setPackageId(UINT32 packageId);
		UINT32 getPackageId() const;

		void setLength(UINT16 length);
		UINT16 getLength() const;

		void setWidth(UINT16 width);
		UINT16 getWidth() const;

		void setPackageBits(WrPackageBits packageBits);
		WrPackageBits getPackageBits();

		bool equals(WrapperPackageInfo &wrPackageInfo) const;

	private:
		bool			present;
		bool			pullFilm;
		bool			ghostPackage;
		bool			slowDelay;
		bool			quickDelay;
		WrapperIntakeMode::IntakeMode	intakeMode;
		bool			slideBlock;
		bool			booster;
		WrPackCode		packCode;
		WrPackWidth		packWidth;
		WrPackLength	packLength;
		UINT32			packageId;
		UINT16			length;
		UINT16			width;				  

		mutable std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};
#endif // WRAPPERPACKAGEINFO_H
