#ifndef QASURABITMAP_H
#define QASURABITMAP_H

#include <QImage>


namespace StarMicronics_QAsura
{


/**
    @ingroup QAsura
    @{
    @~english
        The barcode generator.
        This object is used to generate QImages containing Code 39, Code 93, Code 128, and QR Codes.
        Other barcode types are not yet supported.  Those functions will throw exceptions.

    @~japanese
        バーコードを生成するためのAPIを提供します。
        このクラスは、Code 39、Code 93、Code 128やQR CodeのQImageを作成できます。
        他のバーコードは、まだサポートされていません。
*/
class QABarcodeGenerator
{
public:
    enum QR_MODEL
    {
        QR_MODEL_1 = 0,
        QR_MODEL_2 = 1
    };


    enum QR_CELL_SIZE
    {
        QR_CELL_SIZE_1 = 1,
        QR_CELL_SIZE_2 = 2,
        QR_CELL_SIZE_3 = 3,
        QR_CELL_SIZE_4 = 4,
        QR_CELL_SIZE_5 = 5,
        QR_CELL_SIZE_6 = 6,
        QR_CELL_SIZE_7 = 7,
        QR_CELL_SIZE_8 = 8
    };


    enum QR_CORRECTION_LEVEL
    {
        QR_CORRECTION_LEVEL_1 = 0,
        QR_CORRECTION_LEVEL_2 = 1,
        QR_CORRECTION_LEVEL_3 = 2,
        QR_CORRECTION_LEVEL_4 = 3
    };


    QABarcodeGenerator();


/**
    @~english
        Creates a QImage with code 39.

    @param data
        The data that will be placed in the barcode.  Usually a string is used.
        Defined values for each character(c) are 0 ≤ c ≤ 9, A ≤ c ≤ Z, -, ., (space), $, /, +, %.

    @param narrowModule
        This is the width of the narrow bars, in dots (1/8 mm).
        Defined value should be above 1.  If less then 1 then 1 is used.

    @param wideModule
        This is the width of the wide bars, in dots (1/8 mm).
        Defined value should be between 2 and 3 times narrowModules.
        If less than 2 times narrowModule then 2 times narrowModules is used.
        If more than 3 times narrow modules then 3 times narrowModule is used.

    @param height
        This is the height of the barcode, in dots (1/8 mm).
        Defined value should be above 1.  If less then 1 then 1 is used.

    @return
        QImage containing the barcode.

    @throw QAException
        Thrown if document barcode generation failed.

    @~japanese
        Code 39のQImageを作成します。

    @param data
        表示するバーコード・データを示します。
        データは数字、アルファベット、記号の合計43個の文字から構成され、
        大文字のA 〜 Z、数字の0 〜 9と記号-、.、$、/、+、%とスペースを含むことができます。

    @param narrowModule
        ナローバーの幅をドット単位(1/8 mm)で設定します。
        もし1ドット以下を設定した場合、幅は1ドットになります。

    @param wideModule
        ワイドバーの幅をドット単位(1/8 mm)で設定します。
        ナローバーの幅の2〜3倍を設定してください。
        もしナローバーの幅の2倍以下を設定した場合、2倍の値が使われます。
        またナローバーの幅の3倍以上を設定した場合、3倍の値が使われます。

    @param height
        バーコードの高さをドット単位(1/8 mm)で設定します。
        1ドット以下を設定した場合、高さは1ドットになります。

    @return
        バーコード・イメージを含んだQImageを返します。

    @throw QAException
        バーコード作成に失敗した場合、例外を投げます。
*/
    static QImage GenerateCode39(const char *data,  int narrowModule, int wideModule, int height);


/**
    @~english
        Creates a QImage with code 93 barcode.

    @param data
        The data that will be placed in the barcode.  Usually a string is used.
        Defined character are from 0 to 127.
        Unexpected barcode may be created if this length is less than length parameter.

    @param length
        The length of the data to be placed in the code 93.

    @param module
        This is the width of the bars of each individual charater, in dots (1/8 mm).
        This should be above 1, if less then 1 then 1 is automatically used.
        If this value is too high then the barcode is printed off the end of the page.

    @param height
        This is the height of the barcode, in dots (1/8 mm).
        This should be above 1, if less then 1 then 1 is automatically used.

    @return
        QImage containing the barcode.

    @throw QAException
        Thrown if document barcode generation failed.
*/
    static QImage GenerateCode93(const char *data, unsigned int length, int module, int height);


/**
    @~english
        Creates a QImage with code 128 barcode.

    @param data
        The data that will be placed in the barcode.  Usually a string is used.
        The defined values are 0 to 127.
        Function codes represent 1 to 4 (0x31 to 0x34) applied behind &apos;%&apos;.
        Start codes represent 6 to 8 (0x36 to 0x38) applied behind &apos;%&apos;.

    @param length
        The length of the data to be placed in the code 128.

    @param module
        This is the width of the bars, in dots (1/8 mm).
        This should be above 1, if less then 1 then 1 is automatically used.
        Careful when setting this value too high or you could print off the end of the page.

    @param height
        This is the height of the barcode, in dots (1/8 mm).
        This should be above 1, if less then 1 then 1 is automatically used.

    @return
        QImage containing the barcode.

    @throw QAException
        Thrown if document barcode generation failed.
*/
    static QImage GenerateCode128(const char *data, unsigned int length, int module, int height);

/// @cond internal
/**
    @~english
        Creates a QImage with code EAN8 barcode.
        This function is not yet supported.

    @param data
        The data that will be placed in the barcode.  Usually a string is used.

    @param module
        This is the width of the bars.

    @param height
        This is the height of the barcode.

    @return
        QImage containing the barcode.

    @throw QAException
        Thrown if document barcode generation failed.
*/
    static QImage GenerateEAN8(const char *data, int module, int height);
/// @endcond

/// @cond internal
/**
    @~english
        Creates a QImage with code EAN13 barcode.
        This function is not yet supported.

    @param data
        The data that will be placed in the barcode.  Usually a string is used.

    @param module
        This is the width of the bars.

    @param height
        This is the height of the barcode.

    @return
        QImage containing the barcode.

    @throw QAException
        Thrown if document barcode generation failed.
*/
    static QImage GenerateEAN13(const char *data, int module, int height);
/// @endcond

/// @cond internal
/**
    @~english
        Creates a QImage with code UPC-A barcode.
        This function is not yet supported.

    @param data
        The data that will be placed in the barcode.  Usually a string is used.

    @param module
        This is the width of the bars.

    @param height
        This is the height of the barcode.

    @return
        QImage containing the barcode.

    @throw QAException
        Thrown if document barcode generation failed.
*/
    static QImage GenerateUPC_A(const char *data, int module, int height);
/// @endcond

/// @cond internal
/**
    @~english
        Creates a QImage with code UPC-E barcode.
        This function is not yet supported.

    @param data
        The data that will be placed in the barcode.  Usually a string is used.

    @param module
        This is the width of the bars.

    @param height
        This is the height of the barcode.

    @return
        QImage containing the barcode.

    @throw QAException
        Thrown if document barcode generation failed.
*/
    static QImage GenerateUPC_E(const char *data, int module, int height);
/// @endcond

/// @cond internal
/**
    @~english
        Creates a QImage with code ITF barcode.
        This function is not yet supported.

    @param data
        The data that will be placed in the barcode.  Usually a string is used.

    @param narrowModule
        This is the width of the narrow bars.

    @param wideModule
        This is the width of the wide bars.

    @param height
        This is the height of the barcode.

    @return
        QImage containing the ITF barcode.

    @throw QAException
        Thrown if document barcode generation failed.
*/
    static QImage GenerateITF(const char *data, int narrowModule, int wideModule, int height);
/// @endcond

/// @cond internal
/**
    @~english
        Creates a QImage with code ITF barcode.
        This function is not yet supported.

    @param data
        The data that will be placed in the barcode.  Usually a string is used.

    @param narrowModule
        This is the width of the narrow bars.

    @param wideModule
        This is the width of the wide bars.

    @param height
        This is the height of the barcode.

    @return
        QImage containing the ITF barcode.

    @throw QAException
        Thrown if document barcode generation failed.
*/
    static QImage GenerateNW7(const char *data, int module, int height);
/// @endcond


/**
    @~english
        Creates a QImage with QR Code.
        This function is not yet supported.

    @param data
        The data that will be placed in the QR Code.  Usually a string is used.

    @param dataLength
        The length of the data to be placed in the QR Code.

    @param model
        This is the model the QR Code will use.@n
        Either QR_MODEL_1 or QR_MODEL_2.

    @param size
        This is the cell size that will be used to make the QR Code.@n
        This can be QR_CELL_SIZE_1, QR_CELL_SIZE_2, QR_CELL_SIZE_3, QR_CELL_SIZE_4, QR_CELL_SIZE_5, QR_CELL_SIZE_6, QR_CELL_SIZE_7, or QR_CELL_SIZE_8.

    @param cl
        This is the correction level to use in the QR Code.@n
        This value can be QR_CORRECTION_LEVEL_1, QR_CORRECTION_LEVEL_2, QR_CORRECTION_LEVEL_3, or QR_CORRECTION_LEVEL_4.

    @return
        QImage containing the QR Code.

    @throw QAException
        Thrown if document QR Code generation failed.
*/
    static QImage GenerateQrcode(const char *data, unsigned int dataLength,  QR_MODEL model, QR_CELL_SIZE size, QR_CORRECTION_LEVEL cl);


/// @cond internal
/**
    @~english
        Creates a QImage with code PDF.
        This function is not yet supported.

    @param data
        The data that will be placed in the code PDF.  Usually a string is used.

    @param length
        The length of the data to be placed in the code PDF.

    @param module
        @todo

    @param vertcalScale
        @todo

    @param eccLevel
        Correction level.

    @param ratio
        @todo

    @return
        QImage containing the code PDF

    @throw QAException
        Thrown if document PDF generation failed.

*/
    static QImage GeneratePDF(const char *data, unsigned int length, int module, int verticalScale, int eccLevel, int ratio);
/// @endcond

/// @cond internal
/**
    @~english
        Creates a QImage with code PDF.

    @param data
        The data that will be placed in the code PDF.  Usually a string is used.

    @param length
        The length of the data to be placed in the code PDF.

    @param module
        @todo

    @param vertcalScale
        @todo

    @param eccLevel
        Correction level.

    @param lines
        @todo

    @param columns
        @todo

    @return
        QImage containing the code PDF.

    @throw QAException
        Thrown if document PDF generation failed.
*/
    static QImage GeneratePDF(const char *data, unsigned int length, int module, int verticalScale, int eccLevel, int lines, int columns);
/// @endcond
};


/**
    @}
*/


} // namespace StarMicronics_QAsura


#endif // QASURABITMAP_H
