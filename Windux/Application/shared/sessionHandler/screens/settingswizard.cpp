/**
 * @file settingswizard.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Todd Davis
 */

#include "settingswizard.h"

#include <QVBoxLayout>

#include <mainwindow.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
SettingsWizard::SettingsWizard( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent )
{
    // initialize wizard
    widget_ = new MainWindow( this );

    // setup layout
    QVBoxLayout *screenLayout = new QVBoxLayout( this );
    screenLayout->addWidget( widget_ );

    widget_->setVisible( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SettingsWizard::~SettingsWizard()
{
}
