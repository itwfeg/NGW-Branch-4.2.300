//
// File : Smbus.linux.cpp
//
//  Implementaion of class SMBUS for Linux
//
#if 0 //The i2c interface is no longer used. This was left over from the Hlx code base and should
	  //no longer be used. -- ATS
 
#include "PAL_platformheader.h"
#include "Smbus.h"
#include "SMBUSInterface.h"
#include "Integer.h"
#include "Hex.h"
#include "Debug.h"

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int Smbus::_dataType_ = 0;
	std::string Smbus::_dataTypeName_("Smbus");
#endif


using namespace std;

///////////////////////////////////////////////////////////////////////////////

#ifndef LIB_I2CDEV_H

/* 
 * Data for SMBus Messages 
 */
#define I2C_SMBUS_BLOCK_MAX	32	/* As specified in SMBus standard */	
#define I2C_SMBUS_I2C_BLOCK_MAX	32	/* Not specified but we use same structure */
union i2c_smbus_data {
	__u8 byte;
	__u16 word;
	__u8 block[I2C_SMBUS_BLOCK_MAX + 2]; /* block[0] is used for length */
	                                            /* and one more for PEC */
};

/* smbus_access read or write markers */
#define I2C_SMBUS_READ	1
#define I2C_SMBUS_WRITE	0

/* SMBus transaction types (size parameter in the above functions) 
   Note: these no longer correspond to the (arbitrary) PIIX4 internal codes! */
#define I2C_SMBUS_QUICK		    0
#define I2C_SMBUS_BYTE		    1
#define I2C_SMBUS_BYTE_DATA	    2 
#define I2C_SMBUS_WORD_DATA	    3
#define I2C_SMBUS_PROC_CALL	    4
#define I2C_SMBUS_BLOCK_DATA	    5
#define I2C_SMBUS_I2C_BLOCK_BROKEN  6
#define I2C_SMBUS_BLOCK_PROC_CALL   7		/* SMBus 2.0 */
#define I2C_SMBUS_I2C_BLOCK_DATA    8

static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command, 
                                     int size, union i2c_smbus_data *data)
{
	struct i2c_smbus_ioctl_data args;

	args.read_write = read_write;
	args.command = command;
	args.size = size;
	args.data = data;
	return ioctl(file,I2C_SMBUS,&args);
}

static inline __s32 i2c_smbus_read_byte(int file)
{
	union i2c_smbus_data data;
	if (i2c_smbus_access(file,I2C_SMBUS_READ,0,I2C_SMBUS_BYTE,&data))
		return -1;
	else
		return 0x0FF & data.byte;
}

static inline __s32 i2c_smbus_write_byte(int file, __u8 value)
{
	return i2c_smbus_access(file,I2C_SMBUS_WRITE,value,
	                        I2C_SMBUS_BYTE,NULL);
}

static inline __s32 i2c_smbus_read_byte_data(int file, __u8 command)
{
	union i2c_smbus_data data;
	if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
	                     I2C_SMBUS_BYTE_DATA,&data))
		return -1;
	else
		return 0x0FF & data.byte;
}

static inline __s32 i2c_smbus_write_byte_data(int file, __u8 command, 
                                              __u8 value)
{
	union i2c_smbus_data data;
	data.byte = value;
	return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
	                        I2C_SMBUS_BYTE_DATA, &data);
}

static inline __s32 i2c_smbus_read_word_data(int file, __u8 command)
{
	union i2c_smbus_data data;
	if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
	                     I2C_SMBUS_WORD_DATA,&data))
		return -1;
	else
		return 0x0FFFF & data.word;
}

static inline __s32 i2c_smbus_write_word_data(int file, __u8 command, 
                                              __u16 value)
{
	union i2c_smbus_data data;
	data.word = value;
	return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
	                        I2C_SMBUS_WORD_DATA, &data);
}

#endif

///////////////////////////////////////////////////////////////////////////////

int open_i2c_dev(const int i2cbus, char *filename, const int quiet)
{
    int file;

    sprintf(filename, "/dev/i2c/%d", i2cbus);
    file = open(filename, O_RDWR);

    if (file < 0 && errno == ENOENT) {
        sprintf(filename, "/dev/i2c-%d", i2cbus);
        file = open(filename, O_RDWR);
    }

    if (file < 0 && !quiet) {
        if (errno == ENOENT) {
            fprintf(stderr, "Error: Could not open file "
                    "`/dev/i2c-%d' or `/dev/i2c/%d': %s\n",
                    i2cbus, i2cbus, strerror(ENOENT));
        } else {
            fprintf(stderr, "Error: Could not open file "
                    "`%s': %s\n", filename, strerror(errno));
            if (errno == EACCES)
                fprintf(stderr, "Run as root?\n");
        }
    }

    return file;
}

int set_slave_addr(int file, int address, int force)
{
    /* With force, let the user read from/write to the registers
       even when a driver is also running */
    if (ioctl(file, force ? I2C_SLAVE_FORCE : I2C_SLAVE, address) < 0) {
        fprintf(stderr,
                "Error: Could not set address to 0x%02x: %s\n",
                address, strerror(errno));
        return -errno;
    }

    return 0;
}



Smbus::Smbus(UCHAR devAddr ) : 
    //deviceInterfaceDetailData_(0), 
    file_(0),
    fileOpened_(false), 
    deviceAddress_(devAddr)
{

#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", "Smbus");
#endif

    openHandle();
}

Smbus::~Smbus()
{

#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif

#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", "~Smbus");
#endif

    closeHandle();
}


bool Smbus::openHandle()
{
    /// TODO: report errors
    int i2cbus = 0;     /// TODO: verify on HW
    int force = 0;      /// TODO: verify force isn't needed
    char filename[20];

    file_ = open_i2c_dev(i2cbus, filename, 0);
    if (file_ < 0)
        return false;
    else
    {
        if (set_slave_addr(file_, deviceAddress_, force))
        {
            close(file_);
            return false;
        }
        else
        {
            fileOpened_ = true;
            return true;
        }
    }
}

void Smbus::closeHandle()
{
    if (file_)
    {
        close(file_);
        file_ = NULL;
        fileOpened_ = false;
    }
}

Smbus::SmbusStatus Smbus::readByte(UCHAR * readData) 
{
    int ret = 0;
#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", string("Read Byte started: deviceAddress: ") + Hex::toString(deviceAddress_));
#endif

    if (!fileOpened_)
    {
#ifndef EVOL_NDEBUG
        Debug::trace("Smbus", "Read Byte failed: file not open");
#endif
        return STATUS_FILE_NOT_OPEN;
    }

    ret = i2c_smbus_read_byte(file_);
    if ( ret < 0 )
        return STATUS_FAILED;

    *readData = ret;

#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", string("Read Byte Succeeded: bytes read:") + Integer::toString(1));
#endif

    return STATUS_SUCCEEDED;
}

Smbus::SmbusStatus Smbus::readDataByte(UCHAR command , UCHAR * readData)
{
    int ret = 0;
#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", string("Read Byte Data started: deviceAddress: ") + Hex::toString(deviceAddress_) +
            string("command: ") + Hex::toString(command));
#endif

    if (!fileOpened_)
    {
#ifndef EVOL_NDEBUG
        Debug::trace("Smbus", "Read Byte Data failed: file not open");
#endif
        return STATUS_FILE_NOT_OPEN;
    }

    ret = i2c_smbus_read_byte_data(file_, command);
    if ( ret < 0 )
        return STATUS_FAILED;

    *readData = ret;

#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", string("Read Byte Data Succeeded: bytes read:") + Integer::toString(1));
#endif

    return STATUS_SUCCEEDED;
}

/// TODO: why pass UCHAR when you want USHORT?
Smbus::SmbusStatus Smbus::readDataWord(UCHAR command, UCHAR * readData )
{
    int ret = 0;
#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", string("Read Word Data started: deviceAddress: ") + Hex::toString(deviceAddress_) +
            string("command: ") + Hex::toString(command));
#endif

    if (!fileOpened_)
    {
#ifndef EVOL_NDEBUG
        Debug::trace("Smbus", "Read word Data failed: file not open");
#endif
        return STATUS_FILE_NOT_OPEN;
    }

    ret = i2c_smbus_read_word_data(file_ ,command);
    if ( ret < 0 )
        return STATUS_FAILED;

    *readData = ret;

#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", string("Read Word Data Succeeded: bytes read:") + Integer::toString(2));
#endif

    return STATUS_SUCCEEDED;
}

Smbus::SmbusStatus Smbus::readDataBlock(UCHAR command, UCHAR length, UCHAR * readData)
{
    int i;
    int ret = 0;
#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", string("Read block Data started: deviceAddress: ") + Hex::toString(deviceAddress_) +
            string("command: ") + Hex::toString(command) + string("length: ") + Hex::toString(length));
#endif

    if (!fileOpened_)
    {
#ifndef EVOL_NDEBUG
        Debug::trace("Smbus", "Read block Data failed: file not open");
#endif
        return STATUS_FILE_NOT_OPEN;
    }

    if (length == 0)
    {
#ifndef EVOL_NDEBUG
        Debug::trace("Smbus", "Read block Data failed: zero length read");
#endif
        return STATUS_INVALID_PARAM; // MSVC2010

    }

    for (i=0; i< length; i++)
    {
        ret = i2c_smbus_read_byte_data(file_, command);
        command++;
        if (ret < 0)
            return STATUS_FAILED;
        else
            readData[i] = ret;
    }

#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", string("Read block Data Succeeded: bytes read:") + Integer::toString(length));
#endif


    return STATUS_SUCCEEDED;
}

Smbus::SmbusStatus Smbus::writeByte(UCHAR command)
{
    int ret = 0;

#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", string("Write Byte started: deviceAddress: ") + Hex::toString(deviceAddress_) +
            string("command: ") + Hex::toString(command));
#endif

    if (!fileOpened_)
    {
#ifndef EVOL_NDEBUG
        Debug::trace("Smbus", "write Byte failed: file not open");
#endif
        return STATUS_FILE_NOT_OPEN;
    }

    ret = i2c_smbus_write_byte(file_, command);
    if ( ret < 0 )
        return STATUS_FAILED;

#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", string("Write Byte Succeeded: bytes written:") + Integer::toString(1));
#endif

    return STATUS_SUCCEEDED;
}

Smbus::SmbusStatus Smbus::writeDataByte(UCHAR command, UCHAR writeData)
{
    int ret = 0;

#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", string("Write Byte Data started: deviceAddress: ") + Hex::toString(deviceAddress_) +
            string("command: ") + Hex::toString(command));
#endif

    if (!fileOpened_)
    {
#ifndef EVOL_NDEBUG
        Debug::trace("Smbus", "Write Byte Data failed: file not open");
#endif
        return STATUS_FILE_NOT_OPEN;
    }

    ret = i2c_smbus_write_byte_data(file_, command, writeData);
    if ( ret < 0 )
        return STATUS_FAILED;
#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", string("Write Byte Data Succeeded: bytes written:") + Integer::toString(1));
#endif

    return STATUS_SUCCEEDED;
}


Smbus::SmbusStatus Smbus::writeDataWord(UCHAR command, UCHAR * writeData)
{
    int ret = 0;

#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", string("Write Word Data started: deviceAddress: ") + Hex::toString(deviceAddress_) +
            string("command: ") + Hex::toString(command));
#endif

    if (!fileOpened_)
    {
#ifndef EVOL_NDEBUG
        Debug::trace("Smbus", "Write word Data failed: file not open");
#endif
        return STATUS_FILE_NOT_OPEN;
    }

    ret = i2c_smbus_write_word_data(file_, command, *writeData);
    if ( ret < 0 )
        return STATUS_FAILED;

#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", string("Write Word Data Succeeded: bytes read:") + Integer::toString(2));
#endif

    return STATUS_SUCCEEDED;
}

Smbus::SmbusStatus Smbus::writeDataBlock(UCHAR command, UCHAR length, UCHAR * writeData)
{
    int i;
    int ret = 0;
#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", string("write block Data started: deviceAddress: ") + Hex::toString(deviceAddress_) +
            string("command: ") + Hex::toString(command) + string("length: ") + Hex::toString(length));
#endif

    if (!fileOpened_)
    {
#ifndef EVOL_NDEBUG
        Debug::trace("Smbus", "Write block Data failed: file not open");
#endif
        return STATUS_FILE_NOT_OPEN;
    }

    switch (length)
    {
        case 0:
            {
#ifndef EVOL_NDEBUG
                Debug::trace("Smbus", "Write block Data failed: zero length read");
#endif
                return STATUS_INVALID_PARAM; // MSVC2010
            }
            break;
        case 1:
            {
                return writeDataByte(command, writeData[0]);
            }
            break;
        case 2:
            {
                return writeDataWord(command, writeData);
            }
            break;

    }

    for (i=0; i< length; i++)
    {
        ret = i2c_smbus_write_byte_data(file_, command, writeData[i]);
        if ( ret > 0 )
            return STATUS_FAILED;
        command++;
    }


#ifndef EVOL_NDEBUG
    Debug::trace("Smbus", string("Write block Data Succeeded: bytes read:") + Integer::toString(length));
#endif

    return STATUS_SUCCEEDED;
}
#endif
