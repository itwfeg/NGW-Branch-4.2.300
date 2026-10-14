#ifndef SQLITEFIELD_H
#define SQLITEFIELD_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteField
	{
		friend class SqliteRecord;

		public:

			// Types
			enum Type
			{
				STRING,
				INTEGER,
				DATETIME,
				INTBOOLEAN,
				INVALID
			};

			static const int SqliteTRUE;
			static const int SqliteFALSE;
			static const std::string SqliteBOOLEAN;
			static const std::string SqliteDATETIME;
			static const std::string SqliteINTEGER;
			static const std::string SqliteTEXT;


			// Constructors and destructor
			SqliteField(const SqliteField& );
			SqliteField(const std::string fieldName, Type colType = INVALID);
			SqliteField(const std::string fieldName, Type colType, const std::string& strValue);
			SqliteField(const std::string fieldName, Type colType, int intValue);
			virtual ~SqliteField();


			// Getters and setters
			const std::string getName() const { return name_; }
			void setName(const std::string fieldName) { name_ = fieldName; }

			Type getValueType() const { return valueType_; }
			std::string getValueString() const { return valueString_; }

			operator int() const;
			void setValue(int value);
			operator std::string() const;
			void setValue(const std::string& value);

			bool isFieldNull() const { return isNull_; }
			void setFieldNull() { isNull_ = true; }
			
			bool isModified() const { return modified_; }

			bool isKeyComponent() const { return keyComponent_; }
			void setKeyComponent(bool isKey) { keyComponent_ = isKey; }


		private:

			std::string name_;
			Type valueType_;
			std::string valueString_;
			int valueInt_;
			bool isNull_;
			bool modified_;
			bool keyComponent_;


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEFIELD_H
