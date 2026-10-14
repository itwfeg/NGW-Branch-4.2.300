#ifndef CODE128BASE_H
#define CODE128BASE_H

#include "StarBitmap.h"

namespace StarMicronics
{

class Code128Base
{
public:
    Code128Base(const char *data, unsigned int dataLength, int module, int height);
    ~Code128Base();

    StarBitmap *BuildBitmap();

    static int ValidateDataEx(char * data, unsigned int dataLength);

private:
    static unsigned char unEscape(unsigned char escapedChar);

    void processBaseData();
    void formatBarWidths();
    int countCode128Characters(char cset, unsigned char *buffer, int bufferLength) const;
    unsigned char convertASCIIToCode128SetA(unsigned char testCharacter) const;
    unsigned char convertASCIIToCode128SetB(unsigned char testCharacter) const;

    char *m_data;
    unsigned int m_dataLength;
    int m_module;
    int m_height;

    unsigned char * m_processedData;
    int m_processedDataLength;

    unsigned char * m_formattedData;
    int m_formattedDataLength;
};

// Represent special Code128
const unsigned char code128FNC1 =    0x80;   // Function 1 (reserved)
const unsigned char code128FNC2 =    0x81;   // Function 2 (tell reader to store data and combine with next scan)
const unsigned char code128FNC3 =    0x82;   // Function 3 (reserved for reader initialization)
const unsigned char code128FNC4 =    0x83;   // Function 4 (reserved)
const unsigned char code128SHIFT =   0x84;   // Shift (change to next character set for one char only)
const unsigned char code128CODEA =   0x85;   // Change to chatacter set A
const unsigned char code128CODEB =   0x86;   // Change to character set B
const unsigned char code128CODEC =   0x87;   // Change to character set C
const unsigned char code128STARTA =  0x88;   // Start with set A
const unsigned char code128STARTB =  0x89;   // Start with set B
const unsigned char code128STARTC =  0x8A;   // Start with set C
const unsigned char code128ILLEGAL = 0x8B;   // Indicate a character that can not be encoded into code 128 (0x80 - 0xFF)

const int code128MaxFormattedDataLength = 200;

}

#endif // CODE128BASE_H
