/**
 * @file marqueetest.h
 * Marquee test dialog.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef MARQUEETEST_H
#define MARQUEETEST_H

#include <QDialog>

namespace Ui {
class MarqueeTest;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class MarqueeTest
 * @brief Simple dialog class that tests the @c MarqueeTextEditorWidget and @c MarqueeLabelWidget classes.
 */
class MarqueeTest : public QDialog
{
    Q_OBJECT

    typedef MarqueeTest _Myt;
    typedef QDialog _Mybase;
    
public:

    /**
     * Constructor.
     *
     * @param[in] parent  parent @c QWidget object
     */
    explicit MarqueeTest( QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~MarqueeTest();

private Q_SLOTS:

    /**
     * Slot to recieve when text changes.
     */
    void onTextChanged();

private:

    Ui::MarqueeTest *ui;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MARQUEETEST_H
