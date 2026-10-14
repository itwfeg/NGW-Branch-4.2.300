/**
 * @file dynamicsoftkeybutton.h
 * Dynamic softkey button widget.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DYNAMICSOFTKEYBUTTON_H
#define DYNAMICSOFTKEYBUTTON_H

#include <softkeybutton.h>
#include "../../sessionHandlerWidgets/softkeytogglemanager.h"
#include <Messages.h>

class ScaleConfiguration;
class OperateMode;
class DomSoftkey;

//class SessionMainWindow;

using namespace hobart;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dynamic softkey button widget.
class dynamicSoftkeyButton : public softkeyButton
{
    Q_OBJECT

    typedef dynamicSoftkeyButton _Myt;
    typedef softkeyButton _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit dynamicSoftkeyButton( QWidget *parent = 0 );

    /// Destructor.
    virtual ~dynamicSoftkeyButton();

    /**
     * Set softkey action.
     *
     * @param[in] value  action
     */
    virtual void setAction( int value );


protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Paint event.
    /**
     * @param[in,out] e  event
     */
    virtual void paintEvent( QPaintEvent *e );


private Q_SLOTS:

    /**
     * Slot for receiving the writeSingleField message.
     */
    void onCurrentProductUpdateField( const DomUpdateRunModeField& m );

    /**
     * Slot for receiving the DomCurrentPlu message - so we can update our flashkeys if they are selected.
     */
	void onCurrentProductChanged( bool nof );


private:

    // not implemented
    _Myt &operator = ( const _Myt &rhs );

    ScaleConfiguration *scaleConfig_;
    OperateMode *operateMode_;
	DomCurrentPLU *activePLU_;

	int getActivePluNumber();
	int getActiveLabelType(int printer);
	void parseChangeLabelTypeActionParam(int &printer, int &labelType);

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Factory class for dynamic softkey button widgets.
class dynamicSoftkeyButtonFactory : public softkeyButtonFactory
{
    typedef dynamicSoftkeyButtonFactory _Myt;
    typedef softkeyButtonFactory _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    explicit dynamicSoftkeyButtonFactory();

    /// Destructor.
    virtual ~dynamicSoftkeyButtonFactory();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Create new @c softkeyButton object.
    /**
     * @param[in] parent  parent widget
     */
    virtual softkeyButton *create( QWidget *parent = 0 ) const;

private:

    // not implemented
    _Myt &operator = ( const _Myt &rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DYNAMICSOFTKEYBUTTON_H
