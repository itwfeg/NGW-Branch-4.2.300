#ifndef QABARCODEREADER_H
#define QABARCODEREADER_H

#include <QObject>
#include <QString>


namespace StarMicronics_QAsura
{


/**
    @ingroup QAsura
    @{
    @~english
        The barcode reader.

    @~japanese
        バーコード・デバイスからデータを読み込むためのAPIを提供します。
*/
class QABarcodeReader : public QObject
{
    Q_OBJECT

public:
    enum QABARCODE_MODE
    {
        QABARCODE_MODE_OFF               = 0,
        QABARCODE_MODE_SCAN_ONCE         = 1,
        QABARCODE_MODE_AUTO              = 2,
        QABARCODE_MODE_CONTINUOUS        = 3,
        QABARCODE_MODE_MOBILE_PHONE      = 4,
        QABARCODE_MODE_MOBILE_CONTINUOUS = 5
    };


    QABarcodeReader();


/**
    @~english
        Claims the barcode reader for exclusive use.

    @throw QAException
        Thrown if claim failed.

    @~japanese
        バーコード・リーダーの排他制御を取得します。

    @throw QAException
        排他制御の取得に失敗した場合、例外を投げます。
*/
    void Claim();


/**
    @~english
        Releases the barcode reader for exclusive use.

    @throw QAException
        Thrown if release failed.

    @~japanese
        バーコード・リーダーの排他制御を解放します。

    @throw QAException
        排他制御の解放に失敗した場合、例外を投げます。
*/
    void Release();


/**
    @~english
        Gets the next barcode barcode read in the queue.

    @param data
        This is the char array that will contain the barcode data.

    @param size
        The size of the data array.

    @return
        The number of characters in the barcode or 0 if no barcode is returned.

    @~japanese
        次のバーコードを取得します。

    @param data
        char配列にバーコードのデータが格納されます。

    @param size
        データ配列のサイズを示します。

    @throw QAException
        排他制御の解放に失敗した場合、例外を投げます。

    @return
        バーコードの文字数を返します。バーコードがない場合、oを返します。
*/
    unsigned int GetNextBarcode(char * data, unsigned int size);


/**
    @~english
        Gets the next barcode barcode read in the queue in a QString format.

    @return
        QString that contains the barcode data or NULL if no barcode is returned.

    @~japanese
        キューにある次のバーコードを、QString型で取得します。

    @return
        QString型のバーコード・データを返します。バーコードがない場合、NULLを返します。
*/
    QString GetNextBarcode();


/**
    @~english
    Get the number of barcodes in the queue.

    @return
        The number of barcodes in the queue.

    @~japanese
        キューに溜まっているバーコードの数を取得します。

    @return
        キューにあるバーコードの数を返します。
*/
    unsigned int NumberOfBarcodes();


/**
    @~english
        Sets the mode of the barcode reader.

    @param mode
        The mode of the barcode reader.@n
        This value can be QABARCODE_MODE_OFF, QABARCODE_MODE_SCAN_ONCE, QABARCODE_MODE_AUTO, QABARCODE_MODE_CONTINUOUS,
        QABARCODE_MODE_MOBILE_PHONE, or QABARCODE_MODE_MOBILE_CONTINUOUS.

    @throw QAException
        Thrown set failed.

    @~japanese
        バーコードを読み込むモードを設定します。

    @param mode
        モードを設定します。@n
        設定できる値は、QABARCODE_MODE_OFF、QABARCODE_MODE_SCAN_ONCE、QABARCODE_MODE_AUTO、QABARCODE_MODE_CONTINUOUS、
        QABARCODE_MODE_MOBILE_PHONEとQABARCODE_MODE_MOBILE_CONTINUOUSのどれかになります。

    @throw QAException
        設定に失敗した場合、例外を投げます。
*/
    void SetMode(QABARCODE_MODE mode);


/**
    @~english
        Get the current of the barcode.

    @return
        The mode of the barcode reader.@n
        This value can be QABARCODE_MODE_OFF, QABARCODE_MODE_SCAN_ONCE, QABARCODE_MODE_AUTO, QABARCODE_MODE_CONTINUOUS,
        QABARCODE_MODE_MOBILE_PHONE, or QABARCODE_MODE_MOBILE_CONTINUOUS.

    @throw QAException
        Thrown get failed.

    @~japanese
        現在のバーコード読み込みモードを取得します。

    @return
        現在のモードを返します。@n
        返ってくる値は、QABARCODE_MODE_OFF、QABARCODE_MODE_SCAN_ONCE、QABARCODE_MODE_AUTO、QABARCODE_MODE_CONTINUOUS、
        QABARCODE_MODE_MOBILE_PHONEとQABARCODE_MODE_MOBILE_CONTINUOUSのどれかになります。

    @throw QAException
        モードの取得に失敗した場合、例外を投げます。
*/
    QABARCODE_MODE GetMode();


/**
    @~english
        Sets the scan time out for the scan once mode.

    @param timeoutms
        The timeout for the QABARCODE_MODE_SCAN_ONCE and QABARCODE_MODE_MOBILE_PHONE mode in milliseconds.

    @throw QAException
        Thrown set failed or an invalid parameter value was used.

    @~japanese
        一度きりのスキャンを実行した時の、タイムアウト時間を設定します。

    @param timeoutms
        QABARCODE_MODE_SCAN_ONCEとQABARCODE_MODE_MOBILE_PHONEのモードにおける、タイムアウト時間をミリ秒で設定します。
        設定できる値は、0 〜 300000 (ms)になります。
        0を指定した場合、タイムアウトが発生しません。

    @throw QAException
        タイムアウト時間の設定に失敗、または無効なパラメータを設定した場合、例外を投げます。
*/
    void SetScanOnceTimeout(int timeoutms);


/**
    @~english
        Sets delay for how long to stop scanning for QABARCODE_MODE_AUTO, QABARCODE_MODE_CONTINUOUS and
        QABARCODE_MODE_MOBILE_CONTINUOUS modes.

    @param delayms
        The time used between read for the QABARCODE_MODE_AUTO,
        QABARCODE_MODE_CONTINUOUS and QABARCODE_MODE_MOBILE_CONTINUOUS modes in milliseconds.

    @throw QAException
        Thrown set failed or an invalid parameter value was used.

    @~japanese
        連続またはオート読み込みに設定した時の、同じアイテムの読み込み遅延時間を設定します。

    @param delayms
        QABARCODE_MODE_AUTO、QABARCODE_MODE_CONTINUOUSとQABARCODE_MODE_MOBILE_CONTINUOUSのモードにおける、
        同じアイテムの読み込み遅延時間を設定します。

    @throw QAException
        遅延時間の設定に失敗、または無効なパラメータを設定した場合、例外を投げます。
*/
    void SetDuplicateScanDelay(int delayms);


signals:
/**
    @~english
        Event activated when there is new barcode data to grab.  Use qt function connect to get the event

    @~japanese
        新しいバーコードを取得した時、イベントを発生させます。
*/
    void BarcodeScanned();


public:
/// @cond internal
    void pollBCR();
/// @endcond
};


/**
    @}
*/


} // namespace StarMicronics_QAsura


#endif // QABARCODEREADER_H
