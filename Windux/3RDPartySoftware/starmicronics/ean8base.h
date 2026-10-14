#ifndef EAN8BASE_H
#define EAN8BASE_H

#include "StarBitmap.h"

namespace StarMicronics
{

class EAN8Base
{
public:
    EAN8Base(const char* data, int module, int height);
    ~EAN8Base();

    StarBitmap* BuildBitmap();

private:
    static bool ValidateData(const char* data);
    char* m_data;
    int m_height;
    int m_module;

    char * m_pattern;

    int calculateCheckCode() const;
    const char * barPatternUPCEAN(char ch) const;
    void createBarPattern(int checkCode);

};

}

#endif // EAN8_H
