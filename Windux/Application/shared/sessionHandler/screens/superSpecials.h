/**
 * @file superSpecials.h
 * @brief Widget to set the the graphics shown for specials.
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SUPERSPECIALS_H
#define SUPERSPECIALS_H

#include "screenwidget.h"

class QTableWidgetItem;

namespace Ui {
class superSpecials;
}

/**
* @class superSpecials
* @brief  Changes a product's reporting class
*  
*/

class superSpecials  : public ScreenWidget
{
    Q_OBJECT

    typedef superSpecials _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit superSpecials(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superSpecials();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:

    /**
     * SLOT to improve existing HLX code by preventing spaces in hostnames
     *
     * @param[in] none
     * @returns  none
     */
     void preventSpacesInSpecialNumber(const QString&);

     /**
      * SLOT to delete the selected Special
      *
      * @param[in] none
      * @returns  none
      */
     void deleteNow();

     /**
      * SLOT to detect that a User selects a row and populates entry fields with data
      *
      * @param[in] none
      * @returns  none
      */
     void rowSelected(QTableWidgetItem* item);

     /**
      * SLOT to detect that a User selects a different item in the primary combobox
      *
      * @param[in] none
      * @returns  none
      */
     // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
     //void primaryComboChanged(const QString&);

     /**
      * SLOT to detect that a User selects a different item in the secondary combobox
      *
      * @param[in] none
      * @returns  none
      */
     // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
     //void secondaryComboChanged(const QString&);

     /**
      * SLOT to update the Description in the table widget
      *
      * @param[in] none
      * @returns  none
      */
     void updateDescription();

     /**
      * SLOT to handle new special creation
      *
      * @param[in] none
      * @returns  none
      */
     void OKButClicked();

     /**
      * SLOT to handle cancel new special creation
      *
      * @param[in] none
      * @returns  none
      */
     void CancelButClicked();

     /**
      * SLOT to perform a Special Look Up
      *
      * @param[in] none
      * @returns  none
      */
     void SLU();

protected:
     /**
      * Handle screen changing.
      *
      * This method is called before the screen is changed to something new. If for some reason the
      * screen should not be unloaded and destroyed then a value of @c false should be returned.
      *
      * @returns  @c true to unload screen, @c false to keep screen resident
      */
     virtual bool onScreenChanging();

     /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:
    Ui::superSpecials *ui;

    /**
     * Method to save the SFTP server info
     *
     * @param[in] none
     * @returns  none
     */
    void saveSpecialsInfo();

    /**
     * Method to show new SLU groupbox asking User decision to create new Special
     *
     * @param[in] none
     * @returns  none
     */
    void popupSLU();

    void requestSLU();
    void removeSpecialFromTV(QString specialText);
    void clearTheFields();
    void setTableHeadings( int numberOfRows );

    QStringList graphicsFileNames;

    bool loading;
    bool isNewSLU;
    int lastRow;
    int currentRow;

    bool creatingNewSpecial;
    QString specialNumber;
    QFont fnt;

    QString specialNumberToSave;
    QString descriptionToSave;
    // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
    //QString primaryToSave;
    //QString secondaryToSave;

    // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
    //QString primaryName;
    //QString secondaryName;
};

#endif // SUPERSPECIALS
