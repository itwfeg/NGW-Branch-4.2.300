#ifndef HFCWRPPPVERSION_H
#define HFCWRPPPVERSION_H
#include "evolution.h"
#include "HfcObject.h"
#include "WrPPPVersion.h"

typedef enum HfcPPPVerFieldOffsets {
    fieldOffsetId = 1,
    fieldOffsetSwVersion,
    fieldOffsetHwVersion,
    fieldOffsetBootVersion,
    endHfcPPPVerFieldOffsets
} HfcPPPVerFieldOffsets_t;

class HfcWrPPPVersion : public PPPVersion, public HfcObject {
    public:
        HfcWrPPPVersion( const char *data );
        ~HfcWrPPPVersion();

        virtual const char *serialize( int format = BrowserFormat ) const;
    private:
};



#endif