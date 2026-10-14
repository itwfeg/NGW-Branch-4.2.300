#ifndef STARBITMAP_H
#define STARBITMAP_H

#include "StarMicronics_BitmapFunction.h"

namespace StarMicronics
{
typedef struct
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
} Star_RGBValue;

/** \ingroup libStarBitmap
  @{
*/

class StarBitmap
{
public:
    /**
    @~english
    Creates a StarBitmap object for editing an image

    @param width The width of the Image to create
    @param height The height of the image to create

    @~japanese
    画像を編集するStarBitmapオブジェクトを作成します。

    @param width  画像の幅
    @param height 画像の高さ
    */
    StarBitmap(unsigned int width,unsigned int height);

    /**
    @~english
    Creates a StarBitmap object for editing an image

    @param width The width of the Image to create
    @param height The height of the image to create
    @param startingImage The image data that is read as the default image.  This must be in an rgb horizontal format.  The data is copied to a new buffer so caller can delete the old data.

    @~japanese
    画像を編集するStarBitmapオブジェクトを作成します。

    @param width  画像の幅
    @param height 画像の高さ
    @param startingImage デフォルト画像として読み込まれる画像データです。  このデータはRGB水平フォーマットでなければなりません。 古いデータを削除するために、
        このデータが新しいバッファーに書き込まれます。
    */
    StarBitmap(unsigned int width, unsigned int height,const unsigned char *startingImage);

    /**

    @~english
    Creates a StarBitmap object for editing an image
    StarBitmap Standard copy constructor.  Handles memory managerment
    @param starbitmap
    */
    StarBitmap(const StarBitmap &starbitmap);

    StarBitmap & operator=(const StarBitmap& starbit);

    ~StarBitmap();

    /**
    @~english
    Gets the width of a StarBitmap object

    @return The width of the StarBitmap

    @~japanese
    StarBitmapオブジェクトの幅を取り出します。

    @return StarBitmapの幅が返り値になります。
    */
    unsigned int Width();

    /**
    @~english
    Gets the height of a StarBitmap object

    @return The height of the StarBitmap

    @~japanese
    StarBitmapオブジェクトの高さを取り出します。

    @return StarBitmapの高さが返り値になります。
    */
    unsigned int Height();

    /**
    @~english
    Gets the raw rgb data of the StarImage.  This data should not be modfied.  In can be used for such things as creation of a Qimage

    @return The raw rgb data

    @~japanese
    StarImageのRGBデータを取り出します。  このデータを変更しないでください。  このデータはQimageの作成などに使えます。

    @return RGBデータが返り値になります。
    */
    const unsigned char* BitmapData();

    /**
      @~english
      Sets the rgb value of a single pixel in the StarBitmap

      @param x The x location of the pixel that you are setting
      @param y The y location of the pixel that you are setting
     unsigned int scale @param rgbValue The new rgb value so set to the pixel

      @~japanese
      StarBitmapにおける単独ピクセルのRGB値を設定します。

      @param x ピクセルのX座標
      @param y ピクセルのY座標
      @param rgbValue ピクセルに設定される新しいRGB値
      */
    void SetPixel(unsigned int x, unsigned int y, unsigned int rgb);

    /**
    @~english
    Gets a RGB pixel from the bitmap

    @param x The x location of the pixel that you are getting
    @param y The y location of the pixel that you are getting

    @return the rgb integer that is associated with that pixel

    @~japanese
        ビットマップからRGBデータを取り出します。

    @param x ピクセルのX座標
    @param y ピクセルのY座標
    @return ピクセルに関連したRGB整数が返り値になります。
    */
    unsigned int GetPixel(unsigned int x, unsigned int y);

    /**
      @~english
      Draws a Horizontal Line in the StarBitmap

      @param xStart The x value of where to start drawing the line
      @param xEnd The x vnamespace StarMicronics
{alue of where to end drawing the line
      @param y The y value of where to put the line

      @~japanese
      StarBitmapに水平線を作ります。

      @param xStart 線の始まりのX値
      @param xEnd 線の終わりのX値
      @param y 線が設置されるY値
      */
    void DrawHorizontalLine(unsigned int xStart, unsigned int xEnd, unsigned int y);

    /**
      @~english
      Draws a Vertical line in the StarBitmap

      @param x The Vertical point of where to place the line
      @param yStart The y value of where to start drawing the line
      @param yEnd The y value of where to end drawing the line

      @~japanese
      StarBitmapに垂直線を作ります。

      @param x 線が設置される垂直ポイント
      @param yStart 線の始まりのY値
      @param yEnd 線の終わりのY値
      */
    void DrawVerticalLine(unsigned int x, unsigned int yStart, unsigned int yEnd);

    /**
      @~english
      Draws one StarBimap on top of another StarBitmap

      @param x The x point where to start drawing the source StarBitmap
      @param y The y point where to start drawing the source StarBitmap
      @param source The StarBitmap containing the data for the image that will be received
      @param rec_x The x location in the source StarBitmap to start copying image data
      @param rec_y The y location in the source StarBitmap to start copying image data
      @param rec_width The amount of width data to copy from the source
      @param rec_height The amount of height data to copy from the source

      @~japanese
      StarBitmapをコピーします。

      @param x ソースのStarBitmapの始まりのXポイント
      @param y ソースのStarBitmapの始まりのYポイント
      @param source ソース画像のデータが入っているStarBitmap
      @param rec_x 画像データがコピーされるソースのStarBitmapのX座標
      @param rec_y 画像データがコピーされるソースのStarBitmapのY座標
      @param rec_width ソースからコピーする幅のデータ量
      @param rec_height ソースからコピーする高さのデータ量
    */
    void Blit(unsigned int x, unsigned int y, StarBitmap *source, unsigned int rec_x, unsigned int rec_y, unsigned int rec_width, unsigned int rec_height);

    /**
      @~english
      Draw a filled Rectangle onto the StarBitmap

      @param x The x point where to start drawing on the StarBitmap
      @param y The y point where to start drawing on the StarBitmap
      @param width The width of the Rectangle
      @param height The height of the Rectangle
      @param color the rgb color to use for the Rectangle
    */
    void DrawFilledRectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int color);

    /**
      @~english
      Creates image commands from the StarBitmap that can be understood by the MechHandler

      @param buffer A user allocated buffer that will receive the MechHandler data
      @param bufferSize The size of the user allocated buffer
      @param bufferSizeNeeded A pointer to an integer that will receive that required size the buffer should be in order to receive the MechHandler data.  This parameter is always set even if the fuction fails.

      @return A value indicating weather this function succeded or failed.  
	      Success is 0 and failed is -1

      @~japanese
      StarBitmapからMechHandlerが受信できる画像コマンドを作成します。

      @param buffer ユーザに割り当てられたバッファーです。 このバッファーはMechHandlerのデータを受信します。
      @param bufferSize ユーザに割り当てられたバッファーのサイズです。
      @param bufferSizeNeeded MechHandlerのデータを受信するために必要となるバッファーのサイズを受信する整数へのポインタです。 
            このパラメターは関数が失敗しても、必ず設定されます。

      @return 関数が成功したか、失敗したかを確認する値です。
	      0 = 成功、-1 = 失敗
      */
    int CreateStarMessageFormat(unsigned char *buffer, unsigned int bufferSize, unsigned int *neededBufferSize);

    /**
      @~english
      Creates image comands from the StarBitmap that can be understood by the MechHandler in a vertical format

      @param buffer A user allocated buffer that will recieve the MechHandler data
      @param bufferSize The size of the user allocated buffer
      @param bufferSizeNeeded A pointer to an integer that will receive that required size the the buffer should be inorder to receive the MechHandler data.  This paramiter is always set even if the fuction fails.

      @return A value indicating weather this function succeded or failed.  
              Success is 0 and failed is -1

      @~japanese
      StarBitmapからMechHandlerが垂直フォーマットで受信できる画像コマンドを作成します。

      @param buffer ユーザに割り当てられたバッファーです。 このバッファーはMechHandlerのデータを受信します。
      @param bufferSize ユーザに割り当てられたバッファーのサイズです。
      @param bufferSizeNeeded MechHandlerのデータを受信するために必要となるバッファーのサイズを受信する整数へのポインタです。 
            このパラメターは関数が失敗しても、必ず設定されます。

      @return 関数が成功したか、失敗したかを確認する値です。
	      0 = 成功、-1 = 失敗

      */
    int CreateStarMessageFormatV(unsigned char *buffer, unsigned int bufferSize, unsigned int *neededBufferSize);


    /**
      @~english
      Puts a qrcode onto the StarBitmap

      @param x The x point where to place the qr code
      @param y The y point where to place the qr code
      @param data The data that will be placed in the qr code.  Usually a string is used
      @param dataLength The length of the data to be placed in the qr code
      @param model This is the model the qrcode will use.  Either QRMODEL1 or QRMODEL2
      @param size This is the cell size that will be used to make the qr code.  This can be QR_CELL_SIZE_1, QR_CELL_SIZE_2, QR_CELL_SIZE_3, QR_CELL_SIZE_4, QR_CELL_SIZE_5, QR_CELL_SIZE_6, QR_CELL_SIZE_7, or QR_CELL_SIZE_8.
      @param cl This is the correction level to use in the qr code.  This value can be QR_CORRECTION_LEVEL_1, QR_CORRECTION_LEVEL_2, QR_CORRECTION_LEVEL_3, or QR_CORRECTION_LEVEL_4.
      @param scale Scales the resulting QrCode bitmap, valid values range from 1 to 10. Thipically, for printing this should always be set to 1, and the size parameter used instead.
             Extra scaling may be useful if the QrCode is intended to be displayed on a larger or higher resolution device such as HTML signage.

      @return A value indicating weather this function succeded or failed.
              Success is 0 and failed is -1


      */
    int DrawQRCode(unsigned int x, unsigned int y, const char *data, unsigned int dataLength, QR_MODEL model, QR_CELL_SIZE size, QR_CORRECTION_LEVEL cl, unsigned int scale);


    /**
      @~english
      Puts a barcode 128 onto the StarBitmap

      @param x The x point where to place the barcode
      @param y The y point where to place the barcode
      @param data The data that will be placed in the barcode.  Usually a string is used
      @param dataLength The length of the data to be placed in the barcode 128
      @param module This is the width of each individual bar for for the barcode
      @param height This is the height of the barcode

      @return A value indicating weather this function succeded or failed.
              Success is 0 and failed is -1

      */
    int DrawCode128(unsigned int x, unsigned int y, const char *data, unsigned int dataLength, int module, int height);

    /**
      @~english
      Puts a barcode 39 onto the StarBitmap

      @param x The x point where to place the barcode
      @param y The y point where to place the barcode
      @param data The data that will be placed in the barcode.  Usually a string is used
      @param narrowModule This is the width of the narrow bars
      @param wideModule This is the width of the wide bars
      @param height This is the height of the barcode

      @return A value indicating weather this function succeded or failed.
              Success is 0 and failed is -1

      */
    int DrawCode39(unsigned int x, unsigned int y, const char *data, int narrowModule, int wideModule, int height);

    /**
      @~english
      Puts a barcode 93 onto the StarBitmap

      @param x The x point where to place the barcode
      @param y The y point where to place the barcode
      @param data The data that will be placed in the barcode.  Usually a string is used
      @param dataLength The length of the data that will be put into the barcode
      @param module This is the width of the narrow bars
      @param height This is the height of the barcode

      @return A value indicating weather this function succeded or failed.
              Success is 0 and failed is -1

      */

    int DrawCode93(unsigned int x, unsigned int y, const char *data, unsigned int dataLength, int module, int height);

    /**
     @~english
     Puts a barcode ean8 into the bitmap

     @param x The x point where to place the barcode
     @param y The y point where to place the barcode
     @param data The data the will be place in the barcode.  It should be a null terminated string that consists of the 7 to 8 characters from 0 to 9.
     @param module The width of each individual bar in the barcode
     @param height The height of the barcode
     */
    int DrawCodeEan8(unsigned int x, unsigned int y, const char *data, int module, int height);

    /**
     @~english
     Puts a barcode ean13 into the bitmap

     @param x The x point where to place the barcode
     @param y The y point where to place the barcode
     @param data The data the will be place in the barcode.  It should be a null terminated string that consists of the 11 to 13 characters from 0 to 9.
     @param module The width of each individual bar in the barcode
     @param height The height of the barcode
     */
    int DrawCodeEan13(unsigned int x, unsigned int y, const char *data, int module, int height);

    /**
     @~english
     Puts a barcode Interwiev 2 of 5 barcode into the bitmap

     @param x The x point where to place the barcode
     @param y The y point where to place the barcode
     @param data The data the will be place in the barcode.  It should be a null terminated string that consists of characters from 0 to 9.  The number of characters should be even.
     @param narrowModule The width of the narrow bars in the barcode
     @param wideModule The width of the wide bars in the barcode.  The wideModule should be 2 to 3 time larger than narrowModule.  It its too big or too small then the wideModule it automatically corrected to the closed valid value.
     @param height The height of the barcode
     */
    int DrawCodei2of5(unsigned int x, unsigned int y, const char *data, int narrowModule, int wideModule, int height);
private:
    unsigned int width;
    unsigned int height;
    Star_RGBValue* rgbData;
    unsigned int GetPixelIndex(unsigned int x, unsigned int y);
};
}

/**
  @}
*/

#endif // STARBITMAP_H
