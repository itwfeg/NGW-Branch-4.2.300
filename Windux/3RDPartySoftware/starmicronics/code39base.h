#ifndef CODE39BASE_H
#define CODE39BASE_H

#include "StarBitmap.h"

namespace StarMicronics
{

class Code39Base
{
public:
    Code39Base(const char *data, int narrowModule, int wideModule, int height);
    ~Code39Base();

    StarBitmap * BuildBitmap();

    static bool ValidateData(char *data);
private:
    char *m_data;
    int m_narrowModule;
    int m_wideModule;
    int m_height;

    int calculateWidth();
    static const char * barPattern(char source, bool validate);
};

}

#endif // CODE39BASE_H
