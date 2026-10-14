#ifndef STDRECORD_H
#define STDRECORD_H       



#ifdef WIN32_OS
#include "..\..\edb.h"
#else
#include "../../edb.h"
#endif

#include <string>
#include <map>


class STDRecord
{
	public:
		/* Constructors and destructor. */
		STDRecord(const std::string& type);
		STDRecord(const std::string& line, int);
		STDRecord(const STDRecord& rec);
		virtual ~STDRecord();


		/* Subtypes */
		// May want to have the value side of the map be a little more complex
		// than a simple string. The FieldData could contain a boolean as well
		// to indicate whether the field is null vs. simply blank.
		typedef std::map<std::string, std::string> fields_type;


		/* Getter and setter methods. */
		const std::string& getType() const;
		const std::string& getField(const std::string& tag) const;
		std::string& setField(const std::string& tag, const std::string& value);


		/* Processing methods. */
		bool isFieldNull(const std::string& tag) const;

		
		/* Extractor. */
//		STDRecord& operator>>(const std::string& element);


		/* Printer. */
		friend std::ostream& operator<<(std::ostream& os, const STDRecord& rec);
//		friend std::ostream& operator<<(std::ostream& os, const STDRecord::iterator& it);


		/* Constants. */
		static const char FIELD_SEPARATOR = '\x1F';
		static const std::string::size_type FIELD_SEPARATOR_SIZE = 1;
		static const std::string::size_type TAG_SIZE = 6;
		static const std::string TYPE_TAG;
		static const std::string FIELD_NOT_FOUND;


	private:
		/* Data elements. */
		std::string type;
		fields_type fields;

		// Do not allow the default assignment operator.
		STDRecord operator=(const STDRecord& rhs);

		/* Private methods. */
		void parseRecord(const std::string& s);
		void parseField(const std::string& s, std::string::size_type begin,
				std::string::size_type end);
};



#endif // STDRECORD_H

