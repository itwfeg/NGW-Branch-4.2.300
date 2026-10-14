#ifndef WRAPPERPACKETDIALOG_H
#define WRAPPERPACKETDIALOG_H

#include <Util/Dict.h>

#include <QDialog>
#include <QMap>

class DiagnosticsEdgeDevice;

class edgeDeviceCommThread;

class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Wrapper packet simulation dialog.
class wrapperPacketDialog : public QDialog
{
    Q_OBJECT

    typedef wrapperPacketDialog _Myt;
    typedef QDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    wrapperPacketDialog( QWidget *parent = 0 );

    /// Destructor.
    virtual ~wrapperPacketDialog();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Shutdown simulator.
    virtual void shutdown();

    /// Startup simulator.
    virtual void startup();

private Q_SLOTS:

    /// Slot for button clicked.
    void onPushButtonClicked();

private:

    DiagnosticsEdgeDevice *edgeDevice_;
    edgeDeviceCommThread *edgeDeviceComms_;

    QMap<int, util::Dictionary> healthEventMap_;

    // ---- //

    QGroupBox *serverInfo_;

    QLabel *serverAddressLabel_;
    QLineEdit *serverAddress_;

    QLabel *serialLabel_;
    QLineEdit *serial_;

    QLabel *modelLabel_;
    QLineEdit *model_;

    QLabel *addressLabel_;
    QLineEdit *address_;

    QLabel *hostnameLabel_;
    QLineEdit *hostname_;

    QCheckBox *wrapper_;
    QCheckBox *weigher_;
    QCheckBox *printer1_;
    QCheckBox *printer2_;
    QCheckBox *printer3_;
    QCheckBox *errors_;

    QPushButton *startStopClient_;

    // ---- //

    QGroupBox *statusEvent_;

    QLabel *statusEventErrorCodeLabel_;
    QLineEdit *statusEventErrorCode_;

    QLabel *statusEventSeverityLabel_;
    QLineEdit *statusEventSeverity_;

    QCheckBox *diagOffered_;
    QCheckBox *diagForced_;

    QPushButton *sendStatusEvent_;

    // ---- //

    QGroupBox *healthEvent_;

    QLabel *healthEventErrorCodeLabel_;
    QLineEdit *healthEventErrorCode_;

    QLabel *healthEventSeverityLabel_;
    QLineEdit *healthEventSeverity_;

    QLabel *healthEventMethodsLabel_;
    QLineEdit *healthEventMethod1_;
    QLineEdit *healthEventMethod2_;
    QLineEdit *healthEventMethod3_;
    QLineEdit *healthEventMethod4_;

    QLabel *healthEventPartsLabel_;
    QLineEdit *healthEventPart1_;
    QLineEdit *healthEventPart2_;
    QLineEdit *healthEventPart3_;
    QLineEdit *healthEventPart4_;

    QLabel *healthEventNotesLabel_;
    QLineEdit *healthEventNotes_;

    QCheckBox *generatedFromDiag_;
    QCheckBox *resolved_;

    QPushButton *sendHealthEvent_;

    QWidgetList resolutionWidgets_;

    // ---- //

    QGroupBox *diagEvent_;

    QLabel *diagEventErrorCodeLabel_;
    QLineEdit *diagEventErrorCode_;

    QLabel *diagEventResultCodeLabel_;
    QLineEdit *diagEventResultCode_;

    QLabel *diagEventRoutineLabel_;
    QLineEdit *diagEventRoutine_;

    QLabel *diagEventExpectedLabel_;
    QLineEdit *diagEventExpected_;

    QLabel *diagEventActualLabel_;
    QLineEdit *diagEventActual_;

    QCheckBox *passed_;
    QCheckBox *failed_;
    QCheckBox *timeout_;
    QCheckBox *byOperator_;
    QCheckBox *byService_;

    QPushButton *sendDiagEvent_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Send health events.
    void sendHealthEvents();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // WRAPPERPACKETDIALOG_H
