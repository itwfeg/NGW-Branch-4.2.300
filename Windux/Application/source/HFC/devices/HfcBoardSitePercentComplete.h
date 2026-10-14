#ifndef HFCBOARDSITEPERCENTCOMPLETE_H
#define HFCBOARDSITEPERCENTCOMPLETE_H
#include "evolution.h"
#include "HfcObject.h"
#include "BoardSitePercentComplete.h"

typedef enum HfcBoardSitePercentCompleteFieldOffsets {
    fieldOffsetPercent = 1,
    fieldOffsetTestCompete,
    endHfcBoardSitePercentCompleteFieldOffsets
} HfcBoardSitePercentCompleteFieldOffsets_t;


class HfcBoardSitePercentComplete : public BoardSitePercentComplete, public HfcObject {
    public:
        HfcBoardSitePercentComplete( const char *data );
        ~HfcBoardSitePercentComplete();

        virtual const char *serialize( int format = BrowserFormat ) const;
    private:
         
};

#endif