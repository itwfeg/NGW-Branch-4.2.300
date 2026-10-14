#ifndef STDFILE_H
#define STDFILE_H


#ifdef WIN32_OS
#include "..\..\edb.h"
#else
#include "../../edb.h"
#endif

#include "stdrecord.h"
#include "stdindex.h"

#include <string>



class STDFile
{
	public:
		/* Constructors and destructor. */
		STDFile(const std::string& fname, const std::string& pkey);
		virtual ~STDFile();


		/* Getter and setter methods. */


		/* Operations. */
		STDRecord* findRecord(const std::string& key, const std::string& searchField = "");
		STDRecord* first();
		STDRecord* next(const STDRecord* current, const std::string& searchField = "");
		STDRecord* previous(const STDRecord* current, const std::string& searchField = "");
		STDRecord* last();

		void saveRecord(const STDRecord* rec);
		int deleteRecord(const STDRecord* rec);
		int deleteAll();
		void indexFile();
		void compressFile();
		unsigned long int count();


		/* Extractor. */
//		STDFile& operator>>(STDRecord& record);


		/* Printer. */
//		friend std::ostream& operator<<(std::ostream& os, const STDFile& file);


		/* Constants. */


	private:
		/* Data elements. */
		std::string filename;
		std::string primaryKey;
		STDIndex primaryIndex;


		/* Private methods. */
		STDFile();                               // Prevent temporaries
		STDFile(const STDFile& f);               // Prevent copies
		STDFile& operator=(const STDFile& );     // Prevent assignments

		void createIndexFile();
		STDRecord* fetchRecord(STDIndex::position_type p);

		static std::string createIndexFilename(const std::string& fname,
				const std::string& key);
};



#endif // STDFILE_H

