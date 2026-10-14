/**
 * @file hobartusbweigher.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "hobartcandev.h"
#include "hobartcanweigher.h"

#include <iomanip>
#include <iostream>
#include <QtEndian>

static const bool DISPLAY_STATUS_MESSAGES = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartCanWeigher::hobartCanWeigher() :
    _Mybase()
{
    status_ctr_ = 1;
    fitbuf_ctr_ = 1;

    rawDate_ = 0;

    flags_ = 0;
    rawAtoDCounts_ = 0;
    nonZeroCalibCounts_ = 0;
    zeroCalibCounts_ = 0;
    acclXRawCounts_ = 0;
    acclYRawCounts_ = 0;
    acclZRawCounts_ = 0;
    acclXTiltDegrees_ = 0;
    acclYTiltDegrees_ = 0;

    a1_g_ = 0;
    a2_m_ = 0;
    a3_zerovalue_ = 0;
    a4_spanvalue_ = 0;
    a5_Z0_ = 0;
    a6_D_ = 0;
    a7_x2_ = 0;
    a8_x1_ = 0;
    a9_x0_ = 0;
    a10_y2_ = 0;
    a11_y1_ = 0;
    a12_y0_ = 0;
    a13_xrot0_ = 0;
    a14_xrot1_ = 0;
    a15_xrot2_ = 0;
    a16_yrot0_ = 0;
    a17_yrot1_ = 0;
    a18_yrot2_ = 0;
    a19_zrot0_ = 0;
    a20_zrot1_ = 0;
    a21_zrot2_ = 0;
    a22_xgain_ = 0;
    a23_ygain_ = 0;
    a24_gcoeff_ = 0;
    a25_checksum_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartCanWeigher::~hobartCanWeigher()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::setWeigherMode( unsigned int mode )
{
    // Per the doc, it looks like the mode is just one Byte. Why do we declare a long here?
    // incoming arg is unsigned int, which is 4 bytes. A long is 8 and
    // size_t resolves to an unsigned long which is also == 8 so thats ok too.

    static const size_t msgLen( 1 );        // size_t = long = 8 Bytes

    unsigned char msg[16];                  // why is this 16, if we are talking longs here?
    msg[0] = mode;                          // why not use type passed in?  Matching types not longs or size_t

    if ( msgLen != sendWeigherMessage( wgMode, msg, msgLen ) )
        std::cout << "failed to set weigher mode!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartCanWeigher::rangeType hobartCanWeigher::weigherDeviceRange() const
{
    // this range use to include all 0x400 messages
    return rangeType( CAN_ID, CAN_RANGE );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::weigherEnable()
{
    if ( !sendWeigherMessageBasic( wgEnable ) )
        std::cout << "failed to enable weigher!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::weigherWakeup()
{
    if ( !sendWeigherMessageBasic( wgReqWakeup ) )                  // 0x401
        std::cout << "failed to wakeup weigher!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::weigherZero()
{
    if ( !sendWeigherMessageBasic( wgRezero ) )                     // 0x456
        std::cout << "failed to zero weigher!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::weigherStatusReq()
{
    std::cout << "Received weigherStatusReq but we dont do anything anymore with it." << std::endl;

    // this is triggered off of a 0x452 now - this is no longer used 06-12-19
    if ( !sendWeigherMessageBasic( wgStatus ) )                                     // request 0x402
        std::cout << "failed to request weigher status!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::accelerometerStatusReq()
{
    // this is triggered off of a 0x452 now
    if ( !sendWeigherMessageBasic( wgReqStatus ) )                                  // request 0x452
        std::cout << "failed to request weigher accelerometer status!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::weigherValueMaxReq()
{
    // This wasnt working for me, when read back in, so I created the 3 below and used them instead.
    // Make a request for gain factors. Can I do them all at once or will I have to stage them?"

    if ( !sendValueMaxMessageRequest( VMREQ_CURRENTFACTORS ) )                          // request a 0x420 | 0x08
        std::cout << "failed to request value max calibration data!" << std::endl;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::requestFitCalibration()
{
    if ( !sendValueMaxMessageRequest( VMREQ_FITPARAMS ) )                            // request a 0x420 | 0x04
        std::cout << "failed to request value max FIT Parameters!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::requestDate()
{
    // Step 1
    std::cout << "\nHCW::requestDate() Sent VMREQ_DATE!" << std::endl;

    if ( !sendValueMaxMessageRequest( VMREQ_DATE ) )
        std::cout << "failed to request value max VMREQ_DATE data!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::requestAssemblyNumber()
{
    // Step 2
    std::cout << "\nHCW::requestAssembly() VMREQ_ASSEMBLY!" << std::endl;
    if ( !sendValueMaxMessageRequest( VMREQ_ASSEMBLY ) )
        std::cout << "failed to request value max VMREQ_ASSEMBLY data!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::requestHardwareVersion()
{
    // step 3
    std::cout << "\nHCW::requestHWVersion VMREQ_HWVERSION!" << std::endl;
    if ( !sendValueMaxMessageRequest( VMREQ_HWVERSION ) )
        std::cout << "failed to request value max VMREQ_HWVERSION data!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::requestSerialNumber()
{
    // step 4
    std::cout << "\nHCW::requestSerialNum VMREQ_SERIALNUM!" << std::endl;
    if ( !sendValueMaxMessageRequest( VMREQ_SERIALNUM ) )
        std::cout << "failed to request value max VMREQ_SERIALNUM data!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::setValueMaxAssembly( QString assemblyNum )
{
    // TODO - verify
    std::cout << "HCW::setValueMaxAssembly() assemblyNum: " << assemblyNum.toUtf8().constData() << std::endl;

    static const size_t msgLen( 10 );
    unsigned char msg[16*10];
    memcpy( msg, assemblyNum.toStdString().c_str(), assemblyNum.size() );         // toLatin1() ?

    std::cout << "HCW::setValueMaxAssembly() assemblyNum msgLen: " << msgLen << std::endl;
    std::cout << "HCW::setValueMaxAssembly() assemblyNum: FROM HERE assemblyNum.size()" << assemblyNum.size() << std::endl;
    std::cout << "HCW::setValueMaxAssembly() assemblyNum: TO HERE sizeof(msg)" << sizeof(msg) << std::endl;

    if ( sendWeigherMessage( WgValueMaxWriteAssem, msg, msgLen ) )
        std::cout << "hobartCanWeigher::Set value Assembly Number!" << std::endl;
    else
        std::cout << "hobartCanWeigher::Failed to set Assembly Number!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::setValueMaxHwVersion( QString hardwareNum )
{
    // TODO - verify
    std::cout << "\nsetValueMaxHwVersion   hardwareNum: " << hardwareNum.toUtf8().constData() << std::endl;

    static const size_t msgLen( 2 );        // size_t = long = 8
    unsigned char msg[16*2];                // 2 Bytes?
    memcpy( msg, hardwareNum.toStdString().c_str(), hardwareNum.size() );         // toLatin1() ?

    std::cout << "HCW::setValueMaxHwVersion() HWVer msgLen: " << msgLen << std::endl;
    std::cout << "HCW::setValueMaxHwVersion() HWVer: FROM HERE hardwareNum.size()" << hardwareNum.size() << std::endl;
    std::cout << "HCW::setValueMaxHwVersion() HWVer: TO HERE sizeof(msg)" << sizeof(msg) << std::endl;

    if ( sendWeigherMessage( WgValueMaxWriteHwVersion, msg, msgLen ) )
        std::cout << "HCW::Set value Hardware Version!" << std::endl;
    else
        std::cout << "HCW::Failed to set Hardware Version!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::setValueMaxSerial(QString serialNum)
{
    // TODO - verify
    std::cout << "\nHCW::setValueMaxSerialNum   serialNum: " << serialNum.toUtf8().constData() << std::endl;

    static const size_t msgLen( 16 );        // I put 8, but there are actually 2 frames, 8+8, so do I put 16. Not sure how to handle this
    unsigned char msg[16*16];                // Im losing the 2nd frame here but not sure how to handle it. And in Assembly
    memcpy( msg, serialNum.toStdString().c_str(), serialNum.size() );         // toLatin1() ?

    std::cout << "HCW::setValueMaxSerial() SerialNum msgLen: " << msgLen << std::endl;
    std::cout << "HCW::setValueMaxSerial() SerialNum: FROM HERE serialNum.size()" << serialNum.size() << std::endl;
    std::cout << "HCW::setValueMaxSerial() SerialNum: TO HERE sizeof(msg)" << sizeof(msg) << std::endl;

    if ( sendWeigherMessage( WgValueMaxWriteSerial, msg, msgLen ) )
        std::cout << "HCW::Set value Serial Number!" << std::endl;
    else
        std::cout << "HCW::Failed to set Serial Number!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::setValueMaxFitParameters(int32_t a1_g, int32_t a2_m, int32_t a3_zerovalue, int32_t a4_spanvalue, int32_t a5_Z0, int32_t a6_D, int32_t a7_x2, int32_t a8_x1, int32_t a9_x0, int32_t a10_y2, int32_t a11_y1, int32_t a12_y0, int32_t a13_xrot0, int32_t a14_xrot1, int32_t a15_xrot2, int32_t a16_yrot0, int32_t a17_yrot1, int32_t a18_yrot2, int32_t a19_zrot0, int32_t a20_zrot1, int32_t a21_zrot2, int32_t a22_xgain, int32_t a23_ygain, int32_t a24_gcoeff, int32_t a25_checksum)
{
    std::cout << "\nHCW::setValueMaxFitParameters()!" << std::endl;
    if( DISPLAY_STATUS_MESSAGES )
    {
        std::cout << "HCW::sVMaxFitParams(): a1_g		   " << a1_g			<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a2_m		   " <<	a2_m			<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a3_zerovalue  " <<	a3_zerovalue	<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a4_spanvalue  " <<	a4_spanvalue	<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a5_Z0		   " <<	a5_Z0			<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a6_D		   " << a6_D			<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a7_x2		   " <<	a7_x2			<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a8_x1		   " <<	a8_x1			<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a9_x0		   " <<	a9_x0			<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a10_y2		   " <<	a10_y2			<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a11_y1		   " << a11_y1			<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a12_y0		   " <<	a12_y0			<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a13_xrot0	   " <<	a13_xrot0		<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a14_xrot1	   " <<	a14_xrot1		<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a15_xrot2	   " <<	a15_xrot2		<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a16_yrot0	   " << a16_yrot0		<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a17_yrot1	   " <<	a17_yrot1		<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a18_yrot2	   " <<	a18_yrot2		<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a19_zrot0	   " <<	a19_zrot0		<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a20_zrot1	   " <<	a20_zrot1		<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a21_zrot2	   " << a21_zrot2		<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a22_xgain	   " <<	a22_xgain		<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a23_ygain	   " <<	a23_ygain		<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a24_gcoeff	   " <<	a24_gcoeff		<< std::endl;
        std::cout << "HCW::sVMaxFitParams(): a25_checksum  " <<	a25_checksum	<< std::endl;
    }

    static const size_t msgLen( 100 );                      //    100
    unsigned char * msg = new unsigned char[16 * 100];   // so 100 * 16 = 1600

    int charctr         = 0;
    std::size_t index   = 0;
    int32_t temp32      = 0; //for flipping endianess 32bit uint

    temp32 = htobe32(a1_g);
    char *pChar = (char *) &temp32;
    for (std::size_t i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a1_g " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a2_m);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a2_m " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a3_zerovalue);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a3_zerovalue " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a4_spanvalue);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a4_spanvalue " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a5_Z0);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a5_Z0 " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a6_D);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a6_D " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a7_x2);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a7_x2 " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a8_x1);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a8_x1 " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a9_x0);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a9_x0 " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a10_y2);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a10_y2 " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a11_y1);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a11_y1 " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a12_y0);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a12_y0 " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a13_xrot0);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a13_xrot0 " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a14_xrot1);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a14_xrot1 " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a15_xrot2);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a15_xrot2 " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a16_yrot0);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a16_yrot0 " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a17_yrot1);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a17_yrot1 " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a18_yrot2);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a18_yrot2 " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a19_zrot0);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a19_zrot0 " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a20_zrot1);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a20_zrot1 " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a21_zrot2);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a21_zrot2 " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a22_xgain);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a22_xgain " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a23_ygain);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a23_ygain " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a24_gcoeff);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        //std::cout << "char a24_gcoeff " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    temp32 = htobe32(a25_checksum);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        std::cout << "char a25_checksum " << charctr++ << ": " << pChar << std::endl;        // might look like crud tho
        msg[index++] = *pChar++;
    }

    //std::string buf_test((char*) msg);                                 // WORKS - c style but not suggested.
    std::cout << "\nHCW::setValueMaxFitParameters() FIT msgLen: "                     << msgLen << std::endl;
    std::cout << "HCW::setValueMaxFitParameters() FIT: 25 FROMS TO HERE sizeof(msg)" << sizeof(msg) << std::endl;
    std::cout << "HCW::setValueMaxFitParameters() Built buffer, now Send FIT Parameters to sendWeigherMessage()" << std::endl;
    std::cout << std::endl;

    if ( sendWeigherMessage( WgValueMaxWriteFitParameters, msg, msgLen ) )
        std::cout << "\n++++++++++++++++++++++++++++++++ setValueMaxFitParameters() SUCCESS SENT FIT Parameters YEAAAAAAAAAAAA ++++++++++++++++++++++++++++++++" << std::endl;
    else
        std::cout << "\n++++++++++++++++++++++++++++++++ setValueMaxFitParameters() FAILED to set Fit Parameters BOOOOOOOOOOOO ++++++++++++++++++++++++++++++++" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//void hobartCanWeigher::setValueMaxDateOfMfg(QString dateNum )
void hobartCanWeigher::setValueMaxDateOfMfg(uint32_t dateNum )
{
    std::cout << "\nHCW::setValueMaxDateOfMfg   dateNum: " << dateNum << std::endl;

    ///////////////////////////////////////////////////////////////////////////////

    // ORIGINAL - worked but got back wrong value.
    //static const size_t msgLen( 4 );
    //unsigned char msg[16*4];                  // 4 Bytes?
    //memcpy( msg, &dateNum, sizeof(dateNum) );
    //htobe32( *reinterpret_cast<const uint32_t*>( &msg[0] ) );

    static const size_t msgLen( 4 );
    unsigned char * msg = new unsigned char[16 * 4];

    std::size_t index    = 0;
    uint32_t temp32      = 0;                               // for flipping endianess 32bit uint (int32_t for FIT. Is this OK?)

    temp32 = htobe32(dateNum);
    char *pChar = (char *) &temp32;
    for (std::size_t i = 0; i < sizeof(temp32); i++)
    {
        msg[index++] = *pChar++;
    }

    if (sendWeigherMessage( WgValueMaxWriteDate, msg, msgLen ) )
        std::cout << "HCW::Set value Manufacturing Date!" << std::endl;
    else
        std::cout << "HCW::Failed to set Manufacturing Date!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::registerWeigherMessages( hobartCanDev *dev )
{
    // registered messages are messages that are received

    // nothing to do
    if ( !dev )
        return;

    // setup message size lists
    hobartCanDev::messageSizeList simple;
    simple.append( 1 );

    hobartCanDev::messageSizeList wgWakeupMsgs;
    wgWakeupMsgs.append( 4 );

    hobartCanDev::messageSizeList wgStatusMsgs;
    wgStatusMsgs.append( 6 );
    wgStatusMsgs.append( 8 );
    wgStatusMsgs.append( 4 );

    // add messages
    dev->addMessage( rangeType( wgWakeup, 1 ), wgWakeupMsgs );
    dev->addMessage( rangeType( wgReqWakeup, 1 ), simple );
    //dev->addMessage( rangeType( wgStatusX, 3 ), wgStatusMsgs );    06-12-19 - dont see why I needed this
    dev->addMessage( rangeType( wgMode, 1 ), simple );
    dev->addMessage( rangeType( wgReqStatus, 1 ), simple );
    dev->addMessage( rangeType( wgEnable, 1 ), simple );
    dev->addMessage( rangeType( wgRezero, 1 ), simple );

    hobartCanDev::messageSizeList wgValueMaxRequestMsgs;
    wgValueMaxRequestMsgs.append( 1 );  // Request

    hobartCanDev::messageSizeList wgValueMaxAssemblyMsgs;
    wgValueMaxAssemblyMsgs.append( 8 );        // Read Assembly
    wgValueMaxAssemblyMsgs.append( 2 );

    hobartCanDev::messageSizeList wgValueMaxWriteAssemblyMsgs;
    wgValueMaxWriteAssemblyMsgs.append( 8 );    // Write Assembly
    wgValueMaxWriteAssemblyMsgs.append( 2 );

    hobartCanDev::messageSizeList wgValueMaxHwVersionMsgs;
    wgValueMaxHwVersionMsgs.append( 2 );        // Hardware Version

    hobartCanDev::messageSizeList wgValueMaxWriteHwVersionMsgs;
    wgValueMaxWriteHwVersionMsgs.append( 2 );   // Write Hardware Version

    hobartCanDev::messageSizeList wgValueMaxStatusMsgs;
    wgValueMaxStatusMsgs.append( 6 );           // Status Message
    wgValueMaxStatusMsgs.append( 8 );
    wgValueMaxStatusMsgs.append( 8 );
    wgValueMaxStatusMsgs.append( 8 );           // 30 bytes total

    hobartCanDev::messageSizeList wgValueMaxSerialNumberMsgs;
    wgValueMaxSerialNumberMsgs.append( 8 );     // Serial Number
    wgValueMaxSerialNumberMsgs.append( 8 );

    hobartCanDev::messageSizeList wgValueMaxWriteSerialNumberMsgs;
    wgValueMaxWriteSerialNumberMsgs.append( 8 );// Write Serial Number
    wgValueMaxWriteSerialNumberMsgs.append( 8 );

    hobartCanDev::messageSizeList wgValueMaxDateOfMfgMsgs;
    wgValueMaxDateOfMfgMsgs.append( 4 );        // Date of Manufacturing

    hobartCanDev::messageSizeList wgValueMaxWriteDateOfMfgMsgs;
    wgValueMaxWriteDateOfMfgMsgs.append( 4 );   // Write Date of Manufacturing

    hobartCanDev::messageSizeList WgValueMaxFitParametersMsgs;
    WgValueMaxFitParametersMsgs.append( 8 );            // these were 4, as for each msg, but I think its frames I am supposed to define here
    WgValueMaxFitParametersMsgs.append( 8 );            // so instead of 25 4 bytes msgs, I changed it to 13 frames 8 bytes each.
    WgValueMaxFitParametersMsgs.append( 8 );
    WgValueMaxFitParametersMsgs.append( 8 );
    WgValueMaxFitParametersMsgs.append( 8 );        // 5
    WgValueMaxFitParametersMsgs.append( 8 );
    WgValueMaxFitParametersMsgs.append( 8 );
    WgValueMaxFitParametersMsgs.append( 8 );
    WgValueMaxFitParametersMsgs.append( 8 );
    WgValueMaxFitParametersMsgs.append( 8 );        // 10
    WgValueMaxFitParametersMsgs.append( 8 );
    WgValueMaxFitParametersMsgs.append( 8 );
    WgValueMaxFitParametersMsgs.append( 4 );        // the last frame is just the 4 bytte checksum
    //WgValueMaxFitParametersMsgs.append( 4 );
    //WgValueMaxFitParametersMsgs.append( 4 );        // 15
    //WgValueMaxFitParametersMsgs.append( 4 );
    //WgValueMaxFitParametersMsgs.append( 4 );
    //WgValueMaxFitParametersMsgs.append( 4 );
    //WgValueMaxFitParametersMsgs.append( 4 );
    //WgValueMaxFitParametersMsgs.append( 4 );        // 20
    //WgValueMaxFitParametersMsgs.append( 4 );
    //WgValueMaxFitParametersMsgs.append( 4 );
    //WgValueMaxFitParametersMsgs.append( 4 );
    //WgValueMaxFitParametersMsgs.append( 4 );
    //WgValueMaxFitParametersMsgs.append( 4 );        // 25

    hobartCanDev::messageSizeList WgValueMaxWriteFitParametersMsgs;
    WgValueMaxWriteFitParametersMsgs.append( 8 );           // these were 4, as for each msg, but I think its frames I am supposed to define here
    WgValueMaxWriteFitParametersMsgs.append( 8 );           // so instead of 25 4 bytes msgs, I changed it to 13 frames 8 bytes each.
    WgValueMaxWriteFitParametersMsgs.append( 8 );
    WgValueMaxWriteFitParametersMsgs.append( 8 );
    WgValueMaxWriteFitParametersMsgs.append( 8 );        // 5
    WgValueMaxWriteFitParametersMsgs.append( 8 );
    WgValueMaxWriteFitParametersMsgs.append( 8 );
    WgValueMaxWriteFitParametersMsgs.append( 8 );
    WgValueMaxWriteFitParametersMsgs.append( 8 );
    WgValueMaxWriteFitParametersMsgs.append( 8 );        // 10
    WgValueMaxWriteFitParametersMsgs.append( 8 );
    WgValueMaxWriteFitParametersMsgs.append( 8 );
    WgValueMaxWriteFitParametersMsgs.append( 4 );
    //WgValueMaxWriteFitParametersMsgs.append( 4 );
    //WgValueMaxWriteFitParametersMsgs.append( 4 );        // 15
    //WgValueMaxWriteFitParametersMsgs.append( 4 );
    //WgValueMaxWriteFitParametersMsgs.append( 4 );
    //WgValueMaxWriteFitParametersMsgs.append( 4 );
    //WgValueMaxWriteFitParametersMsgs.append( 4 );
    //WgValueMaxWriteFitParametersMsgs.append( 4 );        // 20
    //WgValueMaxWriteFitParametersMsgs.append( 4 );
    //WgValueMaxWriteFitParametersMsgs.append( 4 );
    //WgValueMaxWriteFitParametersMsgs.append( 4 );
    //WgValueMaxWriteFitParametersMsgs.append( 4 );
    //WgValueMaxWriteFitParametersMsgs.append( 4 );        // 25

    // Reads
    dev->addMessage( rangeType( WgValueMaxRequest, 1 ), wgValueMaxRequestMsgs );
    dev->addMessage( rangeType( WgValueMaxAssem, 2 ), wgValueMaxAssemblyMsgs );
    dev->addMessage( rangeType( WgValueMaxHwVersion, 1 ), wgValueMaxHwVersionMsgs );
    dev->addMessage( rangeType( WgValueMaxSerial, 2 ), wgValueMaxSerialNumberMsgs );
    dev->addMessage( rangeType( wgStatus, 4 ), wgValueMaxStatusMsgs );
    dev->addMessage( rangeType( WgValueMaxDate, 1 ), wgValueMaxDateOfMfgMsgs );
    dev->addMessage( rangeType( WgValueMaxFitParameters, 13 ), WgValueMaxFitParametersMsgs );           // back to 13 frames at 8 bytes
    //dev->addMessage( rangeType( WgValueMaxFitParameters, 25 ), WgValueMaxFitParametersMsgs );           // I did have this 25 but I think this is # of frames which is 13

    // Writes
    dev->addMessage( rangeType( WgValueMaxWriteAssem, 2 ), wgValueMaxWriteAssemblyMsgs );
    dev->addMessage( rangeType( WgValueMaxWriteHwVersion, 1 ), wgValueMaxWriteHwVersionMsgs );
    dev->addMessage( rangeType( WgValueMaxWriteSerial, 2 ), wgValueMaxWriteSerialNumberMsgs );
    dev->addMessage( rangeType( WgValueMaxWriteDate, 1 ), wgValueMaxWriteDateOfMfgMsgs );
    dev->addMessage( rangeType( WgValueMaxWriteFitParameters, 13 ), WgValueMaxWriteFitParametersMsgs );   // back to 13 frames at 8 bytes
    //dev->addMessage( rangeType( WgValueMaxWriteFitParameters, 25 ), WgValueMaxWriteFitParametersMsgs ); // I did have this 25 but I think this is # of frames which is 13
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hobartCanWeigher::sendWeigherMessage( unsigned int id, const unsigned char *buffer, size_t bufferLen )
{
    return sendData( id, buffer, bufferLen );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hobartCanWeigher::sendWeigherMessageBasic( weigherMessage msgType )
{
    static const size_t msgLen( 1 );            // THIS IS A ONE - NOT AN L as in long

    unsigned char msg[16];
    msg[0] = 0;

    return (msgLen == sendWeigherMessage( msgType, msg, msgLen ));      // only works if msgLen = 1 (One) like here
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hobartCanWeigher::sendValueMaxMessageRequest(  ValueMaxReq vmtype )
{
    std::cout << "sendValueMaxMessageRequest::WgValueMaxRequest with vmtype: " << vmtype << std::endl;

    static const size_t msgLen( 1 );            // THIS IS A ONE - NOT AN L as in long

    unsigned char msg[16];
    msg[0] = vmtype;

    // did have vmType as arg1 here but was wrong. The vmtype is just the msg. The first arg is the request, and they are all request msgs. Just different vmType
    return (sendWeigherMessage( WgValueMaxRequest, msg, msgLen ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanWeigher::recvWeigherMessage( unsigned int id, const unsigned char *buffer, size_t bufferLen )
{
    if( id != 1026)
        std::cout << "\nHCW::recvWeigherMessage() START id=" << id << "   bufferLen=" << bufferLen << std::endl;

    // std::string buf_test((char*) buffer);                                 // WORKS - c style but not suggested.

    if( id >= 1056 && id <= 1133 )
        std::cout << "HCW::recvWeigherMessage() id " << id << "   if between 1056 and 1133" << std::endl;

    switch ( id )
    {
        case wgWakeup:
        {
            std::cout << "HCW::recvWeigherMessage()::case::wgWakeup  " << std::endl;
            break;
        }

        case WgValueMaxDate:
        {
            std::cout << "\nhobartCanWeigher::recvWeigherMsg::case Date " << std::endl;

            if( 4 <= bufferLen )
            {
                std::string incoming_buffer;                                            // WORKS
                incoming_buffer.append( reinterpret_cast<const char*>( buffer) );       // WORKS - more c++ish

                std::cout << "HCW::recvWeigherMessage()::case::WgValueMaxDate #2 bufferLen: " << bufferLen << std::endl;
                rawDate_       = be32toh( *reinterpret_cast<const uint32_t*>( &buffer[0] ) );
                std::cout << "hobartCanWeigher::recvWeigherMsg::case Date after casting and conversion #2: " << rawDate_ << std::endl;
                handleValueMaxDate(rawDate_);

                // This caused me nothing but grief
                //std::cout << "\nhobartCanWeigher::recvWeigherMsg::case Date b4 hexToInt on buffer: " << incoming_buffer << std::endl;       // print buffer string
                //uint32_t hexDate = hexToUnsignedInt( buffer );
                //std::cout << "hobartCanWeigher::recvWeigherMsg::case hex Date after reverse: " << hexDate << std::endl;               // print hex string
                //handleValueMaxDate(hexDate);
            }

            break;
        }

        case WgValueMaxAssem:
        {
            std::cout << "HCW::recvWeigherMessage()::case::WgValueMaxAssem  " << std::endl;

            std::string incoming_buffer;                                            // WORKS
            incoming_buffer.append( reinterpret_cast<const char*>( buffer) );       // WORKS - more c++ish

            QString verifiedAssembNum  = QString::fromStdString(incoming_buffer);                                         // testing step
            std::cout << "HCW::recvWeigherMessage(): AssemblyStr verifiedAssembNum: " << verifiedAssembNum.toStdString() << std::endl;    // testing step

            handleValueMaxAssemblyNumber(verifiedAssembNum);                                                              // gets the assembly  up to calibu to save it
            break;
        }

        case WgValueMaxHwVersion:
        {
            std::cout << "HCW::recvWeigherMessage()::case::WgValueMaxHwVersion  " << std::endl;

            std::string incoming_buffer;                                            // WORKS
            incoming_buffer.append( reinterpret_cast<const char*>( buffer) );       // WORKS - more c++ish

            QString verifiedHWVersion = QString::fromStdString(incoming_buffer);                                         // testing step
            std::cout << "RX STEP 5: HW VersionStr: " << verifiedHWVersion.toStdString() << std::endl;                    // testing step

            handleValueMaxHWVersion(verifiedHWVersion);                                                                   // gets the HW  up to calibui to save it
            break;
        }

        case WgValueMaxSerial:
        {
            std::cout << "HCW::recvWeigherMessage()::case::WgValueMaxSerial  " << std::endl;

            std::string incoming_buffer;                                            // WORKS
            incoming_buffer.append( reinterpret_cast<const char*>( buffer) );       // WORKS - more c++ish

            std::cout << "HCW::recvWeigherMessage(): simple incoming_buffer: " << incoming_buffer << std::endl;
            std::cout << "HCW::recvWeigherMessage(): " << std::uppercase << std::hex << std::setfill('0') << std::setw( 4 ) << id << std::dec <<
                         " stdstr incoming_buffer: " << incoming_buffer << std::endl;

            QString verifiedSerialNum = QString::fromStdString(incoming_buffer);                    // test then delete
            std::cout << "HCW::recvWeigherMessage(): Serial Num QStr verifiedSerialNum: " << verifiedSerialNum.toStdString() << std::endl;

            std::cout << "HCW::recvWeigherMessage(): Serial Num buffer bufferLen: "             << bufferLen << std::endl;
            std::cout << "HCW::recvWeigherMessage(): Serial Num stdstr incoming_buffer len: "   << incoming_buffer.length() << std::endl;
            std::cout << "HCW::recvWeigherMessage(): Serial Num stdstr verifiedSerialNum len: "   << verifiedSerialNum.length() << std::endl;

            handleValueMaxSerialNumber(verifiedSerialNum);                      // gets the serial num  up to calibui to save it
            break;
        }

        case wgStatus:
        {
            // originally this was triggered by a 0x402, now its triggered by 0x452
            if( DISPLAY_STATUS_MESSAGES )
                std::cout << "HCW::recvWeigherMessage()::case::wgSTATUS  " << std::endl;

            std::string incoming_buffer;                                            // TESTING - just want to see what this looks like
            incoming_buffer.append( reinterpret_cast<const char*>( buffer) );       // WORKS - more c++ish

            switch( status_ctr_ )
            {
                case Frame1:
                    status_ctr_++;
                    flags_ = be16toh( *reinterpret_cast<const uint16_t*>( &buffer[0] ) );
                    rawAtoDCounts_ = be32toh( *reinterpret_cast<const uint32_t*>( &buffer[2] ) );
                    if( DISPLAY_STATUS_MESSAGES )
                    {
                        std::cout << "Frame 1 STATUS: flags_: "            << flags_                << std::endl;
                        std::cout << "Frame 1 STATUS: rawAtoDCounts_: "   << rawAtoDCounts_         << std::endl;
                    }
                break;

                case Frame2:
                    status_ctr_++;
                    nonZeroCalibCounts_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[0] ) );
                    zeroCalibCounts_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[4] ) );
                    if( DISPLAY_STATUS_MESSAGES )
                    {
                        std::cout << "Frame 2 STATUS: nonZeroCalibCounts_: "            << nonZeroCalibCounts_      << std::endl;
                        std::cout << "Frame 2 STATUS: zeroCalibCounts_: "               << zeroCalibCounts_         << std::endl;
                    }
                break;

                case Frame3:
                    status_ctr_++;
                    acclXRawCounts_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[0] ) );
                    acclYRawCounts_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[4] ) );
                    if( DISPLAY_STATUS_MESSAGES )
                    {
                        std::cout << "Frame 3 STATUS: acclXRawCounts_: "                << acclXRawCounts_          << std::endl;
                        std::cout << "Frame 3 STATUS: acclYRawCounts_: "                << acclYRawCounts_          << std::endl;
                    }
                break;

                case Frame4:
                    status_ctr_ = 1;
                    acclZRawCounts_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[0] ) );
                    acclXTiltDegrees_ = be16toh( *reinterpret_cast<const int16_t*>( &buffer[4] ) );
                    acclYTiltDegrees_ = be16toh( *reinterpret_cast<const int16_t*>( &buffer[6] ) );

                    if( DISPLAY_STATUS_MESSAGES )
                    {
                        // Now we can send everything
                        std::cout << "HCW::recvWeigherMessage() wgSTATUS GOT ALL 30: " << std::endl;
                        std::cout << "HCW::recv::wgSTATUS: rawAtoDCounts: "       << rawAtoDCounts_ << std::endl;
                        std::cout << "HCW::recv::wgSTATUS: acclXRawCounts: "      << acclXRawCounts_ << std::endl;
                        std::cout << "HCW::recv::wgSTATUS: acclYRawCounts: "      << acclYRawCounts_ << std::endl;
                        std::cout << "HCW::recv::wgSTATUS: acclZRawCounts: "      << acclZRawCounts_ << std::endl;
                        std::cout << std::endl;
                    }

                    handleWeigherStatus( flags_, rawAtoDCounts_, nonZeroCalibCounts_, zeroCalibCounts_, acclXRawCounts_, acclYRawCounts_, acclZRawCounts_, acclXTiltDegrees_, acclYTiltDegrees_ );
                break;

                default:
                    std::cout << "HCW::recvWeigherMessage(): case default: wgStatus Something bad happened, bufferLen: " << bufferLen << "   status_ctr_: " << status_ctr_<< std::endl;
                break;
            }

            break;
        }

        case WgValueMaxFitParameters:                                                                                 // repurposed - eventually rename
        {
            std::cout << "HCW::recvWeigherMessage()::case::WgValueMaxFitParameters  " << std::endl;
            std::cout << "====== FIT STRUCT ======" << std::endl;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // This is just some testing converting the *buffer to a string, but I dont think Im gonna do that.
            // Parse out args I need instead and pass them along individually

            std::string incoming_buffer;
            incoming_buffer.append( reinterpret_cast<const char*>( buffer) );                                       // WORKS - more c++ish
            std::cout << "FIT step: FrameCtr: " << fitbuf_ctr_ << "     incoming_buffer: " << incoming_buffer <<  "     bufferLen: " << bufferLen << std::endl;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            switch( fitbuf_ctr_ )
            {
                case Frame1:
                    a1_g_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[0] ) );
                    a2_m_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[4] ) );
                    fitbuf_ctr_++;
                break;

                case Frame2:
                    a3_zerovalue_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[0] ) );
                    a4_spanvalue_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[4] ) );
                    fitbuf_ctr_++;
                break;

                case Frame3:
                    a5_Z0_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[0] ) );
                    a6_D_  = be32toh( *reinterpret_cast<const int32_t*>( &buffer[4] ) );
                    fitbuf_ctr_++;
                break;

                case Frame4:
                    a7_x2_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[0] ) );
                    a8_x1_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[4] ) );
                    fitbuf_ctr_++;
                break;

                case Frame5:
                    a9_x0_  = be32toh( *reinterpret_cast<const int32_t*>( &buffer[0] ) );
                    a10_y2_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[4] ) );
                    fitbuf_ctr_++;
                break;

                case Frame6:
                    a11_y1_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[0] ) );
                    a12_y0_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[4] ) );
                    fitbuf_ctr_++;
                break;

                case Frame7:
                    a13_xrot0_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[0] ) );
                    a14_xrot1_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[4] ) );
                    fitbuf_ctr_++;
                break;

                case Frame8:
                    a15_xrot2_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[0] ) );
                    a16_yrot0_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[4] ) );
                    fitbuf_ctr_++;
                break;

                case Frame9:
                    a17_yrot1_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[0] ) );
                    a18_yrot2_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[4] ) );
                    fitbuf_ctr_++;
                break;

                case Frame10:
                    a19_zrot0_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[0] ) );
                    a20_zrot1_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[4] ) );
                    fitbuf_ctr_++;
                break;

                case Frame11:
                    a21_zrot2_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[0] ) );
                    a22_xgain_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[4] ) );
                    fitbuf_ctr_++;
                break;

                case Frame12:
                    a23_ygain_  = be32toh( *reinterpret_cast<const int32_t*>( &buffer[0] ) );
                    a24_gcoeff_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[4] ) );
                    fitbuf_ctr_++;
                break;

                case Frame13:
                    a25_checksum_ = be32toh( *reinterpret_cast<const int32_t*>( &buffer[0] ) );
                    fitbuf_ctr_ = 1;

                    // We have everything - lets send it on home
                    std::cout << "HCW::recvWeigherMessage() - Frame13: " << std::endl;
                    std::cout << "FIT: a1_g: "         << a1_g_         << std::endl;
                    std::cout << "FIT: a2_m: "         << a2_m_         << std::endl;
                    std::cout << "FIT: a3_zerovalue: " << a3_zerovalue_ << std::endl;
                    std::cout << "FIT: a4_spanvalue: " << a4_spanvalue_ << std::endl;
                    std::cout << "FIT: a5_Z0: "        << a5_Z0_        << std::endl;
                    std::cout << "FIT: a6_D: "         << a6_D_         << std::endl;
                    std::cout << "FIT: a7_x2: "        << a7_x2_        << std::endl;
                    std::cout << "FIT: a8_x1: "        << a8_x1_        << std::endl;
                    std::cout << "FIT: a9_x0: "        << a9_x0_        << std::endl;
                    std::cout << "FIT: a10_y2: "       << a10_y2_       << std::endl;
                    std::cout << "FIT: a11_y1: "       << a11_y1_      << std::endl;
                    std::cout << "FIT: a12_y0: "       << a12_y0_       << std::endl;
                    std::cout << "FIT: a13_xrot0: "    << a13_xrot0_    << std::endl;
                    std::cout << "FIT: a14_xrot1: "    << a14_xrot1_    << std::endl;
                    std::cout << "FIT: a15_xrot2: "    << a15_xrot2_    << std::endl;
                    std::cout << "FIT: a16_yrot0: "    << a16_yrot0_    << std::endl;
                    std::cout << "FIT: a17_yrot1: "    << a17_yrot1_    << std::endl;
                    std::cout << "FIT: a18_yrot2: "    << a18_yrot2_    << std::endl;
                    std::cout << "FIT: a19_zrot0: "    << a19_zrot0_    << std::endl;
                    std::cout << "FIT: a20_zrot1: "    << a20_zrot1_    << std::endl;
                    std::cout << "FIT: a21_zrot2: "    << a21_zrot2_    << std::endl;
                    std::cout << "FIT: a22_xgain: "    << a22_xgain_    << std::endl;
                    std::cout << "FIT: a23_ygain: "    << a23_ygain_    << std::endl;
                    std::cout << "FIT: a24_gcoeff: "   << a24_gcoeff_   << std::endl;
                    std::cout << "FIT: a25_checksum: " << a25_checksum_ << std::endl;

                    handleValueMaxFitParameters(a1_g_, a2_m_, a3_zerovalue_, a4_spanvalue_, a5_Z0_, a6_D_, a7_x2_, a8_x1_, a9_x0_, a10_y2_, a11_y1_, a12_y0_, a13_xrot0_, a14_xrot1_, a15_xrot2_, a16_yrot0_, a17_yrot1_, a18_yrot2_, a19_zrot0_, a20_zrot1_, a21_zrot2_, a22_xgain_, a23_ygain_, a24_gcoeff_, a25_checksum_);
                break;

                default:
                    std::cout << "\nhobartCanWeigher::recvMsg: Something BAD happened with frame: " << fitbuf_ctr_ << std::endl;
                break;
            }
            break;
        }

        default:
            std::cout << "RX STEP 5: recvWeigherMessage() in default: case" << std::endl;
            break;
    }

    //std::cout << "HCW::recvWeigherMessage():: DONE  case" << std::endl;
}

uint32_t hobartCanWeigher::hexToUnsignedInt(const unsigned char *bufferOG)
{
    // reverses the order of the bytes, IN *buffer, 2 at a time
    unsigned char *buffer(const_cast<unsigned char*>(reinterpret_cast<const unsigned char *>(bufferOG)));
    int bufLen = sizeof(buffer);
    unsigned char *istart = buffer;
    unsigned char *iend = istart + bufLen;
    std::reverse(istart, iend);

    // buffer is reversed, now pack it into a qstring
    QString s;
    QString result;
    for ( int i = 0; i < bufLen; i++)
    {
        s = QString("%1").arg(buffer[i],2,16,QLatin1Char('0'));
        result.append(s);
    }

    std::cout << "HCW::hexToUnsignedInt() = Reversed RECV buffer: " << result.toStdString() << std::endl;

    // gets me a string from qstring
    std::string utf8_text = result.toUtf8().constData();
    unsigned int hex_to_int = strtoul(utf8_text.c_str(), NULL, 16);
    std::cout << "HCW::hexToUnsignedInt() return unsigned int: " << hex_to_int << std::endl;

    return hex_to_int;
}
