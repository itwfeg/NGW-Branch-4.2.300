/**
  * @file deleteAllFiles.h
  * @brief Allows supervisor to delete all files from the database.
  *
  * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup deleteAllFilesCsu
  * @{
  */

#ifndef DELETEALLFILES_H
#define DELETEALLFILES_H

#include "screenwidget.h"

class ScaleConfiguration;
class MadpScaleConfig;
class MadpInternalConfig;

namespace Ui {
class deleteAllFiles;
}

/**
 * @class deleteAllFiles
 * @brief Allows supervisor to delete all files from the database.
 *
 */

class deleteAllFiles : public ScreenWidget
{
    Q_OBJECT

    typedef deleteAllFiles _Myt;
    typedef ScreenWidget _Mybase;

    
public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit deleteAllFiles(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~deleteAllFiles();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot for when the "Delete all files" button is pressed.
     */
    void executeDeleteAll();


protected:

    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. Do something
     * here if there is one last thing to do before heading to another spot
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();


private:
    Ui::deleteAllFiles *ui;

    MadpInternalConfig *internalConfig_;
    MadpScaleConfig *scaleConfig_;
};

#endif // DELETEALLFILES_H
