/**
 * @file MtdSectionStatistics.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief HTi serial flash statistics section
 * Created on: Dec 3, 2012
 */

#ifndef MTDSECTIONSTATISTICS_H_
#define MTDSECTIONSTATISTICS_H_
#include <string>

class MtdSectionStatistics
{
	public:
	MtdSectionStatistics();
	MtdSectionStatistics(const std::string &s);
	~MtdSectionStatistics();

	bool isSectionBlank();

	unsigned long getSectionCrc(){return crc_;}
	void setSectionCrc(unsigned long crc) {crc_ = crc;}
	bool isSectionDataValid(unsigned long dataCrc);

	unsigned long getSectionWriteCount() {return writeCounter_;}
	void setSectionWriteCount(unsigned long count) {writeCounter_ = count;}
	void incSectionWriteCount(){writeCounter_++;}

	void setSectorType(unsigned int sectorType) {sectorType_ = sectorType;}
	const std::string serialize() const;

	#define MTD_MTBF	100000	//see data sheet

	private:
		unsigned int sectionMarker_;
		unsigned int sectorType_;
		unsigned long writeCounter_;


		mutable unsigned long crc_;
		mutable std::string serializedData_;
};






#endif
