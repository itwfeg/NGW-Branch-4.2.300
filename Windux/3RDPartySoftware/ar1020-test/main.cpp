#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#define DEBUG

static const char HEADER = 0x55;

static const char GET_VERSION = 0x10;
static const char ENABLE_TOUCH = 0x12;
static const char DISABLE_TOUCH = 0x13;
static const char CALIBRATE_MODE = 0x14;
static const char REGISTER_READ = 0x20;
static const char REGISTER_WRITE = 0x21;
static const char REGISTER_START_ADDRESS_REQUEST = 0x22;
static const char REGISTERS_WRITE_TO_EEPROM = 0x23;
static const char EEPROM_READ = 0x28;
static const char EEPROM_WRITE = 0x29;
static const char EEPROM_WRITE_TO_REGISTERS = 0x2B;

///////////////////////////////////////////////////////////////////////////////////////////////////
int i2cSendCommand( int f, const char *tx, size_t tx_len, char *rx, size_t &rx_len )
{
    int result( -1 );

    rx_len = 0;

#ifdef DEBUG
    printf( "  send: " );
    for ( size_t i( 1 ); i < tx_len; ++i )
        printf( "%02x ", tx[i] );
    printf( "\n" );
#endif

    // write command
    if ( tx_len != write( f, tx, tx_len ) )
        printf( "failed to write i2c!\n" );

    else
    {

retry_header:
        bool valid( false );

        // read header
        for ( size_t attempt( 16 ); attempt--; )
        {
            // read i2c bus
            if ( 1 != read( f, &rx[0], 1 ) )
                usleep( 4096 );

            // validate header
            else if ( HEADER == rx[0] )
            {
                valid = true;
                break;
            }
        }

        if ( !valid )
            printf( "bad i2c header!\n" );

        // read rx length
        else if ( 1 != read( f, &rx[1], 1 ) )
            printf( "failed to read i2c header!\n" );

        else
        {
            rx_len += 2;

            // read remaining bytes
            if ( rx[1] )
            {
                rx_len += rx[1];

                // read body
                if ( rx[1] != read( f, &rx[2], rx[1] ) )
                    printf( "failed to read i2c body!\n" );

                // validate cmd sent matched response
                else if (( 4 <= rx_len ) && ( 4 < tx_len ) && ( tx[3] != rx[3] ))
				{
					rx_len = 0;
                    goto retry_header;
				}
				
                // process result code
                else if ( 3 <= rx_len )
                {
                    result = rx[2];
                }
            }

#ifdef DEBUG
            printf( "  recv: " );
            for ( size_t i( 0 ); i < rx_len; ++i )
                printf( "%02x ", rx[i] );
            printf( "\n" );
#endif
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int sendCommandWithResponse( int f, char cmd, char *rx, size_t &rx_len )
{
    char tx_[64];
    tx_[0] = 0;                 // leading zero (required for i2c)
    tx_[1] = HEADER;            // header byte
    tx_[2] = 1;                 // num bytes
    tx_[3] = cmd;               // command

    char rx_[512];
    size_t rx_len_;

    int result = i2cSendCommand( f, tx_, 4, rx_, rx_len_ );

    if (( rx ) && ( 4 < rx_len_ ))
        memcpy( rx, &rx_[4], rx_len = rx_len_ - 4 );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int sendCommand( int f, char cmd )
{
    size_t ignored;

    return sendCommandWithResponse( f, cmd, NULL, ignored );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int sendCommandWithResponse( int f, char cmd, const char *tx, size_t tx_len, char *rx, size_t &rx_len )
{
    char tx_[64];
    tx_[0] = 0;                 // leading zero (required for i2c)
    tx_[1] = HEADER;            // header byte
    tx_[2] = 1 + tx_len;        // num bytes
    tx_[3] = cmd;               // command

    memcpy( &tx_[4], tx, tx_len );

    char rx_[512];
    size_t rx_len_;

    int result = i2cSendCommand( f, tx_, 4 + tx_len, rx_, rx_len_ );

    if (( rx ) && ( 4 < rx_len_ ))
        memcpy( rx, &rx_[4], rx_len = rx_len_ - 4 );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int sendCommand( int f, char cmd, const char *tx, size_t tx_len )
{
    size_t ignored;

    return sendCommandWithResponse( f, cmd, tx, tx_len, NULL, ignored );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const char *responseDescription( int result )
{
    static const char *r00( "success" );
    static const char *r01( "unrecognized command" );
    static const char *r03( "unrecognized header" );
    static const char *r04( "command timeout" );
    static const char *rfc( "calibration cancel" );
    static const char *unknown( "<unknown>" );

    if ( 0x00 == result )
        return r00;
    else if ( 0x01 == result )
        return r01;
    else if ( 0x03 == result )
        return r03;
    else if ( 0x04 == result )
        return r04;
    else if ( 0xfc == result )
        return rfc;

    return unknown;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool enable_touch( int f )
{
    int result;

    printf( "enable touch...\n" );

    // enable touch
    if ( 0 != (result = sendCommand( f, ENABLE_TOUCH )) )
    {
        printf( "ENABLE_TOUCH failed 0x%02x (%s)!\n", result, responseDescription( result ) );
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool disable_touch( int f )
{
    int result;

    printf( "disable touch...\n" );

    // disable touch
    if ( 0 != (result = sendCommand( f, DISABLE_TOUCH )) )
    {
        printf( "DISABLE_TOUCH failed 0x%02x (%s)!\n", result, responseDescription( result ) );
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool register_read( int f, char offset, char num_registers, char *register_values )
{
    int result;

    char tx[64];
    tx[0] = 0;
    tx[1] = offset;
    tx[2] = num_registers;

    char rx[512];
    size_t rx_len;

    // must read between 1-8 registers
    if (( num_registers < 1 ) || ( 8 < num_registers ))
        return false;

    printf( "register read 0x%02x %d...\n", offset, num_registers );

    // register read
    if ( 0 != (result = sendCommandWithResponse( f, REGISTER_READ, tx, 3, rx, rx_len )) )
    {
        printf( "REGISTER_READ failed 0x%02x (%s)!\n", result, responseDescription( result ) );
        return false;
    }

    // validate return length
    else if ( num_registers != rx_len )
    {
        printf( "REGISTER_READ bad return length %d!\n", rx_len );
        return false;
    }

    memcpy( register_values, rx, num_registers );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool register_write( int f, char offset, char num_registers, const char *register_values )
{
    int result;

    // must write between 1-8 registers
    if (( num_registers < 1 ) || ( 8 < num_registers ))
        return false;

    char tx[64];
    tx[0] = 0;
    tx[1] = offset;
    tx[2] = num_registers;

    memcpy( &tx[3], register_values, num_registers );

    char rx[512];
    size_t rx_len;

    printf( "register write 0x%02x %d...\n", offset, num_registers );

    // register read
    if ( 0 != (result = sendCommandWithResponse( f, REGISTER_WRITE, tx, 3 + num_registers, rx, rx_len )) )
    {
        printf( "REGISTER_WRITE failed 0x%02x (%s)!\n", result, responseDescription( result ) );
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool register_start_address_request( int f, char &start_address )
{
    int result;

    char rx[512];
    size_t rx_len;

    printf( "register start address request...\n" );

    // register start address request
    if ( 0 != (result = sendCommandWithResponse( f, REGISTER_START_ADDRESS_REQUEST, rx, rx_len )) )
    {
        printf( "REGISTER_START_ADDRESS_REQUEST failed 0x%02x (%s)!\n", result, responseDescription( result ) );
        return false;
    }

    // validate return length
    else if ( 1 != rx_len )
    {
        printf( "REGISTER_START_ADDRESS_REQUEST bad return length %d!\n", rx_len );
        return false;
    }

    start_address = rx[0];

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    static const char *i2c_bus( "/dev/i2c-2" );
    static int i2c_address( 0x4d );

    int result( 1 );
    int f;

    // create a file descriptor for the I2C bus
    if ( (f = open( i2c_bus, O_RDWR )) < 0 )
        printf( "failed to open i2c!\n" );
    else
    {
        bool rmmod( false );

        // setup address
        if ( 0 != ioctl( f, I2C_SLAVE, i2c_address ) )
        {
            printf( "unloading kernel module...\n" );
            system( "rmmod ar1020_i2c" );

            rmmod = true;
        }

        // setup address
        if ( 0 != ioctl( f, I2C_SLAVE, i2c_address ) )
            printf( "failed to set i2c address!\n" );

        // disable touch while sending commands
        else if ( disable_touch( f ) )
        {
            char start_address;

            // spec says to sleep 50ms after disabling touch
            usleep( 50000 );

            if ( register_start_address_request( f, start_address ) )
            {
                char config[8];

                printf( "register start address is 0x%02x\n", start_address );

                // read current config
                if ( register_read( f, start_address + 0x02, 7, config ) )
                {
                    printf( "touch threshold         0x%02x\n", config[0] );
                    printf( "sensitivity filter      0x%02x\n", config[1] );
                    printf( "sampling fast           0x%02x\n", config[2] );
                    printf( "sampling slow           0x%02x\n", config[3] );
                    printf( "accuracy filter fast    0x%02x\n", config[4] );
                    printf( "accuracy filter slow    0x%02x\n", config[5] );
                    printf( "speed threshold         0x%02x\n", config[6] );
                }

                // read current config
                if ( register_read( f, start_address + 0x0a, 7, config ) )
                {
                    printf( "sleep delay             0x%02x\n", config[0] );
                    printf( "pen up delay            0x%02x\n", config[1] );
                    printf( "touch mode              0x%02x\n", config[2] );
                    printf( "touch options           0x%02x\n", config[3] );
                    printf( "calibration inset       0x%02x\n", config[4] );
                    printf( "pen state report delay  0x%02x\n", config[5] );
                    printf( "touch report delay      0x%02x\n", config[6] );

                    // disable sleep delay
                    if ( 0x00 != config[0] )
                    {
                        config[0] = 0x00;

                        register_write( f, start_address + 0x0a, 1, config );
                    }
                }
            }

            // disable sleep mode

            // enable touch
            enable_touch( f );
        }

        // load module back in
        if ( rmmod )
        {
            printf( "loading kernel module...\n" );
            system( "modprobe ar1020_i2c" );
        }

        close( f );
    }

    return result;
}
