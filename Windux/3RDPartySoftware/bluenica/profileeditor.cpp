/**
 * @file profileeditor.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "dbtables.h"
#include "profileeditor.h"

#include <QDoubleValidator>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
tempProfileEditorWidget::tempProfileEditorWidget( QWidget *parent ) :
    _Mybase( parent ),
    useCelsius_( false )
{
    // init
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tempProfileEditorWidget::~tempProfileEditorWidget()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void tempProfileEditorWidget::insert()
{
    // create new record from default one
    load( 0 );

    // setup form
    description_->clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void tempProfileEditorWidget::load( unsigned int id )
{
    const unsigned int tempUnitOfMeasure( db_.queryConfigValue( CONFIG_TEMP_UNIT_OF_MEASURE ).toUInt() );

    // set temp uom
    useCelsius_ = (TEMP_UOM_CELSIUS == tempUnitOfMeasure);

    // load sensor info
    record_ = db_.queryProfile( id );

    // setup form
    description_->setText( record_.description );

    warningFrom_->setText( formatDisplayTemp( record_.tempWarningFrom ) );
    normalFrom_->setText( formatDisplayTemp( record_.tempNormalFrom ) );
    normalTo_->setText( formatDisplayTemp( record_.tempNormalTo ) );
    warningTo_->setText( formatDisplayTemp( record_.tempWarningTo ) );

    standard_->setText( formatDisplayTemp( record_.tempStandard ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool tempProfileEditorWidget::remove()
{
    // profile zero is not allowed to be removed
    // profiles that are in use are not allowed to be removed
    if (( 0 == record_.id ) || ( db_.isProfileUsed( record_.id ) ))
    {
        QMessageBox::warning(
            this,
            tr( "Cannot Delete" ),
            tr( "This profile may not be deleted, it is currently in use." ),
            QMessageBox::Ok );

        return false;
    }

    QMessageBox::StandardButton reply(
        QMessageBox::warning(
            this,
            tr( "Confirm Delete" ),
            tr( "Are you sure you want to delete this profile?" ),
            QMessageBox::Yes | QMessageBox::No ) );

    // they chickened out... abort
    if ( QMessageBox::No == reply )
        return false;

    // boom!
    db_.deleteProfile( record_.id );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool tempProfileEditorWidget::save( bool isNewRecord )
{
    // update record
    record_.description = description_->text();
    record_.tempWarningFrom = revertTemp( warningFrom_->text().toDouble() );
    record_.tempNormalFrom = revertTemp( normalFrom_->text().toDouble() );
    record_.tempNormalTo = revertTemp( normalTo_->text().toDouble() );
    record_.tempWarningTo = revertTemp( warningTo_->text().toDouble() );
    record_.tempStandard = revertTemp( standard_->text().toDouble() );

    // validate
    if (( !validateInput( warningFrom_ ) ) ||
        ( !validateInput( normalFrom_ ) ) ||
        ( !validateInput( normalTo_ ) ) ||
        ( !validateInput( warningTo_ ) ) ||
        ( !validateInput( standard_ ) ) ||
        ( record_.description.isEmpty() ) ||
        ( record_.tempWarningTo <= record_.tempNormalTo ) ||
        ( record_.tempNormalTo <= record_.tempStandard ) ||
        ( record_.tempStandard <= record_.tempNormalFrom ) ||
        ( record_.tempNormalFrom <= record_.tempWarningFrom ))
    {
        QMessageBox::StandardButton reply(
            QMessageBox::warning(
                this,
                tr( "Invalid Configuration" ),
                tr( "One or more configuration values are invalid.\n\nDo you wish to exit anyway?" ),
                QMessageBox::Yes | QMessageBox::No ) );

        // stay here
        return (QMessageBox::Yes == reply);
    }

    // insert!!
    if ( isNewRecord )
        db_.insertProfile( record_ );

    // save!!
    else
        db_.updateProfile( record_ );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tempProfileEditorWidget::translate()
{
    static const QChar degree( 0260 ); // degree character in octal

    const QString unitOfMeasure( QString( "%0%1").arg( degree ).arg( useCelsius_ ? "C" : "F" ) );

    // set labels
    descriptionLabel_->setText( tr( "Description:" ) );

    warningFromLabel_->setText( QString( tr( "Warning Temp From (%0):" ) ).arg( unitOfMeasure ) );
    normalFromLabel_->setText( QString( tr( "Normal Temp From (%0):" ) ).arg( unitOfMeasure ) );
    normalToLabel_->setText( QString( tr( "Normal Temp To (%0):" ) ).arg( unitOfMeasure ) );
    warningToLabel_->setText( QString( tr( "Warning Temp To (%0):" ) ).arg( unitOfMeasure ) );

    standardLabel_->setText( QString( tr( "Standard Temp (%0):" ) ).arg( unitOfMeasure ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tempProfileEditorWidget::resizeEvent( QResizeEvent *e )
{
    description_->setFixedWidth( (50 * width()) / 100 );

    warningFrom_->setFixedWidth( (20 * width()) / 100 );
    normalFrom_->setFixedWidth( (20 * width()) / 100 );
    normalTo_->setFixedWidth( (20 * width()) / 100 );
    warningTo_->setFixedWidth( (20 * width()) / 100 );

    standard_->setFixedWidth( (20 * width()) / 100 );

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tempProfileEditorWidget::initialize()
{
    tempValidator_ = new QDoubleValidator( -999.9, 999.9, 1, this );
    tempValidator_->setObjectName( "tempValidator" );

    descriptionLabel_ = new QLabel( this );
    descriptionLabel_->setObjectName( "descriptionLabel" );

    description_ = new QLineEdit( this );
    description_->setObjectName( "description" );

    warningFromLabel_ = new QLabel( this );
    warningFromLabel_->setObjectName( "warningFromLabel" );

    warningFrom_ = new QLineEdit( this );
    warningFrom_->setObjectName( "warningFrom" );
    warningFrom_->setValidator( tempValidator_ );

    normalFromLabel_ = new QLabel( this );
    normalFromLabel_->setObjectName( "normalFromLabel" );

    normalFrom_ = new QLineEdit( this );
    normalFrom_->setObjectName( "normalFrom" );
    normalFrom_->setValidator( tempValidator_ );

    normalToLabel_ = new QLabel( this );
    normalToLabel_->setObjectName( "normalToLabel" );

    normalTo_ = new QLineEdit( this );
    normalTo_->setObjectName( "normalTo" );
    normalTo_->setValidator( tempValidator_ );

    warningToLabel_ = new QLabel( this );
    warningToLabel_->setObjectName( "warningToLabel" );

    warningTo_ = new QLineEdit( this );
    warningTo_->setObjectName( "warningTo" );
    warningTo_->setValidator( tempValidator_ );

    standardLabel_ = new QLabel( this );
    standardLabel_->setObjectName( "standardLabel" );

    standard_ = new QLineEdit( this );
    standard_->setObjectName( "standard" );
    standard_->setValidator( tempValidator_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tempProfileEditorWidget::createLayout()
{
    QFormLayout *configLayoutInner( new QFormLayout() );
    configLayoutInner->addRow( descriptionLabel_, description_ );
    configLayoutInner->addItem( new QSpacerItem( 0, 16 ) );
    configLayoutInner->addRow( warningFromLabel_, warningFrom_ );
    configLayoutInner->addRow( normalFromLabel_, normalFrom_ );
    configLayoutInner->addRow( normalToLabel_, normalTo_ );
    configLayoutInner->addRow( warningToLabel_, warningTo_ );
    configLayoutInner->addItem( new QSpacerItem( 0, 16 ) );
    configLayoutInner->addRow( standardLabel_, standard_ );

    QHBoxLayout *configLayout( new QHBoxLayout() );
    configLayout->addStretch();
    configLayout->addLayout( configLayoutInner );
    configLayout->addStretch();

    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->addStretch();
    layout->addLayout( configLayout );
    layout->addStretch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qreal tempProfileEditorWidget::convertTemp( qreal temp ) const
{
    if ( useCelsius_ )
        return temp;

    temp *= 9.0;
    temp /= 5.0;
    temp += 32.0;

    return temp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qreal tempProfileEditorWidget::revertTemp( qreal temp ) const
{
    if ( useCelsius_ )
        return temp;

    temp -= 32.0;
    temp *= 5.0;
    temp /= 9.0;

    return temp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString tempProfileEditorWidget::formatDisplayTemp( qreal temp ) const
{
    return QString( "%0").arg( convertTemp( temp ), 0, 'f', 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool tempProfileEditorWidget::validateInput( const QLineEdit *w )
{
    const QValidator *v( w->validator() );

    if ( !v )
        return true;

    QString input( w->text() );
    int pos( 0 );

    return (QValidator::Acceptable == v->validate( input, pos ));
}
