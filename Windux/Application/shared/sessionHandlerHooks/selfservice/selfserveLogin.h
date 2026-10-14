/**
  * @file selfserveLogin.h
  * @brief Widget for Self-Service Login screen.
  *
  * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
  * @author Joe Crew
  * @inspired by Doug Antonides
  * @addtogroup selfserveLoginCsu
  * @{
  */

#ifndef SELFSERVELOGIN_H
#define SELFSERVELOGIN_H

#include <QWidget>

namespace Ui {
class selfserveLogin;
}

class selfServiceHook;
class ScaleConfiguration;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class selfserveLogin
 * @brief Widget for Self-Service login screen.
 */
class selfserveLogin : public QWidget
{
    Q_OBJECT

    typedef selfserveLogin _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// Emitted when operator lookup is triggered.
    void oplookup();

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
    explicit selfserveLogin( selfServiceHook *h, QWidget *parent = NULL );

    /**
     * Destructor.
     */
    virtual ~selfserveLogin();

    // ************************************************************************
    // Methods
    // ************************************************************************

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

    /**
     * Slot to reload text.
     */
    void onReloadScreen();

    /**
     * Slot for numeric input widget.
     */
    void onNumberPadTextChanged( const QString& text );

    /**
     * Slot for opertor lookup.
     */
    void onSelfServiceLogin( const QString& id );

private:

    Ui::selfserveLogin *ui;

    selfServiceHook *hook_;

    ScaleConfiguration *scaleConfig_;

    /**
     * Initialize.
     */
    void initialize();

    int checkScreenSize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVELOGIN_H
