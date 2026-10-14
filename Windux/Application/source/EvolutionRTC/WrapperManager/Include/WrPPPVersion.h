/**
 * @file WrPPPVersion.h
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Printer plate position version message
 * Created on: Mar 17, 2021
*/
#ifndef WRPPPVERSION_H
#define WRPPPVERSION_H
#include "WrapperGeneric.h"

class PPPVersion : public WrapperGeneric
{
	public:
    PPPVersion( Hobart::CEvolMsgReceiver *pHobThread = NULL );
    PPPVersion( const char *buffer );
    ~PPPVersion();

    unsigned short getProductId() const { return pid_; }
    void setProductId( unsigned short id ) { pid_ = id; }
    std::string getSoftwareVersion() const;    
    void setSoftwareMajor(unsigned char major ) { swMajor_ = major; }
    void setSoftwareMinor(unsigned char minor ) { swMinor_ = minor; }
    void setSoftwareBuild(unsigned char build ) { swbuild_ = build; }

    std::string getHardwareVersion() const;
    void setHardwareMajor(unsigned char major ) { hwMajor_ = major; }
    void setHardwareMinor(unsigned char minor ) { hwMinor_ = minor; }

    std::string getBootVersion() const;
    void setBootMajor(unsigned char major) { blMajor_ = major; }
    void setBootMinor(unsigned char minor) { blMinor_ = minor; }
    void setBootBuild(unsigned char build) { blBuild_ = build; }

    const char *serialize() const;
    
    void show();

	private:
    unsigned short  pid_;       //Product id
    unsigned char   swMajor_;
    unsigned char   swMinor_;
    unsigned char   swbuild_;
    
    unsigned char   hwMajor_;
    unsigned char   hwMinor_;

    unsigned char   blMajor_;
    unsigned char   blMinor_;
    unsigned char   blBuild_;
};
#endif