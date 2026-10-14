/**
 * @file plumainscreen.h
 * Main screen for plu entry.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef PLUMAINSCREEN_H
#define PLUMAINSCREEN_H

#include "screenwidget.h"

class QIntValidator;
class QSignalMapper;
class QTimer;
class QWebView;

class OperateMode;

class DomCurrentPLU;
class DomUpdateRunModeField;
class MadpUpdateCurrentPLUTotals;

namespace hobart
{
class dynamicContentWidget;
class labeledCoolEdit;
class labeledDateEdit;
class labeledDoubleLineEdit;
class labeledNumericLineEdit;
class labeledPlainTextEdit;
class labeledPluEdit;
class webViewer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Main screen for plu entry.
class PluMainScreen : public ScreenWidget
{
    Q_OBJECT

    Q_PROPERTY( QString currentItemNumber READ currentItemNumber WRITE setCurrentItemNumber )
    Q_PROPERTY( bool isForcedFields READ isForcedFields )
    Q_PROPERTY( bool isPopupNumberPad READ isPopupNumberPad )

    typedef PluMainScreen _Myt;
    typedef ScreenWidget _Mybase;

public:
void sigtest();

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit PluMainScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~PluMainScreen();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve current item number.
    /**
     * @return  item number
     */
    virtual QString currentItemNumber() const {return currentItemNumber_;}

    /// Retrieve dynamic content widget geometry.
    /**
     * @param[in] index  which dca widget
     * @return  geometry
     */
    virtual QRect dcaGeometry( int index = 1 ) const;

    /// Retrieve list of visible and editable features.
    /**
     * @return  list of features
     */
    virtual QStringList editableFeatures() const;

    /// Check for existence of forced fields.
    /**
     * @return  @c true if forced fields, @c false otherwise
     */
    virtual bool isForcedFields() const;

    /// Check for existence of forced tray entry.
    /**
     * @return  @c true if forced tray entry, @c false otherwise
     */
    virtual bool isForcedTrayEntry() const {return traySizesRequired_;}

    /// Check if the number pas is popup or static.
    /**
     * The popup number pad will show and hide depending on the input field focus. When the number
     * pad is not popup it is static, and will always be visible even when no field has focus.
     *
     * @return  @c true when popup number pad, @c false otherwise
     */
    virtual bool isPopupNumberPad() const;

    /// Check if plu field has focus.
    /**
     * @return  @c true if plu field has focus, @c false otherwise
     */
    virtual bool pluHasFocus() const;

    /// Set current item number.
    /**
     * @param[in] value  item number
     */
    virtual void setCurrentItemNumber( const QString& value );

    /// Show/Hide url popup dialog.
    /**
     * @param[in] value  @c true to show popup, @c false to hide
     * @param[in] url  url to display
     */
    virtual void setOnlineOrderingProductUrl( const QString& url );

    /// Set forced tray entry.
    /**
     * @param[in] value  @c true if forced, @c false otherwise
     */
    virtual void setForcedTrayEntry( bool value );

    /// Set tray name.
    /**
     * @param[in] name  name
     */
    virtual void setTrayName( const QString& name );


    // ========================================================================
    // Methods
    // ========================================================================

    /// Process forced input fields.
    virtual void processForcedFields();

    /// Check for expiration label forced input field.
    virtual bool checkExpirationDaysForced();

    /// Check for production entry forced input field.
    virtual bool checkProductionEntryForced();

    /// Reload web viewer pages.
    virtual void reloadWebViewers();

    /// Reset flashkeys to default (initial) group.
    /**
     * @param[in] force  @c true to force reset all flashkey banks, @c false otherwise
     */
    virtual void resetFlashkeys( bool force = false );

    /// Enables or disables the plu field.
    virtual void setPluDisabled(bool disable = false);

    /**
     * Set the flashkey set widget
     */
    virtual void setSpecialFlasheyWidget(bool value);

    /// send Online Order Totals to the webViewer so it can send them to the blackbox/server
    //virtual void sendOnlineOrderTotals( );

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public Q_SLOTS:
    /// Hide accumulate items (i.e. in the dca area).
    virtual void hideAccumulateItems();

    /// Show accumulate items (i.e. in the dca area).
    virtual void showAccumulateItems();

    /// Hide operator message (i.e. in the dca area).
    virtual void hideOperatorMessage();

    /// Sets focus to the plu field.
    virtual void setFocusPluEntry();

    /// Show operator message (i.e. in the dca area).
    /**
     * @param[in] type  message type
     * @param[in] message  message text
     * @param[in] allowCancelProdEntry  allow cancellation of production entry
     */
    virtual void showOperatorMessage( int type, const QString& message, bool allowCancelProdEntry = false );

    /// Show weight update (i.e. in the dca area).
    /**
     * @param[in] type  message type
     * @param[in] message  message text
     */
    virtual void updateProductionPlanningMessage(const QString &weight);

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Show event.
    /**
     * @param[in,out] e  show event
     */
    virtual void showEvent( QShowEvent *e );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if enhanced run screen is active.
    /**
     * @return  @c true if enhanced screen is active, @c false otherwise
     */
    virtual bool isEnhancedCenterScreenActive() const;

    /// Compute timestamp with added days and hours.
    /**
     * @param[in] addDays  days to add
     * @param[in] addHours  hours to add
     * @return  timestamp
     */
    virtual QDate retrieveTimestamp( int addDays, int addHours = 0 ) const;

    /// Compute timestamp with added days and hours - Using baseDate provided instead of current date
    /**
     * @param[in] baseDate
     * @param[in] addDays  days to add
     * @param[in] addHours  hours to add
     * @return  timestamp
     */
    // FT-564
    virtual QDate retrieveTimestamp( const QDate& baseDate, int addDays, int addHours = 0 ) const;

    /// Screen that gets displayed when swiping (gesturing) the screen down.
    /**
     * @return  next screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenDown() const;

    /// Screen that gets displayed when swiping (gesturing) the screen left.
    /**
     * @return  next screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenLeft() const;

    /// Screen that gets displayed when swiping (gesturing) the screen right.
    /**
     * @return  previous screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenRight() const;

    /// Screen that gets displayed when swiping (gesturing) the screen up.
    /**
     * @return  next screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenUp() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Event filter.
    /**
     * @param[in] obj  object being filtered
     * @param[int e  event
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

    /// Handle screen changing.
    /**
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @return  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:
    /// Slot for best before date changed.
    void onBestBeforeDateChanged();

    /// Slot for best before date do not print changed.
    void onBestBeforeDateDoNotPrintChanged();

    /// Slot to handle COOL changed.
    void onCoolChanged();

    /// Slot for cool dialog action complete.
    void onCoolDialogActionComplete( int rc );

    /// Slot to handle COOL tracking number changed.
    void onCoolTrackingNumberChanged();

    /**
     * Slot to handle clearing a non meat spec sheet PLU.
     */
    void onClearPluForCutTest();

    /**
     * Slot to handle PLU changed.
     */
    void onCurrentProductChanged( bool nof );

    /// Slot to handle PLU field updates.
    void onCurrentProductUpdateField( const DomUpdateRunModeField& f );

    /// Slot to handle when cutting test is entered or exited.
    void onCuttingTestChanged( bool value );

    /// Slot to handle invalid access.
    void onInvalidAccessLevel( QWidget *field );

    /// Slot to handle number pad key layout change.
    void onNumberPadKeyLayoutStyleChanged();

    /// Slot to handle number pad moved.
    void onNumberPadMoved( Qt::DockWidgetArea where );

    /// Slot for packed on date changed.
    void onPackedOnDateChanged();

    /// Slot for packed on date dialog action complete.
    void onPackedOnDateDialogActionComplete( int rc );

    /// Slot for packed on date do not print changed.
    void onPackedOnDateDoNotPrintChanged();

    /// Slot to handle plu entry.
    void onPluEditingFinished();

    /// Slot to handle plu return pressed.
    void onPluReturnPressed();

    /// Slot to handle plu text editing.
    void onPluTextEdited( const QString& text );

    /// Slot for when screen changes.
    void onScreenChanged();

    /// Slot for sell by date changed.
    void onSellByDateChanged();

    /// Slot for sell by date do not print changed.
    void onSellByDateDoNotPrintChanged();

    /// Slot for lock/unlock of widget settings.
    void onWidgetSettingsLockChanged( bool locked );

    /// Slot for web viewer load finished.
    void onWebViewLoadFinished( bool success );

    /// Slot for web viewer load progress.
    void onWebViewLoadProgress( int progress );

private:

    typedef QMap<int, QString> coolTextMap;

    QSignalMapper *invalidAccessLevelSignal_;

    hobart::labeledPluEdit *plu_;				//	FT-1466
    hobart::labeledPlainTextEdit *description_;
    hobart::labeledDateEdit *sellByDate_;
    hobart::labeledCoolEdit *cool_;
    hobart::labeledDateEdit *bestBeforeDate_;
    hobart::labeledDateEdit *packedOnDate_;

    //hobart::labeledPlainTextEdit *tof_;
    //hobart::labeledPlainTextEdit *tof2_;

    hobart::dynamicContentWidget *dcaPrimary_;
    hobart::dynamicContentWidget *dcaSecondary_;
    hobart::dynamicContentWidget *dcaEnhancedCenterScreen_;

    QWebView *dcaPrimaryWebView_;
    QWebView *dcaSecondaryWebView_;
    QWebView *dcaEnhancedCenterScreenWebView_;

    QTimer *webViewFocusTimer_;                     // SOF-3750

    int dcaPrimaryWebViewProgress_;
    int dcaSecondaryWebViewProgress_;
    int dcaEnhancedCenterScreenWebViewProgress_;

    QTimer *dcaEnhancedCenterScreenReload_;

    QIntValidator *pluValidator_;

    QString currentItemNumber_;
    coolTextMap coolText_;
    bool originalCoolRequired_;

    hobart::webViewer *webView_;
    QString homeUrl_;

    bool forceCoolDialog_;
    bool forcePackedOnDateDialog_;
    bool forceNumberOfLabelsDialog_;
    bool forceNetWeightDialog_;
    bool forceExpirationDaysDialog_;
    bool forceTraySizeDialog_;
    bool hasRemoteLic;

    bool traySizesRequired_;
    bool traySizesExist_;

    OperateMode *operateMode_;

    bool ignoreSignals_;
    bool registered_;

    bool printBeforeNumOfLabelsEntered_;
    bool networkOnline_;

    static const bool convertUppercase_;

    static const int pluValidatorBottom_;
    static const int pluValidatorTop_;

    static const int dcaPrimaryWidgets_;
    static const int dcaPrimaryWidgetsExcluded_;
    static const int dcaPrimarySoftkeyGroup_;

    static const int dcaSecondaryWidgets_;
    static const int dcaSecondaryWidgetsExcluded_;
    static const int dcaSecondarySoftkeyGroup_;

    static const int dcaEnhancedCenterScreenWidgets_;
    static const int dcaEnhancedCenterScreenWidgetsExcluded_;
    static const int dcaEnhancedCenterScreenSoftkeyGroup_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Update layout.
    void updateLayout( Qt::DockWidgetArea where );

    /// Clear fields.
    void clearFields();

    /// Update forced fields.
    void updateForcedFields();

    /// Retrieve current product.
    DomCurrentPLU *currentProduct() const;

    /// Update plu entry validation from scale config.
    void setPluAutoLookup();

    /// Show/hide operator fields from configuration.
    void setOperatorDisplayFields();

    /// Enable/disable packed on date.
    void setPackedOnDateReadOnly();

    /// Write screen configuration data.
    void writeConfiguration();

    /// Prompt for number of labels.
    void promptNumberOfLabels();

    /// Prompt for net weight (turkey weigher).
    void promptNetWeight();

    /// Prompt for expiration days (print shelf life label).
    void promptExpirationDays();

    /// Check if widget child of a web view widget.
    bool isWebViewChild( QWidget *obj ) const;

    /// Convert number of digits into a top range integer.
    static int makeIntValidatorTop( int digits );

    /// Retrieve current product totals.
    MadpUpdateCurrentPLUTotals *currentProductTotals() const;

    /// return the row clicked for a flashkey that can toggle in a fixed flashkey feature
    int toggleFixedXCoordinate( int fixedRowOrCol, int defaultBtn );

    /// return the column clicked for a flashkey that can toggle in a fixed flashkey feature
    int toggleFixedYCoordinate( int fixedRowOrCol, int defaultBtn );

    /// return if we are activating self service mode or already in it, ie, n
    bool isSelfServiceModeActive();

	/// resize event
	void resizeEvent(QResizeEvent *event);
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // PLUMAINSCREEN_H
