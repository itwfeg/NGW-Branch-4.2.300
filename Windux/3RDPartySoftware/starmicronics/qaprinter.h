#ifndef QAPRINTER_H
#define QAPRINTER_H

#include "printerstatus.h"
#include <QImage>
#include <QString>


namespace StarMicronics_QAsura
{


/**
    @ingroup QAsura
    @{
    @~english
        The printer function.

    @~japanese
        プリンタへの設定や印字を行うためのAPIを提供します。
*/
class QAPrinter
{
public:
    enum QAPRINT_SPEED
    {
        QAPRINT_SPEED_HIGH   = 0,
        QAPRINT_SPEED_MEDIUM = 1,
        QAPRINT_SPEED_LOW    = 2
    };


    enum QAPRINT_DITHERING
    {
        QAPRINT_DITHERING_NONE            = 0,
        QAPRINT_DITHERING_ERROR_DIFFUSION = 1
    };


    QAPrinter();


    ~QAPrinter();


/**
    @~english
        Clams the printer for exclusive use.

    @throw QAException
        Thrown if claim of the printer failed.

    @~japanese
        プリンタの排他制御を取得します。

    @throw QAException
        排他制御の取得に失敗した場合、例外を投げます。
*/
    void Claim();


/**
    @~english
        Release the printer from exclusive use.

    @throw QAException
        Thrown releasing the printer failed.

    @~japanese
        プリンタの排他制御を解放します。

    @throw QAException
        排他制御の解放に失敗した場合、例外を投げます。
*/
    void Release();


/**
    @~english
        Sends a QImage to the printer for printing.

    @param image
        The image to send to the printer.

    @throw QAException
        Thrown if sending the image to the printer false.

    @~japanese
        QImage形式のデータをプリンタへ送信し、イメージを印字します。

    @param image
        プリンタへ送信するイメージを指定します。

    @throw QAException
        プリンタへのイメージ送信に失敗した場合、例外を投げます。
*/
    void PrintQImage(QImage image);


/**
    @~english
        Cuts the paper.

    @param feed
        The bool value telling the printer to feed paper before cutting.

    @throw QAException
        Thrown if sending the cut command to the printer failed.

    @~japanese
        紙をカットします。

    @param feed
        紙をカットする前に、紙送りを実行するか指定します。

    @throw QAException
        カットコマンドの送信に失敗した場合、例外を投げます。
*/
    void Cut(bool feed);


/**
    @~english
        Feeds the paper a number of dots.

    @param dots
        The number of dots to feed.
        The number of dots defined are between 1 and 65,535 (dots).

    @throw QAException
        Thrown if sending dots feed command to the printer fails or an invalid parameter value was used.

    @~japanese
        指定したドット数、紙送りを実行します。

    @param dots
        紙送りするドット数を指定します。
        設定できるドット数は、1 〜 65,535 (dots)です。

    @throw QAException
        ドット紙送りコマンドの送信に失敗、または無効なパラメータを設定した場合、例外を投げます。
*/
    void FeedDots(int dots);


/**
    @~english
        Feeds the paper a number of millimeter.

    @param mm
        The number of millimeter to feed.
        This should be from 1 to 8,191 (mm).

    @throw QAException
        Thrown if sending mm feed command to the printer fails or an invalid parameter value was used.

    @~japanese
        指定したミリメートル分、紙送りを実行します。

    @param mm
        設定したミリメートル分、紙送りを実行します。
        設定できる値は、1 〜 8,191 (mm)です。

    @throw QAException
        紙送りコマンドの送信に失敗、または無効なパラメータを設定した場合、例外を投げます。
*/
    void FeedMM(int mm);


/**
    @~english
        Gets the status of the printer.

    @return
        A QAPrinterStatus structure containing the status of the printer.

    @throw QAException
        Thrown if getting the status fails.

    @~japanese
        プリンタのステータスを取得します。

    @return
        QAPrinterStatus構造体は、プリンタのステータスを含みます。

    @throw QAException
        ステータスの取得に失敗した場合、例外を投げます。
*/
    static QAPrinterStatus Status();


/**
    @~english
        Locks or unlocks the cover of the printer.

    @param locked
        The bool value indicating that the printer should be locked or unlocked.

    @throw QAException
        Thrown if sending lock or unlock command fails.

    @~japanese
        プリンタのカバーをロックまたはアンロックします。

    @param locked
        プリンタのカバーをロックするかアンロックするか指定します。

    @throw QAException
        カバーコマンドの送信に失敗した場合、例外を投げます。
*/
    void SetCoverLock(bool locked);


/**
    @~english
        Resets led commands to start state

    @throw QAException
        Thrown if sending LED reset command fails.

    @~japanese
        LEDの設定をリセットします。

    @throw QAException
        LEDのリセットコマンドの送信に失敗した場合、例外を投げます。
*/
    void LedReset();


/**
    @~english
        Sets the LED lights on the printer

    @param red
        This is the value of the red component of the LED.
        This should be from 0 to 100.

    @param green
        This is the value of the green component of the LED.
        This should be from 0 to 100

    @param blue
        This is the value of the blue component of the LED.
        This should be from 0 to 100

    @throw QAException
        Thrown if sending LED sets command fails or an invalid parameter value was used.

    @~japanese
        LEDの表示を設定します。

    @param red
        この値は、LEDの赤色を設定します。
        設定できる値は、0 〜 100です。

    @param green
        この値は、LEDの緑色を設定します。
        設定できる値は、0 〜 100です。

    @param blue
        この値は、LEDの青色を設定します。
        設定できる値は、0 〜 100です。

    @throw QAException
        LEDの設定コマンドの送信に失敗、または無効なパラメータを設定した場合、例外を投げます。
*/
    void LedSet(unsigned char red, unsigned char green, unsigned char blue);


/**
    @~english
        This tells the led lights to flash from one color to another color and back again repeatedly.

    @param time
        The interval time for the flash to make in milliseconds.  This should be from 50 to 10000.

    @param red1
        This is the red component for the first color in the flash.  This should be 0 to 100.

    @param green1
        This is the green component for the first color in the flash.  This should be 0 to 100.

    @param blue1
        This is the blue component for the first color in the flash.  This should be 0 to 100.

    @param red2
        This is the red component for the second color in the flash.  This should be 0 to 100.

    @param green2
        This is the green component for the second color in the flash.  This should be 0 to 100.

    @param blue2
        This is the blue component for the second color in the flash.  This should be 0 to 100.

    @throws QAException
        Thrown if sending LED flash command fails or an invalid parameter value was used.
*/
    void LedFlash(int time, unsigned char red1, unsigned char green1, unsigned char blue1, unsigned char red2, unsigned char green2, unsigned char blue2);


/**
    @~english
        This tells the led lights to flash from one color to another color and back again for a set number of times.

    @param times
        The number of times to flash the led lights.  This should be from 1 to 10.

    @param red1
        This is the red component for the first color in the flash.  This should be 0 to 100.

    @param green1
        This is the green component for the first color in the flash.  This should be 0 to 100.

    @param blue1
        This is the blue component for the first color in the flash.  This should be 0 to 100.

    @param red2
        This is the red component for the second color in the flash.  This should be 0 to 100.

    @param green2
        This is the green component for the second color in the flash.  This should be 0 to 100.

    @param blue2
        This is the blue component for the second color in the flash.  This should be 0 to 100.

    @throws QAException
        Thrown if sending LED flash number command fails or an invalid parameter value was used.
*/
    void LedFlashForANumberOfTime(int times, unsigned char red1, unsigned char green1, unsigned char blue1, unsigned char red2, unsigned char green2, unsigned char blue2);


/**
    @~english
        This tells the led lights tell the led lights to gradually shift from one color to another and back again.

    @param time
        The interval of time for the lights to shift from one color to another in milliseconds.
        This should be from 50 to 10000.

    @param red1
        This is the red component for the first color in the gradual shift.  This should be 0 to 100.

    @param green1
        This is the green component for the first color in the gradual shift.  This should be 0 to 100.

    @param blue1
        This is the blue component for the first color in the gradual shift.  This should be 0 to 100.

    @param red2
        This is the red component for the second color in the gradual shift.  This should be 0 to 100.

    @param green2
        This is the green component for the second color in the gradual shift.  This should be 0 to 100.

    @param blue2
        This is the blue component for the second color in the gradual shift.  This should be 0 to 100.

    @throws QAException
        Thrown if sending LED gradual shift command fails or an invalid parameter value was used.
*/
    void LedGradualShift(int time, unsigned char red1, unsigned char green1, unsigned char blue1, unsigned char red2, unsigned char green2, unsigned char blue2);


/**
    @~english
        This tells the led lights tell the led lights to gradually shift from one color to another and flash back.

    @param time
        The interval of time for the lights to shift from one color to another in milliseconds.
        This should be from 50 to 10000.

    @param red1
        This is the red component for the first color in the gradual shift.  This should be 0 to 100.

    @param green1
        This is the green component for the first color in the gradual shift.  This should be 0 to 100.

    @param blue1
        This is the blue component for the first color in the gradual shift.  This should be 0 to 100.

    @param red2
        This is the red component for the second color in the gradual shift.  This should be 0 to 100.

    @param green2
        This is the green component for the second color in the gradual shift.  This should be 0 to 100.

    @param blue2
        This is the blue component for the second color in the gradual shift.  This should be 0 to 100.

    @throws QAException
        Thrown if sending LED gradual shift and flash command fails or an invalid parameter value was used.
*/
    void LedGradualShiftThenFlash(int time, unsigned char red1, unsigned char green1, unsigned char blue1, unsigned char red2, unsigned char green2, unsigned char blue2);


/**
    @~english
        Set the print speed of the printer.

    @param printSpeed
        An value indicating what speed this QAprinter object should use.@n
        This value can be QAPRINT_SPEED_HIGH, QAPRINT_SPEED_MEDIUM, QAPRINT_SPEED_LOW.

    @throw QAException
        Thrown sending speed command fails.

    @~japanese
        プリンタの印字速度を指定します。

    @param printSpeed
        QAPrinterクラスによる印字速度を設定します。@n
        指定する値は、QAPRINT_SPEED_HIGH、QAPRINT_SPEED_MEDIUM、QAPRINT_SPEED_LOWのどれかになります。

    @throw QAException
        印字速度設定コマンドの送信に失敗した場合、例外を投げます。
*/
    void SetPrintSpeed(QAPRINT_SPEED printSpeed);


/**
    @~english
        Set the density of the printer.

    @param density
        An integer indicating what density this QAprinter object should use.@n
        This value can be -3 to 3.

    @throw QAException
        Thrown sending speed command fails or an invalid parameter value was used.

    @~japanese
        プリンタの印字濃度を指定します。

    @param density
        QAPrinterクラスによる印字濃度をinteger型で指定します。@n
        値の範囲は、 -3 〜 3 です。

    @throw QAException
        印字濃度設定コマンドの送信に失敗、または無効なパラメータを設定した場合、例外を投げます。
*/
    void SetDensity(int density);


/**
    @~english
        Sets the type of dithering this QAprinter object will use when printing QImages (dithering is not yet supported by mech handler).

    @param setDithering
        The type of dithering to use.@n
        This value can be QAPRINT_DITHERING_NONE or QAPRINT_DITHERING_ERROR_DIFFUSION.

    @throw QAException
        Thrown sending dithering change command fails.

    @~japanese
        QAPrinterクラスによる印字の、テザリングの種類を指定します。

    @param setDithering
        利用するテザリングの種類を指定します。@n
        指定できる値は、QAPRINT_DITHERING_NONEかQAPRINT_DITHERING_ERROR_DIFFUSIONになります。

    @throw QAException
        テザリングの種類指定コマンドの送信に失敗した場合、例外を投げます。
*/
    void SetDithering(QAPRINT_DITHERING setDithering);


/**
    @~english
        Sets the gray level for the printer to use.
        This always sets the lower gray value if the value set is unsupported.

    @param grayLevel
        The gray level to use.
        This value is dependent on the printer used with this command.@n
        TSP100GT - 1 or 8@n
        ACP 1, 8, 16, or 32@n

    @throw QAException
        Thrown if sending gray level command fails.

    @~japanese
        プリンタで使用するグレイ・レベルを指定します。
        サポートされていないグレイ・レベルを指定した場合、一番低いグレイ・レベルが設定されます。

    @param greyLevel
        使用するグレイ・レベルを設定します。
        この値は、利用するプリンタの種類に依存します。@n
        TSP100GT - 1, 8@n
        ACP - 1, 8, 16, 32@n

    @throw QAException
        グレイ・レベル設定コマンドの送信に失敗した場合、例外を投げます。
*/
    void SetGrayLevel(int grayLevel);


/**
    @~english
        Returns the printer to its default state.

    @throw QAException
        Thrown if sending reset command fails.

    @~japanese
        プリンタの設定を、デフォルトに戻します。

    @throw QAException
        設定リセット・コマンドの送信に失敗した場合、例外を投げます。
*/
    void ResetToDefault();


/**
    @~english
        Increments the etb counter, the counter should be reflected in the status.

    @throw QAException
        Thrown if sending etb counter command fails.

    @~japanese
        ETBカウンタの値を1つ増やし、ステータスに反映させます。

    @throw QAException
        ETBコマンドの送信に失敗した場合、例外を投げます。
*/
    void IncrementEtb();


/**
    @~english
        Clears the ETB counter and sets it to 0.

    @throw QAException
        Thrown if sending etb clear command fails.

    @~japanese
        ETBカウンタをクリアし、0に設定します。

    @throw QAException
        ETBクリア・コマンドの送信に失敗した場合、例外を投げます。
*/
    void ClearEtb();


/**
    @~english
        Controls a periferal device for the printer (eg.. cashdraw, buzzer ...).

    @param pulse
        The pulse used in controlling the periferal device.
        This is in micro seconds.
        Its defined value is from 10 to 1270 and can only be used in intervals of 10.
        If not in an intervals of 10 then the one's diget will automatically set to 0.

    @param delay
        The delay to use before sending the pulse to the periferal.
        This value can be 10 to 1270 (ms).

    @throw QAException
        Thrown if sending pulse command fails or an invalid parameter value was used.

    @~japanese
        プリンタの外部機器駆動を制御します。

    @param pulse
        外部機器を駆動するための通電時間を、ミリ秒で設定します。
        設定できる値は 10 〜 1270 (ms)で、10ms単位で指定してください。
        10ms以下の値を指定しても、0として扱います。
    @param delay
        外部機器を駆動するときのディレイ時間を、ミリ秒で設定します。
        設定できる値は 10 〜 1270 (ms)で、10ms単位で指定してください。
        10ms以下の値を指定しても、0として扱います。

    @throw QAException
        外部機器の設定コマンド送信に失敗、または無効なパラメータを設定した場合、例外を投げます。
*/
    void ControlPeripheral(unsigned int pulse, unsigned int delay);


/**
    @~english
        Drives external device (eg.. cashdraw, buzzer ...).

    @param deviceNumber
        The deviceNumber used in choosing the device.@n
        This value can be 1 or 2.

    @throw QAException
        Thrown if sending drive command fails or an invalid parameter value was used.

    @~japanese
        外部機器を駆動します。

    @param deviceNumber
        駆動する外部機器を指定します。@n
        指定できる値は、 1 か 2 です。

    @throw QAException
        外部機器の駆動コマンド送信に失敗、または無効なパラメータを設定した場合、例外を投げます。
*/
    void OpenDrawer(int deviceNumber);
};


/**
    @}
*/


} // namespace StarMicronics_QAsura


#endif // QAPRINTER_H
