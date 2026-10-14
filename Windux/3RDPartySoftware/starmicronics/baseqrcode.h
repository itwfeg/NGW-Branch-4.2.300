#ifndef BASEQRCODE_H
#define BASEQRCODE_H

#include "SMJTypes.h"
#include "StarMicronics_BitmapFunction.h"
#include "StarBitmap.h"

namespace StarMicronics
{

#define QRCODE_MODEL  2
#define QRCODE_MODEL1 0
#define QRCODE_MODEL2 1

#define QRCODE_LEVEL            4
#define QRCODE_LEVEL_LOW        0
#define QRCODE_LEVEL_MIDDLE     1
#define QRCODE_LEVEL_MIDDLEHIGH 2
#define QRCODE_LEVEL_HIGH       3

#define QRCODE_NUMBER 0
#define QRCODE_ALNUM  1
#define QRCODE_BINARY 2
#define QRCODE_KANJI  3

#define QRCODE_VERSION    40
#define QRCODE_CELL_SIZE (QRCODE_VERSION * 4 + 17)

#define QRCODE_MASKING_PENALTY0  3
#define QRCODE_MASKING_PENALTY1  3
#define QRCODE_MASKING_PENALTY2  40
#define QRCODE_MASKING_PENALTY3 (10 / 5)

#define QRCODE_NUMBER_MODE 0x01
#define QRCODE_ALNUM_MODE  0x02
#define QRCODE_BINARY_MODE 0x04
#define QRCODE_KANJI_MODE  0x08

#define QRCODE_DUMMY_PATTERN 0xec11

#define QRCODE_HORZ_EXT_CHAR 0xf0
#define QRCODE_VERT_EXT_CHAR 0xaa

#define QRCODE_DIVIDEND_SIZE 254

#define QRCODE_MAXIMUM_DATA_LENGTH 7086

struct QrCodeBuildInformationStruct
{
    WORD wTotalLength;
    WORD swLength   [QRCODE_LEVEL];
    BYTE sbyDivision[QRCODE_LEVEL];
};

typedef struct QrCodeBuildInformationStruct   QRCODEBUILDINFORMATIONSTRUCT;
typedef struct QrCodeBuildInformationStruct *PQRCODEBUILDINFORMATIONSTRUCT;

struct QrCodeAnalyzeStruct
{
    DWORD dwPenalty;
    BYTE  byNumber;
};

typedef struct QrCodeAnalyzeStruct   QRCODEANALYZESTRUCT;
typedef struct QrCodeAnalyzeStruct *PQRCODEANALYZESTRUCT;

struct QrCodeInformationStruct
{
    WORD wSize;
    WORD wBlock;
    WORD wLength;
    BYTE byVersion;
    BYTE byModel;
    BYTE byModule;
    BYTE byLevel;
    BOOL bUpdate;
    BYTE sbyBaseCell[QRCODE_CELL_SIZE][(QRCODE_CELL_SIZE + 7) / 8];
    BYTE sbyCell    [QRCODE_CELL_SIZE][(QRCODE_CELL_SIZE + 7) / 8];
    BYTE sbyBuffer  [QRCODE_MAXIMUM_DATA_LENGTH];
};

typedef struct QrCodeInformationStruct   QRCODEINFORMATIONSTRUCT;
typedef struct QrCodeInformationStruct *PQRCODEINFORMATIONSTRUCT;

struct QrCodeBlockStruct
{
    WORD  wLength;
    PBYTE pbyCharacter;
    BYTE  byMode;
};

typedef struct QrCodeBlockStruct   QRCODEBLOCKSTRUCT;
typedef struct QrCodeBlockStruct *PQRCODEBLOCKSTRUCT;


class BaseQRCode
{
public:
    QrCodeInformationStruct QrCodeInformation;
    QrCodeBlockStruct QrCodeBlock[256];

    BaseQRCode();
    BaseQRCode(const char *data, unsigned int dataLength, unsigned char model, unsigned char size, unsigned char cl);

    StarBitmap * BuildBitmap();

    void InitializeQrCodeInformation(void);
    void AnalyzeQrCodeCharacter(void);
    void MakeQrCode(void);

private:
    WORD ConvertNumber(PQRCODEBLOCKSTRUCT pBlock, PBYTE pbyCode, PBYTE pbyPosition);
    WORD ConvertAlnum(PQRCODEBLOCKSTRUCT pBlock, PBYTE pbyCode, PBYTE pbyPosition);
    WORD ConvertBinary(PQRCODEBLOCKSTRUCT pBlock, PBYTE pbyCode, PBYTE pbyPosition);
    WORD ConvertKanji(PQRCODEBLOCKSTRUCT pBlock, PBYTE pbyCode, PBYTE pbyPosition);
    void SetVerticalCharacter(BYTE byCharacter, WORD wX, WORD wY);
    void SetHorizontalCharacter(BYTE byCharacter, WORD wX, WORD wY);
    void UpdateSymbolCharacterPosition(PWORD pwX, PWORD pwY);
    void SetCharacterWithUpdatePosition(BYTE byCharacter, PWORD pwX, PWORD pwY);
    void MakePattern(BYTE byNumber);
    DWORD CheckPatternEasy(PQRCODEINFORMATIONSTRUCT pQrCodeInformation);
    DWORD CheckPatternHard(PQRCODEINFORMATIONSTRUCT pQrCodeInformation);
    BOOL GetVersion(PQRCODEINFORMATIONSTRUCT pQrCodeInformation);
    BOOL ConvertToCharacter(PQRCODEINFORMATIONSTRUCT pQrCodeInformation);
    BOOL SetCorrectionCode(PQRCODEINFORMATIONSTRUCT pQrCodeInformation);
    BOOL SetPattern(PQRCODEINFORMATIONSTRUCT pQrCodeInformation);
    BOOL AnalyzePattern(PQRCODEINFORMATIONSTRUCT pQrCodeInformation);
    BOOL WipeOutPattern(PQRCODEINFORMATIONSTRUCT pQrCodeInformation);

};

}

#endif // BASEQRCODE_H
