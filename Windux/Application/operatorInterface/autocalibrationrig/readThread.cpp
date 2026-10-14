/**
 * @file main.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "readThread.h"

#include <iostream>
#include <QMutex>

#include <QApplication>
#include <QThread>

//#include "hobart/rs485/calrigrs485.h"
#include "hobart/mtrd24motor.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#if defined ( __linux__ )		// Windows 
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/unistd.h>
#endif

QMutex g_serial_mutex;

     void ReadThread::run()
    {
        struct timeval tv;
        tv.tv_sec = 5;/* Wait up to five seconds. */
        tv.tv_usec = 0;

        std::cerr << "ReadThread Started" << std::endl;

        while ( !QThread::currentThread()->isInterruptionRequested() )
        {
            unsigned char readPayload[RS485_MAX_LINE_LENGTH] = {0};
            int nBytes, totalBytes;
            //unsigned char myChar;
            bool cr_found = false;               
    

            totalBytes  = 0;
            cr_found    = false;
        


            //Read version
            //Clear in buffer
            memset(&readPayload[0], 0, RS485_MAX_LINE_LENGTH);
            
            //SELECT
            /* Watch stdin (fd mytty_fd) to see when it has input. */
            tv.tv_sec = 5;/* Wait up to five seconds. */
            tv.tv_usec = 0;
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(fd_, &rfds);
            
            //std::cerr << "readThread Calling: select(fd_+1, &rfds, NULL, NULL, &tv);" << std::endl;
            int retval = select(fd_+1, &rfds, NULL, NULL, &tv);

            if (retval == -1)
            {
                std::cerr << "read Thread select() = -1" << std::endl;
                //break;
            }
            else if (retval)
            {            
                /* FD_ISSET(0, &rfds) will be true. */
                if(FD_ISSET(fd_, &rfds) == true)
                {
                    QMutexLocker guard(&g_serial_mutex);

                    //std::cerr << "readThread Data is available now." << std::endl;
                    nBytes = read( fd_, &readPayload[0],  RS485_MAX_LINE_LENGTH);
                    //std::cerr << "readThread read() bytes = " << nBytes << std::endl;

                    totalBytes += nBytes;

                    //look for CR
                    for(int i=0; i<RS485_MAX_LINE_LENGTH; i++)
                    {
                        //std::cerr << readPayload[i] << std::endl;
                        
                        if(readPayload[i] == CR_CHARACTER )
                        {
                            cr_found = true;
                            //std::cerr << "ReadThread CR found " << std::endl;
                            break;//out of forloop
                        }
                    }
                    
                    if ( cr_found )
                    {
                        emit dataReady( (char*)readPayload, totalBytes );
                        //std::cerr << "emit dataReady " << std::endl;
                    }
                        
                    

                }

            }
            else
            {
                //std::cerr << "readThread No data within Timeout period." << retval << std::endl;
                readPayload[0] = 'R';readPayload[1] = 'x';readPayload[2] = ' ';
                readPayload[3] = 'T';readPayload[4] = 'i';readPayload[5] = 'm';readPayload[6] = 'e';readPayload[7] = 'o';readPayload[8] = 'u';readPayload[9] = 't';
                readPayload[10] = 10;readPayload[11] = 13;
                
                totalBytes = 12;

                emit dataReady( (char*)readPayload, totalBytes );
                //std::cerr << "emit dataReady " << std::endl;

            }
            //End SELECT

    
        }
    }
