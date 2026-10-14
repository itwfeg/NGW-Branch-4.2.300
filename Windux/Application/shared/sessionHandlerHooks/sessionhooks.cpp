/**
 * @file sessionhooks.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "sessionhook.h"
#include "sessionhooks.h"

#include <Messages.h>
#include <Session.h>

#include <QMutexLocker>

QMutex sessionHooksMutex_;
sessionHooks *sessionHooks_ = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
sessionHooks::sessionHooks( Session *s ) :
    _Mybase(),
    m_( QMutex::Recursive ),
    session_( NULL )
{
    // set session pointer
    if ( s )
        setSession( s );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sessionHooks::~sessionHooks()
{
    QMutexLocker guard( &m_ );

    // delete all hooks
    foreach ( sessionHook *hook, hooks_ )
        delete hook;

    hooks_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::setSession( Session *value )
{
    QMutexLocker guard( &m_ );

    // nothing to do
    if ( session_ == value )
        return;

    // disconnect from previous session
    if ( session_ )
    {
        disconnect( session_, 0, this, 0 );
    }

    // set
    session_ = value;

    // connect to new session
    if ( session_ )
    {
        connect( session_, SIGNAL( receive(SessionMessage) ), SLOT( onSessionReceive(SessionMessage) ) );
    }

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->setSession( session_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Session *sessionHooks::session() const
{
    return session_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool sessionHooks::installHook( sessionHook *hook )
{
    // sanity check
    if ( !hook )
        return false;

    QMutexLocker guard( &m_ );

    // check if exists already
    foreach ( sessionHook *h, hooks_ )
        if ( hook->objectName() == h->objectName() )
            return false;

    // update field values
    foreach ( const fieldType &key, fields_.keys() )
        hook->setFieldValue( key, fields_[key] );

    // connect signals/slots
    connect( hook, SIGNAL( changeFieldValue(int,QString) ), SIGNAL( changeFieldValue(int,QString) ) );
    connect( hook, SIGNAL( changeLanguage() ), SIGNAL( changeLanguage() ) );
    connect( hook, SIGNAL( resetPrimaryLanguage() ), SIGNAL( resetPrimaryLanguage() ) );
    connect( hook, SIGNAL( dialogButtonPress(int) ), SIGNAL( dialogButtonPress(int) ) );
    connect( hook, SIGNAL( print() ), SIGNAL( print() ) );
    connect( hook, SIGNAL( softkeyClicked(QWidget*,QWidget*,int,QString,int) ), SIGNAL( softkeyClicked(QWidget*,QWidget*,int,QString,int) ) );
    connect( hook, SIGNAL( shutdownKeyboards() ), SIGNAL( shutdownKeyboards() ) );
    connect( hook, SIGNAL( zeroScale() ), SIGNAL( zeroScale() ) );
    connect( hook, SIGNAL( auditLog() ), SIGNAL( auditLog() ) );

    // setup session pointer
    if ( session_ )
        hook->setSession( session_ );

    // append
    hooks_.append( hook );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::removeHook( const QString& name )
{
    removeHook( findHook( name ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::removeHook( sessionHook *hook )
{
    // sanity check
    if ( !hook )
        return;

    QMutexLocker guard( &m_ );

    // check exists
    if ( !hooks_.contains( hook ) )
        return;

    // reset session pointer
    hook->setSession( NULL );

    // disconnect all signals/slots
    disconnect( hook, 0, this, 0 );

    // remove
    hooks_.removeOne( hook );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sessionHook *sessionHooks::findHook( const QString& name ) const
{
    QMutexLocker guard( &m_ );

    // find hook
    foreach ( sessionHook *hook, hooks_ )
        if ( name == hook->objectName() )
            return hook;

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::destroy()
{
    if ( sessionHooks_ )
    {
        QMutexLocker guard( &sessionHooksMutex_ );

        if ( sessionHooks_ )
        {
            delete sessionHooks_;
            sessionHooks_ = NULL;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sessionHooks *sessionHooks::instance()
{
    if ( !sessionHooks_ )
    {
        QMutexLocker guard( &sessionHooksMutex_ );

        if ( !sessionHooks_ )
            sessionHooks_ = new sessionHooks();
    }

    return sessionHooks_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::onSessionReceive( const SessionMessage& msg )
{
    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // service keys
    if (( SessionDev == device ) && ( SnKeyActivationStates == event ))
    {
        MadpKeyActivation m( msg );

        updateFieldValue( selfServiceKey, m.isSelfServeKey() );
        updateFieldValue( serviceKey, m.isServiceKey() );
        updateFieldValue( supervisorKey, (( m.isSupervisorKey() ) || ( m.isSupervisorF10() )) );

        if (( m.isSelfServeKey() ) || ( m.isServiceKey() ) ||
            ( m.isSupervisorKey() ) || ( m.isSupervisorF10() ))
        {
            loadCurrentState();
        }
    }

    // softkeys
    else if (( SoftkeyDev == device ) && ( LoadSoftkeyGroup == event ))
    {
        if( "1" == sessionHooks::instance()->getFieldValue( selfServiceModeActive ) )
        {

            DomLoadSoftkeyGroup m( msg );

            loadSoftkeyGroup( &m );
        }
    }

    // network status
    else if (( NetworkConnectivityDev == device ) && ( NetworkDeviceType == event ))
    {
        MadpNetworkDeviceType m( msg );

        networkDeviceType( &m );
    }

    // network status
    else if (( NetworkConnectivityDev == device ) && ( CurrentStatus == event ))
    {
        MadpNetworkConnectivity m( msg );

        networkConnectivity( &m );
    }
    else if( ( Weigher == device ) && ( WgWriteAuditTrail == event ) )
    {
        DomAuditLogInformation m(msg);

        callUpdateAuditLogList( &m );
    }
    else if ( ( Weigher == device ) && ( WgSaveAuditTrailToDiskFinished == event ) )
    {
        callUpdateSaveMessage();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::updateFieldValue( fieldType type, const QString& value )
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->setFieldValue( type, value );

    fields_[type] = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::updateFieldValue( fieldType type, const QStringList& valueList )
{
    bool first( true );
    QString value;

    // concat all fields into a single string
    foreach ( const QString& f, valueList )
    {
        if ( first )
            first = false;
        else
            value.append( QChar( 0x1e ) );

        value.append( f );
    }

    updateFieldValue( type, value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::updateFieldValue( const fieldTypeMap& values )
{
    for ( fieldTypeMap::const_iterator i = values.begin(); i != values.end(); ++i )
        updateFieldValue( i.key(), i.value() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString sessionHooks::getFieldValue( fieldType type ) const
{
    return fields_[type];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::removeCameraPredictionsDialog()
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->onRemoveCameraPredictionsDialog();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::showDialog( const QString& title, const QString& message, const buttonMap& buttons )
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->onShowDialog( title, message, buttons );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::showCameraPredictionsDialog( const DomLoadSoftkeyGroup *m )
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->onShowCameraPredictionsDialog( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::dismissDialog()
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->onDismissDialog();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::updateCurrentLanguage( const QString& language )
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->onUpdateCurrentLanguage( language );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::updateCurrentPlaylist( const QString& playlist )
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->onUpdateCurrentPlaylist( playlist );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::updateCurrentItem( itemType type )
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->setCurrentItem( type );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::updateCurrentErrDialog(const QString& code , int buttonstyle)
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->onUpdateCurrentErrDialog( code, buttonstyle );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::dismissCurrentErrDialog( const QString& code, int style )
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->onDismissCurrentErrDialog( code, style );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::beginPrinting( const QString &filename )
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->onBeginPrinting( filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::beginBarcodeOnlyPrinting( const QString &filename )
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->onBeginBarcodeOnlyPrinting( filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::endPrinting()
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->onEndPrinting();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::loadSoftkeyGroup( const DomLoadSoftkeyGroup *m )
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->onLoadSoftkeyGroup( m );
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::loadCurrentState()
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->onLoadCurrentState();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::updateStartState()
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->onUpdateStartState();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::callUpdateAuditLogList( const DomAuditLogInformation *m )
{
    QMutexLocker guard( &m_ );

    foreach ( sessionHook *hook, hooks_ )
      hook->onUpdateAuditLogList( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::callUpdateSaveMessage()
{

    foreach ( sessionHook *hook, hooks_ )
      hook->onUpdateSaveMessage(); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::networkConnectivity( const MadpNetworkConnectivity *m )
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->onNetworkConnectivity( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHooks::networkDeviceType( const MadpNetworkDeviceType *m )
{
    QMutexLocker guard( &m_ );

    // invoke attached hooks
    foreach ( sessionHook *hook, hooks_ )
        hook->onNetworkDeviceType( m );
}
