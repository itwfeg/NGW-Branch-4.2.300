/**
 * @file main.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef READTHREAD_H
#define READTHREAD_H

#include <QThread>

class ReadThread : public QThread
{
    Q_OBJECT

signals:

    void dataReady( const QString &buffer, int totalBytes );

public:

    ReadThread( int fd, QObject *parent = 0 ) : fd_( fd ), QThread( parent ) {}
    virtual ~ReadThread() {}

protected:

    virtual void run();

private:

    int fd_;

};

#endif