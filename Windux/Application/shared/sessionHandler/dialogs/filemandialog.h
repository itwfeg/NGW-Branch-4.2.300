/**
  * @file filemandialog.h
  * @brief Dialog for displaying and allowing deletion of files in designated directory.
  *
  * @copyright Copyright (c) 2022, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup filemandialogCsu
  * @{
  */

#ifndef FILEMANDIALOG_H
#define FILEMANDIALOG_H

#include "../sessiondialog.h"

class QTableWidgetItem;

namespace Ui {
class filemandialog;
}

/**
 * @class filemandialog
 * @brief Dialog for displaying and allowing deletion of files.
 *
 */

class filemandialog : public SessionDialog
{
    Q_OBJECT

    typedef filemandialog _Myt;
    typedef SessionDialog _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] dirPath full path of directory to display
     * @param[in] heading dialog heading
     * @param[in] parent  parent widget
     */
    // NEW
    // explicit filemandialog( QWidget *parent = 0, QString dirpath = "", QString heading = "" );
    // ORIG
    explicit filemandialog( QString dirpath, QString heading, QWidget *parent = 0 );


    /**
     * Destructor.
     */
    virtual ~filemandialog();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:

    /**
     * SLOT to exit the dialog
     *
     * @param[in] none
     * @returns  none
     */
    void exitFileMan();

    /**
     * SLOT called when checkbox is checked/unchecked, to determine if Delete button should be enabled
     *
     * @param[in] none
     * @returns  none
     */
    void fileCheckboxChanged( QTableWidgetItem* );

    /**
     * SLOT called for Delete button
     *
     * @param[in] none
     * @returns  none
     */
    void deleteFiles();

private:

    Ui::filemandialog *ui;

    QString dirPath_;
    QString dialogHeading_;

    void initialize();
};

#endif // FILEMANDIALOG_H
