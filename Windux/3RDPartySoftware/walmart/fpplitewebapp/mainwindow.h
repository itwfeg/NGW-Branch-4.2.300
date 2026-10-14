/**
 * @file mainwindow.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef MAINWINDOW
#define MAINWINDOW

#include <QMainWindow>
#include <QMap>
#include <QString>

class QLabel;
class QToolButton;
class QWidget;

namespace hobart {
class webView;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Main window.
class mainWindow : public QMainWindow
{
    Q_OBJECT

    typedef mainWindow _Myt;
    typedef QMainWindow _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] params  parameters
     * @param[in] parent  parent object
     */
    mainWindow( const QMap<QString, QString> &params, QWidget *parent = 0 );

    /// Destructor.
    virtual ~mainWindow();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate text.
    virtual void translate();

Q_SIGNALS:

    /// Emitted when input device needs hidden.
    void hideInputDevice();

    /// Emitted when input device needs shown.
    /**
     * @param[in] widget  focus widget
     * @param[in] altLocation  use alternate location, if possible (i.e. top/bottom of screen)
     */
    void showInputDevice( QWidget *widget, bool altLocation = false );

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

private Q_SLOTS:

    /// Slot for button clicked.
    void onButtonClicked();

private:

    enum style
    {
        StyleNormal = 0,
        StyleCompact,
        StyleFloatingLeft,
        StyleFloatingCenter,
        StyleFloatingRight,
        StyleFloatingOffsetLeft,
        StyleFloatingOffsetRight,
    };


    QMap<QString, QString> params_;

    style style_;

    int store_;
    int dept_;

    QWidget *centralWidget_;
    hobart::webView *webView_;

    QWidget *buttonBar_;
    QLabel *logo_;
    QLabel *title_;
    QToolButton *toggleKeyboard_;
    QToolButton *refresh_;
    QToolButton *minimize_;
    QToolButton *exit_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Check if floating buttons.
    bool isFloatingButtons() const {return (StyleFloatingLeft <= style_);}

    /// Request store settings.
    bool requestStoreSettings();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MAINWINDOW
