#ifndef WRAPPERSTATEINFO_H
#define WRAPPERSTATEINFO_H

#include <string>

#include "Wrapper.h"

class WrapperStateInfo
{
	public:
		WrapperStateInfo(WrapperState state = WrUnknown);
		WrapperStateInfo(const char *buffer);
		~WrapperStateInfo();

		const char *serialize() const;
		UINT32 getSerializedLength() const;

		void show();

		// Wrapper State Variants
		WrapperState getWrapperState() const;
		void setWrapperState(WrapperState state);

		bool isSwitching();
		bool isIdle();
		bool isPaused();
		bool isRunning();
		bool isDisabled();
		bool isBoardTest();
		bool isStandBy();
		bool isUnknown();

		void setSwitching();
		void setIdle();
		void setPaused();
		void setRunning();
		void setDisabled();
		void setBoardTest();
		void setStandBy();
		void setUnknown();

		bool equals(const WrapperStateInfo &wrStateInfo);
	private:
		WrapperState state;

		mutable std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};
#endif // WRAPPERSTATEINFO_H
