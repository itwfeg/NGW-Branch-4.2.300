
#ifndef EVOLWRAPPERGENERALCONFIG_H
#define EVOLWRAPPERGENERALCONFIG_H

#include "Wrapper.h"
#include "WrapperIntakeMode.h"
#include "WrapperFilmWidthDimensions.h"

class EvolWrapperGeneralConfig
{
	public:    
		EvolWrapperGeneralConfig();
		EvolWrapperGeneralConfig(const char* buffer);
		EvolWrapperGeneralConfig(const EvolWrapperGeneralConfig &eConfig);
		virtual ~EvolWrapperGeneralConfig();

		const char *serialize() const;
		UINT32 getSerializedLength() const;
		void show();

		ULONG getChecksum() const;
		void setChecksum(ULONG checksum);

		WrapperFilmWidthDimensions &getTopFilmRollDimensions();
		const WrapperFilmWidthDimensions &getTopFilmRollDimensions() const;
		void setTopFilmRollDimensions(const WrapperFilmWidthDimensions &topFilmRoll);

		WrapperFilmWidthDimensions &getBottomFilmRollDimensions();
		const WrapperFilmWidthDimensions &getBottomFilmRollDimensions() const;
		void setBottomFilmRollDimensions(const WrapperFilmWidthDimensions &bottomFilmRoll);


		WrOperation getWrapperOperation() const;
		void setWrapperOperation(WrOperation value);

		UINT16 getFilmTensionNetDefault() const;
		void setFilmTensionNetDefault(UINT16 value);
		/*changed, was not used on AWS -- ats
		INT16 getFilmTensionEnvironAdjust() const;
		void setFilmTensionEnvironAdjust(INT16 value);
		*/
		WrHardwareAddOn getHardwareAddOn() const {return hardwareAddOn_;}
		void setHardwareAddOn(const WrHardwareAddOn addOn) {hardwareAddOn_ = addOn;}

		INT16 getFilmTensionServiceAdjust() const;
		void setFilmTensionServiceAdjust(INT16 value);

		UINT16 getMaxPackageWeight() const;
		void setMaxPackageWeight(UINT16 value);
		UINT16 getMinPackageWeight() const;
		void setMinPackageWeight(UINT16 value);

		UINT16 getMaxPackageLength() const;
		void setMaxPackageLength(UINT16 value);
		UINT16 getMinPackageLength() const;
		void setMinPackageLength(UINT16 value);

		UINT16 getMaxPackageWidth() const;
		void setMaxPackageWidth(UINT16 value);

		UINT16 getLowerFilmPullTension() const;
		void setLowerFilmPullTension(UINT16 value);
		UINT16 getUpperFilmPullTension() const;
		void setUpperFilmPullTension(UINT16 value);

		UINT16 getSealerBeltPackageSensorType() const;
		void setSealerBeltPackageSensorType(UINT16 value);

		ULONG getPackageGrandTotal() const;
		void setPackageGrandTotal(ULONG value);
		ULONG getWrapperCycleTotal() const;
		void setWrapperCycleTotal(ULONG value);

  		INT16 getFilmTensionFrontToBack() const;
  		void setFilmTensionFrontToBack(INT16 tension);
  		INT16 getFilmTensionRightToLeft() const;
  		void setFilmTensionRightToLeft(INT16 tension);

		void operator=(const EvolWrapperGeneralConfig &eConfig);


	private:
	    ULONG		checksum;

	    WrapperFilmWidthDimensions	topFilmRoll;
	    WrapperFilmWidthDimensions	bottomFilmRoll;
		WrOperation 	wrapperOperation;				//	New Apr 08.  Was opFilmTensionMultiplier;		//	Range:  -30 to +30

		UINT16 	filmTensionNetDefault;
		/*changed, is not used --ats 
		INT16 	filmTensionEnvironAdjust;*/
		WrHardwareAddOn hardwareAddOn_;

		INT16 	filmTensionServiceAdjust;

		UINT16	maxPackageWeight;
		UINT16	minPackageWeight;

		UINT16	maxPackageLength;
		UINT16	minPackageLength;

		UINT16	maxPackageWidth;

		UINT16	lowerFilmPullTension;  // previously unused and named minPackageWidth;
		UINT16	upperFilmPullTension;  // previously unused and named maxPackageHeight;

		UINT16	sealerBeltPackageSensorType;  // previously unused and named minPackageHeight;

		ULONG	packageGrandTotal;
		ULONG	wrapperCycleTotal;

		UINT16	filmTensionFrontToBack;
		UINT16	filmTensionRightToLeft;

		mutable std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //EVOLWRAPPERGENERALCONFIG_H
