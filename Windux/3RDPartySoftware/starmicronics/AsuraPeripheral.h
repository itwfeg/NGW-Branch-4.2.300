/**
    @defgroup libAsuraPeripheral libAsuraPeripheral

    @~english
        Used by applications to use Asura Peripheral

    @~japanese
        ACPに接続されたデバイスを操作するために使用されます。
*/


#ifndef ASURAPERIPHERAL_H
#define ASURAPERIPHERAL_H


#ifdef __cplusplus
extern "C"
{
#endif


enum BARCODE_MODE
{
    BARCODE_MODE_OFF               = 0,
    BARCODE_MODE_SCAN_ONCE         = 1,
    BARCODE_MODE_AUTO              = 2,
    BARCODE_MODE_CONTINUOUS        = 3,
    BARCODE_MODE_MOBILE_PHONE      = 4,
    BARCODE_MODE_MOBILE_CONTINUOUS = 5
};


/**
    @~english
        This starts using the barcode reader.

    @return
        0 = Success, 0 > Failed.

    @~japanese
    @brief
        BCRデバイスの使用を開始します。

    @return
        0 = 成功, 0 > 失敗
*/
int SM_BCR_Open();


/**
    @~english
        This ends using the barcode reader.

    @return
        0 = Success, 0 > Failed.

    @~japanese
    @brief
        BCRデバイスの使用を終了します。

    @return
        0 = 成功, 0 > 失敗
*/
int SM_BCR_Close();


/**
    @~english
        Sets the mode of the barcode reader

    @param mode
        The mode of the barcode reader.
        This value can be BARCODE_MODE_OFF, BARCODE_MODE_SCAN_ONCE, BARCODE_MODE_AUTO, BARCODE_MODE_CONTINUOUS,
        BARCODE_MODE_MOBILE_PHONE, or BARCODE_MODE_MOBILE_CONTINUOUS.
    @return
        0 = Success, 0 > Failed.

    @~japanese
        BCRデバイスからバーコード取得するタイミングを設定します。

    @param mode
        バーコードを取得するタイミングを設定します。@n
        BARCODE_MODE_OFF : バーコード取得を止めます。@n
        BARCODE_MODE_SCAN_ONCE : 一度だけ、バーコードを取得します。@n
        BARCODE_MODE_AUTO : 自動でバーコードを取得します。@n
        BARCODE_MODE_CONTINUOUS : バーコードを取得し続けます。@n
    @return
        0 = 成功, 0 > 失敗
*/
int SM_BCR_SetMode(BARCODE_MODE mode);


/**
    @~english
        Gets a current mode of the barcode reader.

    @return
        BARCODE_MODE containing a current mode.

    @~japanese
        BCRデバイスに対する、現在の設定を取得します。

    @return
        現在BCRデバイスに設定されている、バーコードを取得するタイミングを返します。
*/
BARCODE_MODE SM_BCR_GetMode();


/**
    @~english
        This function attempts to read up to 'maxLength' bytes from the barcode reader into the buffer starting at 'data'.

    @param data
        This is the char array that will contain the barcode data
    @param maxLength
        Set the maximum size of the data.
    @return
        On success, the number of bytes read is returned (zero indicates no data).@n
        On error, -1 is returned.

    @~japanese
        BCRデバイスからバーコード・データを読み込みます。

    @param data
        読み込んだバーコード・データを格納する配列を指定します。
    @param maxLength
        読み込むバーコード・データの最大サイズを指定します。
    @return
        読み込んだデータのサイズを返します（0は、データが無かったことを示します）。@n
        読み込みに失敗した場合、-1を返します。
*/
int SM_BCR_ReadBCR(char *data, int maxLength);


/**
    @~english
        Specify a delay in milliseconds during which the barcode scanner will not re-read an identical barcode.

    @param delay
        Time, in ms, before the scanner will reread the same barcode. Minimum 0ms, maximum 30000ms.
    @return
        0 = Success, 0 > Failed.
*/
int SM_BCR_SetDuplicateScanDelay(unsigned int delay);


/**
    @~english
        Specify a time, in milliseconds, that the scanner will be active for when scan once mode has been enabled.
        If the scanner reaches this timeout without scanning a barcode, then it will switch itself off.

    @param delay
        Time, in ms, before the scanner will switch off if a barcode has not been scanned (in scan once mode only).
        Mimumum value 0ms, maximum 300000ms.
    @return
        0 = Success, 0 > Failed.
*/
int SM_BCR_SetScanOnceTimeout(int delay);


#ifdef __cplusplus
}
#endif


#endif // ASURAPERIPHERAL_H
