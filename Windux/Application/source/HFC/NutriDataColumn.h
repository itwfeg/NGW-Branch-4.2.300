/* Evolution Scale Project
 * COPYRIGHT (c) 2001 - 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef NUTRIDATACOLUMN_H
#define NUTRIDATACOLUMN_H



#include <string>
#include <map>



class NutriDataColumn
{
	public:
		// --- Types and subtypes ---
		typedef std::map<std::string, std::string> tagDataContainer_type;


		// --- Constructors and destructor ---
		NutriDataColumn(int column, const std::string& data); 
		virtual ~NutriDataColumn();


		// --- Accessor methods ---
		std::string getData(const std::string& tag) const;


	private:
		int columnNumber_;
		tagDataContainer_type columnData_;


		void parseColumnData(const std::string& data);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif /* NUTRIDATACOLUMN_H */
