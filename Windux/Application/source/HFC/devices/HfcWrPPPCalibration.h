#ifndef HFCWRPPPCALIBRATION_H
#define HFCWRPPPCALIBRATION_H
#include "evolution.h"
#include "HfcObject.h"
#include "WrPPPCalibration.h"

typedef enum HfcPPPCalFieldOffsets {
    fieldOffsetPlate0Status = 1,
    fieldOffsetCalDist0MM,
    fieldOffsetCalDist0IN,
    fieldOffsetPlate1Status,
    fieldOffsetCalDist1MM,
    fieldOffsetCalDist1IN,
    endHfcPPPCalFieldOffsets
} HfcPPPCalFieldOffsets_t;

class HfcWrPPPCal : public PPPCalib, public HfcObject {
    public:
        HfcWrPPPCal( const char *data );
        ~HfcWrPPPCal();

        virtual const char *serialize( int format = BrowserFormat ) const;
    private:

};
#endif
