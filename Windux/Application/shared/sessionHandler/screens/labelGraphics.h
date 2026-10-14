/**
  * @file labelGraphic.h
  * @brief Allows viewing of label graphics.
  *
  * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup labelGraphicsCsu
  * @{
  */

#ifndef LABELGRAPHICS_H
#define LABELGRAPHICS_H

#include "screenwidget.h"

#include <QDir>
#include <QStringList>


namespace Ui {
class labelGraphics;
}

/**
 * @class labelGraphics
 * @brief Allows viewing of label graphics.
 *
 * This class is used to display System-level and User-level label graphics.
 * The user can navigate pressing the Next and Previous buttons, or by selecting an
 * image from the graphic name dropdown.  The user can also delete User-level
 * graphics by pressing the Delete button.
 */
class labelGraphics : public ScreenWidget
{
    Q_OBJECT
    
    typedef labelGraphics _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit labelGraphics(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor
     */
    virtual ~labelGraphics();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot for when the Previous button is pressed.
     */
    void DisplayPreviousImage();

    /**
     * Slot for when the Next button is pressed.
     */
    void DisplayNextImage();

    /**
     * Slot for when the Delete button is pressed.
     */
    void DeleteImage();

    /**
     * Slot for when an image is selected from the graphic name dropdown.
     *
     * @param[in] index image to delete
     */
    void ImageDropdownChanged(int index);


private:

    Ui::labelGraphics *ui;

    QDir dir;

    QStringList list;
    int count;
    int currentImage;
    bool ignoreSignals_;

    /**
     * Builds a sorted QStringList of System-level and User-level label
     * graphics that are present in the "System/Graphics" and "User/Graphics"
     * folders.
     *
     * @returns count of number of labels in the list
     */
    int GetListOfImages();

    /**
     * Populates the graphic name dropdown from the QStringList
     */
    void PopulateGraphicDropdown();

    /**
     * Displays the Image designated by the index passed in
     *
     * @param[in] index image to display
     */
    void DisplayImage(int index);
};

/** @} */

#endif // LABELGRAPHICS_H
