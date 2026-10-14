#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
     * @param[in] parent  parent
     */
    mainWindow( QWidget *parent = 0 );

    /// Destructor.
    virtual ~mainWindow();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MAINWINDOW_H
