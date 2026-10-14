#ifndef INTELHEXPARSER_H
	#define INTELHEXPARSER_H

	#include <vector>
	#include <fstream>
	#include "HobString.h"
	#include "PAL_platformheader.h"

	#define MAX_INTEL_HEX_RECORD_LENGTH 4096

	typedef struct _INTEL_HEX_RECORD
	{
	        UINT16  Length;
	        UINT16  Address;
	        UINT8  Type;
	        UINT8  Data[MAX_INTEL_HEX_RECORD_LENGTH];
	} INTEL_HEX_RECORD;

	class IntelHexParser
	{
		public:
			IntelHexParser();
			IntelHexParser(HobString &filename);
			IntelHexParser(const char *filename);
			~IntelHexParser();

			void setFilename(HobString &filename);
			UINT16 getNumRecords() const;
			bool getFirstRecord(INTEL_HEX_RECORD &hexRecord);
			bool getNextRecord(INTEL_HEX_RECORD &hexRecord);

		private:
			typedef struct 
			{
			        UINT16  Length;
			        UINT16  Address;
			        UINT8  Type;
			} IntelHexRecord;

			void parseFile();
			bool parseRecord(HobString &hexRecord);

			HobString filename;

			std::vector<IntelHexRecord *> hexRecords;
			std::vector<IntelHexRecord *>::iterator recordIter;

			std::vector<HobString> hexData;
			std::vector<HobString>::iterator dataIter;

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};

#endif //INTELHEXPARSER_H

