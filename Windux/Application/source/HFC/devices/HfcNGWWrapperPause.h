#ifndef HFCNGWWRAPPERPAUSE_H
#define HFCNGWWRAPPERPAUSE_H
#include "evolution.h"
#include "HfcObject.h"
#include "NGWWrapperPause.h"

typedef enum HfcNGWWrapperPauseFieldOffsets {
    fieldOffsetState = 1,
    fieldOffsetPkgId,
    fieldOffsetPriPosition,
    fieldOffsetReqPriPosition,
    fieldOffsetSecPosition,
    fieldOffsetReqSecPosition,
    endHfcNGWWrapperPauseFieldOffsets
} HfcNGWWrapperPauseFieldOffsets_t;

class HfcNGWWrapperPause : public NGWWrapperPause, public HfcObject {
    public:
        HfcNGWWrapperPause( const char *data );
        ~HfcNGWWrapperPause();

        virtual const char *serialize( int format = BrowserFormat ) const;
    private:
};
#endif