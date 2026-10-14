/**
 * @file devtesthook.h
 * Hook for developer testing.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef DEVTESTHOOK_H
#define DEVTESTHOOK_H

#include "sessionhook.h"

namespace hobart {
    class selfServiceSoftkeysWidget;
}

class QButtonGroup;
class QDialog;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QTabWidget;
class QWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Hook for developer testing.
class developmentTestHook : public sessionHook
{
    Q_OBJECT

    typedef developmentTestHook _Myt;
    typedef sessionHook _Mybase;

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * @brief CTOR.
     * @param[in] s  pointer to session
     * @param[in] parent  parent object
     */
    developmentTestHook( Session *s, QObject *parent = NULL );

    /// DTOR
    virtual ~developmentTestHook();

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

    /// Dismiss active dialog.
    virtual void onDismissDialog();

    /**
     * @brief  Load softkey group.
     * @param[in] m  pointer to message
     */
    virtual void onLoadSoftkeyGroup( const DomLoadSoftkeyGroup* m );

    /**
     * @brief  Show dialog on screen.
     * @param[in] title  dialog title
     * @param[in] message  dialog message
     * @param[in] buttons  dialog buttons
     */
    virtual void onShowDialog( const QString& title, const QString& message, const buttonMap& buttons );

    /**
     * @brief  Update current item.
     * @param[in] type  item type
     */
    virtual void onUpdateCurrentItem( itemType type );

    /**
     * @brief Update current language.
     * @param[in] language  new language
     */
    virtual void onUpdateCurrentLanguage( const QString& language );

    /**
     * @brief Update current playlist.
     * @param[in] playlist  new playlist
     */
    virtual void onUpdateCurrentPlaylist( const QString& language );

    /**
     * @brief  Update field value.
     * @param[in] type  field type
     * @param[in] value  field value
     */
    virtual void onUpdateFieldValue( fieldType type, const QString& value );

private Q_SLOTS:

    /// Slot for by count text changed.
    void onByCountTextChanged( const QString& value );

    /// Slot for plu editing finished.
    void onPluEditingFinished();

private:

    QDialog *d_;

    QTabWidget *tabs_;

    QWidget *mainTab_;
    QWidget *flashkeysTab_;

    //
    // main tab
    //

    QGroupBox *error_;
    QLabel *errorTitle_;
    QLabel *errorMessage_;

    QGroupBox *errorButtons_;
    QHBoxLayout *errorButtonsLayout_;
    QButtonGroup *errorButtonsGroup_;

    QGroupBox *weight_;
    QLabel *capacityLabel_;
    QLabel *capacity_;
    QLabel *capHiRangeLabel_;
    QLabel *capHiRange_;
    QLabel *capLoRangeLabel_;
    QLabel *capLoRange_;
    QLabel *netWeightLabel_;
    QLabel *netWeight_;
    QLabel *zeroLabel_;
    QLabel *zero_;

    QGroupBox *item_;
    QLabel *itemCodeLabel_;
    QLabel *itemCode_;
    QLabel *itemDescLabel_;
    QLabel *itemDesc_;
    QLabel *itemByCountLabel_;
    QLabel *itemByCount_;
    QLabel *itemTareLabel_;
    QLabel *itemTare_;
    QLabel *itemUnitPriceLabel_;
    QLabel *itemUnitPrice_;
    QLabel *itemTotalPriceLabel_;
    QLabel *itemTotalPrice_;
    QLabel *itemImageLabel_;
    QLabel *itemImage_;
    QLabel *itemExTextLabel_;
    QLabel *itemExText_;
    QLabel *itemNutrifactsLabel_;
    QLabel *itemNutrifacts_;
    QPushButton *print_;

    QPlainTextEdit *log_;

    QPushButton *changeLanguage_;
    QPushButton *zeroScale_;
    QPushButton *auditLog_;
    QLabel *pluLabel_;
    QLineEdit *plu_;
    QLabel *byCountLabel_;
    QLineEdit *byCount_;

    //
    // flashkeys tab
    //

    hobart::selfServiceSoftkeysWidget *softkeys_;

    // not implemented
    developmentTestHook( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

    /// Initialize object.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Update item information.
    void updateItemInfo();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // DEVTESTHOOK_H
