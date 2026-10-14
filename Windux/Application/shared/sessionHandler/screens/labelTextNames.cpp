/**
 * @file labelTextNames.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "labelTextNames.h"
#include "ui_labelTextNames.h"

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"

#include <QtGui>

labelTextNames::labelTextNames(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::labelTextNames)
{
    ui->setupUi(this);
    scaleScreen();

    //------------------------------------------------------------
    // Set the connections for when a label text field is changed.
    //
    // If a label text field is changed is it saved immediately,
    //  ... not when the page is exited.
    //------------------------------------------------------------
    connect(ui->txtLabelText_1, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_1()));
    connect(ui->txtLabelText_2, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_2()));
    connect(ui->txtLabelText_3, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_3()));
    connect(ui->txtLabelText_4, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_4()));
    connect(ui->txtLabelText_5, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_5()));
    connect(ui->txtLabelText_6, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_6()));
    connect(ui->txtLabelText_7, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_7()));
    connect(ui->txtLabelText_8, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_8()));
    connect(ui->txtLabelText_9, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_9()));
    connect(ui->txtLabelText_10, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_10()));
    connect(ui->txtLabelText_11, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_11()));
    connect(ui->txtLabelText_12, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_12()));
    connect(ui->txtLabelText_13, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_13()));
    connect(ui->txtLabelText_14, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_14()));
    connect(ui->txtLabelText_15, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_15()));
    connect(ui->txtLabelText_16, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_16()));
    connect(ui->txtLabelText_17, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_17()));
    connect(ui->txtLabelText_18, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_18()));
    connect(ui->txtLabelText_19, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_19()));
    connect(ui->txtLabelText_20, SIGNAL(editingFinished()), this, SLOT(LabelTextChanged_20()));

    //-------------------------------------
    // Request the list of Label Text Names
    //-------------------------------------
    session_->send( GenericTextDev, GenericDtReadAll );

    //--------------------------------------
    // Request the "Generic Text for Labels"
    //--------------------------------------
    session_->send( GenericTextDev, GenericDtRead, "1" );

    translate();
}

labelTextNames::~labelTextNames()
{
    delete ui;
}

void labelTextNames::translate()
{
    ui->retranslateUi(this);

    update();
}

void labelTextNames::onSessionMessage( const SessionMessage& msg )
{
    //--------------------------------------------------
    // Populate the text boxes with the Label Text Names
    //--------------------------------------------------
    if(( GenericTextDev == msg[0].toInt() ) && ( GenericDtWriteAll == msg[1].toInt() ))
    {
        DomGenericTextList m( msg );

        int numTexts = m.numOfTexts().toInt();

        // Get the list of type names
        QVector<QString> namesList = TokenParser::splitStringTokens(m.textList());

        for (int i = 0; i < numTexts; ++i)
        {
            // Update the correct label text field with the value
            QLineEdit *lineEdit = this->findChild<QLineEdit *>( "txtLabelText_" + QString::number(i + 1) );
            lineEdit->setText(namesList.at(i));
        }
   }

    //--------------------------------------
    // Process the "Generic Text for Labels"
    //--------------------------------------
    if ((GenericTextDev == msg[0].toInt() ) && ( GenericDtWrite == msg[1].toInt() ))
    {
        DomGenericText m( msg );

        QString recordID = m.recordId();
        QString genericText = m.text();

        // Update the correct label text field with the value
        // and set its focus
        QLineEdit *lineEdit = this->findChild<QLineEdit *> ( "txtLabelText_" + recordID );
        lineEdit->setText( genericText );
//        lineEdit->setFocus(); // Windux code set focus. This causes keyboard to pop on Qt app
    }
}

//----------------------------------------------------------
// Label Text field has changed, so it needs to be saved now
//----------------------------------------------------------
void labelTextNames::LabelTextChanged_1()
{
    SaveLabelTextData("1", ui->txtLabelText_1);
}

void labelTextNames::LabelTextChanged_2()
{
    SaveLabelTextData("2", ui->txtLabelText_2);
}

void labelTextNames::LabelTextChanged_3()
{
    SaveLabelTextData("3", ui->txtLabelText_3);
}

void labelTextNames::LabelTextChanged_4()
{
    SaveLabelTextData("4", ui->txtLabelText_4);
}

void labelTextNames::LabelTextChanged_5()
{
    SaveLabelTextData("5", ui->txtLabelText_5);
}

void labelTextNames::LabelTextChanged_6()
{
    SaveLabelTextData("6", ui->txtLabelText_6);
}

void labelTextNames::LabelTextChanged_7()
{
    SaveLabelTextData("7", ui->txtLabelText_7);
}

void labelTextNames::LabelTextChanged_8()
{
    SaveLabelTextData("8", ui->txtLabelText_8);
}

void labelTextNames::LabelTextChanged_9()
{
    SaveLabelTextData("9", ui->txtLabelText_9);
}

void labelTextNames::LabelTextChanged_10()
{
    SaveLabelTextData("10", ui->txtLabelText_10);
}

void labelTextNames::LabelTextChanged_11()
{
    SaveLabelTextData("11", ui->txtLabelText_11);
}

void labelTextNames::LabelTextChanged_12()
{
    SaveLabelTextData("12", ui->txtLabelText_12);
}

void labelTextNames::LabelTextChanged_13()
{
    SaveLabelTextData("13", ui->txtLabelText_13);
}

void labelTextNames::LabelTextChanged_14()
{
    SaveLabelTextData("14", ui->txtLabelText_14);
}

void labelTextNames::LabelTextChanged_15()
{
    SaveLabelTextData("15", ui->txtLabelText_15);
}

void labelTextNames::LabelTextChanged_16()
{
    SaveLabelTextData("16", ui->txtLabelText_16);
}

void labelTextNames::LabelTextChanged_17()
{
    SaveLabelTextData("17", ui->txtLabelText_17);
}

void labelTextNames::LabelTextChanged_18()
{
    SaveLabelTextData("18", ui->txtLabelText_18);
}

void labelTextNames::LabelTextChanged_19()
{
    SaveLabelTextData("19", ui->txtLabelText_19);
}

void labelTextNames::LabelTextChanged_20()
{
    SaveLabelTextData("20", ui->txtLabelText_20);
}

//-----------------------------------------------
// Build the string containing the Label Text and
// send the message to update the database
//-----------------------------------------------
void labelTextNames::SaveLabelTextData(QString textNo, QWidget *widget)
{
    QLineEdit *lineEdit = dynamic_cast<QLineEdit*> ( widget );
    QString value = lineEdit->text();

    QString param;

    param = textNo;
    param += ",\"" + value + "\"";

    session_->send( GenericTextDev, GenericDtSave, param );
}

//----------------------------------------
// Build the text fields dynamically . . .
//----------------------------------------
//void labelTextNames::initialize()
//{
//    scrollLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);

//    for(int i = 0; i < NUM_LABEL_TEXT_NAMES; ++i)
//    {
//        groupLayout = new QHBoxLayout();

//        // Row number
//        lblText = new QLabel(QString::number(i + 1), this);
//        lblText->setGeometry( QRect( 1, 1, 30, 80) );
//        groupLayout->addWidget(lblText);

//        // Text edit box
//        txtLabelText = new QLineEdit(this);
//        txtLabelText->setObjectName( "txtLabelText_" + QString::number(i) );
//        txtLabelText->setGeometry( QRect( 1, 1, 835, 80) );
//        groupLayout->addWidget(txtLabelText);

//        scrollLayout->addLayout(groupLayout);
//    }
//}

