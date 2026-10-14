#ifndef WRAPPERSTATUSINFO_H
#define WRAPPERSTATUSINFO_H

#include <string>

#include "Wrapper.h"

#include "WrapperStateInfo.h"

class WrapperStatusInfo
{
	public:
		WrapperStatusInfo();
		WrapperStatusInfo(const char *buffer);
		~WrapperStatusInfo();

		const char *serialize() const;
		UINT32 getSerializedLength() const;

		void show();

		void setRate(UINT16 rate);
		UINT16 getRate() const;

		bool isReportStatus() const;
		void setReportStatus();
		void clearReportStatus();

		bool isPusherHome() const;
		void setPusherHome();
		void clearPusherHome();

		bool isRateAnalysis() const;
		void setRateAnalysis();
		void clearRateAnalysis();

		bool isOpenWindow() const;
		void setOpenWindow();
		void clearOpenWindow();

		bool isMainPower() const;
		void setMainPower();
		void clearMainPower();

		bool isMainMotor() const;
		void setMainMotor();
		void clearMainMotor();

		bool isLimpHome() const;
		void setLimpHome();
		void clearLimpHome();

		// Pack Width Variants
		WrPackWidth getPackWidth() const;
		void setPackWidth(WrPackWidth packWidth);

		bool isNarrowPackage() const;
		bool isWidePackage() const;
		bool isFilmWidthIdle() const;
		bool isAutoWidthSelect() const;

		void setNarrowPackage();
		void setWidePackage();
		void setFilmWidthIdle();
		void setAutoWidthSelect();
		
		// Film Tension Variants
		WrFilmTension getFilmTension() const;
		void setFilmTension(WrFilmTension filmTension);

		bool isLowFilmTension() const;
		bool isMediumFilmTension() const;
		bool isHighFilmTension() const;

		void setLowFilmTension();
		void setMediumTension();
		void setHighFilmTension();

		void setStateInfo(WrapperStateInfo &stateInfo);
		const WrapperStateInfo &getStateInfo() const;

		void setStatusBits(WrStatusBits statusBits);
		WrStatusBits getStatusBits() const;

		void setStatusBits(UINT32 statusBits);

		bool equals(WrapperStatusInfo &wrStatusInfo);

	private:
		mutable UINT16		rate;
		bool				reportStatus;
		bool				pusherHome;
		bool				rateAnalysis;
		bool				openWindow;
		bool				mainPower;
		bool				mainMotor;
		bool				limpHome;
		WrPackWidth			packWidth;
		WrFilmTension		filmTension;
		WrapperStateInfo	stateInfo;

		mutable std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};
#endif // WRAPPERSTATUSINFO_H
