/**
 * @file NGWDimensionOverrides.h
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Wrapper to ignore camera package dimensions.
 * Created on: Oct 21, 2021
 * 
 */
#ifndef NGWDIMENSIONOVERRIDES_H
#define NGWDIMENSIONOVERRIDES_H
#include "EvolMessage.h"
#include "WrapperEvent.h"

class NGWDimensionOverrides : public EvolMessage
{
	public:

	NGWDimensionOverrides( Hobart::CEvolMsgReceiver *pHobThread = NULL );
	NGWDimensionOverrides( const char *buffer );
	~NGWDimensionOverrides();

    UINT16 getOverrideLength() const                { return length_; }
    void setOverrideLength( UINT16 length )         { length_ = length; }
    
    UINT16 getOverrideWidth() const                 { return width_; }
    void setOverrideWidth( UINT16 width )           { width_ = width; }

    UINT16 getOverrideLipHeight() const             { return lipHeight_; }
    void setOverrideLipHeight( UINT16 lipHeight )   { lipHeight_ = lipHeight; }


	const char *serialize()const;

	private:
    UINT16 length_;
    UINT16 width_;
    UINT16 lipHeight_;
};
#endif