/**
  * @file selfserveImageLookupG2.h
  * @brief Widget for Self-Service lookup by Image.
  *
  * @copyright Copyright (c) 2023, ITW Hobart Food Equipment Group, U.S.A.
  * @author Joe E. Crew
  * @copiedfrom Doug Antonides
  * @addtogroup selfserveImageLookupG2Csu
  * @{
  */

#ifndef SELFSERVEIMAGELOOKUPG2_H
#define SELFSERVEIMAGELOOKUPG2_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMutex>
#include <QTimer>
#include <QSignalMapper>

namespace Ui {
class selfserveImageLookupG2;
}

class Session;
class ScaleConfiguration;
class selfServiceHook;
class DomLoadSoftkeyGroup;

/// Hobart namespace.
namespace hobart {

class softkeysWidget;
class selfserveCharByCharSoftkeysG2;
class selfServiceSoftkeysWidgetG2;


///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class selfserveImageLookupG2
 * @brief Widget for Self-Service lookup by Image
 */
class selfserveImageLookupG2 : public QWidget
{
    Q_OBJECT

    typedef selfserveImageLookupG2 _Myt;
    typedef QWidget _Mybase;

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
    explicit selfserveImageLookupG2( selfServiceHook *h, QWidget *parent = NULL );

    /**
     * Destructor.
     */
    virtual ~selfserveImageLookupG2();

    // ************************************************************************
    // Methods
    // ************************************************************************

    /**
     * @brief  Load softkey group.
     * @param[in] m  pointer to message
     */
    virtual void loadSoftkeyGroup( const DomLoadSoftkeyGroup* m );

    /**
    * Refresh weight.
    */
    virtual void refreshWeight();

    /**
     * Refresh widget.
     */
    virtual void refresh();

    /**
     * Reset event.
     */
    virtual void resetScreen();

    /**
     * Dim the screen opacity to pop dialog boxes.
     */
    virtual void dimScreenOpacity();

    /**
     * Reset the screen opacity to pop dialog boxes.
     */
    virtual void resetScreenOpacity();


    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

    /**
     * Update the connectivity background
     */
    virtual void updateNetworkConnectivity( bool online );

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

    /**
     * Hide event.
     *
     * @param[in,out] e  event
     */
    virtual void hideEvent( QHideEvent *e );

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

    /// Slot for return key pressed.
    void onCharacterSearchReturnPressed();

    /// Slot for when text changes.
    void onCharacterSearchChanged( const QString& text );

private:

    Ui::selfserveImageLookupG2 *ui;

    Session *session_;
    selfServiceHook *hook_;
    ScaleConfiguration *scaleConfig_;
    selfserveCharByCharSoftkeysG2 *cbcsoftkeys_;
    selfServiceSoftkeysWidgetG2 *softkeys_;

    hobart::softkeysWidget *results_;
    QWidget *resultsPanel_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * get the pixels
     */
    int getScreenHeight();

    QLineEdit *characterSearch_;

    QLabel *weight_;
    QLabel *capacityLabel_;
    QWidget *capacityArea_;
    QWidget *screen_;

    QLabel *capacityHighRange_;
    QLabel *capacityLowRange_;
    QLabel *zero_;
    QLabel *lowerLimit_;
    QLabel *upperLimit_;

    // buttons
    QPushButton *back_;
    QPushButton *clear_;
    QPushButton *enter_;
    QPushButton *prevPage_;
    QPushButton *nextPage_;
    QPushButton *rezero_;
    QPushButton *toggleLanguage_;
    QPushButton *startOver_;                // makes no sence to have this key really
    QPushButton *backspace_;
    QPushButton *enterNums_;                // or this one
    QPushButton *backspaceNums_;
    QPushButton *butSearchView123_;         // 0-9 from Char by Char
    QPushButton *butSearchViewAll_;         // A-Z from Char by Char
    QPushButton *butSearchViewABC_;         // Char by Char from number keys
    QPushButton *activeKey_;

    QPushButton *digitA_;
    QPushButton *digitB_;
    QPushButton *digitC_;
    QPushButton *digitD_;
    QPushButton *digitE_;
    QPushButton *digitF_;
    QPushButton *digitG_;
    QPushButton *digitH_;
    QPushButton *digitI_;
    QPushButton *digitJ_;
    QPushButton *digitK_;
    QPushButton *digitL_;
    QPushButton *digitM_;
    QPushButton *digitN_;
    QPushButton *digitO_;
    QPushButton *digitP_;
    QPushButton *digitQ_;
    QPushButton *digitR_;
    QPushButton *digitS_;
    QPushButton *digitT_;
    QPushButton *digitU_;
    QPushButton *digitV_;
    QPushButton *digitW_;
    QPushButton *digitX_;
    QPushButton *digitY_;
    QPushButton *digitZ_;
    QPushButton *digitAPOS_;
    QPushButton *digitDASH_;
    QPushButton *digitSPACE_;

    QPushButton *digit0_;
    QPushButton *digit1_;
    QPushButton *digit2_;
    QPushButton *digit3_;
    QPushButton *digit4_;
    QPushButton *digit5_;
    QPushButton *digit6_;
    QPushButton *digit7_;
    QPushButton *digit8_;
    QPushButton *digit9_;

    QSignalMapper *buttonSignalMapper_;

    QTimer *cycleNumpadTimer_;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVEIMAGELOOKUPG2_H
