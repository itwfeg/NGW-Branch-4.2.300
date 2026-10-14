/**
 * @file supertextfileeditor.h
 * Text file editor for supervisor mode.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SUPERTEXTFILEEDITOR_H
#define SUPERTEXTFILEEDITOR_H

#include "screenwidget.h"

class QPlainTextEdit;
class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class superTextFileEditor
 * @brief Text file editor for supervisor mode.
 */
class superTextFileEditor : public ScreenWidget
{
    Q_OBJECT

    typedef superTextFileEditor _Myt;
    typedef ScreenWidget _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit superTextFileEditor( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~superTextFileEditor();

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Load file into editor.
     *
     * @param[in] filename  playlist filename
     */
    virtual void loadFile( const QString& filename );

    /**
     * Save file.
     */
    virtual void saveFile();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private Q_SLOTS:

    /**
     * Slot for done button clicked.
     */
    void onDoneButtonClicked();

    /**
     * Slot for text cursor position changed.
     */
    void onTextCursorPositionChanged();

private:

    QString filename_;

    QPlainTextEdit *text_;
    QPushButton *doneButton_;

    /**
     * Initialize.
     */
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SUPERTEXTFILEEDITOR_H
