/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERCONTROL_H
#define HFCEVOLWRAPPERCONTROL_H

#include "evolution.h"
#include "HfcObject.h"

#include "EvolWrapperControl.h"

typedef enum HfcEvolWrapperControlFieldOffsets
{
    fieldOffsetEvolWrIntakeMode = 1,
    fieldOffsetEvolWrFilmTension,
    fieldOffsetEvolWrFilmTensionFrontToBack,
    fieldOffsetEvolWrFilmTensionRightToLeft,
    fieldOffsetEvolWrFilmWidth,
    fieldOffsetEvolWrUpperRollWidth,
    fieldOffsetEvolWrLowerRollWidth,
    fieldOffsetEvolWrSBHeaterEnable,
    fieldOffsetEvolWrSBHeaterSetting,
    fieldOffsetEvolWrFilmSupplyEnable,
    fieldOffsetEvolWrFilmSupplySetting,

    endHfcEvolWrapperControlFieldOffsets		//	MUST be last!
} HfcEvolWrapperControlFieldOffsets_t;

class HfcEvolWrapperConfig;

class HfcEvolWrapperControl : public EvolWrapperControl, public HfcObject
{
    public:
        HfcEvolWrapperControl(const char *data);
        HfcEvolWrapperControl(const char *data, int len, int formatType);
        HfcEvolWrapperControl(HfcEvolWrapperConfig *wrConfig);

        ~HfcEvolWrapperControl();

        virtual const char *serialize(int format = InternalFormat)const;

        bool update(HfcEvolWrapperControl *wrControl, bool &sbTempValid, bool &sbTempChanged);
        bool update(HfcEvolWrapperConfig *wrConfig);

        unsigned int getUpperRollWidth() const;
        void setUpperRollWidth(unsigned int width);

        unsigned int getLowerRollWidth() const;
        void setLowerRollWidth(unsigned int width);

        bool operator==(HfcEvolWrapperControl &hewc);


    private:
        unsigned int upperRollWidth;
        unsigned int lowerRollWidth;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//	HFCEVOLWRAPPERCONTROL_H
