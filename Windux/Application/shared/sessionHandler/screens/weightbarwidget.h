/**
 * @file weightbarwidget.h
 * Weight bar widget.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef WEIGHTBARWIDGET_H
#define WEIGHTBARWIDGET_H

#include "screenwidget.h"

class QIntValidator;
class QSignalMapper;

class OperateMode;
class ScaleConfiguration;
class MadpInternalConfig;

class DomCurrentPLU;
class DomWeigherConfigRead;
class DomUpdateRunModeField;
class MadpUpdateCurrentPLUTotals;

namespace hobart {
class labeledLineEdit;
class labeledNumericLineEdit;
class labeledPlainTextEdit;
class labeledUnitPriceEdit;
class labeledUpcEdit;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Areas where the weight bar may be located.
enum WeightBarArea
{
    BottomWeightBarArea,                    ///< Weight bar located on the bottom.
    TopWeightBarArea                        ///< Weight bar located on the top.
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Weight bar display object.
class WeightBarWidget : public ScreenWidget
{
    Q_OBJECT

    Q_PROPERTY( DisplayMode displayMode READ displayMode WRITE setDisplayMode )
    Q_PROPERTY( bool isForcedFields READ isForcedFields )

    typedef WeightBarWidget _Myt;
    typedef ScreenWidget _Mybase;

Q_SIGNALS:

    /// Signal for when the window is moved.
    /**
     * @param[in] where  where the bar was moved to
     */
    void moved( WeightBarArea where );

public:

    /// Weight bar display modes.
    enum DisplayMode
    {
        WeightDisplayMode,                          ///< Display weight information.
        FreqShopperDisplayMode,                     ///< Display frequent shopper information.
        NetWeightOnlyMode,                          ///< Display net weight only.
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     * @param[in] area  area to display weight bar
     */
    explicit WeightBarWidget(
        SessionMainWindow *mainWindow,
        ModeWidget *parent = 0,
        WeightBarArea area = TopWeightBarArea );

    /// Destructor.
    virtual ~WeightBarWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve weight bar area.
    /**
     * @return  weight bar area
     */
    virtual WeightBarArea area() const {return area_;}

    /// Retrieve display mode.
    /**
     * @return  display mode
     */
    virtual DisplayMode displayMode() const {return displayMode_;}

    /// Check for existence of forced fields.
    /**
     * @return  @c trueif forced fields, @c false otherwise
     */
    virtual bool isForcedFields() const;

    /// Retrieve margins.
    /**
     * Some screens add widgets to the top and/or bottom. This property returns how much space
     * the widgets are taking up. This property is useful because we may want to offset the
     * numeric keypad or other widgets.
     *
     * @return  margins
     */
    virtual QMargins margins() const;

    /// Set item by count value.
    /**
     * @param[in] value  by count
     */
    virtual void setByCount( const QString& value );

    /// Set display mode.
    /**
     * @param[in] value  display mode
     */
    virtual void setDisplayMode( DisplayMode value );

    /// Clears the Product Totals fields between PLU requests
    /**
    * @param[in] none
    * @return[out] none
    */
    virtual void clearProductTotals();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Process forced input fields.
    virtual void processForcedFields();

    /// Translate the user interface to current language.
    virtual void translate();

    // SOF-2806: Used to set the focus on the Tare field
    virtual void setTareFocus();

public Q_SLOTS:

    /// Slot to set the weight string.
    /**
     * @param[in] netWeight  net weight
     * @param[in] dualRangeIndicator  dual range indicator
     */
    virtual void setWeightText( const QString& netWeight, const QString& dualRangeIndicator );

    /// Slot to set the weight string if tilt max is exceeded.
    virtual void setOutOfTilt();

    /// Slot to set the weigher configuration.
    virtual void setWeigherConfig();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Mouse move event.
    /**
     * @param[in,out] e  mouse event
     */
    virtual void mouseMoveEvent( QMouseEvent *e );

    /// Mouse press event.
    /**
     * @param[in,out] e  mouse event
     */
    virtual void mousePressEvent( QMouseEvent *e );

    /// Mouse release event.
    /**
     * @param[in,out] e  mouse event
     */
    virtual void mouseReleaseEvent( QMouseEvent *e );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot to handle by count focus.
    void onByCountFocused( bool hasFocus );

    /**
     * Slot to handle clearing a non meat spec sheet PLU.
     */
    void onClearPluForCutTest();

    /**
     * Slot to handle PLU changed.
     */
    void onCurrentProductChanged( bool nof );

    /// Slot to handle PLU totals changed.
    void onCurrentProductTotalsChanged();

    /// Slot to handle PLU field updates.
    void onCurrentProductUpdateField( const DomUpdateRunModeField& f );

    /// Slot to handle manual entry of field.
    void onEditingFinished( QWidget *field );

    /// Slot to handle fixed weight focus.
    void onFixedWeightFocused( bool hasFocus );

    /// Slot to handle invalid access.
    void onInvalidAccessLevel( QWidget *field );

    /// Slot to handle price focus.
    void onPriceFocused( bool hasFocus );

    /// Slot to handle tare focus.
    void onTareFocused( bool hasFocus );

    /// Slot to handle unit price focus.
    void onUnitPriceFocused( bool hasFocus );

    /// Slot to handle unit price fractional change.
    void onUnitPriceFractionalChanged( int value );

    /// Slot to handle unit price pounds for change.
    void onUnitPriceWeightForChanged( int value );

    /// Slot to handle upc prefix changing.
    void onUpcPrefixChanged( int prefix );

    /// Slot to handle volume focus.
    void onVolumeFocused( bool hasFocus );

private:

    MadpInternalConfig *internalConfigData;

    QSignalMapper *editingFinishedSignal_;
    QSignalMapper *invalidAccessLevelSignal_;

    hobart::labeledNumericLineEdit *weight_;                // random weight (lb/kg from scale)
    QIntValidator *weightValidator_;

    int weightTop_;
    int weightPrecision_;
    int weightStep_;

    int dualRangeWeightTop_;
    int dualRangeWeightPrecision_;
    int dualRangeWeightStep_;

    hobart::labeledNumericLineEdit *tare_;                  // tare (lb/kg)
    QIntValidator *tareValidator_;

    hobart::labeledNumericLineEdit *propTare_;
    hobart::labeledNumericLineEdit *propBlue_;

    // max tare from normal scale operation
    QString maximumTare_;

    hobart::labeledUnitPriceEdit *unitPrice_;               // unit price
    QIntValidator *unitPriceValidator_;

    hobart::labeledNumericLineEdit *price_;                 // total price
    QIntValidator *priceValidator_;

    hobart::labeledNumericLineEdit *fixedWeight_;           // fixed weight (oz/g)
    QIntValidator *fixedWeightValidator_;

    hobart::labeledNumericLineEdit *volume_;                // volume (fl oz/ml)
    QIntValidator *volumeValidator_;

    hobart::labeledPlainTextEdit *displayWeight_;           // display weight (for fixed weight and volume)

    hobart::labeledNumericLineEdit *byCount_;               // by count
    hobart::labeledNumericLineEdit *byCountHalfie_;         // by count
    QIntValidator *byCountValidator_;

    hobart::labeledNumericLineEdit *exceptionPrice_;        // exception price (for by count items with count > 1)

    hobart::labeledUpcEdit *upc_;                           // upc/ean code
    QIntValidator *upcValidator_;

    hobart::labeledNumericLineEdit *discountUnitPrice_;     // discount unit price
    QIntValidator *discountUnitPriceValidator_;

    hobart::labeledNumericLineEdit *discountPrice_;         // discount total price
    QIntValidator *discountPriceValidator_;

    hobart::labeledNumericLineEdit *savings_;               // savings
    QIntValidator *savingsValidator_;

    hobart::labeledLineEdit *weightEmpty_;                  // empty weight field

    hobart::labeledLineEdit *tareEmpty_;                    // empty tare field

    hobart::labeledLineEdit *unitPriceEmpty_;               // empty unit price

    hobart::labeledLineEdit *priceEmpty_;                   // empty total price

    hobart::labeledLineEdit *exceptionEmpty_;              // exception price empty

    hobart::labeledLineEdit *byCountUnitPrice_;

    QList<QWidget*> widgets_;

    WeightBarArea area_;
    DisplayMode displayMode_;

    OperateMode *operateMode_;
    ScaleConfiguration *config_;

    QString curSeparator_;

    bool ignoreSignals_;
    bool clearingFields_;

    bool tareFlashkeysExist_;
    bool isKeyboardNetWeightItem_;

    QPoint position_;
    bool moving_;
    bool pick5PriceFieldEnabled_;
    bool pick5WeightFieldEnabled_;

    bool valuemaxIsEnabled_;        // is this a valuemax enabled scale
    bool scaleWentOutOfTilt_;       // indicates we have gone out of tilt and we need to reset ourselves

    static const bool convertUppercase_;
    static const bool moveable_;

    /// Initialize.
    void initialize();

    /// Create Layout.
    void createLayout();

    /// Clear fields.
    void clearFields();

    /// Update programmable fields.
    void updateProgrammableFields();

    /// Update weight and tare field visibility.
    void updateWeightTareFields();

    /// Update the weight field if too far out of tilt
    void updateWeightOutOfTilt(bool hideIt);

    /// Update forced fields.
    void updateForcedFields();

    /// Update permissions of field.
    template <class T>
    void setPermissions( T *field, bool canModify, int accessLevel );

    /// Update field permissions.
    void updateFieldPermissions();

    /// Update UPC field.
    QString updateUpcNumberField();

    /// Update display weight field.
    void updateDisplayWeightField( const QString& newValue );

    /// Update unit price field.
    void updateDisplayUnitPriceField( const QString& newValue ); // FT-472

    /// Retrieve current product.
    DomCurrentPLU *currentProduct() const;

    /// Retrieve current product totals.
    MadpUpdateCurrentPLUTotals *currentProductTotals() const;

    /// Update fields settings from currency settings.
    void updateCurrencySettings();

    /// Update fields settings from scale config.
    void updateScaleConfig();

    /// Update Total Price field background color.
    void updateTotalPriceBackgroundColor();

    /// Update tare field settings from scale config and weight field.
    void updateTareDigits();

    /// Set scale units.
    void setScaleUnits();

    /// Set scale precision.
    void setScalePrecision( bool lowRange );

    /// Hide operator message (i.e. in the dca area).
    void hideOperatorMessage();

    /// Show operator message (i.e. in the dca area).
    void showOperatorMessage( int type, const QString& message );

    /// Need to update the weight in the dca if weight changes
    void updateFreshProductionPlanningWeight(const QString &weight);

    /// Make validator top from digits.
    static int makeValidatorTop( int digits );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // WEIGHTBARWIDGET_H
