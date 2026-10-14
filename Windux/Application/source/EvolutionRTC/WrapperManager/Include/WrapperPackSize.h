#ifndef WRAPPERPACKSIZE_H
#define WRAPPERPACKSIZE_H
#include "PAL_platformheader.h"
#include <string>

class WrapperPackSize
{
	public:
		WrapperPackSize(UINT16 minLength = 0, UINT16 maxLength = 0,
						UINT16 minWidth = 0, UINT16 maxWidth = 0);
		WrapperPackSize(const char *buffer);
		virtual ~WrapperPackSize();

		const char *serialize() const;
		UINT32 getSerializedLength() const;

		void show();

		UINT16 getMinLength() const;
		void setMinLength(UINT16 minLength);

		UINT16 getMaxLength() const;
		void setMaxLength(UINT16 maxLength);

		UINT16 getMinWidth() const;
		void setMinWidth(UINT16 minWidth);

		UINT16 getMaxWidth() const;
		void setMaxWidth(UINT16 maxWidth);

	private:
		UINT16 minLength;
		UINT16 maxLength;
		UINT16 minWidth;
		UINT16 maxWidth;

		mutable std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};
#endif // WRAPPERPACKSIZE_H
