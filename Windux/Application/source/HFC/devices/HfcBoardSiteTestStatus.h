#ifndef HFCBOARDSITETESTSTATUS_H
#define HFCBOARDSITETESTSTATUS_H
#include "evolution.h"
#include "HfcObject.h"
#include "BoardSiteTestStatus.h"

typedef enum HfcBoardSiteTestStatusFieldOffsets {
    fieldOffsetReady = 1,
    endHfcBoardSiteTestStatusFieldOffsets
} HfcBoardSiteTestStatusFieldOffsets_t;


class HfcBoardSiteTestStatus : public BoardSiteTestStatus, public HfcObject {
    public:
        HfcBoardSiteTestStatus( const char *data );
        ~HfcBoardSiteTestStatus();

        virtual const char *serialize( int format = BrowserFormat ) const;
    private:
         
};

#endif