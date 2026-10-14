#ifndef I2OF5BASE_H
#define I2OF5BASE_H

#include "StarBitmap.h"

namespace StarMicronics
{

#define I25_codeSize 9

class i2of5base
{
public:
    i2of5base(const char* data, int narrowModule, int wideModule, int height);
    ~i2of5base();

    StarBitmap * BuildBitmap();

private:
    char* m_data;
    char* I25_code[I25_codeSize];
    int m_narrowModule;
    int m_wideModule;
    int m_height;

    static bool ValidateData(const char *data);
    int calculateWidth();
    static const char * barPattern(char source);

};

}

#endif // i2of5_H
