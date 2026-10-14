#ifndef WRPPPREQUEST_H
#define WRPPPREQUEST_H
/**
 * @file WrPPPRequest.h
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Printer plate position request message
 * Created on: Mar 9, 2021
 */
#include "WrapperGeneric.h"

class PPPRequest : public WrapperGeneric
{
	public:

    typedef enum
    {
        _Unknown,
        _Config,
        _Version,
        _Cal,
        _Cal_Next,
        _Cal_Values,
        _PriDist,
        _SecDist,
        _Wake,
        _Start_Measure,
        _Stop_Measure,
        _Set_Rate,
        _Status,
        _CurrentPosition,
        _BootReq
    }PPPReq;
        
    PPPRequest( Hobart::CEvolMsgReceiver *pHobThread = NULL, PPPReq request = _Unknown, unsigned short data = 0 );
    PPPRequest( const char *buffer );
    ~PPPRequest();


    PPPReq getRequest() const { return request_; }
    void setRequest( PPPReq request ) { request_ = request; }

    unsigned short getData() const { return data_; }
    void setData( unsigned short data ) { data_ = data; }
    const char *serialize() const;
    
    void show();

	private:
    PPPReq request_;
    unsigned short data_;
         
};
#endif