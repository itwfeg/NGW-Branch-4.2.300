/**
 * @file numberpadwidget.h
 * Virtual number pad widget.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef NUMBERPADWIDGET_H
#define NUMBERPADWIDGET_H

#include "inputwidget.h"

#include <Session.h>
#include <QMap>

class OperateMode;

namespace hobart {
    class pushButton;
}

class QSignalMapper;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Virtual number pad widget.
class NumberPadWidget : public InputWidget
{
    Q_OBJECT

    Q_ENUMS( KeyLayoutStyle )
    Q_ENUMS( BackKeyLayoutStyle )

    Q_PROPERTY( KeyLayoutStyle keyLayoutStyle READ keyLayoutStyle WRITE setKeyLayoutStyle NOTIFY keyLayoutStyleChanged )
    Q_PROPERTY( BackKeyLayoutStyle backKeyLayoutStyle READ backKeyLayoutStyle WRITE setBackKeyLayoutStyle )

    typedef NumberPadWidget _Myt;
    typedef InputWidget _Mybase;

Q_SIGNALS:

    /// This signal is emitted when the layout style changes.
    void keyLayoutStyleChanged();

    /// This signal is emitted when the window is moved.
    /**
     * @param[in] where  where the widget was moved to
     */
    void moved( Qt::DockWidgetArea where );

public:

    /// Number pad key layout styles.
    enum KeyLayoutStyle
    {
        CalculatorKeyLayoutStyle,                   /// 7,8,9 keys on top
        TelephoneKeyLayoutStyle                     /// 1,2,3 keys on top
    };

    /// Number pad key layout styles.
    enum BackKeyLayoutStyle
    {
        largeBackKeyLayoutStyle,                   /// double wide width
        smallBackKeyLayoutStyle                    /// single wide width
    };


    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit NumberPadWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~NumberPadWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve key layout style.
    /**
     * @return  key layout style
     */
    virtual KeyLayoutStyle keyLayoutStyle() const {return keyLayoutStyle_;}

    /// Retrieve BackSpace Key layout style.
    /**
     * @return  key layout style
     */
    virtual BackKeyLayoutStyle backKeyLayoutStyle() const {return backKeyLayoutStyle_;}

    /// Set key layout style.
    /**
     * @param[in] value  key layout style
     */
    virtual void setKeyLayoutStyle( KeyLayoutStyle value );

    /// Set backspace key layout style.
    /**
     * @param[in] value  backspace key layout style
     */
    virtual void setBackKeyLayoutStyle( BackKeyLayoutStyle value );

    /// Keep the current backspace key layout style.
    /**
     * @param[in] nil
     */
    virtual void updateBackKeyLayoutStyle();

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Retrieve margins.
    /**
     * @return  margins
     */
    static QMargins margins() {return margins_;}

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Mouse move event.
    /**
     * @param[in,out] e  mouse event
     */
    virtual void mouseMoveEvent( QMouseEvent *e );

    /// Mouse press event.
    /**
     * @param[in,out] e  mouse event
     */
    virtual void mousePressEvent( QMouseEvent *e );

    /// Mouse release event.
    /**
     * @param[in,out] e  mouse event
     */
    virtual void mouseReleaseEvent( QMouseEvent *e );

    /// Resize event.
    /**
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /// Show event.
    /**
     * @param[in,out] e  show event
     */
    virtual void showEvent( QShowEvent *e );

    virtual void hideEvent( QHideEvent *e );


    // ========================================================================
    // Methods
    // ========================================================================

    /// Emit layout style changes signal.
    virtual void emitKeyLayoutStyleChanged();

private Q_SLOTS:

    /// Slot to emit layout style changed.
    void onEmitKeyLayoutStyleChanged();

    /// Slot to handle key click.
    void onKeyClicked( QWidget *widget );

private:

    typedef QMap<QString, hobart::pushButton*> pushButtonMap;

    pushButtonMap keys_;
    QSignalMapper *signalMapper_;

    KeyLayoutStyle keyLayoutStyle_;
    bool keyLayoutStyleHasChanged_;

    BackKeyLayoutStyle backKeyLayoutStyle_;
    bool backKeyLayoutStyleHasChanged_;

    QPoint position_;
    bool moving_;

    bool isDeliVisionLicensed_;

    OperateMode *operateMode_;

    QTimer *emitKeyLayoutStyleChangedTimer_;

    static const QMargins margins_;

    static const QSize buttonIconSize_;
    static const int buttonFontPointSize_;

    static const int layoutSpacing_;

    SessionMainWindow *mainWindow_;                 ///< Pointer to session main window.
    Session *session_;                              ///< Pointer to session.

    /// Initialize widget.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Create Look for Backspace and Search Keys
    void createBackspaceLayout();

    /// Check for drag complete.
    void checkDragCompletion();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // NUMBERPADWIDGET_H
