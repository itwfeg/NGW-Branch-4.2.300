/**
 * @file mainwindow.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Main window.
class mainWindow : public QMainWindow
{
    Q_OBJECT

    typedef mainWindow _Myt;
    typedef QMainWindow _Mybase;

public:

    /// Mode type.
    enum modeType
    {
        printTicket,                                ///< Ticket printing mode.
        signage,                                    ///< Digital signage mode.
        onlineOrdering                              ///< Online Ordering mode.
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mode  mode type
     * @param[in] params  configuration
     * @param[in] parent  parent widget
     */
    mainWindow( modeType mode, const QMap<QString, QString>& params, QWidget *parent = 0 );

    /// Destructor.
    virtual ~mainWindow();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve mode.
    /**
     * @return  mode type
     */
    virtual modeType mode() const {return mode_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate widgets.
    virtual void translate();

private:

    modeType mode_;
    QMap<QString, QString> params_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MAINWINDOW_H
