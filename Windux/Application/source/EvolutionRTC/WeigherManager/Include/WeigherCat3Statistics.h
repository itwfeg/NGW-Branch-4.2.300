/**
 * @file WeigherCat3Statistics.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief Message weigher response to request for CAT3 statistics.
 * Created on: July 1, 2016
 */

#ifndef WEIGHERCAT3STATISTICS_H_
#define WEIGHERCAT3STATISTICS_H_
#include "EvolMessage.h"
#include "Weigher.h"
#include "CEvolMsgReceiver.h"

class WeigherCat3Statistics : public EvolMessage
{
	public:

		WeigherCat3Statistics(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		WeigherCat3Statistics(const char *buffer);
		~WeigherCat3Statistics();

		unsigned char getTotalPages() const							{ return totalPages; }
		void setTotalPages(unsigned char pages) 					{ totalPages = pages; }

		unsigned char getTotalRecordsPerPage() const				{ return totalRecordsPerPage; }
		void setTotalRecordsPerPage(unsigned char value)			{ totalRecordsPerPage = value; }

		unsigned char getCurrentPage() const						{ return currentPage; }
		void setCurrentPage(unsigned char page)						{ currentPage = page; }

		unsigned short  getTotalFreeRecords() const					{ return totalFreeRecords; }
		void setTotalFreeRecords(unsigned short  value)				{ totalFreeRecords = value; }

		unsigned short  getTotalRecordedEvents() const				{ return totalRecordedEvents; }
		void setTotalRecordedEvents(unsigned short  value)			{ totalRecordedEvents = value; }

		unsigned short  getTotalCalibrationRecords() const			{ return totalCalibrationRecords; }
		void setTotalCalibrationRecords(unsigned short  value)		{ totalCalibrationRecords = value; }

		unsigned short getTotalConfigurationRecords() const			{ return totalConfigurationRecords; }
		void setTotalConfigurationRecords(unsigned short value)		{ totalConfigurationRecords = value; }

		unsigned short getGainCoefficientCounter() const			{ return gainCoefficientCounter; }
		void setGainCoefficientCounter(unsigned short value)		{ gainCoefficientCounter = value; }

		unsigned short getOffsetCoefficientCounter() const			{ return offsetCoefficientCounter; }
		void setOffsetCoefficientCounter(unsigned short value)		{ offsetCoefficientCounter = value; }

		unsigned short getGainFactorCounter() const					{ return gainFactorCounter; }
		void setGainFactorCounter(unsigned short value)				{ gainFactorCounter = value; }

		unsigned short getCenterOfMainCounter() const				{ return centerOfMainCounter; }
		void setCenterOfMainCounter(unsigned short value)			{ centerOfMainCounter = value; }

		unsigned short getZeroReferenceCounter() const				{ return zeroReferenceCounter; }
		void setZeroReferenceCounter(unsigned short value)			{ zeroReferenceCounter = value; }

		unsigned short getWeigherModelCounter() const				{ return weigherModelCounter; }
		void setWeigherModelCounter(unsigned short value)			{ weigherModelCounter = value; }

		unsigned short getMinWeightPrintCounter() const				{ return minWeightPrintCounter; }
		void setMinWeightPrintCounter(unsigned short value)			{ minWeightPrintCounter = value; }

		unsigned short getMaxWeightCounter() const					{ return maxWeightCounter; }
		void setMaxWeightCounter(unsigned short value)				{ maxWeightCounter = value; }

		unsigned short getFilterSpeedCounter() const				{ return filterSpeedCounter; }
		void setFilterSpeedCounter(unsigned short value)			{ filterSpeedCounter = value; }

		unsigned short getDivisionSizeCounter() const				{ return divisionSizeCounter; }
		void setDivisionSizeCounter(unsigned short value)			{ divisionSizeCounter = value; }

		unsigned short getWeigherModeCounter() const				{ return weigherModeCounter; }
		void setWeigherModeCounter(unsigned short value)			{ weigherModeCounter = value; }

		const char *serialize() const;

		void show();

	private:
		unsigned char 	totalPages;
		unsigned char 	totalRecordsPerPage;
		unsigned char 	currentPage;
	    unsigned short 	totalFreeRecords;               /* Total number of free records in the serial flash */
	    unsigned short  totalRecordedEvents;            /* Total number of CAT3 events saved */
	    unsigned short  totalCalibrationRecords;        /* Total number of saved calibration events */
	    unsigned short  totalConfigurationRecords;      /* Total number of saved configuration events */
	    /* sealable parameter counters (requirement 8.24c, 8.24d ) */
	    unsigned short  gainCoefficientCounter;         /* Total number of times gain coefficent has changed */
	    unsigned short  offsetCoefficientCounter;       /* Total number of times offset coefficent has changed */
	    unsigned short	gainFactorCounter;              /* Total number of times gain factor has changed */
	    unsigned short  centerOfMainCounter;            /* Total number of times center of maintenance has changed */
	    unsigned short  zeroReferenceCounter;           /* Total number of times zero reference has changed */
	    unsigned short  weigherModelCounter;            /* Total number of times weigher model has changed */
	    unsigned short  minWeightPrintCounter;          /* Total number of times min weight to print has changed */
	    unsigned short  maxWeightCounter;               /* Total number of times max weight has changed */
	    unsigned short  filterSpeedCounter;             /* Total number of times filter speed has changed */
	    unsigned short  divisionSizeCounter;            /* Total number of times division size has changed */
	    unsigned short	weigherModeCounter;				/* Total number of times weigh mode has changed */
};

#endif
