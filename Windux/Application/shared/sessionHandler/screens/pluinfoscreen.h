/**
 * @file pluinfoscreen.h
 * Plu information screen.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef PLUINFOSCREEN_H
#define PLUINFOSCREEN_H

#include "screenwidget.h"
#include "../widgets/printPrompt.h"

#include "../modes/operatemode.h"

class DomCurrentPLU;
class DomWeighModeTotalType;

class QIntValidator;

namespace hobart {
class labeledDoubleLineEdit;
class labeledDoubleNumericLineEdit;
class labeledDoubleNumericLineEditArray;
class labeledGraphicEdit;
class labeledGraphicEditArray;
class labeledLabelViewer;
class labeledLineEdit;
class labeledNumericLineEdit;
class labeledPlainTextEdit;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Main screen for plu entry.
class PluInfoScreen : public ScreenWidget
{
    Q_OBJECT

    typedef PluInfoScreen _Myt;
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
    explicit PluInfoScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~PluInfoScreen();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Request label stock metrics.
    virtual void requestPrinterStockMetrics();

    /// Reset label data.
    virtual void resetLabelData();

    /// Translate the user interface to current language.
    virtual void translate();

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check for metric scale.
    /**
     * @return  @c true if metric, @c false otherwise
     */
    virtual bool isScaleMetric() const {return scaleIsMetric_;}

    /// Set metric scale.
    /**
     * @param[in] value  @c true for metric, @c false otherwise
     */
    virtual void setScaleMetric( bool value );

    /// Screen that gets displayed when swiping (gesturing) the screen down.
    /**
     * @return  next screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenDown() const;

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

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for additional graphic id editing finished.
    void onAdditionalGraphicIdEditingFinished( int index );

    /// Slot for additional label type editing finished.
    void onAdditionalLabelIdEditingFinished( int index );

    /// Slot to handle allergens text printing.
    void onAllergensPrint();

    /// Slot to handle clearing a non meat spec sheet PLU.
    void onClearPluForCutTest();

    /// Slot to handle PLU changed.
    void onCurrentProductChanged( bool nof );

    /// Slot to handle PLU support changed.
    void onCurrentProductSupportChanged( int type );

    /// Slot to handle PLU field updates.
    void onCurrentProductUpdateField( const DomUpdateRunModeField& f );

    /// Slot to handle expanded text printing.
    void onExpandedTextPrint();

    /// Slot to handle invalid access.
    void onInvalidAccessLevel( QWidget *field );

    /// Slot to handle number pad moved.
    void onNumberPadMoved( Qt::DockWidgetArea where );

    /// Slot to handle nutrifacts printing.
    void onNutrifactsPrint();

    /// Slot to handle nutrifacts zoom.
    void onNutrifactsZoom();

    /// Slot for primary graphic id editing finished.
    void onPrimaryGraphicIdEditingFinished();

    /// Slot for primary label type editing finished.
    void onPrimaryLabelIdEditingFinished();

    /// Slot for product life editing finished.
    void onProductLifeEditingFinished();

    /// Slot to handle recipe printing.
    void onRecipePrint();

    /// Slot for when screen changes.
    void onScreenChanged();

    /// Slot for shelf life editing finished.
    void onShelfLifeEditingFinished();

    /// Slot to handle cooking instructions printing.
    void onCookingInstructionsPrint();

    /**
     * SLOT method called when the DONE button is clicked in the printer prompter.
     *
     * @param[in] none
     * @returns  int printer selected
     */
    void onPrintPromptDoneClicked(int promptIndex);

private:

    QSignalMapper *invalidAccessLevelSignal_;

    hobart::labeledNumericLineEdit *shelfLife_;
    hobart::labeledNumericLineEdit *productLife_;
    hobart::labeledPlainTextEdit *labelStock_;
    hobart::labeledDoubleNumericLineEdit *primaryLabelId_;
    hobart::labeledGraphicEdit *primaryGraphicId_;

    hobart::labeledLabelViewer *nutrition_;

    hobart::labeledPlainTextEdit *expandedText_;
    hobart::labeledPlainTextEdit *recipe_;
    hobart::labeledPlainTextEdit *cookingInstructions_;
    hobart::labeledPlainTextEdit *allergens_;

    hobart::labeledDoubleNumericLineEditArray *additionalLabelId_;
    hobart::labeledGraphicEditArray *additionalGraphicId_;

    hobart::printPrompt *printerPrompter_;

    QIntValidator *shelfLifeValidator_;
    QIntValidator *productLifeValidator_;
    QIntValidator *primaryLabelIdValidator_;
    QIntValidator *primaryGraphicIdValidator_;

    QStringList printerNumsList;
    QStringList printerNamesList;
    QStringList printerTypesList;
    QStringList printerDriversList;

    QMap<QString, int> prTextFieldNames;
    QMap<int, QString> prDataNamesMap;

    OperateMode *operateMode_;
    DomWeighModeTotalType *weighModeTotalType_;

    int fieldToPrint_;
    QMap<int, DomPrinterStockMetrics> labelStockMetrics_;
    QMap<int, QMap<int, int> > modeOffsets_;                              // this will map a label to a printer(1st,2nd,3rd) and a mode
    QMap<int, QString > modeLabels_;

    bool scaleIsMetric_;
    bool signalPluLableOnce_;

    static const bool convertUppercase_;

    static const int nutrifactsLabelId_;
    static const int nutrifactsLabelIdZoomed_;

    static const int promptUser_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Clear fields.
    void clearFields();

    /// Update label stock with printer metrics.
    void updateLabelStock();

    /// get the right label depending upon the mode
    OperateMode::ProductWriteSingleFieldOffset getModeLabelOffset(OperateMode::ProductWriteSingleFieldOffset off);

    /// get the right label value depending on the mode
    QString getModeLabelValue(OperateMode::ProductWriteSingleFieldOffset off);

    /// Retrieve current product.
    DomCurrentPLU *currentProduct() const;

    /// Make label height and width into a string.
    QString makeLabelStockDimensions( const QString& width, const QString& height, bool wrapping = true ) const;

    /**
     * Update the names of the printers in the prompt dialog
     */
    void updatePrinterData(const SessionMessage &msg);

    /**
     * Update how we may have to notify the user when they press the print key on a field
     */
    void updatePrinterPromptData(const SessionMessage &msg);

    /**
     * Launches the printer prompt for anyone who needs to ask if they are prompted to provide a printer
     */
    void launchPrinterPrompt();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // PLUINFOSCREEN_H
