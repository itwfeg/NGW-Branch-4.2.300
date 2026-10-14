/**
 * @file NGWWrapperPause.h
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief NGW Wrapper pause message. 
 *        Wrapper pauses to determine if printer plates are in
 *        the correct position to apply labels
 * Created on: Mar 30, 2021
 */
#ifndef NGWWRAPPERPAUSE_H
#define NGWWRAPPERPAUSE_H
#include "WrapperGeneric.h"

class NGWWrapperPause : public WrapperGeneric
{
	public:
    NGWWrapperPause( Hobart::CEvolMsgReceiver	*pHobThread	= NULL );
    NGWWrapperPause( const char *buffer );
    ~NGWWrapperPause();

    typedef enum
    {
        PAUSE,
        RESUME,
        REJECT
    }NGWState;

    NGWState getState() const { return ngwState_; }
    void setState( NGWState state ) { ngwState_ = state; }

    unsigned char getPackageId() const { return pkgId_; }
    void setPackageId( unsigned char pkgId ) { pkgId_ = pkgId; }

    unsigned char getPrimaryPlatePosition() const { return priPlatePosition_; }
    void setPrimaryPlatePosition( unsigned char position ) { priPlatePosition_ = position; }
    unsigned char getRequiredPrimaryPlatePosition() const { return reqPriPlatePosition_; }
    void setRequiredPrimaryPlatePosition( unsigned char position ) { reqPriPlatePosition_ = position; }

    unsigned char getSecondaryPlatePosition() const { return secPlatePosition_; }
    void setSecondaryPlatePosition( unsigned char position ) { secPlatePosition_ = position; }
    unsigned char getRequiredSecondaryPlatePosition() const { return reqSecPlatePosition_; }
    void setRequiredSecondaryPlatePosition( unsigned char position ) { reqSecPlatePosition_ = position; }

    const char *serialize() const;
    
    void show();

	private:
    NGWState ngwState_;
    unsigned char pkgId_;
    unsigned char priPlatePosition_;
    unsigned char reqPriPlatePosition_;
    unsigned char secPlatePosition_;
    unsigned char reqSecPlatePosition_;
};
#endif