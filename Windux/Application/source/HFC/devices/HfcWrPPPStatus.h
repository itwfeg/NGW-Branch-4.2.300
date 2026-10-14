#ifndef HFCWRPPPSTATUS_H
#define HFCWRPPPSTATUS_H
#include "evolution.h"
#include "HfcObject.h"
#include "WrPPPStatus.h"

typedef enum HfcPPPStatusFieldOffsets {
    fieldOffsetSensorMode = 1,
    fieldOffsetSensor,
    endHfcPPPStatusFieldOffsets
} HfcPPPStatusFieldOffsets_t;

class HfcWrPPPStatus : public PPPStatus, public HfcObject {
    public:
        HfcWrPPPStatus( const char *data );
        ~HfcWrPPPStatus();

        virtual const char *serialize( int format = BrowserFormat ) const;
    private:
};
#endif