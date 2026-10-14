/**
  * @file selfserveRandomWeight.h
  * @brief Widget for Self-Service Random Weight entry.
  *
  * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup selfserveRandomWeightCsu
  * @{
  */

#ifndef SELFSERVERANDOMWEIGHT_H
#define SELFSERVERANDOMWEIGHT_H

#include <QWidget>
#include <QMutex>


namespace Ui {
class selfserveRandomWeight;
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
 * @class selfserveRandomWeight
 * @brief Widget for Self-Service Random Weight entry.
 */
class selfserveRandomWeight : public QWidget
{
    Q_OBJECT

    typedef selfserveRandomWeight _Myt;
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
    explicit selfserveRandomWeight( selfServiceHook *h, QWidget *parent = NULL );

    /**
     * Destructor.
     */
    virtual ~selfserveRandomWeight();

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
     * Translate the user interface to current language.
     */
    virtual void translate();

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

    Ui::selfserveRandomWeight *ui;

    Session *s;
    selfServiceHook *hook_;
    bool hideBigPrintKey;
    ScaleConfiguration *scaleConfig_;

    hobart::softkeysWidget *ssTareFlashkeys_;

    QSignalMapper *buttonSignalMapper_;

    QWidget *buttons_;                              // might not need this is I use the one in the UI
    hobart::pushButton *printLabel_;
    hobart::pushButton *instructPrintLabel_;

    double minWeightToPrint_;

    QMutex m_;

    /**
     * check 10 inch or 14.
     */
    int checkScreenSize();

    /**
     * check 10 inch or 14.
    */
    int checkScreenHeight();

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Render label image to display.
     */
    void renderLabelImage();


};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVERANDOMWEIGHT_H
