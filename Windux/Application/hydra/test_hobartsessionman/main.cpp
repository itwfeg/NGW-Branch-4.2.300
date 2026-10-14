/**
 * @file main.cpp
 * @brief Application entry point.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include <messagecenter_interface.h>
#include <sessioneventtype.h>
#include <session_interface.h>
#include <sessionmanager_interface.h>

#include <cassert>

#include <QCoreApplication>
#include <QDebug>

static const int SLEEP_TIME( 1000 );

///////////////////////////////////////////////////////////////////////////////////////////////////
void sleepFor( uint ms )
{
    const QDateTime until( QDateTime::currentDateTime().addMSecs( ms ) );

    do
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );

    } while ( QDateTime::currentDateTime() < until );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void testInterface()
{
    // create interface to session manager
    com::hobartcorp::SessionManager *sessionMan = new com::hobartcorp::SessionManager(
        SESSION_MANAGER_DBUS_SERVICE,
        SESSION_MANAGER_DBUS_OBJECT_PATH,
        QDBusConnection::sessionBus() );

    assert( sessionMan );
    assert( sessionMan->isValid() );
    assert( 0 == sessionMan->count() );

    // create session
    QString session1_path = sessionMan->createSession( 0 );

    assert( session1_path.length() );
    assert( 1 == sessionMan->count() );

    com::hobartcorp::Session *session1 = new com::hobartcorp::Session(
        SESSION_MANAGER_DBUS_SERVICE,
        session1_path,
        QDBusConnection::sessionBus() );

    assert( session1 );
    assert( session1->isValid() );

    QString session1_id = session1->id();
    QString session1_name = session1->name();

    assert( session1_path.contains( session1_id ) );
    assert( session1_name.isEmpty() );

    // resume non-existant session
    assert( QString() == sessionMan->resumeNamedSession( "blahblahblah" ) );

    // create another session
    QString session2_path = sessionMan->createNamedSession( 0, "namedsession" );

    assert( session2_path.length() );
    assert( session2_path != session1_name );
    assert( 2 == sessionMan->count() );

    com::hobartcorp::Session *session2 = new com::hobartcorp::Session(
        SESSION_MANAGER_DBUS_SERVICE,
        session2_path,
        QDBusConnection::sessionBus() );

    assert( session2 );
    assert( session2->isValid() );

    QString session2_id = session2->id();
    QString session2_name = session2->name();

    assert( session2_path.contains( session2_id ) );
    assert( session2_name == "namedsession" );

    // resume session
    assert( session2_path == sessionMan->createNamedSession( 0, "namedsession" ) );
    assert( 2 == sessionMan->count() );

    assert( session2_path == sessionMan->resumeNamedSession( "namedsession" ) );
    assert( 2 == sessionMan->count() );

    // destroy session
    sessionMan->destroySession( session1->path() );
    sleepFor( SLEEP_TIME );

    assert( 1 == sessionMan->count() );

    sessionMan->destroySession( session1->path() );
    sleepFor( SLEEP_TIME );

    assert( 1 == sessionMan->count() );

    // destroy session
    sessionMan->destroySession( session2->path() );
    sleepFor( SLEEP_TIME );

    assert( QString() == sessionMan->resumeNamedSession( session2_name ) );
    assert( 0 == sessionMan->count() );

    delete session2;
    delete session1;

    delete sessionMan;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void testMessageCenter()
{
    // create interface to session manager
    com::hobartcorp::SessionManager *sessionMan = new com::hobartcorp::SessionManager(
        SESSION_MANAGER_DBUS_SERVICE,
        SESSION_MANAGER_DBUS_OBJECT_PATH,
        QDBusConnection::sessionBus() );

    assert( sessionMan );
    assert( sessionMan->isValid() );
    assert( 0 == sessionMan->count() );

    // create interface to message center
    com::hobartcorp::MessageCenter *mc = new com::hobartcorp::MessageCenter(
        MESSAGE_CENTER_DBUS_SERVICE,
        MESSAGE_CENTER_DBUS_OBJECT_PATH,
        QDBusConnection::sessionBus() );

    assert( mc );
    assert( mc->isValid() );

    // create session
    {
        SessionManCreateSession e;
        e.setData( SESSION_MAN_CREATE_SESSION_TYPE, (uint) 0 );

        mc->post( e.serialize() );
    }

    sleepFor( SLEEP_TIME );

    assert( 1 == sessionMan->count() );

    // resume non-existant session
    {
        SessionManResumeNamedSession e;
        e.setData( SESSION_MAN_RESUME_NAMED_SESSION_NAME, "blahblahblah" );

        mc->post( e.serialize() );
    }

    sleepFor( SLEEP_TIME );

    assert( 1 == sessionMan->count() );

    // create another session
    {
        SessionManCreateNamedSession e;
        e.setData( SESSION_MAN_CREATE_NAMED_SESSION_TYPE, (uint) 0 );
        e.setData( SESSION_MAN_CREATE_NAMED_SESSION_NAME, "namedsession" );

        mc->post( e.serialize() );
    }

    sleepFor( SLEEP_TIME );

    assert( 2 == sessionMan->count() );

    // resume session
    {
        SessionManCreateNamedSession e;
        e.setData( SESSION_MAN_CREATE_NAMED_SESSION_TYPE, (uint) 0 );
        e.setData( SESSION_MAN_CREATE_NAMED_SESSION_NAME, "namedsession" );

        mc->post( e.serialize() );
    }

    {
        SessionManResumeNamedSession e;
        e.setData( SESSION_MAN_RESUME_NAMED_SESSION_NAME, "namedsession" );

        mc->post( e.serialize() );
    }

    sleepFor( SLEEP_TIME );

    assert( 2 == sessionMan->count() );

    // destroy session
    {
        SessionManDestroySession e;
        e.setData( SESSION_MAN_DESTROY_SESSION_OBJ_PATH, "/Hobart/Session/00000003" );

        mc->post( e.serialize() );
    }

    sleepFor( 1000 );

    assert( 1 == sessionMan->count() );

    {
        SessionManDestroySession e;
        e.setData( SESSION_MAN_DESTROY_SESSION_OBJ_PATH, "/Hobart/Session/00000003" );

        mc->post( e.serialize() );
    }

    sleepFor( SLEEP_TIME );

    assert( 1 == sessionMan->count() );

    // destroy session
    {
        SessionManDestroySession e;
        e.setData( SESSION_MAN_DESTROY_SESSION_OBJ_PATH, "/Hobart/Session/00000004" );

        mc->post( e.serialize() );
    }

    sleepFor( SLEEP_TIME );

    assert( 0 == sessionMan->count() );

    delete mc;
    delete sessionMan;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QCoreApplication a( argc, argv );

    testInterface();
    testMessageCenter();

    qWarning() << "All tests passed!";

    return a.exec();
}
