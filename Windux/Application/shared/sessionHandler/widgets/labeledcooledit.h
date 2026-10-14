/**
 * @file labeledcooledit.h
 * COOL edit widget.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELEDCOOLEDIT_H
#define LABELEDCOOLEDIT_H

#include "labeleddoublelineedit.h"

class SessionMainWindow;

class QTimer;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labeledCoolEdit
 * @brief COOL edit widget.
 */
class labeledCoolEdit : public labeledDoubleLineEdit
{
    Q_OBJECT

    Q_PROPERTY( int cool READ cool WRITE setCool NOTIFY coolChanged )
    Q_PROPERTY( QString coolText READ coolText )
    Q_PROPERTY( bool coolEnabled READ isCoolEnabled WRITE setCoolEnabled )
    Q_PROPERTY( bool coolNotRequired READ isCoolNotRequired )
    Q_PROPERTY( coolTextMap coolTextValues READ coolTextValues WRITE setCoolTextValues )
    Q_PROPERTY( int defaultCool READ defaultCool WRITE setDefaultCool )
    Q_PROPERTY( QString trackingNumber READ trackingNumber WRITE setTrackingNumber NOTIFY trackingNumberChanged )
    Q_PROPERTY( bool trackingNumberEnabled READ isTrackingNumberEnabled WRITE setTrackingNumberEnabled )

    typedef labeledCoolEdit _Myt;
    typedef labeledDoubleLineEdit _Mybase;

Q_SIGNALS:

    /**
     * Signal for cool changed.
     */
    void coolChanged();

    /**
     * Signal for operator done editing.
     *
     * @param[in] rc  result code
     */
    void dialogActionComplete( int rc );

    /**
     * Signal for tracking number changed.
     */
    void trackingNumberChanged();

public:

    /**
     * Map of COOL text values.
     */
    typedef QMap<int, QString> coolTextMap;

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledCoolEdit(
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Destructor.
     */
    virtual ~labeledCoolEdit();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve cool.
     *
     * @returns  cool
     */
    virtual int cool() const {return cool_;}

    /**
     * Retrieve cool text.
     *
     * @returns  cool text
     */
    virtual QString coolText() const;

    /**
     * Retrieve cool text values.
     *
     * @returns  cool text map
     */
    virtual coolTextMap coolTextValues() const {return coolText_;}

    /**
     * Retrieve default cool.
     *
     * @returns  default cool
     */
    virtual int defaultCool() const {return defaultCool_;}

    /**
     * Check if widget has focus.
     *
     * @returns  @c true if focused, @c false otherwise
     */
    virtual bool hasFocus() const {return (( dialogOpen_ ) || ( _Mybase::hasFocus() ));}

    /**
     * Retrieve COOL enabled.
     *
     * @returns  @c true if cool enabled, @c false otherwise
     */
    virtual bool isCoolEnabled() const {return coolEnabled_;}

    /**
     * Retrieve if cool is not required.
     *
     * @returns  @c true if cool is not required, @c false otherwise
     */
    virtual bool isCoolNotRequired() const {return (noCoolTextNum_ == cool());}

    /**
     * Retrieve COOL tracking enabled.
     *
     * @returns  @c true if tracking is enabled, @c false otherwise
     */
    virtual bool isTrackingNumberEnabled() const {return trackingNumberEnabled_;}

    /**
     * Retrieve tracking number.
     *
     * @returns  cool tracking
     */
    virtual QString trackingNumber() const;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Pops input dialog for setting cool and tracking text.
     *
     * @param[in] coolRequired  @c true to require cool, @c false otherwise
     * @param[in] trackingNumberRequired  @c true to require tracking number, @c false otherwise
     */
    virtual void promptForInput( bool coolRequired = false, bool trackingNumberRequired = false );

public Q_SLOTS:

    /**
     * Clear edit control.
     */
    virtual void clear();

    /**
     * Clear edit control.
     */
    virtual void clearSecond();

    /**
     * Set cool.
     *
     * @param[in] value  cool
     */
    virtual void setCool( int value );

    /**
     * Set COOL enabled.
     *
     * @param[in]  @c true if cool enabled, @c false otherwise
     */
    virtual void setCoolEnabled( bool value );

    /**
     * Set cool text values.
     *
     * @param[in] value  cool text map
     */
    virtual void setCoolTextValues( const coolTextMap& value ) {coolText_ = value;}

    /**
     * Set default cool.
     *
     * @param[in] value  default cool
     */
    virtual void setDefaultCool( int value ) {defaultCool_ = value;}

    /**
     * Set tracking number.
     *
     * @param[in] value  cool tracking
     */
    virtual void setTrackingNumber( const QString& value );

    /**
     * Set COOL tracking enabled.
     *
     * @returns  @c true if tracking is enabled, @c false otherwise
     */
    virtual void setTrackingNumberEnabled( bool value );

protected:

    /**
     * Event filter.
     *
     * @param[in,out] obj  object event filter was attached to
     * @param[in,out] event  event that is occuring
     * @returns  @c true if event @p event should be filtered, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *event );

    /**
     * Mouse press event.
     *
     * @param[in,out] e  mouse event
     */
    virtual void mousePressEvent( QMouseEvent *e );

private Q_SLOTS:

    /**
     * Slot for selection changed.
     */
    void onSelectionChanged();

    /**
     * Slot for selection changed on second child.
     */
    void onSelectionChangedSecond();

    /**
     * Slot for timer timeout.
     */
    void onTimeout();

private:

    QWidget *icon_;

    coolTextMap coolText_;

    int cool_;
    int defaultCool_;
    QString trackingNumber_;

    bool coolEnabled_;
    bool trackingNumberEnabled_;

    bool coolRequired_;
    bool trackingNumberRequired_;

    bool dialogOpen_;

    QTimer *timer_;

    SessionMainWindow *mainWindow_;

    static const int noCoolTextNum_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Set icon visibility.
     */
    void updateIconVisible();

    /**
     * Set focus proxy.
     */
    void updateFocusProxy();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // LABELEDCOOLEDIT_H
