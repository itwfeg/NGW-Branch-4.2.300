#ifndef EPCRFID_H
#define EPCRFID_H

#include <string>
void generateEpcRfid( const std::string barcodeStr, const std::string serialNumberStr, unsigned char *result );

#endif //EPCRFID_H
