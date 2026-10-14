#ifndef OUTFUNCTIONS_H
#define OUTFUNCTIONS_H

/*! \defgroup libStarBitmap libStarBitmap

  This Library is used to create and modify custom StarBitmaps to be used in StarMessage library

  This Library has functions for drawing lines and other starBitmap
  */

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

/** \ingroup libStarBitmap
  @{
*/
extern "C"
{
/** 
Creates a StarBitmap object for editing an image

@param width The width of the Image to create
@param height The height of the image to create

@return A pointer to a StarBitmap object <br/>
        Make sure to call StarMicronics_DeleteStarBitmap when your done with it

*/
void * StarMicronics_CreateBitmap(unsigned int width, unsigned int height);

/**
Creates a StarBitmap object for editing an image

@param width The width of the Image to create
@param height The height of the image to create
@param startingImage The image data that is read as the default image.  This must be in an rgb horizontal format.  The data is copied to a new buffer to caller can delete the old data.

@return A pointer to a StarBitmap object <br/>
        Make sure to call StarMicronics_DeleteStarBitmap when your done with it
*/
void * StarMicronics_CreateBitmapWithImage(unsigned int width, unsigned int height, const unsigned char *startingImage);

/**
Deletes a StarBitmap and returns the memory to the os

@param bitmap The pointer
*/
void StarMicronics_DeleteStarBitmap(void * bitmap);

/**
Gets the width of a StarBitmap object

@param bitmap The pointer to a bitmap object created with StarMicronics_CreateBitmap

@return The width of the StarBitmap
*/
unsigned int StarMicronics_GetWidth(void *bitmap);

/**
Gets the height of a StarBitmap object

@param bitmap The pointer to a bitmap object created with StarMicronics_CreateBitmap

@return The height of the StarBitmap
*/
unsigned int StarMicronics_GetHeight(void *bitmap);

/**
Gets the raw rgb data of the StarImage.  This data should not be modfied.  In can be used for such things as creation of a qimage

@param bitmap The pointer to a bitmap object created with CreateBitmap

@return The raw rgb data
*/
const unsigned char* StarMicronics_GetBitmapData(void *bitmap);

/**
Gets a rgb Pixel from the bitmap

@param bitmap The pointer to a bitmap object created with StarMicronics_CreateBitmap
@param x The x location of the pixel that you are getting
@param y The y location of the pixel that you are getting

@return the rgb integer that is associated with that pixel
*/
unsigned int StarMicronics_GetPixel(void *bitmap, unsigned int x, unsigned int y);

/**
  Sets the rgb value of a single pixel in the StarBitmap

  @param bitmap The pointer to a bitmap object created with StarMicronics_CreateBitmap
  @param x The x location of the pixel that you are setting
  @param y The y location of the pixel that you are setting
  @param rgbValue The new rgb value so set to the pixel
  */
void StarMicronics_SetPixel(void *bitmap, unsigned int x, unsigned int y, unsigned int rgbValue);

/**
  Draws a Horizontal Line in the StarBitmap

  @param bitmap The pointer to a bitmap object created with StarMicronics_CreateBitmap
  @param xStart The x value of where to start drawing the line
  @param xEnd The x value of where to end drawing the line
  @param y The y value of where to put the line
  */
void StarMicronics_DrawHorizontalLine(void *bitmap, unsigned int xStart, unsigned int xEnd, unsigned int y);

/**
  Draws a Virtual line in the StarBitmap

  @param bitmap The pointer to a bitmap object created with StarMicronics_CreateBitmap
  @param x The Horizontal point of where to place the line
  @param yStart The y value of where to start drawing the line
  @param yEnd The y value of where to end drawing the line
  */
void StarMicronics_DrawVerticalLine(void *bitmap, unsigned int x, unsigned int yStart, unsigned int yEnd);

/**
  Draws one StarBimap ontop of another StarBitmap

  @param bitmap The pointer to a bitmap object created with StarMicronics_CreateBitmap that will be drawn onto
  @param x The x point where to start drawing the source StarBitmap
  @param y The y point where to start drawing the source StarBitmap
  @param source The StarBitmap contain the data for the image that will be recieved
  @param rec_x The x location in the source StarBitmap to start copying image data
  @param rec_y The y location in the source StarBitmap to start copying image data
  @param rec_width The amount of width data to copy from the source
  @param rec_height The amount of height data to copy from the source
*/
void StarMicronics_Blit(void *bitmap, unsigned int x, unsigned int y, void *source, unsigned int rec_x, unsigned int rec_y, unsigned int rec_width, unsigned int rec_height);

/**
  Creates image comands from the StarBitmap that can be understood by the MechHandler

  @param bitmap The pointer to a bitmap object created with StarMicronics_CreateBitmap
  @param buffer A user allocated buffer that will recieve the MechHandler data
  @param bufferSize The size of the user allocated buffer
  @param bufferSizeNeeded A pointer to an integer that will recieve that required size the the buffer should be inorder to recieve the MechHandler data.  This paramiter is always set even if the fuction fails.

  @return A value indicating weather this function succeded or failed.  Success is 0 and failed is -1
  */
int StarMicronics_CreateStarMessageFormat(void *bitmap, unsigned char *buffer, unsigned int bufferSize, unsigned int *neededBufferSize);

/**
  Creates image comands from the StarBitmap that can be understood by the MechHandler in a vertical format

  @param bitmap The pointer to a bitmap object created with StarMicronics_CreateBitmap
  @param buffer A user allocated buffer that will recieve the MechHandler data
  @param bufferSize The size of the user allocated buffer
  @param bufferSizeNeeded A pointer to an integer that will recieve that required size the the buffer should be inorder to recieve the MechHandler data.  This paramiter is always set even if the fuction fails.

  @return A value indicating weather this function succeded or failed.  Success is 0 and failed is -1
  */
int StarMicronics_CreateStarMessageFormatV(void *bitmap, unsigned char *buffer, unsigned int bufferSize, unsigned int *neededBufferSize);

/// @cond internal

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

int StarMicronics_DrawQRCode(void *bitmap, unsigned int x, unsigned int y, const char * data, unsigned int dataLength, QR_MODEL model, QR_CELL_SIZE size, QR_CORRECTION_LEVEL cl, unsigned int scale);

/// @endcond


/**
  @~english
  Puts a barcode 128 onto the StarBitmap

  @param bitmap The point to a bitmap object created with StarMicronics_CreateBitmap
  @param x The x point where to place the barcode
  @param y The y point where to place the barcode
  @param data The data that will be placed in the barcode.  Usually a string is used
  @param dataLength The length of the data to be placed in the barcode 128
  @param module This is the width of each individual bar for for the barcode
  @param height This is the height of the barcode

  @return A value indicating weather this function succeded or failed.
          Success is 0 and failed is -1

  */
int StarMicronics_DrawCode128(void *bitmap, unsigned int x, unsigned int y, const char * data, unsigned int dataLength, int module, int height);

/**
  @~english
  Puts a barcode 39 onto the StarBitmap

  @param bitmap The point to a bitmap object created with StarMicronics_CreateBitmap
  @param x The x point where to place the barcode
  @param y The y point where to place the barcode
  @param data The data that will be placed in the barcode.  Usually a string is used
  @param narrowModule This is the width of the narrow bars
  @param wideModule This is the width of the wide bars
  @param height This is the height of the barcode

  @return A value indicating weather this function succeded or failed.
         Success is 0 and failed is -1

      */
int StarMicronics_DrawCode39(void *bitmap, unsigned int x, unsigned int y, const char *data, int narrowModule, int wideModule, int height);

/**
  @~english
  Puts a barcode 93 onto the StarBitmap

  @param bitmap The point to a bitmap object created with StarMicronics_CreateBitmap
  @param x The x point where to place the barcode
  @param y The y point where to place the barcode
  @param data The data that will be placed in the barcode.  Usually a string is used
  @param module This is the width of the narrow bars
  @param height This is the height of the barcode

  @return A value indicating weather this function succeded or failed.
          Success is 0 and failed is -1

  */
int StarMicronics_DrawCode93(void *bitmap, unsigned int x, unsigned int y, const char *data, unsigned int dataLength, int module, int height);

/**
 @~english
 Puts a barcode ean8 into the bitmap

 @param bitmap The point to a bitmap object created with StarMicronics_CreateBitmap
 @param x The x point where to place the barcode
 @param y The y point where to place the barcode
 @param data The data the will be place in the barcode.  It should be a null terminated string that consists of the 7 to 8 characters from 0 to 9.
 @param module The width of each individual bar in the barcode
 @param height The height of the barcode
 */
int StarMicronics_DrawCodeEAN8Code(void *bitmap, unsigned int x, unsigned int y, const char *data, int module, int height);

/**
 @~english
 Puts a barcode ean8 into the bitmap

 @param bitmap The point to a bitmap object created with StarMicronics_CreateBitmap
 @param x The x point where to place the barcode
 @param y The y point where to place the barcode
 @param data The data the will be place in the barcode.  It should be a null terminated string that consists of the 11 to 13 characters from 0 to 9.
 @param module The width of each individual bar in the barcode
 @param height The height of the barcode
 */
int StarMicronics_DrawCodeEAN13Code(void *bitmap, unsigned int x, unsigned int y, const char *data, int module, int height);

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
int StarMicronics_Drawi2of5Code(void *bitmap, unsigned int x, unsigned int y, const char *data, int narrowModule, int wideModule, int height);
}

/**
  @}
*/
#endif // OUTFUNCTIONS_H
