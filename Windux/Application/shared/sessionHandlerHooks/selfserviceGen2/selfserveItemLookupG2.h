/**
  * @file selfserveItemLookupG2.h
  * @brief Widget for Self-Service lookup by Item Number.
  *
  * @copyright Copyright (c) 2023, ITW Hobart Food Equipment Group, U.S.A.
  * @author Joe Crew
  * @inspiredby Doug Antonides
  * @addtogroup selfserveItemLookupG2Csu
  * @{
  */

#ifndef SELFSERVEITEMLOOKUPG2_H
#define SELFSERVEITEMLOOKUPG2_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include <QIntValidator>
#include <QLineEdit>
#include <QTimer>
#include <numericlineedit.h>

namespace Ui {
class selfserveItemLookupG2;
}

class selfServiceHook;
class ScaleConfiguration;
class DomLoadSoftkeyGroup;
class DomWeigherConfigRead;
class QIntValidator;


/// Hobart namespace.
namespace hobart {
class softkeysWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class selfserveItemLookupG2
 * @brief Widget for Self-Service lookup by Item Number.
 */
class selfserveItemLookupG2 : public QWidget
{
    Q_OBJECT

    typedef selfserveItemLookupG2 _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// Emitted when auto lookup is triggered.
    void lookup();

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
    explicit selfserveItemLookupG2( selfServiceHook *h, QWidget *parent = NULL );

    /**
     * Destructor.
     */
    virtual ~selfserveItemLookupG2();

    // ************************************************************************
    // Properties
    // ************************************************************************

    /**
     * Retrieve keyed item code.
     *
     * @return  item code keyed by user
     */
    virtual QString itemCode() const;

    // ************************************************************************
    // Methods
    // ************************************************************************

    /**
     * @brief  Load softkey group.
     * @param[in] m  pointer to message
     */
    virtual void loadSoftkeyGroup( const DomLoadSoftkeyGroup* m );

    /**
     * Refresh widget.
     */
    virtual void refresh();

    /**
    * Refresh weight.
    */
    virtual void refreshWeight();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();
	
	///resets number pad entry
    virtual void resetEntry();

    /**
     * Dim the screen opacity to pop dialog boxes.
     */
    virtual void dimScreenOpacity();

    /**
     * Reset the screen opacity to pop dialog boxes.
     */
    virtual void resetScreenOpacity();

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
     * @brief Update the connectivity background
     * @param[in] bool - scale online
     */
     virtual void updateNetworkConnectivity( bool online );

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

    /**
     * Show event.
     *
     * @param[in,out] e  event
     */
    virtual void showEvent( QShowEvent *e );

    /**
     * Mouse press event.
     *
     * @param[in,out] e  mouse event to pop weights and measures screen
     */
    virtual void mousePressEvent( QMouseEvent *e );


private Q_SLOTS:


    /// Slot to reload images.
    void onReloadImages();

    /// Slot to handle button clicked.
    void onButtonClicked( QWidget *w );

    /// Slot for timer timeout.
    void onKeypadTimeout();

    /// Slot for numeric input widget.
    void onNumberPadTextChanged( const QString& text );

private:

    Ui::selfserveItemLookupG2 *ui;

    ScaleConfiguration *scaleConfig_;
    DomWeigherConfigRead *weigherConfig_;
    selfServiceHook *hook_;

    hobart::softkeysWidget *bestSellers_; 

    QString curSeparator_;

    int prevSearch_;
    int currSearch_;

    // labels
    QLabel *weight_;
    QLabel *weightTitle_;
    QLabel *capacityLabel_;
    QWidget *capacityArea_;
    QWidget *screen_;

    QLabel *capacityHighRange_;
    QLabel *capacityLowRange_;
    QLabel *zero_;
    QLabel *pluNumTitle_;

    // buttons
    QPushButton *rezero_;
    QPushButton *toggleLanguage_;
    QPushButton *startOver_;

    QPushButton *search_;
    QPushButton *searchBestSellers_;
    QPushButton *searchFruits_;
    QPushButton *searchVeggies_;
    QPushButton *searchOrganic_;
    QPushButton *searchOther_;

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

    hobart::numericLineEdit   *pluNum_;           // lineEdit_
    QIntValidator *numValidator_;

    QSignalMapper *buttonSignalMapper_;

    bool autoLookupProduct_;
    int autoLookupNumOfDigits_;
    int autoLookupOverrideDigit_;

    const QIntValidator *validator_;
    QTimer *cycleNumpadTimer_;

    /**
     * get the pixels
     */
    int getScreenHeight();

    /**
     * get the best sellers
     */
    void resetBestSellers();

    /**
     * Sets the top number of digits in the string being masked and validated
     *
     * @param[in] the number of current character max
     * @returns The formatted date dd/mm/yyyy hh:mm
     */
    int makeValidatorTop(int digits);

    /**
     * Initialize.
     */
    void initialize();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVEITEMLOOKUPG2_H
