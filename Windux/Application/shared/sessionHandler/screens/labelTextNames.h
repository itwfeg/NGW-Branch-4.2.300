/**
  * @file labelTextNames.h
  * @brief Allows supervisor to edit the Field Header Names that print on labels - if the Label Type and Setup permit.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup labelTextNamesCsu
  * @{
  */

#ifndef LABELTEXTNAMES_H
#define LABELTEXTNAMES_H

#define NUM_LABEL_TEXT_NAMES    20

#include "screenwidget.h"

class QWidget;

namespace Ui {
class labelTextNames;
}

/**
 * @class labelTextNames
 * @brief Allows supervisor to edit the Field Header Names that print on labels - if the Label Type and Setup permit.
 *
 * These include:
 * Field 1:  Packed On
 * Field 2:  Best Before
 * Field 3:  Sell By
 * Field 4:  Net Wt/Ct
 * Field 5:  Unit Price
 * Field 6:  Total Price
 * Field 7 thru 20:  Available as Label Type permits.
 */

class labelTextNames : public ScreenWidget
{
    Q_OBJECT
    
    typedef labelTextNames _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit labelTextNames(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~labelTextNames();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slots for when a label text field is modified.
     *
     */
    void LabelTextChanged_1();
    void LabelTextChanged_2();
    void LabelTextChanged_3();
    void LabelTextChanged_4();
    void LabelTextChanged_5();
    void LabelTextChanged_6();
    void LabelTextChanged_7();
    void LabelTextChanged_8();
    void LabelTextChanged_9();
    void LabelTextChanged_10();
    void LabelTextChanged_11();
    void LabelTextChanged_12();
    void LabelTextChanged_13();
    void LabelTextChanged_14();
    void LabelTextChanged_15();
    void LabelTextChanged_16();
    void LabelTextChanged_17();
    void LabelTextChanged_18();
    void LabelTextChanged_19();
    void LabelTextChanged_20();

protected:

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:
    Ui::labelTextNames *ui;

    /**
     * Builds the parameter string and sends the message to save the updated Label Text.
     *
     */
    void SaveLabelTextData(QString textNo, QWidget *widget);

};

#endif // LABELTEXTNAMES_H
