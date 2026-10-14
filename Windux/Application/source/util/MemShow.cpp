#include "PAL_platformheader.h"

#include "MemShow.h"

void memShow( int UNUSED(type))
{
    PAL_MEMORYSTATUS memStat;
    memStat.dwLength = sizeof(PAL_MEMORYSTATUS);

    PAL_absclass::PAL_globalMemoryStatus(&memStat);
    wchar_t wbuff[100];

    swprintf(
        wbuff,
        sizeof(wbuff)/sizeof(wchar_t),
#if INTPTR_MAX == INT64_MAX
        L"Memory available: %lld\r\n ",
#else
        L"Memory available: %d\r\n ",
#endif
        memStat.dwAvailPhys );

    PAL_absclass::PAL_OutputDbgStr( wbuff );
}
