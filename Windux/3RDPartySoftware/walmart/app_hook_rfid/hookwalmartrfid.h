/**
 * @file hookwalmartrfid.h
 * Walmart RFID Hook Interface.
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOOKWALMARTRFID_H
#define HOOKWALMARTRFID_H

#include <QMutex>
#include <QObject>

class QSqlDatabase;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Walmart RFID Hook Interface.
class walmartHookRFID : public QObject
{
    Q_OBJECT

    typedef walmartHookRFID _Myt;
    typedef QObject _Mybase;

public:

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Initialize hook.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    bool initialize();

    /// Finalize hook.
    void finalize();

    /// Config saved hook.
    void configSaved();

    /// Initialize database name attribute.
    /**
     * @param[in] dbname  database name
     */
    void beforeOpenDatabase( const QString& dbname );

    /// Open the database.
    void openDatabase();

    /// Read PLU record hook.
    /**
     * @param[in] plu  plu number
     * @param[in] deptNum  department number
     * @return  @c true to have chobart_d use the plu record in the scale's database, @c false otherwise
     */
    bool beforePluRead( const QString& plu, const QString& deptNum );

    /// Read PLU Text record hook.
    /**
     * @param[in] number  text number
     * @param[in] deptNum  department number
     * @param[in] type  text type
     * @return  @c true to continue, @c false otherwise
     */
    bool beforePluTextRead( const QString& number, const QString& deptNum, int type );

    /// Print button hook.
    /**
     * @param[in] plu  plu number
     * @param[in] deptNum  department number
     * @param[in] isRemotePrint  @c true if remote printing, @c false otherwise
     * @return  @c true to continue, @c false otherwise
     */
    bool beforePrint( const QString& plu,  const QString& deptNum, bool isRemotePrint = false );

    /// After print hook.
    /**
     * @param[in] success  @c true if print was successful, @c false otherwise
     * @param[in] numLabelsToPrint  number of labels to print
     * @param[in] numLabelsPrinted  number of labels printed
     * @param[in] templateLabel  @c true if template (cake) label, @c false otherwise
     */
    void afterPrint( bool success, int numLabelsToPrint, int numLabelsPrinted, bool templateLabel = false );

    /// Perform custom hook action.
    /**
     * @param[in] commandString  action to perform
     * @param[out] result  action result
     * @param[out] resultExtended  action result extended
     */
    void customAction( const QString& commandString, int& result, QString& resultExtended );

    /// Update periphal device version info.
    /**
     * @param[in] device  device
     * @param[in] deviceEx  extended device
     * @param[in] kind  device kind
     * @param[in] swVer  software version
     * @param[in] hwVer  hardware version
     */
    void updatePeripheralVersion( int device, int deviceEx, int kind, const QString& swVer, const QString& hwVer );

    /// Update scale state.
    /**
     * @param[in] type  scale state
     * @param[in] value  value
     */
    void updateScaleState( int type, const QString& value );

    // ------------------------------------------------------------------------
    // Static Methods
    // ------------------------------------------------------------------------

    /// Retrieve global instance.
    /**
     * @return  pointer to instance
     */
    static _Myt *instance();

    /// Destroy global instance.
    static void destroy();

protected:

    mutable QMutex m_;                              ///< Mutex for access control.

    QString dbname_;                                ///< Database name.

    QString printingPlu_;                           ///< PLU for printing.
    QString printingDeptNum_;                       ///< Dept Number for printing.
    bool printingIsRemotePrint_;                    ///< Printing PLU is remote print.

    QString serverAddr_;                            ///< Server Address.
    quint16 serverPort_;                            ///< Server Port.

    QString format_;                                ///< RFID format.

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Close database.
    void closeDatabase();

    /// Read configuration.
    void readConfiguration();

private:

    enum {TRANS_SECS_MAX = 180};

    enum {SOCKET_TIMEOUT = 2500};

    static QMutex instanceMutex_;
    static _Myt *instance_;

    int prevTrans_;

    /// Constructor.
    walmartHookRFID( QObject *parent = nullptr );

    /// Destructor.
    ~walmartHookRFID();

    /// Generate RFID Label Data.
    QByteArray generateRFID( QSqlDatabase& db );

    /// Write RFID Label Data.
    void writeRFID( const QByteArray& a ) const;

    /// Database connection name.
    static QString connectionName();

    // not implemented
    walmartHookRFID( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOOKWALMARTRFID_H
