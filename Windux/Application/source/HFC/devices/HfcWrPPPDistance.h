#ifndef HFCWRPPPDISTANCE_H
#define HFCWRPPPDISTANCE_H
#include "evolution.h"
#include "HfcObject.h"
#include "WrPPPDist.h"

typedef enum HfcPPPDistFieldOffsets {
    fieldOffsetPlate = 1,
    fieldOffsetDistMM,
    fieldOffsetDistIN,
    endHfcPPPDistFieldOffsets
} HfcPPPDistFieldOffsets_t;

class HfcWrPPPDist : public PPPDist, public HfcObject {
    public:
        HfcWrPPPDist( const char *data );
        ~HfcWrPPPDist();

        virtual const char *serialize( int format = BrowserFormat ) const;
    private:
};
#endif