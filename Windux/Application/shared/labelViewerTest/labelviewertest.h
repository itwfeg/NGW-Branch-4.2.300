/**
 * @file labelviewertest.h
 * Test dialog for label viewing.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef LABELVIEWERTEST_H
#define LABELVIEWERTEST_H

#include <label.h>
#include <datastore.h>
#include <mapdataobjectcontainer.h>

#include <QDialog>

namespace Ui {
class labelViewerTest;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labelViewerTest
 * @brief Dialog for viewing labels.
 */
class labelViewerTest : public QDialog
{
    Q_OBJECT

    typedef labelViewerTest _Myt;
    typedef QDialog _Mybase;
    
public:

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit labelViewerTest( QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~labelViewerTest();

private Q_SLOTS:

    /**
     * Slot for when first is pressed.
     */
    virtual void onFirstButtonPressed();

    /**
     * Slot for when previous is pressed.
     */
    virtual void onPreviousButtonPressed();

    /**
     * Slot for when next is pressed.
     */
    virtual void onNextButtonPressed();

    /**
     * Slot for when last is pressed.
     */
    virtual void onLastButtonPressed();

private:

    Ui::labelViewerTest *ui_;

    dataStore<mapDataObjectContainer> dataStore_;
    dataObjectContainer<label>::const_iterator current_;

    /**
     * Load label data.
     */
    void loadLabelData();

    /**
     * Show current label.
     */
    void showCurrentLabel();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // LABELVIEWERTEST_H
