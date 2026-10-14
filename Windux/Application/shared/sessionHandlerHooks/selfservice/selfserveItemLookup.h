/**
  * @file selfserveItemLookup.h
  * @brief Widget for Self-Service lookup by Item Number.
  *
  * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup selfserveItemLookupCsu
  * @{
  */

#ifndef SELFSERVEITEMLOOKUP_H
#define SELFSERVEITEMLOOKUP_H

#include <QWidget>

namespace Ui {
class selfserveItemLookup;
}

class selfServiceHook;
class ScaleConfiguration;
class DomLoadSoftkeyGroup;

/// Hobart namespace.
namespace hobart {

class softkeysWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class selfserveItemLookup
 * @brief Widget for Self-Service lookup by Item Number.
 */
class selfserveItemLookup : public QWidget
{
    Q_OBJECT

    typedef selfserveItemLookup _Myt;
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
    explicit selfserveItemLookup( selfServiceHook *h, QWidget *parent = NULL );

    /**
     * Destructor.
     */
    virtual ~selfserveItemLookup();

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
     * Translate the user interface to current language.
     */
    virtual void translate();
	
	///resets number pad entry
    virtual void resetEntry();

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

private Q_SLOTS:

    /**
     * Slot to reload images.
     */
    void onReloadImages();

private:

    Ui::selfserveItemLookup *ui;

    ScaleConfiguration *scaleConfig_;

    selfServiceHook *hook_;

    hobart::softkeysWidget *bestSellers_;
    hobart::softkeysWidget *chiliSellers_; 

    /**
     * Initialize.
     */
    void initialize();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVEITEMLOOKUP_H
