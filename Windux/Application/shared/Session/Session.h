//! Session.h
/*! The Session() class is a singleton where only a single instance of this class ever exists.
 *  The class is responsible for funneling messages on to the socket class, and once messages
 *  are reeived from the socket, the messages are sent here to Session. Session is then
 *  responsible for parsing the message and forwarding it on to the handleMessage() method which
 *  will finally decide how to process and the message and call any forms or methods
 *  responsible for updating the user interface.
 * @Author: Joe E. Crew
 * @bugs: TBD
 * @addtogroup thinClientMessagingCsu
*/

#ifndef SESSION_H
#define SESSION_H

#include <QAbstractSocket>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QVector>

class QIODevice;
class ScaleConfiguration;
class UiUtilities;

typedef QVector<QString> SessionMessage;

class Session : public QObject
{
    Q_OBJECT

    typedef Session _Myt;
    typedef QObject _Mybase;

signals:
    void updateUserInterface( const QString& msgType, const QString& text );
    void updateDataStructure( const SessionMessage& victor );

    void statusBarFinished();                                       // 109,6
    void updateStatusBar();                                         // 109,7

    void receive( const SessionMessage& msg );
    void receiveErrorMessage( const SessionMessage& msg );
    void receiveLockDownScreenMessage( bool isLocked );

    void disconnected();

public:
    Session( QIODevice *device );
    virtual ~Session();

    /**
     * Flashkey types.
     */
    enum FlashkeyType
    {
        // these widgets operator may flip between
        Empty = 0x0001,                                 ///< Empty area with nothing shown.
        NumberPad = 0x0002,                             ///< Fixed (non-popup) number pad.
        Flashkeys = 0x0004,                             ///< Flashkeys widget.
        WebBrowser = 0x0008,                            ///< Web browser widget.
        ProductImage = 0x0010,                          ///< Product image.
        BestSellerList = 0x0020,                        ///< Best sellers.
        RunTotals = 0x0040,                             ///< Run totals for prepack.
        FixedFlashkeys = 0x80,                          ///< PJR-383: Fixed Flash Keys

        // special widget which are shown/hidden programmatically
        AccumulateItems = 0x0100,                       ///< Accumulate items widget.
        FreshProduction = 0x0200,                       ///< Fresh production planning.
        OperatorMessage = 0x0400,                       ///< Operator message.
        TareFlashkeys = 0x0800,                         ///< Tare flashkeys.
        CutTestFlashkeysDefault = 0x1000,               ///< Cutting test flashkeys for the original cut test.
        CutTestFlashkeysOptionOne = 0x2000,             ///< Cutting test flashkeys for the new Option One Cut Test
    };

    virtual void handleMessage( const SessionMessage& victor );                    // formerly known as MADP.update()
    virtual void handleSelfServModeMsg( bool value );

    virtual void send( const SessionMessage& victor );                      // formerly known as Client.send()
    virtual void send( const QString& sessionMsg );                         // formerly known as Client.send()
    virtual void send( int device, int event );                             // formerly known as Client.send() - overridden
    virtual void send( int device, int event, const QString& msgBody );     // formerly known as Client.send() - overridden

    virtual bool isPrepack() const;
    virtual bool isServiceCounter() const;
    virtual bool isSelfServiceMode() const;
    virtual bool isSelfServeAndPrintingToScreen() const;

    ScaleConfiguration *scaleConfig;
    UiUtilities *uiUtilities;

private slots:

    void deviceReadyRead();                                // formerly known as Client.update()

    void deviceError( QAbstractSocket::SocketError );

private:
    void init();
    QString setHeader(int msgSize) const;

    /**
     * The method determines if we are running on LINUX or WINDOWS to set initial directories
     *
     * @param[in] none
     * @returns  @c true if system OS is a LINUX based system, @c else false which indicates Windows
     */
    static bool isLinux();


    QByteArray buffer;

    QMutex readMutex;
    QMutex writeMutex;

    int dataTransferType;

    static const int HEADER_SIZE;
    static const int SOCKET_DATA_UTF8;
    static const int SOCKET_DATA_ANSI;

    QVector<QString> zeros;

    QIODevice *sessionDevice;

};

#endif // SESSION_H
