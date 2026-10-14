/**
 * @file printersettingsdialog.h
 * Dialog for adjusting printer settings.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef PRINTERSETTINGSDIALOG_H
#define PRINTERSETTINGSDIALOG_H

#include "../sessiondialog.h"

#include <Messages.h>

#include <QFrame>
#include <QVector>

class PrinterSettingsPreviewWidget;

class QAbstractButton;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSignalMapper;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog for adjusting printer settings.
class PrinterSettingsDialog : public SessionDialog
{
    Q_OBJECT

    typedef PrinterSettingsDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit PrinterSettingsDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~PrinterSettingsDialog();

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve selected printer.
    /**
     * @return  selected printer
     */
    virtual int selectedPrinter() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Disable interface.
    virtual void disableInterface();

    /// Callback for session messages.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Check if printer type is supported.
    /**
     * @param[in] type  printer type
     * @return  @c true if supported, @c false otherwise
     */
    static bool isSupportedPrinterType( int type );

private Q_SLOTS:

    /// Slot for button click.
    void onButtonClicked( QWidget *button );

    /// Slot for printer changed.
    void onCurrentPrinterChanged( int index );

    /// Slot for field changed.
    void onFieldChanged();

private:

    typedef QVector<DomPrinterConfigRead> printerConfigMap;
    typedef QVector<DomPrinterStockMetrics> printerStockMetricsMap;

    PrinterSettingsPreviewWidget *preview_;

    QLabel *currentPrinterLabel_;
    QComboBox *currentPrinter_;

    QLabel *contrastLabel_;
    QComboBox *contrast_;

    QLabel *labelWidthLabel_;
    QComboBox *labelWidth_;

    QLabel *expelPositionLabel_;
    QLineEdit *expelPosition_;

    QLabel *horizontalPrintPositionLabel_;
    QLineEdit *horizontalPrintPosition_;

    QLabel *verticalPrintPositionLabel_;
    QLineEdit *verticalPrintPosition_;

    QPushButton *applyAndPrint_;
    QPushButton *ejectAndCut_;
    QPushButton *down_;
    QPushButton *left_;
    QPushButton *right_;
    QPushButton *up_;

    QSignalMapper *buttons_;

    bool dirty_;
    bool ignoreSignals_;
    bool cutterEnabled_;

    printerConfigMap printerConfig_;
    printerStockMetricsMap printerStockMetrics_;
    DomPrinterCutterStatus printerCutterStatus_;
    int activePrinter_;

    static const bool convertUpperCase_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Check for EPP printer.
    bool isEPP() const;

    /// Update printer config.
    void updatePrinterConfig();

    /// Update printer stock metrics.
    void updatePrinterStockMetrics();

    /// Update printer cutter status.
    void updatePrinterCutterStatus();

    /// Save printer configuration.
    void savePrinterConfig();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for previewing printer settings.
class PrinterSettingsPreviewWidget : public QFrame
{
    Q_OBJECT

    typedef PrinterSettingsPreviewWidget _Myt;
    typedef QFrame _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit PrinterSettingsPreviewWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~PrinterSettingsPreviewWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set the printer configuration.
    /**
     * @param[in] m  printer config
     */
    virtual void setPrinterConfig( const DomPrinterConfigRead& m );

    /// Set the printer stock metrics.
    /**
     * @param[in] m  printer stock metrics
     */
    virtual void setPrinterStockMetrics( const DomPrinterStockMetrics& m );

    virtual void setPrinterCutterStatus( const DomPrinterCutterStatus& m );

protected:

    /// Paint event.
    /**
     * @param[in,out] e  paint event
     */
    virtual void paintEvent( QPaintEvent* e );

private:

    DomPrinterConfigRead printerConfig_;
    DomPrinterStockMetrics printerStockMetrics_;
    DomPrinterCutterStatus printerCutterStatus_;

    bool havePrinterConfig_;
    bool havePrinterStockMetrics_;
    bool havePrinterCutterStatus_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // PRINTERSETTINGSDIALOG_H
