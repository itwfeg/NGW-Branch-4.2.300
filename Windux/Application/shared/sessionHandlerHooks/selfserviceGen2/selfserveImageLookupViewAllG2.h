/**
  * @file selfserveImageLookupViewAllG2.h
  * @brief Widget for Self-Service lookup by Image.
  *
  * @copyright Copyright (c) 2023, ITW Hobart Food Equipment Group, U.S.A.
  * @author Joe E. Crew
  * @copiedfrom Doug Antonides
  * @addtogroup selfserveImageLookupViewAllG2Csu
  * @{
  */

#ifndef SELFSERVEIMAGELOOKUPVIEWALLG2_H
#define SELFSERVEIMAGELOOKUPVIEWALLG2_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QMutex>
#include <QTimer>
#include <QSignalMapper>

namespace Ui {
class selfserveImageLookupViewAllG2;
}

class Session;
class selfServiceHook;
class DomLoadSoftkeyGroup;
class ScaleConfiguration;

/// Hobart namespace.
namespace hobart {
class softkeysWidget;
class selfServiceSoftkeysWidgetG2;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class selfserveImageLookupViewAllG2
 * @brief Widget for Self-Service lookup by Image
 */
class selfserveImageLookupViewAllG2 : public QWidget
{
    Q_OBJECT

    typedef selfserveImageLookupViewAllG2 _Myt;
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
    explicit selfserveImageLookupViewAllG2( selfServiceHook *h, QWidget *parent = NULL );

    /**
     * Destructor.
     */
    virtual ~selfserveImageLookupViewAllG2();

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

private:

    Ui::selfserveImageLookupViewAllG2 *ui;

    Session *session_;
    selfServiceHook *hook_;
    selfServiceSoftkeysWidgetG2 *softkeys_;     // whats the diff with bestSellerKeys_


    hobart::softkeysWidget *results_;
    QWidget *resultsPanel_;

    ScaleConfiguration *scaleConfig_;
    hobart::softkeysWidget *bestSellerKeys_;

    // labels
    QLabel *weight_;
    QLabel *capacityLabel_;
    QWidget *capacityArea_;
    QWidget *screen_;

    QLabel *capacityHighRange_;
    QLabel *capacityLowRange_;
    QLabel *zero_;

    QLabel *lowerLimit_;
    QLabel *upperLimit_;

    int currentFlaskey_;
    int totalFlashkeys_;

    // buttons
    QPushButton *rezero_;
    QPushButton *toggleLanguage_;
    QPushButton *startOver_;

    // buttons
    QPushButton *back_;
    QPushButton *nextPage_;
    QPushButton *prevPage_;

    QPushButton *activeKey_;
    QPushButton *bestsellers_;
    QPushButton *search_;

    QPushButton *keyA_;
    QPushButton *keyBC_;
    QPushButton *keyDJ_;
    QPushButton *keyKM_;
    QPushButton *keyNO_;
    QPushButton *keyP_;
    QPushButton *keyQS_;
    QPushButton *keyTZ_;

    QSignalMapper *buttonSignalMapper_;

    QTimer *cycleNumpadTimer_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * get the best sellers
     */
    void resetBestSellers();

    /**
     * get the pixels
     */
    int getScreenHeight();
};


///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVEIMAGELOOKUPVIEWALLG2_H
