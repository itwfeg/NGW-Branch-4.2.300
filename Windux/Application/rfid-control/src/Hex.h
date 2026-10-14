/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */
#ifndef HEX_H
#define HEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string>

#define HEX_STR_SIZE 26

class Hex {
public:
    static std::string nibblize(const char *data, size_t len);
    static std::string toString(const char *data, size_t len, bool useSpace = true);
    static void nibblize(std::string& destination, const char *data, size_t len, bool append);
    static void toString(std::string& destination, const char *data, size_t len, bool append, bool useSpace = false);
    static std::string toString(const void *data);
    static std::string toString(const char *data);
    static std::string toString(long l);
    static std::string toString(long long l, bool stripLeadingSpaces = true);
    static std::string toString(unsigned long l);
    static std::string toString(int i);
    static std::string toString(unsigned int i);
    static std::string toString(unsigned short value);
    static std::string toString(const char c);
    static std::string toString(const unsigned char c);

    // Updated valueOf functions
    static unsigned int valueOf(const char *input);
    static unsigned int valueOf(const char input);

    static long longValueOf(const std::string& s);
    static std::string toHTMLColorString(long l);
    static std::string toHTMLColorString(unsigned long l);

    static const bool APPEND = true;
    static const bool DONT_APPEND = false;

private:
    Hex();
};

#endif // HEX_H