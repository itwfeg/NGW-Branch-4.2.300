#ifndef HFCWRPPPCONFIG_H
#define HFCWRPPPCONFIG_H
#include "evolution.h"
#include "HfcObject.h"
#include "WrPPPConfig.h"

typedef enum HfcPPPConfigFieldOffsets {
    fieldOffsetPriCal = 1,
    fieldOffsetSecCal,
    fieldOffsetRate,
    fieldOffsetPriSlope,
    fieldOffsetSecSlope,
    fieldOffsetPriHomeDist,
    fieldOffsetSecHomeDist,
    fieldOffsetMechanicalDist,
#if 1 /* VL53L1X */
    fieldOffsetX,
    fieldOffsetY,
    fieldOffsetCenter,
    fieldOffsetCalOffsetA,
    fieldOffsetCalOffsetB,
#endif    
    endHfcPPPConfigFieldOffsets
} HfcPPPConfigFieldOffsets_t;

class HfcWrPPPConfig : public PPPConfig, public HfcObject {
    public:
        HfcWrPPPConfig( const char *data );
        ~HfcWrPPPConfig();

        virtual const char *serialize( int format = BrowserFormat ) const;
    private:

};










#endif