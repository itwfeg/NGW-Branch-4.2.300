/**
  * @file serviceHealthAgent.h
  * @brief Displays scale health information from a log file.
  *
  * @copyright Copyright (c) 2013-2016, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @author Randy Blankley
  */

#ifndef SERVICEHEALTHAGENT_H
#define SERVICEHEALTHAGENT_H

#include "screenwidget.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QTabWidget;
class QTimer;
class QTreeWidget;
class QTreeWidgetItem;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Displays scale health information from a log file.
/**
 * Log file name is "healthlog.txt" and is located:
 *      Linux: /usr/local/hobart
 *      Windows: C:\\Program Files\\Hobart\\Access Scale
 */
class serviceHealthAgent : public ScreenWidget
{
    Q_OBJECT

    typedef serviceHealthAgent _Myt;
    typedef ScreenWidget _Mybase;
    
public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit serviceHealthAgent( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~serviceHealthAgent();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

protected Q_SLOTS:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Clear log.
    void clearLog();

    /// Refresh log.
    void refreshLog();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Event filter.
    /**
     * @param[in,out] obj  object being filtered
     * @param[in,out] e  event
     * @return  @c true to filter event, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Cancel printing.
    virtual void cancelPrinting();

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

    /// Request audit logs.
    virtual void requestAuditLogs();

    /// Request diagnostics status.
    virtual void requestDiagStatus();

    /// Request maint log.
    virtual void requestMaintLog();

    /// Translate print audit button.
    virtual void translatePrintAuditButton();

    /// Translate print maint log button.
    virtual void translatePrintMaintLogButton();

private Q_SLOTS:

    /// Slot for push button clicked.
    void onPushButtonClicked();

    /// Slot for timer timeout.
    void onTimerTimeout();

    /// Slot for tree selection changed.
    void onTreeSelectionChanged();

private:

    enum DiagTableColumns
    {
        DiagId = 0,
        DiagDate,
        DiagErrorCode,
        DiagOccurrences,
        DiagLastOccurrence,
        DiagClearButton,
        DiagDiagnoseButton
    };

    enum AuditTableColumns
    {
        AuditId = 0,
        AuditDate,
        AuditSeverity,
        AuditSource,
        AuditSourceExt,
    };

    QTimer *requestDiagStatus_;

    bool emptyLog_;
    bool serviceMode_;
    bool valueMaxWeigher_;

    bool configs_;

    bool printing_;
    bool printingDiagReport_;
    bool printingAuditReport_;

    QStringList trays_;

    int upperRollWidth_;
    int lowerRollWidth_;
    bool singleRoll_;
    bool doubleRoll_;

    QTabWidget *tab_;
    int diagMaintLogTab_;
    int diagConfigTab_;
    int healthLogTab_;
    int auditLogTab_;

    //
    // diag maint log
    //

    QWidget *diagMaintLog_;
    QTreeWidget *diagTable_;

    QFont diagTableFont_;

    QPushButton *printMaintLog_;
    QPushButton *exportDiagLog_;
    QPushButton *emailDiagLog_;
    QPushButton *uploadDiagLog_;

    //
    // diag config
    //

    QWidget *diagConfig_;

    QLabel *serviceContactLabel_;
    QLineEdit *serviceContact_;

    QLabel *gatewayAddressLabel_;
    QLineEdit *gatewayAddress_;

    QLabel *gatewayLogLevelLabel_;
    QComboBox *gatewayLogLevel_;

    QCheckBox *disableValueMaxDiagCollection_;

    QLabel *gatewayStatus_;

    //
    // health log
    //

    QWidget *healthLog_;
    QLabel *heading_;
    QPushButton *clearLog_;
    QPushButton *exportLog_;
    QPlainTextEdit *log_;

    //
    // audit log
    //

    QWidget *auditLog_;
    QTreeWidget *auditTable_;

    QPlainTextEdit *auditLogDetails_;

    QFont auditTableFont_;

    QPushButton *prevAuditLog_;
    QPushButton *nextAuditLog_;
    QPushButton *startServiceVisit_;
    QPushButton *stopServiceVisit_;
    QPushButton *printAuditLog_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Create layout.
    void calcDiagMaintTableColumnWidths();

    /// Create layout.
    void calcDiagMaintTableFontSizes( QTreeWidgetItem *item ) const;

    /// Update diagnostics configuration.
    void updateDiagConfig();

    /// Create layout.
    void calcAuditTableColumnWidths();

    /// Create layout.
    void calcAuditTableFontSizes( QTreeWidgetItem *item ) const;

    /// Request settings.
    void requestServiceSettings();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SERVICEHEALTHAGENT_H
