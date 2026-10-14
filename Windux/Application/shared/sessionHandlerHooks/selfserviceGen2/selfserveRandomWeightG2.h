/**
  * @file selfserveRandomWeightG2.h
  * @brief Widget for Self-Service Random Weight entry.
  *
  * @copyright Copyright (c) 2023, ITW Hobart Food Equipment Group, U.S.A.
  * @author Joe E. Crew
  * @borrowedfrom Doug Antonides
  * @addtogroup selfserveRandomWeightG2Csu
  * @{
  */

#ifndef SELFSERVERANDOMWEIGHTG2_H
#define SELFSERVERANDOMWEIGHTG2_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMutex>


namespace Ui {
class selfserveRandomWeightG2;
}


class Session;
class selfServiceHook;
class ScaleConfiguration;
class DomLoadSoftkeyGroup;
class QPushButton;
class QSignalMapper;


/// Hobart namespace.
namespace hobart {

class softkeysWidget;
class pushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class selfserveRandomWeightG2
 * @brief Widget for Self-Service Random Weight entry.
 */
class selfserveRandomWeightG2 : public QWidget
{
    Q_OBJECT

    typedef selfserveRandomWeightG2 _Myt;
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
    explicit selfserveRandomWeightG2( selfServiceHook *h, QWidget *parent = NULL );

    /**
     * Destructor.
     */
    virtual ~selfserveRandomWeightG2();

    // ************************************************************************
    // Methods
    // ************************************************************************

    /**
     * @brief  Load softkey group.
     * @param[in] m  pointer to message
     */
    virtual void loadSoftkeyGroup( const DomLoadSoftkeyGroup* m );

    /**
     * @brief  Update barcode only label.
     */
    virtual void renderBarcodeOnlyLabel();

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
     * Update the connectivity background
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


private Q_SLOTS:

    /**
     * Slot to reload images.
     */
    void onReloadImages();

    /**
    * Slot to handle button clicked.
    */
    void onButtonClicked( QWidget *w );


private:

    Ui::selfserveRandomWeightG2 *ui;

    Session *s;
    selfServiceHook *hook_;
    bool hideBigPrintKey;
    ScaleConfiguration *scaleConfig_;

    QSignalMapper *buttonSignalMapper_;

    // labels
    QLabel *weight_;
    QLabel *totalPrice_;
    QLabel *unitPrice_;
    QLabel *capacityLabel_;
    QLabel *capacityHighRange_;
    QLabel *capacityLowRange_;
    QLabel *plu_;
    QLabel *zero_;
    QLabel *desc_;

    // buttons
    QPushButton *printLabel_;
    QPushButton *startOver_;
    QPushButton *back_;

    QWidget *capacityArea_;
    QWidget *scanAndPrint_;
    QWidget *barcode_;
    QWidget *screen_;

    double minWeightToPrint_;

    QMutex m_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * get the pixels
     */
    int getScreenHeight();

    /**
     * Render label image to display.
     */
    void renderLabelImage();


};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVERANDOMWEIGHTG2_H
