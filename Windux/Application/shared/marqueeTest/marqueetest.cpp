/**
 * @file marqueetest.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "marqueetest.h"
#include "ui_marqueetest.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
MarqueeTest::MarqueeTest( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::MarqueeTest )
{
    ui->setupUi(this);

    connect(
        ui->marqueeTextEditor,
        SIGNAL( textChanged() ),
        this,
        SLOT( onTextChanged() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MarqueeTest::~MarqueeTest()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeTest::onTextChanged()
{
    ui->marqueeLabel->setText( ui->marqueeTextEditor->toPlainText() );
}
