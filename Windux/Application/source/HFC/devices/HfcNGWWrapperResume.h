#ifndef HFCNGWWRAPPERRESUME_H
#define HFCNGWWRAPPERRESUME_H
#include "evolution.h"
#include "HfcObject.h"
#include "NGWWrapperResume.h"

typedef enum HfcNGWWrapperResumeFieldOffsets {
    fieldOffsetResumeId = 1,
    fieldOffsetResume,
    endHfcNGWWrapperResumeFieldOffsets
} HfcNGWWrapperResumeFieldOffsets_t;

class HfcNGWWrapperResume : public NGWWrapperResume, public HfcObject {
    public:
        HfcNGWWrapperResume( const char *data );
        HfcNGWWrapperResume(const char *data, int len, int formatType);
        ~HfcNGWWrapperResume();

        virtual const char *serialize( int format = BrowserFormat ) const;
    private:
};
#endif