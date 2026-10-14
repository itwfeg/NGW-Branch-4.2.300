/**
 * @file cooldialog.h
 * Cool dialog for choosing cool or entering a cool tracking number.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef COOLDIALOG_H
#define COOLDIALOG_H

#include "dcadialog.h"

class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QString;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Cool dialog for choosing cool or entering a cool tracking number.
class CoolDialog : public DynamicContentAreaDialog
{
    Q_OBJECT

    Q_PROPERTY( int cool READ cool WRITE setCool )
    Q_PROPERTY( bool coolEnabled READ isCoolEnabled WRITE setCoolEnabled )
    Q_PROPERTY( bool coolRequired READ isCoolRequired WRITE setCoolRequired )
    Q_PROPERTY( QString trackingNumber READ trackingNumber WRITE setTrackingNumber )
    Q_PROPERTY( bool trackingNumberEnabled READ isTrackingNumberEnabled WRITE setTrackingNumberEnabled )
    Q_PROPERTY( bool trackingNumberRequired READ isTrackingNumberRequired WRITE setTrackingNumberRequired )
    Q_PROPERTY( QString title READ title WRITE setTitle )

    typedef CoolDialog _Myt;
    typedef DynamicContentAreaDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit CoolDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~CoolDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Whether or not clicking the overlay rejects the dialog.
    /**
     * @return  @c true if allowed, @c false otherwise
     */
    virtual bool allowOverlayClickDismiss() const;

    /// Retrieve COOL text.
    /**
     * @return  cool text id
     */
    virtual int cool() const;

    /// Retrieve COOL enabled.
    /**
     * @return  @c true if cool enabled, @c false otherwise
     */
    virtual bool isCoolEnabled() const;

    /// Retrieve COOL required.
    /**
     * @return  @c true if cool is required, @c false otherwise
     */
    virtual bool isCoolRequired() const {return (( isCoolEnabled() ) && ( coolTextRequired_ ));}

    /// Retrieve COOL tracking enabled.
    /**
     * @return  @c true if tracking is enabled, @c false otherwise
     */
    virtual bool isTrackingNumberEnabled() const;

    /// Retrieve COOL tracking required.
    /**
     * @return  @c true if tracking is required, @c false otherwise
     */
    virtual bool isTrackingNumberRequired() const {return (( isTrackingNumberEnabled() ) && ( coolTrackingNumberRequired_ ));}

    /// Set COOL text.
    /**
     * @param[in] value  cool text id
     */
    virtual void setCool( int value );

    /// Set COOL enabled.
    /**
     * @param[in]  @c true if cool enabled, @c false otherwise
     */
    virtual void setCoolEnabled( bool value );

    /// Set COOL required.
    /**
     * @return  @c true for cool required, @c false otherwise
     */
    virtual void setCoolRequired( bool value ) {coolTextRequired_ = value;}

    /// Set dialog title.
    /**
     * @param[in] value  dialog title
     */
    virtual void setTitle( const QString& value );

    /// Set COOL tracking number.
    /**
     * @param[in]  cool tracking text
     */
    virtual void setTrackingNumber( const QString& value );

    /// Set COOL tracking enabled.
    /**
     * @return  @c true if tracking is enabled, @c false otherwise
     */
    virtual void setTrackingNumberEnabled( bool value );

    /// Set COOL tracking required.
    /**
     * @return  @c true if tracking is required, @c false otherwise
     */
    virtual void setTrackingNumberRequired( bool value ) {coolTrackingNumberRequired_ = value;}

    /// Retrieve COOL tracking number.
    /**
     * @return  cool tracking text
     */
    virtual QString trackingNumber() const;

    /// Retrieve title of dialog.
    /**
     * @return  dialog title
     */
    virtual QString title() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Add cool text item into list widget.
    /**
     * @param[in] id  cool text id
     * @param[in] text  cool text
     * @param[in] isDefault  @c true if default cool, @c false otherwise
     */
    virtual void addCoolTextItem( int id, const QString& text, bool isDefault = false );

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Show event.
    /**
     * @param[in,out] event  show event
     */
    virtual void showEvent( QShowEvent *event );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve priority of this dialog.
    /**
     * When showing a high priority dialog all dialogs of lower priority will be automatically
     * closed.
     *
     * @return  priority of this dialog
     */
    virtual int priority() const {return Low;}

    /// Retrieve renderer appearance.
    /**
     * @return  appearance object
     */
    virtual hobart::widgetRendererAppearanceBase *rendererAppearance() const;

    /// Retrieve renderer style.
    /**
     * @return  style
     */
    virtual int rendererStyle() const;

private Q_SLOTS:

    /// Slot for cool item clicked.
    void onCoolItemClicked( QListWidgetItem *item );

    /// Slot for cool change.
    void onCoolItemSelectionChanged();

    /// Slot for cool tracking number return pressed.
    void onCoolTrackingNumberReturnPressed();

    /// Slot for done button clicked.
    void onDoneButtonClicked();

private:

    int savedCoolText_;
    QString savedCoolTrackingNumber_;

    bool coolTextRequired_;
    bool coolTrackingNumberRequired_;

    QLabel *title_;

    QLabel *coolTextLabel_;
    QListWidget *coolText_;

    QLabel *coolTrackingNumberLabel_;
    QLineEdit *coolTrackingNumber_;

    QPushButton *doneButton_;

    bool handleSignals_;

    static const bool convertUppercase_;


    /// Initialize.
    void initialize();

    /// Check if can reject dialog.
    bool allowReject() const;

    /// Check if can accept dialog.
    bool allowAccept() const;

    /// Check fields for valid input.
    bool isValidInput( int c, const QString& t ) const;

    /// Update tracking number field for when forced cool.
    void updateTrackingNumberForForcedCool();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // COOLDIALOG_H
