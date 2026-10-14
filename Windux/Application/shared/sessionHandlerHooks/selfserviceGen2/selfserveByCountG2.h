/**
  * @file selfserveByCountG2.h
  * @brief Widget for Self-Service By Count entry.
  *
  * @copyright Copyright (c) 2023, ITW Hobart Food Equipment Group, U.S.A.
  * @author Joe B. Crew
  * @swipedfrom Doug Antonides
  * @addtogroup selfserveByCountG2Csu
  * @{
  */

#ifndef SELFSERVEBYCOUNTG2_H
#define SELFSERVEBYCOUNTG2_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMutex>
#include <QIntValidator>
#include <QTimer>

#include "selfserveByCountQtyG2.h"


namespace Ui {
class selfserveByCountG2;
}

class Session;
class selfServiceHook;
class ScaleConfiguration;
class DomCurrentPLU;
class DomLoadSoftkeyGroup;
class QPushButton;
class QSignalMapper;

/// Hobart namespace.
namespace hobart {

class pushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class selfserveByCountG2
 * @brief Widget for Self-Service By Count entry.
 */
class selfserveByCountG2 : public QWidget
{
    Q_OBJECT

    typedef selfserveByCountG2 _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// Emitted when print is triggered.
    void print();

    /// Emitted when auto lookup is triggered.
    void lookup();

    /// Emitted when enter clicked
    void enterClicked( const QString &qty);

    ///  emitted when cancel clicked
    void cancelClicked();

    /// Emitted when print error is triggered.
    void printError(int err);


public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * Constructor.
     *
     * @param[in] h  pointer to hook
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit selfserveByCountG2( selfServiceHook *h, QWidget *parent = NULL );

    /**
     * Destructor.
     */
    virtual ~selfserveByCountG2();

    // ************************************************************************
    // Methods
    // ************************************************************************

    /**
     * @brief  Update barcode only label.
     */
    virtual void renderBarcodeOnlyLabel();

    /**
     * Refresh widget.
     */
    virtual void refresh();

    /**
     * Refresh Prices and Quantity
     */
    virtual void refreshPriceAndCount();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

    /**
     * Reset the keypad back to the initail state
     */
    virtual void resetKeypad();

    /**
     * Reset the to blank
     */
    virtual void resetKeypadDisplay();

    /**
     * Resets number pad entry.
     */
    virtual void resetEntry();

    /**
     * Reset event.
     */
    virtual void resetScreen();

    /**
     * @brief  Set auto lookup number of digits.
     * @param[in] value  number of digits
     */
    virtual void setAutoLookupNumOfDigits( int value );

    /**
     * @brief  Set auto lookup override digit.
     * @param[in] value  override digit
     */
    virtual void setAutoLookupOverrideDigit( int value );

    /**
     * @brief  Set auto lookup enabled or not.
     * @param[in] value  @c true if enabled, @c false otherwise
     */
    virtual void setAutoLookupProduct( bool value );

    /**
     * @brief  Dim the screen opacity to pop dialog boxes.
     * @param[in] none
     */
    virtual void dimScreenOpacity();

    /**
     * @brief  Reset the screen opacity to pop dialog boxes.
     * @param[in] none
     */
    virtual void resetScreenOpacity();

    /**
     * @brief Update the connectivity background
     * @param[in] bool - scale online
     */
    virtual void updateNetworkConnectivity( bool online );

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

    /**
     * Resize event.
     *
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /**
     * Show event.
     *
     * @param[in,out] e  event
     */
    virtual void showEvent( QShowEvent *e );

    /**
     * Hide event.
     *
     * @param[in,out] e  event
     */
    virtual void hideEvent( QHideEvent *e );

    /**
     * Mouse press event.
     *
     * @param[in,out] e  mouse event to print a label
     */
    virtual void mousePressEvent( QMouseEvent *e );

private slots:

    /**
     * Slot to reload images.
     */
    void onReloadImages();

    /**
    * Slot to handle button clicked.
    */
    void onButtonClicked( QWidget *w );

    /**
     * Slot for numeric input widget.
     */
    void onNumberPadTextChanged( const QString& text );

    /**
    * Slot emitted when a bycount is entered.
    */
    void onShowBackButton();

    /**
    * Slot for timer timeout.
    */
    void onKeypadTimeout();

    /**
     * SLOT method called when the ENTER button is clicked in the ByCount Quantity editor.
     *
     * @param[in] none
     * @returns  none
     */
    void onByCountPopupEnterClicked(const QString &qty);

    /**
    * Slot emitted when a print error is triggered.
    */
    void onPrintError( int err );

    /**
    * Slot emitted when a cancel is triggered.
    */
    void onCancelClicked();

    /**
     * SLOT method called when action happens and we need to reset the idle timer.
     *
     * @param[in] none
     * @returns  none
     */
    void onResetIdleInactive();

private:

    Ui::selfserveByCountG2 *ui;

    Session *s;
    selfServiceHook *hook_;
    ScaleConfiguration *scaleConfig_;

    hobart::selfserveByCountQtyG2 *byCountQtyPopup_;

    QSignalMapper *buttonSignalMapper_;

    // labels
    QLabel *byCount_;
    QLabel *totalPrice_;
    QLabel *unitPrice_;
    QLabel *desc_;
    QLabel *plu_;

    // buttons for number pad
    /*
    QPushButton *digitZero_;
    QPushButton *digitOne_;
    QPushButton *digitTwo_;
    QPushButton *digitThree_;
    QPushButton *digitFour_;
    QPushButton *digitFive_;
    QPushButton *digitSix_;
    QPushButton *digitSeven_;
    QPushButton *digitEight_;
    QPushButton *digitNine_;

    QPushButton *backspace_;
    QPushButton *enter_;
    QPushButton *activeKey_;
*/
    // buttons
    QWidget *screen_;
    QPushButton *printLabel_;
    QPushButton *startOver_;
    QPushButton *back_;

    //QLineEdit   *quantity_;           // lineEdit_

    //bool autoLookupProduct_;
    //int autoLookupNumOfDigits_;
    //int autoLookupOverrideDigit_;

    QWidget *scanAndPrint_;
    QWidget *barcode_;

    QMutex m_;

    DomCurrentPLU *currentPLUData;

    bool isByCountPopup_;
    /**
     * Initialize.
     */
    void initialize();

    /**
     * Get how wide we are
     */
    int getScreenWidth();

    /**
     * Get how tall we are
     */
    int getScreenHeight();

    /**
     * Render label image to display.
     */
    void renderLabelImage();

    /**
     *  Popup up or hide the number pad
     */
    void  popNumberPad(bool value);
};


///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVEBYCOUNTG2_H
