#ifndef STDINDEX_H
#define STDINDEX_H



#ifdef WIN32_OS
#include "..\..\edb.h"
#else
#include "../../edb.h"
#endif

#include <string>
#include <iosfwd>
#include <fstream>     // needed for typename std::fstream::pos_type
#include <set>


class STDIndex
{
	public:
		/* Constructors and destructor. */
		STDIndex(const std::string& fname, const std::string& k);
		virtual ~STDIndex();


		/* Subtypes. */
#ifdef VXWORKS
		typedef streampos position_type;
#else
		typedef std::fstream::pos_type position_type;
#endif

		class Element;
		friend class Element;
		class Element
		{
			public:
				// Constructors and destructor.
				// Need default constructor in order to read elements from a file
				Element(const std::string& key = "", position_type pos = 0);
				virtual ~Element();

				// Subtypes.
				class Compare;
				friend class Compare;
				class Compare
				{
					public:
						bool operator()(const STDIndex::Element& e1, const STDIndex::Element& e2) const
								{ return e1.indexValue < e2.indexValue; }
				};

				// Getters and setters.
				position_type getPosition() const;
				position_type setPosition(position_type pos);

				// Operations.
				static bool compareElement(const Element& e1, const Element& e2);

				friend std::istream& operator>>(std::istream& is, STDIndex::Element& e);
				friend std::ostream& operator<<(std::ostream& os, const STDIndex::Element& e);


			private:
				std::string indexValue;
				position_type position;
		};
		
		typedef std::set<Element, Element::Compare> index_set;


		/* Getter and setter methods. */
		bool initialized();


		/* Operations. */
		position_type find(const std::string& key) const;
		position_type first() const;
		position_type findNext(const std::string& key) const;
		position_type findPrevious(const std::string& key) const;
		position_type last() const;

		int deleteIndex(const std::string& key);
		int deleteAll();
		void addIndex(const std::string& key, position_type position, bool save = true);
		void save();

		unsigned long int count();


		/* Extractor. */


		/* Printer. */


		/* Constants. */
		static const position_type INDEX_NOTFOUND;


	private:
		/* Data elements. */
		bool initialized_;
		std::string filename;
		std::string key;
		index_set indices;


		/* Private methods. */
		STDIndex();                               // Prevent temporaries
		STDIndex(const STDIndex& f);              // Prevent copies
		STDIndex& operator=(const STDIndex& );    // Prevent assignments

		void readFile();
		void writeFile();
};



#endif // STDINDEXFILE_H

