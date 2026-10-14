#ifndef DBEXPIRATIONLABELI_H
#define DBEXPIRATIONLABELI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



    class DBExpirationLabelI
    {
        public:

            virtual ~DBExpirationLabelI() {}


            virtual DBExpirationLabelI* clone() const = 0;
            virtual const DBExpirationLabelI* getDecorated() const = 0;


            virtual std::string getField(const std::string& field) const = 0;
            virtual void setField(const std::string& field, const std::string& value) = 0;


            virtual std::string getSequenceNumber() const = 0;
            virtual bool isSequenceNumberNull() const { return false; }
            virtual bool isSequenceNumberWritable() const { return true; }

            virtual std::string getPLU() const = 0;
            virtual void setPLU(const std::string& plu) = 0;
            virtual bool isPLUNull() const { return false; }
            virtual void setPLUNull() = 0;
            virtual bool isPLUWritable() const { return true; }

			virtual std::string getDeptNumber() const = 0;
			virtual void setDeptNumber(const std::string& number) = 0;
			virtual bool isDeptNumberNull() const { return false; }
			virtual bool isDeptNumberWritable() const { return true; }

            virtual std::string getProductDescription() const = 0;
            virtual void setProductDescription(const std::string& desc) = 0;
            virtual bool isProductDescriptionNull() const { return false; }
            virtual void setProductDescriptionNull() = 0;
            virtual bool isProductDescriptionWritable() const { return true; }

            virtual std::string getExpiration() const = 0;
            virtual void setExpiration(const std::string& exp) = 0;
            virtual bool isExpirationNull() const { return false; }
            virtual void setExpirationNull() = 0;
            virtual bool isExpirationWritable() const { return true; }


            virtual bool isRecordValid() { return true; }
    };


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBEXPIRATIONLABELI_H
