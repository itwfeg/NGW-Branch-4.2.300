/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "MainBoardEEPROM.h"
#include "Integer.h"
#include "HobVersion.h"

#include "Debug.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int MainBoardEEPROM::_dataType_ = 0;
std::string MainBoardEEPROM::_dataTypeName_("MainBoardEEPROM"); 
#endif

MainBoardEEPROM::MainBoardEEPROM(): Smbus(DEVICE_ADDRESS) 
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

#ifndef EVOL_NDEBUG
    Debug::trace("MainBoardEEPROM", "MainBoardEEPROM");
#endif


}

MainBoardEEPROM::~MainBoardEEPROM()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
#ifndef EVOL_NDEBUG
    Debug::trace("MainBoardEEPROM", "~MainBoardEEPROM");
#endif

}

MainBoardEEPROM::SmbusStatus MainBoardEEPROM::readBoardType(USHORT * boardType)
{

    return readDataWord(OFFSET_BOARD_TYPE, (PUCHAR)boardType);

}

MainBoardEEPROM::SmbusStatus MainBoardEEPROM::readHardwareMajorRev(USHORT * hardwareMajorRev)
{

    return readDataWord(OFFSET_HARDWARE_MAJOR_REV, (PUCHAR)hardwareMajorRev);

}

MainBoardEEPROM::SmbusStatus MainBoardEEPROM::readHardwareMinorRev(USHORT * hardwareMinorRev)
{

    return readDataWord(OFFSET_HARDWARE_MINOR_REV, (PUCHAR)hardwareMinorRev);

}

MainBoardEEPROM::SmbusStatus MainBoardEEPROM::readModelType(USHORT * modelType)
{

    return readDataWord(OFFSET_MODEL_TYPE, (PUCHAR)modelType);

}

MainBoardEEPROM::SmbusStatus MainBoardEEPROM::readSerialNumber(ULONG * serialNumber)
{

    return readDataBlock(OFFSET_SERIAL_NUMBER, sizeof(ULONG), (PUCHAR)serialNumber);

}


MainBoardEEPROM::SmbusStatus MainBoardEEPROM::readHardwareRevision(HobVersion & hardwareRev)
{
    UCHAR hwRev[6];
    PUSHORT hwVT = (PUSHORT) &hwRev[0];
    PUSHORT hwMaj = (PUSHORT) &hwRev[2];
    PUSHORT hwMin = (PUSHORT) &hwRev[4];


    MainBoardEEPROM::SmbusStatus stat = readDataBlock(OFFSET_BOARD_TYPE, sizeof(hwRev), hwRev);

    if (stat != STATUS_SUCCEEDED)
        return stat;

    hardwareRev.setVersionType((HobVersion::VersionType)*hwVT);
    hardwareRev.setMajorRevision(*hwMaj);
    hardwareRev.setMinorRevision(*hwMin);

    return stat;
}

MainBoardEEPROM::SmbusStatus MainBoardEEPROM::writeBoardType(USHORT boardType)
{

    return writeDataWord(OFFSET_BOARD_TYPE, (PUCHAR)&boardType);

}

MainBoardEEPROM::SmbusStatus MainBoardEEPROM::writeHardwareMajorRev(USHORT hardwareMajorRev)
{

    return writeDataWord(OFFSET_HARDWARE_MAJOR_REV, (PUCHAR)&hardwareMajorRev);

}

MainBoardEEPROM::SmbusStatus MainBoardEEPROM::writeHardwareMinorRev(USHORT hardwareMinorRev)
{

    return writeDataWord(OFFSET_HARDWARE_MINOR_REV, (PUCHAR)&hardwareMinorRev);

}

MainBoardEEPROM::SmbusStatus MainBoardEEPROM::writeModelType(USHORT modleType)
{

    return writeDataWord(OFFSET_MODEL_TYPE, (PUCHAR)&modleType);

}

MainBoardEEPROM::SmbusStatus MainBoardEEPROM::writeSerialNumber(ULONG serialNumber)
{

    return writeDataBlock(OFFSET_SERIAL_NUMBER, sizeof (ULONG), (PUCHAR)&serialNumber);

}


MainBoardEEPROM::SmbusStatus MainBoardEEPROM::writeHardwareRevision(HobVersion & hardwareRev)
{
    UCHAR hwRev[6];
    PUSHORT hwVT = (PUSHORT) &hwRev[0];
    PUSHORT hwMaj = (PUSHORT) &hwRev[2];
    PUSHORT hwMin = (PUSHORT) &hwRev[4];


    *hwVT = (SHORT)hardwareRev.getVersionType();
    *hwMaj = (SHORT)hardwareRev.getMajorRevision();
    *hwMin = (SHORT)hardwareRev.getMinorRevision();

    return writeDataBlock(OFFSET_BOARD_TYPE, sizeof(hwRev), hwRev);

}

MainBoardEEPROM *MainBoardEEPROM::getInstance()
{
    if (instance == 0)
    {
        instance = new MainBoardEEPROM();
    }
    return instance;
}


MainBoardEEPROM *MainBoardEEPROM::instance = (MainBoardEEPROM *)NULL;

