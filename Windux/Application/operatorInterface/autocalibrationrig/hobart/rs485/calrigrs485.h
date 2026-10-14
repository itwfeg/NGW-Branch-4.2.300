/**
 * @file RS485Manager.h
 * @copyright Copyright (c) 2018 Hobart Corportation
 * @author Aaron Swift
 * @brief Responsible for sending label data over RS485
 * @addtogroup CommManagers
 * @{
 */
#ifndef calRigrs485manager_h
#define CalRigrs485manager_h



#if defined ( __linux__ )		// Windows 
#include <linux/serial.h>
#include <termios.h>
#endif // __linux__

#include <QObject>

#define TIOCGRS485              0x542E
#define TIOCSRS485              0x542F
#define _MAX_ASYNC_BAUD         3000000
#define _2000000_BAUD			2000000
#define _500000_BAUD			500000
#define _115200_BAUD		 	115200
#define _9600_BAUD		 	    9600

#define RS485_MAX_LINE_LENGTH 20
#define CR_CHARACTER 13
#define LF_CHARACTER 10

/**
 * @class RS485Manager
 * @brief Manager for the RS485 Serial Bus
 * 
 */
class RS485Manager : public QObject {
     
        
    public:      
        RS485Manager();

        int get_tty_fd(){ return tty_fd;}

        int RS485Send(unsigned char *buffer, int length);
        int RS485ReadBytes(unsigned char *buffer, int length);
        int RS485ReadLine(unsigned char *buffer);

        /**
         * @brief Destructor for the RS485 manager
         * 
         */
        ~RS485Manager();

    private:
        int myInt;

        static const std::string UART;
        int tty_fd;
#if defined ( __linux__ )
        struct termios uart_config;
#endif // __linux__




};


/** @} */
#endif //calRigrs485manager_h
