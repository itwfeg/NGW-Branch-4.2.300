#ifndef EAN13BASE_H
#define EAN13BASE_H

#include "StarBitmap.h"

namespace StarMicronics
{

class EAN13Base
{
public:
    EAN13Base(const char *data, int module, int height);
    ~EAN13Base();

    StarBitmap *BuildBitmap();


private:

    char *m_data;
    int m_height;
    int m_hScale;

    static bool ValidateData(const char *data);
    int calculateCheckCode();
    const char * barPatternUPCEAN(char ch, int parity);
    const char * getParityPattern(char first);
    void createBarPattern(char *barPattern, const char *parityPattern, const char *data, int checkCode);
    void drawPattern(StarBitmap *patternBitmap, char * barPattern);
};

}

#endif // EAN13_H
