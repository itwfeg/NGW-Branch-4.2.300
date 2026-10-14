#ifndef EVOLLABELAPPLIERCONFIGURATION_H
#define EVOLLABELAPPLIERCONFIGURATION_H

#include <string>

#include "LabelApplier.h"
#include "Printer.h"

class EvolLabelApplierConfiguration
{
	public:
		EvolLabelApplierConfiguration(ULONG checksum = 0);
		EvolLabelApplierConfiguration(const char *buffer);
		EvolLabelApplierConfiguration(const EvolLabelApplierConfiguration &elac);
		~EvolLabelApplierConfiguration();

		const char *serialize() const;
		UINT32 getSerializedLength() const;

		void show();

		ULONG getChecksum() const;
		void setChecksum(ULONG checksum);

		bool doesHaveAutoRotation() const;
		bool getHasAutoRotation() const;
		void setHasAutoRotation(bool value);

		UINT16 getPriLabelSizeDefault() const;
		void setPriLabelSizeDefault(UINT16 size);

		LabelWidth getPriLabelWidth() const;
		void setPriLabelWidth(LabelWidth size);

		LaRotation getPriLabelRotationDefault() const;
		void setPriLabelRotationDefault(LaRotation size);

		LaPosLoc getPriLabelLocationDefault() const;
		void setPriLabelLocationDefault(LaPosLoc value);

		UINT16 getPriLabelOffsetOnPkg() const;
		void setPriLabelOffsetOnPkg(UINT16 value);

		UINT16 getSecLabelSizeDefault() const;
		void setSecLabelSizeDefault(UINT16 size);

		LabelWidth getSecLabelWidth() const;
		void setSecLabelWidth(LabelWidth size);

		LaRotation getSecLabelRotationDefault() const;
		void setSecLabelRotationDefault(LaRotation size);

		LaPosLoc getSecLabelLocationDefault() const;
		void setSecLabelLocationDefault(LaPosLoc value);

		UINT16 getSecLabelOffsetOnPkg() const;
		void setSecLabelOffsetOnPkg(UINT16 value);

		bool isSecondCLAAttached() const;
		bool getIsSecondCLAAttached() const;
		void setIsSecondCLAAttached(bool value);

		LaSideDischarge getSideDischargeType() const;
		void setSideDischargeType(LaSideDischarge type);

		void operator=(const EvolLabelApplierConfiguration &laConfig);

		UINT16 getPriRotationCalibrationOffset() const;//0Deg
		void setPriRotationCalibrationOffset(UINT16 value);//0Deg

		UINT16 getPriRotationCalibrationOffset_90Deg() const;
		void setPriRotationCalibrationOffset_90Deg(UINT16 value);
		UINT16 getPriRotationCalibrationOffset_180Deg() const;
		void setPriRotationCalibrationOffset_180Deg(UINT16 value);
		UINT16 getPriRotationCalibrationOffset_270Deg() const;
		void setPriRotationCalibrationOffset_270Deg(UINT16 value);

		UINT16 getSecRotationCalibrationOffset() const;
		void setSecRotationCalibrationOffset(UINT16 value);

		UINT16 getPriWandCalibrationOffset() const;
		void setPriWandCalibrationOffset(UINT16 value);
		UINT16 getSecWandCalibrationOffset() const;
		void setSecWandCalibrationOffset(UINT16 value);

		UINT16 getSecRotationCalibrationOffset_90Deg() const { return secRotationCalibrationOffset_90Deg; }
		void setSecRotationCalibrationOffset_90Deg(UINT16 value) { secRotationCalibrationOffset_90Deg = value; }
		UINT16 getSecRotationCalibrationOffset_180Deg() const	{ return secRotationCalibrationOffset_180Deg; }
		void setSecRotationCalibrationOffset_180Deg(UINT16 value) { secRotationCalibrationOffset_180Deg = value; } 
		UINT16 getSecRotationCalibrationOffset_270Deg() const { return secRotationCalibrationOffset_270Deg; }
		void setSecRotationCalibrationOffset_270Deg(UINT16 value) { secRotationCalibrationOffset_270Deg = value; }

		UINT16 getPriApplierHandAdjust() const { return priApplierHandAdjust; }
		void setPriApplierHandAdjust(UINT16 value) { priApplierHandAdjust = value; }
		UINT16 getSecApplierHandAdjust() const { return secApplierHandAdjust; }
		void setSecApplierHandAdjust(UINT16 value) { secApplierHandAdjust = value; }

	private:
	    ULONG			checksum;

		bool			autoRotation;

		UINT16			priLabelSizeDefault;			//	Default label size if none sent.
		LabelWidth		priLabelWidth;					//	Width of primary label.
		LaRotation		priLabelRotationDefault;		//	Default label rotation if none sent.
		LaPosLoc		priLabelLocationDefault;		//	Default label location if none sent.
		INT16			priLabelOffsetOnPkg;			//	Used to adjust the label's position on the pkg.
		UINT16			priRotationCalibrationOffset;	//	0Deg - Used to fine tune the rotation.
		UINT16			priWandCalibrationOffset;	    //	Used to fine tune wand home.

		UINT16			secLabelSizeDefault;			//	Default label size if none sent.
		LabelWidth		secLabelWidth;					//	Width of secondary label.
		LaRotation		secLabelRotationDefault;		//	Default label rotation if none sent.
		LaPosLoc		secLabelLocationDefault;		//	Default label location if none sent.
		INT16			secLabelOffsetOnPkg;			//	Used to adjust the label's position on the pkg.
		UINT16			secRotationCalibrationOffset;	//	Used to fine tune the rotation.
		UINT16			secWandCalibrationOffset;	    //	Used to fine tune wand home.

		bool			secondCLAAttached;				//	Was: sideDischarge;		//	True = Side, False = Front discharge.
		LaSideDischarge	sideDischargeType;				//	RightDischarge or LeftDischarge.

		UINT16			priRotationCalibrationOffset_90Deg;		//	Used to fine tune the rotation.
		UINT16			priRotationCalibrationOffset_180Deg;	//	Used to fine tune the rotation.
		UINT16			priRotationCalibrationOffset_270Deg;	//	Used to fine tune the rotation.

		UINT16			secRotationCalibrationOffset_90Deg;		//	Used to fine tune the rotation.
		UINT16			secRotationCalibrationOffset_180Deg;	//	Used to fine tune the rotation.
		UINT16			secRotationCalibrationOffset_270Deg;	//	Used to fine tune the rotation.

		UINT16 			priApplierHandAdjust;
		UINT16			secApplierHandAdjust;

		mutable std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif // EVOLLABELAPPLIERCONFIGURATION_H
