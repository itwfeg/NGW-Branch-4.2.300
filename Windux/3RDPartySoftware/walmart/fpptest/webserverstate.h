/**
 * @file webserverstate.h
 * Web server state.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef WEBSERVERSTATE_H
#define WEBSERVERSTATE_H

#include <QObject>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Web server states.
class webServerState : public QObject
{
    Q_OBJECT
    Q_ENUMS( type )

public:

    /// Web server state type.
    enum type
    {
        Okay,                                       ///< 200 OK
        OkayNoContent,                              ///< 204 No Content
        IncompleteHeader,                           ///< 200 OK, but hung connection
        MalformedHeader,                            ///< 200 OK, but incomplete header
        MalformedBody,                              ///< 200 OK, but incomplete body
        NoResponse,                                 ///< Do not respond, leave socket open
        CloseSocket,                                ///< Do not respond, close socket immediatly
        Moved,                                      ///< 301 Moved
        Unauthorized,                               ///< 401 Unauthorized
        NotFound,                                   ///< 404 Not Found
        ThrowException,                             ///< Throw an exception
    };

    // ========================================================================
    // Static Methods
    // ========================================================================

    static QString description( type t )
    {
        switch ( t )
        {
        case Okay:
            return "200 OK";
        case OkayNoContent:
            return "204 No Content";
        case IncompleteHeader:
            return "200 OK [incomplete, open socket]";
        case MalformedHeader:
            return "200 OK [malformed header, closed socket]";
        case MalformedBody:
            return "200 OK [malformed body, closed socket]";
        case NoResponse:
            return "n/a [no response, open socket]";
        case CloseSocket:
            return "n/a [no response, closed socket]";
        case Moved:
            return "301 Moved";
        case Unauthorized:
            return "401 Unauthorized";
        case NotFound:
            return "404 Not Found";

        default:
            break;
        }

        return "Unknown state!";
    }

private:

    // not implemented
    webServerState( QObject *parent = 0 );
};


///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // WEBSERVERSTATE_H
