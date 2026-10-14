/**
 * @file minandmaxprintdialog.cpp
 *
 * @copyright Copyright (c) 2018, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe B. Crew
 * @addtogroup MinAndMaxPrintWidgetCsu
 * @{

 */

#ifndef MINANDMAXPRINTWIDGET_H
#define MINANDMAXPRINTWIDGET_H

#include <QWidget>
#include "../screens/screenwidget.h"

class QLabel;
class OperateMode;
class DomCurrentPLU;
class MadpScaleConfig;
class SessionMainWindow;
class MadpUpdateCurrentPLUTotals;
class MadpCurrencySettings;

namespace Ui {
class MinAndMaxPrintWidget;
}

/// Hobart namespace.
namespace hobart {

class MinAndMaxPrintWidget : public QWidget
{
    Q_OBJECT

    typedef MinAndMaxPrintWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit MinAndMaxPrintWidget(QWidget *parent, SessionMainWindow *mainWindow);

    /**
     * Destructor.
     */
    virtual ~MinAndMaxPrintWidget();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

    /// Check if dialog is valid.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValid() const {return valid_;}

    /// Check display the updated data.
    /**
     * @return  nothing
     */
    virtual void displayOutOfRangeData(QString errorCode, QString minWeight, QString maxWeight, QString minPrice, QString maxPrice);

    /// Press onscreen PRINT key on keypad instead of dialog.
    /**
     * @return  nothing
     */
    virtual void printOnScreenPrintKey();

private Q_SLOTS:

    // ========================================================================
    // Slots
    // ========================================================================

    /**
     * SLOT when receiving a session message so it can be acted on.
     *
     * @param[in] a message from application
     * @returns none
     */
    void onSessionMessageReceiveInternal( const SessionMessage& msg );

   /**
    * SLOT when the OK and Print button is clicked.
    *
    * @param[in] none
    * @returns none - but it does print when clicked
    */
   void onOKButtonClicked();


   /**
    * SLOT when the Cancel button is clicked.  We just be leaving.
    *
    * @param[in] none
    * @returns none
    */
   void onCancelButtonClicked();

protected:

   // ========================================================================
   // Events
   // ========================================================================

   SessionMainWindow *mainWindow_;
   Session *session_;

   /**
    * Resize event.
    *
    * @param[in,out] e  resize event
    */
   virtual void resizeEvent( QResizeEvent *e );

   // ========================================================================
   // Methods
   // ========================================================================

   /// Callback for session messages.
   /**
    * @param[in] msg  session message
    */
   virtual void onSessionMessageReceive( const SessionMessage& msg );

private:

    Ui::MinAndMaxPrintWidget *ui;

    ScaleConfiguration *config_;
    OperateMode *operateMode_;
    MadpScaleConfig *scaleConfigData;
    DomCurrentPLU *currentPLU_;

    bool valid_;
    bool stablePrice_;
    bool stableWeight_;

    QString code_;
    double minWt_;
    double maxWt_;
    double minPr_;
    double maxPr_;

    bool showPrice_;
    bool showWeight_;
    QString range_weight_;
    QString range_price_;

    QString minWeight_;
    QString maxWeight_;
    QString minPrice_;
    QString maxPrice_;

    bool use10Inch_;

    /**
     * Method to initialize all variables and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * @brief highlightGreen highlights a label green
     * @param label the label to be highlighted
     */
    void highlightGreen(QLabel* label);

    /**
     * @brief highlightRed highlights a label red
     * @param label the label to be highlighted
     */
    void highlightRed(QLabel* label);

    /**
     * @brief highlightcamo highlights a label the same color as background
     * @param label the label to be highlighted
     */
    void highlightCamo(QLabel* label);

    /**
     * @brief highlightWhite highlights a label the same color as original background
     * @param label the label to be white
     */
    void highlightWhite(QLabel* label);

    /// Retrieve current product.
    DomCurrentPLU *currentProduct() const;

    /// Retrieve current product totals.
    MadpUpdateCurrentPLUTotals *currentProductTotals() const;

    /// turn fields on and off
    void enablePriceFields( bool isEnabled);

    /// turn fields on and off
    void enableWeightFields( bool isEnabled);

    /// reset everything
    void resetValues();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */


#endif // MINANDMAXPRINTWIDGET_H
