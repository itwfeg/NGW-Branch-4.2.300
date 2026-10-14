/**
 * @file filedownloader.h
 * Object that downloads a file from the network.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Scott Davis, Randy Blankley
 */

#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class FileDownloader
 * @brief Object that downloads a file from the network.
 */
class FileDownloader : public QObject
{
    Q_OBJECT

signals:

    /**
     * @brief This signal is emitted when the download succeeds or fails.
     * @param[in] success  @c true upon success, @c false on failure
     */
    void downloaded( bool success );

    /**
     * @brief This signal is emitted to indicate download progress.
     * @param[in] percent  value in the range [0,100]
     */
    void downloadProgress( int percent );

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR
    ///////////////////////////////////////////////////////////////////////////

    /**
     * @brief Constructor.
     * @param[in] fileUrl  url of file to download
     * @param[in] parent  parent object
     */
    explicit FileDownloader( QUrl fileUrl, QObject *parent = 0 );

    /**
     * @brief Destructor.
     */
    virtual ~FileDownloader();

    ///////////////////////////////////////////////////////////////////////////
    // Properties
    ///////////////////////////////////////////////////////////////////////////

    /**
     * @brief Retrieve the file name.
     * @return  filename (no pathing) of file being downloaded
     */
    QString fileName() const;

    ///////////////////////////////////////////////////////////////////////////
    // Methods
    ///////////////////////////////////////////////////////////////////////////

    /**
     * @brief Upon successfull download, the file data.
     * @return  byte array of file downloaded
     */
    QByteArray downloadedData();

    /**
     * @brief Upon successfull download, the file data.
     * @return  filename of saved file downloaded (in the temp folder)
     */
    QString downloadedFile();

private slots:

    void fileDownloadProgress( qint64 received, qint64 total );
    void fileDownloaded( QNetworkReply *pReply );
    void fileError( QNetworkReply::NetworkError e );
    void fileSslErrors( const QList<QSslError> &errors );

private:

    QUrl m_Url;
    QNetworkAccessManager m_WebCtrl;
    QNetworkReply *m_Reply;
    QByteArray m_DownloadedData;
    bool m_Failure;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

#endif // FILEDOWNLOADER_H
