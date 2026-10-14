#ifndef HFCWRPPPTEST_H
#define HFCWRPPPTEST_H
#include "evolution.h"
#include "HfcObject.h"
#include "WrPPPTest.h"


typedef enum HfcPPPTstFieldOffsets {
    fieldOffsetStart = 1,
    endHfcPPPTstFieldOffsets
} HfcPPPTstFieldOffsets_t;

class HfcWrPPPTest : public PPPTest, public HfcObject {
    public:
        HfcWrPPPTest( const char *data );
        HfcWrPPPTest( const char *data, int len, int formatType );
        ~HfcWrPPPTest();

        virtual const char *serialize( int format = BrowserFormat ) const;
    private:

};
#endif 