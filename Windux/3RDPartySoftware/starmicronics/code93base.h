#ifndef CODE93BASE_H
#define CODE93BASE_H

#include "StarBitmap.h"

namespace StarMicronics
{

class Code93Base
{
public:
    Code93Base(const char* data, unsigned int dataLength, int module, int height);
    ~Code93Base();

    StarBitmap * BuildBitmap();

private:
    static bool ValidateData(const char *data, unsigned int dataLength);
    static const char *ConvertCharToEx(char character);
    static const char *GetBarcodeEncoding(char *character, unsigned int *dataUsed);
    static const char *GetCheckSumCharC(char *character);
    static const char *GetCheckSumCharK(char *characters);
    static unsigned int ConvertCharToValue(char *character);
    static const char *ConvertValueToString(unsigned int value);

    char* m_data;
    unsigned int m_dataLength;
    int m_module;
    int m_height;
};

}

#endif // CODE93BASE_H
