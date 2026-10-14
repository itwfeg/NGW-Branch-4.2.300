/**
  * @file selfserveImageLookup.h
  * @brief Widget for Self-Service lookup by Image.
  *
  * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup selfserveImageLookupCsu
  * @{
  */

#ifndef SELFSERVEIMAGELOOKUP_H
#define SELFSERVEIMAGELOOKUP_H

#include <QWidget>

namespace Ui {
class selfserveImageLookup;
}

class selfServiceHook;

class DomLoadSoftkeyGroup;

/// Hobart namespace.
namespace hobart {

class selfServiceSoftkeysWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class selfserveImageLookup
 * @brief Widget for Self-Service lookup by Image
 */
class selfserveImageLookup : public QWidget
{
    Q_OBJECT

    typedef selfserveImageLookup _Myt;
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
    explicit selfserveImageLookup( selfServiceHook *h, QWidget *parent = NULL );

    /**
     * Destructor.
     */
    virtual ~selfserveImageLookup();

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
     * Translate the user interface to current language.
     */
    virtual void translate();

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

private Q_SLOTS:

    /**
     * Slot to reload images.
     */
    void onReloadImages();

private:

    Ui::selfserveImageLookup *ui;

    selfServiceHook *hook_;
    selfServiceSoftkeysWidget *softkeys_;

    /**
     * Initialize.
     */
    void initialize();

};


///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVEIMAGELOOKUP_H
