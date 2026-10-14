/**
 * @file dcadialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "dcadialog.h"

#include "calendardialog.h"
#include "cooldialog.h"

#include "../sessioninfo.h"

#include "../modes/modewidget.h"

#include <backgroundwidgettypes.h>
#include <palette.h>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <Messages.h>

#include <QPixmap>

///////////////////////////////////////////////////////////////////////////////////////////////////
DynamicContentAreaDialog::DynamicContentAreaDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DynamicContentAreaDialog::~DynamicContentAreaDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *DynamicContentAreaDialog::rendererAppearance() const
{
    if ( mainWindow_->isOperateMode() )
        return _Mybase::rendererAppearance();

    return new hobart::darkOperatorBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int DynamicContentAreaDialog::rendererStyle() const
{
    if ( mainWindow_->isOperateMode() )
        return _Mybase::rendererStyle();

    return hobart::ReadOnly;
}
