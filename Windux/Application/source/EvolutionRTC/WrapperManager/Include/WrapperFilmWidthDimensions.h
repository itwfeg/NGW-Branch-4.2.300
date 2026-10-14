#ifndef WRAPPERFILMWIDTHDIMENSIONS_H
#define WRAPPERFILMWIDTHDIMENSIONS_H

#include <string>

#include "Wrapper.h"

class WrapperFilmWidthDimensions
{
	public:
		WrapperFilmWidthDimensions();
		WrapperFilmWidthDimensions(const char *buffer);
		WrapperFilmWidthDimensions(const WrapperFilmWidthDimensions &wfwd);
		~WrapperFilmWidthDimensions();

		const char *serialize() const;
		UINT32 getSerializedLength() const;

		void show() const;

		UINT16 getRollWidth() const;
		void setRollWidth(UINT16 length);

		UINT16 getRollDiameter() const;
		void setRollDiameter(UINT16 length);

		UINT16 getRemainingOnRoll() const;
		void setRemainingOnRoll(UINT16 length);

		void operator=(const WrapperFilmWidthDimensions &filmDim);


	private:
	    UINT16	rollWidth;
	    UINT16	rollDiameter;
	    UINT16	remainingOnRoll;

		mutable std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif // WRAPPERFILMWIDTHDIMENSIONS_H
