/**
 * @file NGWWrapperResume.h
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief NGW Wrapper resume message. 
  * Created on: May 18, 2021
 */
#ifndef NGWWRAPPERRESUME_H
#define NGWWRAPPERRESUME_H
#include "WrapperGeneric.h"

class NGWWrapperResume : public WrapperGeneric
{
	public:
    NGWWrapperResume( Hobart::CEvolMsgReceiver	*pHobThread	= NULL );
    NGWWrapperResume( const char *buffer );
    ~NGWWrapperResume();

    typedef enum
    {
        PAUSE,
        RESUME,
        REJECT
    }NGWState;

    unsigned char getPkgId() const { return pkgId_; }
    void setPkgId( unsigned char val ) { pkgId_ = val; }
    NGWState getState() const { return ngwState_; }
    void setState( NGWState state ) { ngwState_ = state; }
    
    const char *serialize() const;
    
    void show();

	private:
    unsigned char pkgId_;
    NGWState ngwState_;    
 };
#endif