/*
 * NGWWrapBox.h
 *
 *  Created on: Oct 29, 2015
 *      Author: aaron
 */

#ifndef NGWWRAPBOX_H_
#define NGWWRAPBOX_H_
#include "WrapperGeneric.h"

class NGWWrapBox : public WrapperGeneric
{
	public:
	NGWWrapBox( Hobart::CEvolMsgReceiver *pHobThread = NULL );
	NGWWrapBox( const char *buffer );
	~NGWWrapBox();

	UINT8 getWrapBoxIndex() { return _wrapBoxIndex; }
	void setWrapBoxIndex( UINT8 index ){ _wrapBoxIndex = index; }

	std::string getBoxName() { return std::string((const char *)&_boxName[0]); }
	void setBoxName();

	UINT16 getLength() { return _length; }
	void setLength(UINT16 l) { _length = l; }

	UINT16 getWidth() { return _width; }
	void setWidth(UINT16 w) { _width = w; }

	UINT16 getLip() { return _lip; }
	void setLip( UINT16 l ) { _lip = l; }

	UINT16 getHeight() { return _height; }
	void setHeight( UINT16 h ) { _height = h; }

	UINT32 getPlu() { return _plu; }
	void setPlu( UINT32 p ) { _plu = p; }

	const char *serialize() const;

	#define _MAX_NAME_SIZE 10


	private:
	UINT8 _wrapBoxIndex;
	INT8 _boxName[_MAX_NAME_SIZE];
	UINT16 _length;
	UINT16 _width;
	UINT16 _lip;
	UINT16 _height;
	UINT32 _plu;

};

#endif /* NGWWRAPBOX_H_ */
