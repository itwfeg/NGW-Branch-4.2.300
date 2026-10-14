#ifndef PROPERTIES_H
#define PROPERTIES_H



#include <string>
#include <map>
#include <vector>


#ifndef NO_NAMESPACES
////////////////////////minhaj
namespace util_env
{
#endif

	// A Properties object can handle up to two properties files passed onto its constructor:
	// a user-overrides and a defaults files. The latter is optional and it contains default
	// values. It is considered to be read only. The former contains overrides to the defaults
	// and possibly new properties. This file can be modified through this class' interface
	// (setProperty) and then saved back into the file. 
	class Properties
	{
		public:
			// --- Types and subtypes ---
			friend class Value;     /* (SJJC port 6/19/06) */
			class Value
			{
				public:
					typedef std::vector<std::string> multiValue_type;

					Value(const std::string& v) : value_(v), multiValue_()
							{ parseValue(value_, multiValue_); }
					~Value() {}

					const std::string& getValue() const { return value_; }
					void setValue(const std::string& val)
							{ value_ = val; parseValue(value_, multiValue_); }
					void appendValue(const std::string& val)
							{ value_.append(", ").append(val); parseValue(val, multiValue_, true); }
					const multiValue_type& getMultiValue() const { return multiValue_; }

				private:
					std::string value_;
					multiValue_type multiValue_;

					static void parseValue(const std::string& value,
							multiValue_type& multiValue, bool append = false);
			};
			typedef std::map<std::string, Value> properties_type;
			typedef Value::multiValue_type multiValue_type;


			// --- Constructors and destructor ---
			Properties(const std::string& overrides, const std::string& defaults = "",
					bool alwaysAppend = false, bool autosave = false);
			virtual ~Properties();


			// --- Accessor methods ---
			// The following methods enable the client code to get and set the
			// value associated with a property. This value is passed in and out
			// in the form of a string, event if it contains "multiple values"
			// (i.e., a list of comma-separated values). In order to access those
			// multiple values individually, see "getMultiProperty" below.
			const std::string* getProperty(const std::string& key) const;
			void setProperty(const std::string& key, const std::string& value);

			// The following method enables the client code to get the value or
			// values (if value contains a list of comma-separated values)
			// associated with a property in the form of a container of strings
			// that can be navigated using iterators.
			const multiValue_type* getMultiProperty(const std::string& key) const;

			// The "autoSave" property controls whether the properties are save
			// automatically (true value) after any property change (i.e., calling
			// method setProperty).
			bool getAutoSave() const { return autosave_; }
			void setAutoSave(bool save) { autosave_ = save; }

			// The "saveMultiLines" property controls whether a multivalue property
			// is saved in one single line (false value) or in multiple lines (true).
			bool getSaveMultiLines() const { return saveMultiLines_; }
			void setSaveMultiLines(bool multiLines) { saveMultiLines_ = multiLines; }

			const std::string& getFileName() const { return filename_; }

			std::string unescape(const std::string &s);
			std::string escape(const std::string &s);


			// --- Processing methods ---
			void load(const std::string& overrides);
			std::string removeProperty(const std::string& key);
			void save(const std::string& filename = "");
			bool operator==(const Properties& rhs) const;
			Properties& operator<<(const Properties& rhs);


		protected:
			properties_type properties_;   // Property values
			properties_type defaults_;     // Default values


		private:
			std::string filename_;         // Filename where overrides (properties) are located
			bool alwaysAppend_;            // Controls whether to append multiple instances of keys
			bool autosave_;                // Controls whether to automatically save on changes
			bool saveMultiLines_;          // Controls whether to save multi properties on multiple lines

			void loadProperties(properties_type& props, const std::string& filename);
			void saveProperties(properties_type& props, const std::string& filename);

			static std::string whiteSpace;
			static std::string keySeparators;
			static std::string commentMarkers;
			static std::string valueSeparators;
			static const char escapeChar = '\\';
	};

#ifndef NO_NAMESPACES
} // namespace util_env
#endif



#endif // PROPERTIES_H
