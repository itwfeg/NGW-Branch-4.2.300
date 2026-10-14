#ifndef HFCWRPPPREQUEST_H
#define HFCWRPPPREQUEST_H
#include "evolution.h"
#include "HfcObject.h"
#include "WrPPPRequest.h"


typedef enum HfcPPPReqFieldOffsets {
    fieldOffsetRequest = 1,
    fieldOffsetData,
    endHfcPPPReqFieldOffsets
} HfcPPPReqFieldOffsets_t;

class HfcWrPPPReq : public PPPRequest, public HfcObject {
    public:
        HfcWrPPPReq( const char *data );
        HfcWrPPPReq( const char *data, int len, int formatType );
        ~HfcWrPPPReq();

        virtual const char *serialize( int format = BrowserFormat ) const;
    private:

};
#endif 