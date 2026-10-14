/**
 * @file hookedgify.h
 * Edgify Hook Interface.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOOKEDGIFY_H
#define HOOKEDGIFY_H

#include "hookedgifybase.h"

class networkCheckingThread;
class scaleDatabase;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Edgify Hook Interface.
class edgifyHook : public edgifyHookBase
{
    Q_OBJECT

    typedef edgifyHook _Myt;
    typedef edgifyHookBase _Mybase;

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

    QString dbname_;                                ///< Database name.
    scaleDatabase *db_;                             ///< Database.

    QString currentPlu_;                            ///< Current product.
    QString currentDeptNum_;                        ///< Current department number.

    QString serverAddress_;                         /// Server address for hook request.
    quint16 serverPort_;                            /// Server port for hook request.

    QString resultCommand_;                         ///< Command to get result.
    QString resetResultCommand_;                    ///< Command to reset result.
    QString labelLastResultCommand_;                ///< Command to post last label result.
    QString serverSettingsCommand_;                 ///< Command to set server settings.

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Check if hook initialized and ready.
    /**
     * @return  @c true if ready, @c false otherwise
     */
    bool isReady() const;

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Close the database.
    void closeDatabase();

    /// Read configuration.
    void readConfiguration();

    /// Send prediction results to Hobart service layer.
    /**
     * @param[in] result  prediction result
     * @param[in] image  captured image (if applicable)
     * @param[in] items  prediction items
     * @param[in] probs  prediction probabilities
     */
    void sendPredictResults( PredictResult result, const QString& image = QString(), const QStringList& items = QStringList(), const QStringList& probs = QStringList() );

    /// Update last online time.
    void updateLastOnlineTime();

private slots:

    /// Slot for online status changed.
    void onOnlineStatusChanged( bool online, int offlineAmount );

private:

    enum {HOOK_REQUEST_DELAY_TIME = 0};

    /// Scale state information.
    enum ScaleState
    {
        SCALE_BUILD_NUMBER = 0,
        SCALE_MODE,
        SCALE_MODE_VALUE,
        SCALE_MODE_VALUE_DESCRIPTION,
        SCALE_ITEM_ENTRY_TYPE,
        SCALE_LABEL_STOCK_1,
        SCALE_LABEL_STOCK_2,
        SCALE_LABEL_STOCK_3,
        SCALE_WEIGHER_PRECISION,
        SCALE_WEIGHER_CALIB_STAMP,
        SCALE_WEIGHER_CONF_STAMP
    };

    typedef QPair<int, int> dialogStartStopTime;

    static const char *version_;

    static QMutex instanceMutex_;
    static _Myt *instance_;

    networkCheckingThread *net_;

    /// Constructor.
    edgifyHook( QObject *parent = nullptr );

    /// Destructor.
    ~edgifyHook();

    // not implemented
    edgifyHook( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOOKEDGIFY_H
