#ifndef HFCBOARDSITETESTCOMPLETEUI_H
#define HFCBOARDSITETESTCOMPLETEUI_H
#include "evolution.h"
#include "HfcObject.h"
#include "BoardSiteTestCompleteUI.h"


typedef enum HfcBoardSiteTestCompleteUIFieldOffsets {
    fieldOffsetArkSerialFlash = 1,
    fieldOffsetArkSDSlot0,
    fieldOffsetArkSDSlot1,
    fieldOffsetArkRTC,
    fieldOffsetArkUSB,
    fieldOffsetArkWifi,
    fieldOffsetArkBlueTooth,
    fieldOffsetArkEthernet,
    fieldOffsetArkMemory,
    fieldOffsetArkCan0,
    fieldOffsetKpcCan,
    fieldOffsetKpcModel,
    fieldOffsetKpcSerialFlash,
    fieldOffsetKpcValueMax,
    fieldOffsetKpcCutter,
    fieldOffsetKpcDotWear,
    endHfcBoardSiteTestCompleteUIFieldOffsets
} HfcBoardSiteTestCompleteUIFieldOffsets_t;

class HfcBoardSiteTestCompleteUI : public BoardSiteTestCompleteUI, public HfcObject {
    public:
        HfcBoardSiteTestCompleteUI( const char *data );
        ~HfcBoardSiteTestCompleteUI();

        virtual const char *serialize( int format = BrowserFormat ) const;
    private:
};
#endif 