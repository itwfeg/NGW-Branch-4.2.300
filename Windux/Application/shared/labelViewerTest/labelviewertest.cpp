/**
 * @file labelviewertest.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "labelviewertest.h"
#include "ui_labelviewertest.h"

#include "labelviewertestdata.h"

#include <QFile>

///////////////////////////////////////////////////////////////////////////////////////////////////
labelViewerTest::labelViewerTest( QWidget *parent ) :
    _Mybase( parent ),
    ui_( new Ui::labelViewerTest )
{
    // setup the ui
    ui_->setupUi( this );

    // set the window title
    setWindowTitle( "Label Viewer" );

    // connect up some slots
    connect( ui_->first, SIGNAL( pressed() ), this, SLOT( onFirstButtonPressed() ) );
    connect( ui_->previous, SIGNAL( pressed() ), this, SLOT( onPreviousButtonPressed() ) );
    connect( ui_->next, SIGNAL( pressed() ), this, SLOT( onNextButtonPressed() ) );
    connect( ui_->last, SIGNAL( pressed() ), this, SLOT( onLastButtonPressed() ) );

    // load some label data
    loadLabelData();

    ui_->labelViewer->setStore( &dataStore_ );
    ui_->labelViewer->setNutrifactsId( 23 );

    // show the current label
    showCurrentLabel();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labelViewerTest::~labelViewerTest()
{
    delete ui_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelViewerTest::onFirstButtonPressed()
{
    current_ = dataStore_.store<label>()->begin();
    showCurrentLabel();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelViewerTest::onPreviousButtonPressed()
{
    if ( current_ != dataStore_.store<label>()->begin() )
    {
        --current_;
        showCurrentLabel();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelViewerTest::onNextButtonPressed()
{
    dataObjectContainer<label>::const_iterator i = current_;

    if ( ++i != dataStore_.store<label>()->end() )
    {
        current_ = i;
        showCurrentLabel();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelViewerTest::onLastButtonPressed()
{
    dataObjectContainer<label>::const_iterator i = current_;

    if ( ++i != dataStore_.store<label>()->end() )
    {
        current_ = i;

        while ( ++i != dataStore_.store<label>()->end() )
            current_ = i;

        showCurrentLabel();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelViewerTest::loadLabelData()
{
    dataStore_.store<label>()->insert( label( label_21, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_22, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_23, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_24, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_25, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_26, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_51, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_52, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_53, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_54, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_55, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_56, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_57, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_101, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_102, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_103, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_104, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_105, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_106, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_111, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_112, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_113, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_114, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_115, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_121, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_122, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_123, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_124, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_125, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_153, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_154, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_155, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_249, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_323, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_324, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_325, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_334, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_335, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_345, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_355, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_9901, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_9903, base16_encoding ) );
    dataStore_.store<label>()->insert( label( label_9904, base16_encoding ) );

    dataStore_.store<systemString>()->insert( systemString( 1, label_text_1 ) );
    dataStore_.store<systemString>()->insert( systemString( 2, label_text_2 ) );
    dataStore_.store<systemString>()->insert( systemString( 3, label_text_3 ) );
    dataStore_.store<systemString>()->insert( systemString( 4, label_text_4 ) );
    dataStore_.store<systemString>()->insert( systemString( 5, label_text_5 ) );
    dataStore_.store<systemString>()->insert( systemString( 6, label_text_6 ) );
    dataStore_.store<systemString>()->insert( systemString( 7, label_text_7 ) );
    dataStore_.store<systemString>()->insert( systemString( 8, label_text_8 ) );
    dataStore_.store<systemString>()->insert( systemString( 9, label_text_9 ) );
    dataStore_.store<systemString>()->insert( systemString( 10, label_text_10 ) );
    dataStore_.store<systemString>()->insert( systemString( 11, label_text_11 ) );
    dataStore_.store<systemString>()->insert( systemString( 12, label_text_12 ) );
    dataStore_.store<systemString>()->insert( systemString( 13, label_text_13 ) );
    dataStore_.store<systemString>()->insert( systemString( 14, label_text_14 ) );
    dataStore_.store<systemString>()->insert( systemString( 15, label_text_15 ) );
    dataStore_.store<systemString>()->insert( systemString( 16, label_text_16 ) );
    dataStore_.store<systemString>()->insert( systemString( 17, label_text_17 ) );
    dataStore_.store<systemString>()->insert( systemString( 18, label_text_18 ) );
    dataStore_.store<systemString>()->insert( systemString( 19, label_text_19 ) );
    dataStore_.store<systemString>()->insert( systemString( 20, label_text_20 ) );

    nutrifact n( 23 );
    n.columns().insert( 0, nutrifactColumn( QString( nutrifacts_23_0 ) ) );
    n.columns().insert( 1, nutrifactColumn( QString( nutrifacts_23_1 ) ) );
    dataStore_.store<nutrifact>()->insert( n );

    current_ = dataStore_.store<label>()->begin();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelViewerTest::showCurrentLabel()
{
    dataObjectContainer<label>::const_iterator i = current_;

    bool begin = (current_ == dataStore_.store<label>()->begin());
    bool end = (++i == dataStore_.store<label>()->end());

    ui_->first->setEnabled( !begin );
    ui_->previous->setEnabled( !begin );
    ui_->next->setEnabled( !end );
    ui_->last->setEnabled( !end );

    ui_->labelViewer->setLabel( *current_ );

    // set the window title
    QString title( "Label Viewer - %1" );
    setWindowTitle( title.arg( current_->id() ) );
}
