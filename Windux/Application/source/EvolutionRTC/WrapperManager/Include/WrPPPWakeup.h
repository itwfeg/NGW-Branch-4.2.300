/**
 * @file WRPPPWAKEUP.h
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Printer plate position wakeup message
 * Created on: Mar 9, 2021
 */
#ifndef WRPPPWAKEUP_H
#define WRPPPWAKEUP_H
#include "WrapperGeneric.h"

class PPPWakeup : public WrapperGeneric
{
	public:
    PPPWakeup( Hobart::CEvolMsgReceiver	*pHobThread	= NULL );
    PPPWakeup( const char *buffer );
    ~PPPWakeup();

    unsigned short getProductId() { return productId_; }
    void setProductId( unsigned short id ) { productId_ = id; }

    unsigned short getRate() { return rate_; }
    void setRate( unsigned short milliSeconds ) { rate_ = milliSeconds; }

    std::string getSoftwareVersion() { return swVer_; }     
    void setSoftwareMajor(unsigned char major ) { swMajor_ = major; }
    void setSoftwareMinor(unsigned char minor ) { swMinor_ = minor; }
    void setSoftwareBuild(unsigned char build ) { swbuild_ = build; }

    std::string getHardwareVersion() { return hwVer_; }
    void setHardwareMajor(unsigned char major ) { hwMajor_ = major; }
    void setHardwareMinor(unsigned char minor ) { hwMinor_ = minor; }

    const char *serialize() const;
    
    void show();

	private:
    unsigned short  productId_;
    unsigned short  rate_;        //rate of measurements in milliseconds
    unsigned char   swMajor_;
    unsigned char   swMinor_;
    unsigned char   swbuild_;
    std::string     swVer_;
    unsigned char   hwMajor_;
    unsigned char   hwMinor_;
    std::string     hwVer_;
};
#endif

