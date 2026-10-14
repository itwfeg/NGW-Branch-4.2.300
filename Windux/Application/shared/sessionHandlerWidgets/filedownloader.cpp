
#include "filedownloader.h"

#include <QDir>
#include <QFileInfo>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
FileDownloader::FileDownloader( QUrl fileUrl, QObject *parent ) :
    QObject( parent ),
    m_Url( fileUrl ),
    m_Failure( false )
{
    connect( &m_WebCtrl, SIGNAL( finished(QNetworkReply*) ), SLOT( fileDownloaded(QNetworkReply*) ) );

    QNetworkRequest request( fileUrl );

    m_Reply = m_WebCtrl.get( request );
    connect( m_Reply, SIGNAL( error(QNetworkReply::NetworkError) ), SLOT( fileError(QNetworkReply::NetworkError) ) );
    connect( m_Reply, SIGNAL( downloadProgress(qint64,qint64) ), SLOT( fileDownloadProgress(qint64,qint64) ) );
    connect( m_Reply, SIGNAL( sslErrors(QList<QSslError>) ), SLOT( fileSslErrors(QList<QSslError>) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
FileDownloader::~FileDownloader()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString FileDownloader::fileName() const
{
    QString path( m_Url.path() );
    QString basename( QFileInfo( path ).fileName() );

    if ( basename.isEmpty() )
        basename = "unknown";

    return basename;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray FileDownloader::downloadedData()
{
    if (( m_Reply ) && ( !m_Failure ))
    {
        m_DownloadedData = m_Reply->readAll();

        m_Reply->deleteLater();
        m_Reply = NULL;
    }

    return m_DownloadedData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString FileDownloader::downloadedFile()
{
    QDir folder( QDir::temp() );
    QString f( folder.absoluteFilePath( fileName() ) );

    if (( m_Reply ) && ( !m_Failure ))
    {
        // save data to file
        QFile os( f );

        if ( os.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
        {
            char buffer[8192];
            qint64 size;

            // buffered write
            while ( 0 < (size = m_Reply->read( buffer, sizeof(buffer) )) )
                os.write( buffer, size );

            os.close();
        }

        // delete the reply
        m_Reply->deleteLater();
        m_Reply = NULL;
    }

    return f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FileDownloader::fileDownloadProgress( qint64 received, qint64 total )
{
    // sanity check
    if ( total <= 0 )
        return;

    emit downloadProgress( (received * 100) / total );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FileDownloader::fileDownloaded( QNetworkReply *pReply )
{
    Q_UNUSED( pReply );

    // emit a signal
    emit downloaded( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FileDownloader::fileError( QNetworkReply::NetworkError e )
{
    if ( QNetworkReply::NoError == e )
        return;

    // do not send failure twice in a row
    else if ( m_Failure )
        return;

    m_Failure = true;

    // emit a signal
    emit downloaded( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FileDownloader::fileSslErrors( const QList<QSslError> &errors )
{
    Q_UNUSED( errors );

    m_Reply->ignoreSslErrors();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
