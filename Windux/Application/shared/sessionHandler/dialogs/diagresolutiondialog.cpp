/**
 * @file diagresolutiondialog.cpp
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "diagresolutiondialog.h"

#include "../sessionmainwindow.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <numericlineedit.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagnosticResolutionDialog::DiagnosticResolutionDialog( int code, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f )
{
    setObjectName( "DiagnosticResolutionDialog" );

    // initialize
    initialize();
    createLayout();

    // request service actions
    session_->send( Diagnostics, DiagServiceActionsReq, QString::number( code ) );

    // set title
    header_->setText( tr( "DIAGNOSTIC CODE" ) + " " + QString::number( code ) );

    // start timer
    timer_->setSingleShot( false );
    timer_->start( 1000 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagnosticResolutionDialog::~DiagnosticResolutionDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int DiagnosticResolutionDialog::serviceMethod( int index ) const
{
    if ( 0 == index )
        return serviceMethod1_->currentData().toInt();
    else if ( 1 == index )
        return serviceMethod2_->currentData().toInt();
    else if ( 2 == index )
        return serviceMethod3_->currentData().toInt();
    else if ( 3 == index )
        return serviceMethod4_->currentData().toInt();

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString DiagnosticResolutionDialog::serviceNotes() const
{
    return serviceNotes_->toPlainText();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int DiagnosticResolutionDialog::servicePart( int index ) const
{
    if ( 0 == index )
        return servicePart1_->currentData().toInt();
    else if ( 1 == index )
        return servicePart2_->currentData().toInt();
    else if ( 2 == index )
        return servicePart3_->currentData().toInt();
    else if ( 3 == index )
        return servicePart4_->currentData().toInt();

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize DiagnosticResolutionDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 800) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 500) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticResolutionDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "DiagnosticResolutionDialog", "onSessionMessageReceive" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // version info
    if (( Diagnostics == device ) && ( DiagServiceActions == event ))
    {
        DiagServiceActionsMsg m( msg );

        // retrieve actions
        if ( m.count() )
        {
            QVector<QString> actions( TokenParser::splitStringTokens( m.actions() ) );

            if ( actions.count() != m.count() )
                LOG_WARN( "mismatch in action count %d %d", m.count(), actions.count() );
            else
            {
                // process actions
                foreach ( const QString &action, actions )
                {
                    QVector<QString> fields( TokenParser::splitStringTokens( action ) );

                    // record structure:
                    // 0 category
                    // 1 method
                    // 2 part
                    // 3 order
                    // 4 categoryName
                    // 5 methodName
                    // 6 partName
                    // 7 notesRequired

                    // validate structure
                    if ( 8 != fields.count() )
                        LOG_ERROR( "unknown record structure for action!", NULL );
                    else
                    {
                        const methodPartKey key( fields[1].toInt(), fields[2].toInt() );

                        // save method
                        methods_[key.first] = fields[5];

                        if ( !methodOrder_.contains( key.first ) )
                            methodOrder_.append( key.first );

                        // save part
                        const partName part( fields[6], (0 != fields[7].toInt()) );

                        parts_[key] = part;
                        partOrder_.append( key );
                    }

                } // for each action

                // insert default method
                methods_[0] = tr( "SELECT METHOD" );
                methodOrder_.prepend( 0 );

                // insert default parts
                foreach ( const int &i, methodOrder_ )
                {
                    const methodPartKey key( i, 0 );
                    const partName part( tr( "SELECT PART" ), false );

                    parts_[key] = part;
                    partOrder_.prepend( key );
                }

                populateMethods();
                populateParts( serviceMethod1_, servicePart1_ );
                populateParts( serviceMethod2_, servicePart2_ );
                populateParts( serviceMethod3_, servicePart3_ );
                populateParts( serviceMethod4_, servicePart4_ );

                connect( serviceMethod1_, SIGNAL(currentIndexChanged(int)), SLOT(onCurrentIndexChanged(int)) );
                connect( servicePart1_, SIGNAL(currentIndexChanged(int)), SLOT(onCurrentIndexChanged(int)) );

                connect( serviceMethod2_, SIGNAL(currentIndexChanged(int)), SLOT(onCurrentIndexChanged(int)) );
                connect( servicePart2_, SIGNAL(currentIndexChanged(int)), SLOT(onCurrentIndexChanged(int)) );

                connect( serviceMethod3_, SIGNAL(currentIndexChanged(int)), SLOT(onCurrentIndexChanged(int)) );
                connect( servicePart3_, SIGNAL(currentIndexChanged(int)), SLOT(onCurrentIndexChanged(int)) );

                connect( serviceMethod4_, SIGNAL(currentIndexChanged(int)), SLOT(onCurrentIndexChanged(int)) );
                connect( servicePart4_, SIGNAL(currentIndexChanged(int)), SLOT(onCurrentIndexChanged(int)) );

            } // actions == parsed actions
        } // actions
    }

    _Mybase::onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticResolutionDialog::onCurrentIndexChanged( int index )
{
    Q_UNUSED( index );

    if ( serviceMethod1_ == sender() )
        populateParts( serviceMethod1_, servicePart1_ );
    else if ( serviceMethod2_ == sender() )
        populateParts( serviceMethod2_, servicePart2_ );
    else if ( serviceMethod3_ == sender() )
        populateParts( serviceMethod3_, servicePart3_ );
    else if ( serviceMethod4_ == sender() )
        populateParts( serviceMethod4_, servicePart4_ );

    servicePart1_->setEnabled( 0 < serviceMethod1_->currentIndex() );
    servicePart2_->setEnabled( 0 < serviceMethod2_->currentIndex() );
    servicePart3_->setEnabled( 0 < serviceMethod3_->currentIndex() );
    servicePart4_->setEnabled( 0 < serviceMethod4_->currentIndex() );

    serviceActionLabel2_->setVisible( 0 < servicePart1_->currentIndex() );
    serviceMethod2_->setVisible( 0 < servicePart1_->currentIndex() );
    servicePart2_->setVisible( 0 < servicePart1_->currentIndex() );

    serviceActionLabel3_->setVisible( 0 < servicePart2_->currentIndex() );
    serviceMethod3_->setVisible( 0 < servicePart2_->currentIndex() );
    servicePart3_->setVisible( 0 < servicePart2_->currentIndex() );

    serviceActionLabel4_->setVisible( 0 < servicePart3_->currentIndex() );
    serviceMethod4_->setVisible( 0 < servicePart3_->currentIndex() );
    servicePart4_->setVisible( 0 < servicePart3_->currentIndex() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticResolutionDialog::onPushButtonClicked()
{
    if ( okayButton_ == sender() )
        accept();
    else if ( cancelButton_ == sender() )
        reject();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticResolutionDialog::onTimerTimeout()
{
    bool okayEnabled( true );

    // at least one part must be selected
    if (( !serviceMethod1_->count() ) || ( serviceMethod1_->currentIndex() <= 0 ))
        okayEnabled = false;
    else if (( !servicePart1_->count() ) || ( servicePart1_->currentIndex() <= 0 ))
        okayEnabled = false;

    // next we check for notes required
    else
    {
        bool notesRequired( false );

        const methodPartKey key1( serviceMethod1_->currentData().toInt(), servicePart1_->currentData().toInt() );
        const methodPartKey key2( serviceMethod2_->currentData().toInt(), servicePart2_->currentData().toInt() );
        const methodPartKey key3( serviceMethod3_->currentData().toInt(), servicePart3_->currentData().toInt() );
        const methodPartKey key4( serviceMethod4_->currentData().toInt(), servicePart4_->currentData().toInt() );

        notesRequired |= parts_[key1].second;
        notesRequired |= parts_[key2].second;
        notesRequired |= parts_[key3].second;
        notesRequired |= parts_[key4].second;

        if ( notesRequired )
            okayEnabled &= ( !serviceNotes_->toPlainText().isEmpty() );
    }

    // no empty fields!
    // allow them to accept form when filled out
    okayButton_->setEnabled( okayEnabled );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticResolutionDialog::initialize()
{
    modifyFontPointSize setFontPointSize14( 14 );
    modifyFontPointSize setFontPointSize18( 18 );

    modifyPaletteForeground setPaletteForeground(
        modifyPaletteForeground::createPalette(
            hobart::colorPalette::black,
            hobart::colorPalette::lightGray ) );

    setFontPointSize14( this );

    timer_ = new QTimer( this );

    connect( timer_, SIGNAL(timeout()), SLOT(onTimerTimeout()) );

    // ---- //

    // header
    header_ = new QLabel( this );
    header_->setObjectName( "header" );

    setFontPointSize18( header_ );

    // service method label
    serviceMethodLabel_ = new QLabel( tr( "Resolution Methods:" ), this );
    serviceMethodLabel_->setObjectName( "serviceMethodLabel" );

    setFontPointSize18( serviceMethodLabel_ );

    // service action 1
    serviceActionLabel1_ = new QLabel( tr( "Action 1" ), this );
    serviceActionLabel1_->setObjectName( "serviceActionLabel1" );

    setFontPointSize18( serviceActionLabel1_ );

    serviceMethod1_ = new QComboBox( this );
    serviceMethod1_->setObjectName( "serviceMethod1" );

    servicePart1_ = new QComboBox( this );
    servicePart1_->setObjectName( "servicePart1" );
    servicePart1_->setEnabled( false );

    // service action 2
    serviceActionLabel2_ = new QLabel( tr( "Action 2" ), this );
    serviceActionLabel2_->setObjectName( "serviceActionLabel2" );
    serviceActionLabel2_->setVisible( false );

    setFontPointSize18( serviceActionLabel2_ );

    serviceMethod2_ = new QComboBox( this );
    serviceMethod2_->setObjectName( "serviceMethod2" );
    serviceMethod2_->setVisible( false );

    servicePart2_ = new QComboBox( this );
    servicePart2_->setObjectName( "servicePart2" );
    servicePart2_->setVisible( false );

    // service action 3
    serviceActionLabel3_ = new QLabel( tr( "Action 3" ), this );
    serviceActionLabel3_->setObjectName( "serviceActionLabel3" );
    serviceActionLabel3_->setVisible( false );

    setFontPointSize18( serviceActionLabel3_ );

    serviceMethod3_ = new QComboBox( this );
    serviceMethod3_->setObjectName( "serviceMethod3" );
    serviceMethod3_->setVisible( false );

    servicePart3_ = new QComboBox( this );
    servicePart3_->setObjectName( "servicePart3" );
    servicePart3_->setVisible( false );

    // service action 4
    serviceActionLabel4_ = new QLabel( tr( "Action 4" ), this );
    serviceActionLabel4_->setObjectName( "serviceActionLabel4" );
    serviceActionLabel4_->setVisible( false );

    setFontPointSize18( serviceActionLabel4_ );

    serviceMethod4_ = new QComboBox( this );
    serviceMethod4_->setObjectName( "serviceMethod4" );
    serviceMethod4_->setVisible( false );

    servicePart4_ = new QComboBox( this );
    servicePart4_->setObjectName( "servicePart4" );
    servicePart4_->setVisible( false );

    // service notes label
    serviceNotesLabel_ = new QLabel( tr( "Notes:" ), this );
    serviceNotesLabel_->setObjectName( "serviceNotesLabel" );

    setFontPointSize18( serviceNotesLabel_ );

    // service notes
    serviceNotes_ = new QPlainTextEdit( this );
    serviceNotes_->setObjectName( "serviceNotes" );

    // okay
    okayButton_ = new QPushButton( tr( "Accept" ), this );
    okayButton_->setEnabled( false );
    okayButton_->setObjectName( "okayButton" );

    setFontPointSize18( okayButton_ );

    connect( okayButton_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    // okay
    cancelButton_ = new QPushButton( tr( "Cancel" ), this );
    cancelButton_->setObjectName( "cancelButton" );

    setFontPointSize18( cancelButton_ );

    connect( cancelButton_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    // ---- //

    QWidgetList widgets;
    widgets.append( serviceMethod1_ );
    widgets.append( servicePart1_ );
    widgets.append( serviceMethod2_ );
    widgets.append( servicePart2_ );
    widgets.append( serviceMethod3_ );
    widgets.append( servicePart3_ );
    widgets.append( serviceMethod4_ );
    widgets.append( servicePart4_ );
    widgets.append( serviceNotes_ );
    widgets.append( okayButton_ );
    widgets.append( cancelButton_ );

    // modify foreground colors
    foreach ( QWidget *w, widgets )
        setPaletteForeground( w );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticResolutionDialog::createLayout()
{
    QVBoxLayout *actionLabels( new QVBoxLayout() );
    actionLabels->addWidget( serviceActionLabel1_ );
    actionLabels->addWidget( serviceActionLabel2_ );
    actionLabels->addWidget( serviceActionLabel3_ );
    actionLabels->addWidget( serviceActionLabel4_ );

    QVBoxLayout *methods( new QVBoxLayout() );
    methods->addWidget( serviceMethod1_ );
    methods->addWidget( serviceMethod2_ );
    methods->addWidget( serviceMethod3_ );
    methods->addWidget( serviceMethod4_ );

    QVBoxLayout *parts( new QVBoxLayout() );
    parts->addWidget( servicePart1_ );
    parts->addWidget( servicePart2_ );
    parts->addWidget( servicePart3_ );
    parts->addWidget( servicePart4_ );

    QHBoxLayout *actions( new QHBoxLayout() );
    actions->addLayout( actionLabels );
    actions->addLayout( methods, 1 );
    actions->addLayout( parts, 1 );

    QHBoxLayout *buttons( new QHBoxLayout() );
    buttons->addStretch();
    buttons->addWidget( okayButton_ );
    buttons->addWidget( cancelButton_ );

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->addWidget( header_ );
    screenLayout->addSpacing( 16 );
    screenLayout->addWidget( serviceMethodLabel_ );
    screenLayout->addLayout( actions );
    screenLayout->addSpacing( 16 );
    screenLayout->addWidget( serviceNotesLabel_ );
    screenLayout->addWidget( serviceNotes_, 1 );
    screenLayout->addSpacing( 16 );
    screenLayout->addLayout( buttons );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticResolutionDialog::populateMethods()
{
    serviceMethod1_->clear();
    serviceMethod2_->clear();
    serviceMethod3_->clear();
    serviceMethod4_->clear();

    // insert methods
    foreach ( const int &i, methodOrder_ )
    {
        const QString name( methods_[i] );

        serviceMethod1_->addItem( name, QVariant( i ) );
        serviceMethod2_->addItem( name, QVariant( i ) );
        serviceMethod3_->addItem( name, QVariant( i ) );
        serviceMethod4_->addItem( name, QVariant( i ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticResolutionDialog::populateParts( QComboBox *serviceMethod, QComboBox *servicePart )
{
    const QVariant method( serviceMethod->currentData() );

    servicePart->clear();

    // insert parts
    foreach ( const methodPartKey &key, partOrder_ )
        if ( method.toInt() == key.first )
            servicePart->addItem( parts_[key].first, QVariant( key.second ) );

}
