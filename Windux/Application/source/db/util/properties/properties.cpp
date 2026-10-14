#include "PAL_platformheader.h"

#include <string>
#include <fstream>
#include <algorithm>

#include "../../../util/Debug.h"



#include "properties.h"
#if !defined ( _WIN32 )
#  include "../platform/platform.h"
#else
#  include "..\platform\platform.h"
#endif


#ifndef NO_NAMESPACES
namespace util_env
{
#endif


using namespace std;
#ifndef NO_NAMESPACES
using namespace util_platform;
#endif


std::string Properties::whiteSpace = " \t";
std::string Properties::keySeparators = "+=:";
std::string Properties::commentMarkers = "#!;";
std::string Properties::valueSeparators = ",";


Properties::Properties(const string& propsFilename, const string& defsFilename, 
		bool alwaysAppend, bool save)
		: properties_(), defaults_(), filename_(propsFilename), 
		alwaysAppend_(alwaysAppend), autosave_(save), saveMultiLines_(false)
{
	if ( !defsFilename.empty() ) loadProperties(defaults_, defsFilename);
	if ( !propsFilename.empty() ) loadProperties(properties_, filename_);
}


Properties::~Properties()
{
}


const string* Properties::getProperty(const string& key) const
{
	properties_type::const_iterator iter = properties_.find(key);

	if ( iter == properties_.end() )
	{
		iter = defaults_.find(key);
		if ( iter == defaults_.end() )
			return 0;
	}

	return &(iter->second.getValue());
}


void Properties::setProperty(const std::string& key, const std::string& value)
{
	properties_type::iterator iter = properties_.find(key);
	if ( iter != properties_.end() )
		iter->second.setValue(value);
	else
		properties_.insert(make_pair(key, value));

	if ( autosave_ )
		save();
}


const Properties::multiValue_type* Properties::getMultiProperty(const string& key) const
{
	properties_type::const_iterator iter = properties_.find(key);

	if ( iter == properties_.end() )
	{
		iter = defaults_.find(key);
		if ( iter == defaults_.end() )
			return 0;
	}

	return &(iter->second.getMultiValue());
}


void Properties::load(const string& overrides)
{
	loadProperties(properties_, overrides);
}


string Properties::removeProperty(const string& key)
{
	properties_type::iterator iter = properties_.find(key);
	if ( iter != properties_.end() )
	{
		string value = iter->second.getValue();

		properties_.erase(iter);

		return value;
	}

	return "";
}


void Properties::save(const string& filename)
{

	if (filename.size())
	{
		Debug::trace("Properties", "Properties-save(): filename=" + filename );
		saveProperties(properties_, filename);
	}
	else
	{
		Debug::trace("Properties", "Properties-save(): filename_=" + filename_);
		saveProperties(properties_, filename_);
	}

	system("sync");
}


bool Properties::operator==(const Properties& rhs) const
{
	// Easy case
	if ( !defaults_.size() && !rhs.defaults_.size() )
	{
		if ( properties_.size() != rhs.properties_.size() )
			return false;

		// If the sizes are the same, check that all properties on one side are
		// defined and with the same value on the other side.
		properties_type::const_iterator rhsKeyIter = rhs.properties_.begin();
		while ( rhsKeyIter != rhs.properties_.end() )
		{
			properties_type::const_iterator keyIter = properties_.find(rhsKeyIter->first);
			if ( keyIter == properties_.end() )
				return false;

			// Use the multivalue array to compare the values in case they have many
			const multiValue_type& lhsMultiValue = keyIter->second.getMultiValue();
			const multiValue_type& rhsMultiValue = rhsKeyIter->second.getMultiValue();
			if ( lhsMultiValue.size() != rhsMultiValue.size() )
				return false;

			multiValue_type::const_iterator valueIter = rhsMultiValue.begin();
			while ( valueIter != rhsMultiValue.end() )
			{
				if ( find(lhsMultiValue.begin(), lhsMultiValue.end(), *valueIter) == lhsMultiValue.end() )
					return false;

				++valueIter;
			}

			++rhsKeyIter;
		}

		return true;
	}

	// Difficult case not implemented yet
	return false;
}


Properties& Properties::operator<<(const Properties& rhs)
{
	// First add all default values from rhs
	properties_type::const_iterator iter = rhs.defaults_.begin();
	while ( iter != rhs.defaults_.end() )
	{
		setProperty(iter->first, iter->second.getValue());

		++iter;
	}

	// Now add all override values from rhs
	iter = rhs.properties_.begin();
	while ( iter != rhs.properties_.end() )
	{
		setProperty(iter->first, iter->second.getValue());

		++iter;
	}

	return *this;
}



void Properties::loadProperties(properties_type& props, const string& filename)
{
	string line;
	ifstream file(filename.c_str());
	if ( !file )
		return;

	while ( getline(file, line) )
	{
		string::size_type end = line.size();

#if defined(VXWORKS) || defined( __linux__ )
		// A Windows text file, from VxWorks perspective, has a '\r' at the end of each line.
		// Here we remove that character, if it is there.
		if ( end > 0 && line[end - 1] == '\r' )
			-- end;
#endif


		// If the line contains a single-line comment (whether at the beginning or in the
		// middle), remove it.
		if ( end > 0 )
		{
			string::size_type beginLine = 0, beginComment;

			do
			{
				for ( beginComment = beginLine; beginComment < end &&
						commentMarkers.find(line[beginComment]) == string::npos; ++beginComment )
					;

				// Verify that the comment marker is not escaped with '\\'.
				beginLine = beginComment + 1;
			} while ( 0 < beginComment && beginComment < end && line[beginComment - 1] == escapeChar );

			// Effectively remove the comment
			if ( beginComment < end )
				end = beginComment;

			// Remove any trailing whitespace
			for ( ; beginComment > 0 &&
					whiteSpace.find(line[beginComment - 1]) != string::npos; --beginComment)
				;

			// Effectively remove the trailing whitespace
			if ( beginComment < end )
				end = beginComment;
		}


		if ( end )  // The line is not empty and is not a comment
		{
			string::size_type beginKey, endKey, keySeparator, beginValue;

			// Remove any leading whitespace
			for ( beginKey = 0; beginKey < end &&
					whiteSpace.find(line[beginKey]) != string::npos; ++beginKey )
				;

			if ( beginKey < end )    // There is something in the line
			{
				bool appendValue = alwaysAppend_;
//				bool appendValue = false;

				// Look for key-value separators
				for ( keySeparator = beginKey; keySeparator < end &&
						keySeparators.find(line[keySeparator]) == string::npos; ++keySeparator )
					;

				// Remove any whitespace after the key
				for ( endKey = keySeparator - 1; endKey > beginKey &&
						whiteSpace.find(line[endKey]) != string::npos; --endKey )
					;

				if ( beginKey < endKey )    // There is a key
				{
					if ( keySeparator < end )
					{
						if ( line[keySeparator++] == '+' )
						{
							if ( keySeparator < end && line[keySeparator] == '=' )
								++keySeparator;

							appendValue = true;
						}

						beginValue = keySeparator;
					}
					else
						beginValue = end;

					// Remove any whitespace before the value
					for ( ; beginValue < end &&
							whiteSpace.find(line[beginValue]) != string::npos; ++beginValue )
						;

					if ( appendValue )
					{
						// If the property was specified as "key += value" then the value is
						// appended to any previous value for the same key
						string key(line, beginKey, endKey - beginKey + 1);
						properties_type::iterator iter = props.find(key);
						if ( beginValue < end )  // There is a value associated with the key
						{
							if ( iter != props.end() )
								iter->second.appendValue(unescape(string(line, beginValue, end - beginValue)));
							else
								props.insert(make_pair(unescape(key), unescape(string(line, beginValue, end - beginValue))));
						}
						else
						{
							if ( iter == props.end() )
								props.insert(make_pair(unescape(key), string("")));
						}
					}
					else
					{
						if ( beginValue < end )  // There is a value associated with the key
							props.insert(make_pair(unescape(string(line, beginKey, endKey - beginKey + 1)),
										unescape(string(line, beginValue, end - beginValue))));
						else  // There is no value associated with the key
							props.insert(make_pair(unescape(string(line, beginKey, endKey - beginKey + 1)), string("")));
					}
				}
			}
		}
	}
}


void Properties::saveProperties(properties_type& props, const std::string& filename)
{
#ifdef VXWORKS
	const Platform& os = Platform::getInstance();
#endif
	ofstream file(filename.c_str());
	if ( !file )
		return;

	properties_type::const_iterator iter = props.begin();
	while ( iter != props.end() )
	{
		if (saveMultiLines_)
		{
			const multiValue_type& multiValue = iter->second.getMultiValue();
			multiValue_type::const_iterator iter2 = multiValue.begin();
			while (iter2 != multiValue.end())
			{
#ifdef VXWORKS
				if (alwaysAppend_)
					file << escape(iter->first) << " = " << escape(*iter2) << os.altLineSeparator();
				else
					file << escape(iter->first) << " += " << escape(*iter2) << os.altLineSeparator();
#else
				if ( alwaysAppend_ )
				{
					Debug::trace("Properties", "Properties-saveProperties(), Multi,Append: " + escape(iter->first) + "=" + escape(*iter2) );
					file << escape(iter->first) << "=" << escape(*iter2) << endl;
				}
				else
					file << escape(iter->first) << "+=" << escape(*iter2) << endl;
#endif

				++iter2;
			}
		}
		else
		{
			const std::string& val = iter->second.getValue();
#ifdef VXWORKS
			if ( val.size() )
				file << escape(iter->first) << " = " << escape(val) << os.altLineSeparator();
			else
				file << escape(iter->first) << os.altLineSeparator();
#else
			if (val.size())
			{
				Debug::trace("Properties", "Properties-saveProperties(), Non-Multi, value: " + escape(iter->first) + "=" + escape(val));
				file << escape(iter->first) << "=" << escape(val) << endl;
			}
			else
			{
				Debug::trace("Properties", "Properties-saveProperties(), Non-Multi, no-value: " + escape(iter->first) + "=" );
				file << escape(iter->first) << "=" << endl;
			}
#endif
		}

		++iter;
	}
}


void Properties::Value::parseValue(const string& value, multiValue_type& multiValue, bool append)
{
	string::size_type end = value.length();
	string::size_type beginValue = 0;

	if ( multiValue.size() && !append )
		multiValue.clear();

	while ( beginValue < end )
	{
		// Remove leading spaces
		for ( ; beginValue < end &&
				whiteSpace.find(value[beginValue]) != string::npos; ++beginValue )
			;


		if ( beginValue < end )  // there is a value to store
		{
			string::size_type endValue, separator;

			// Find the next value separator
			for ( separator = beginValue; separator < end &&
					valueSeparators.find(value[separator]) == string::npos; ++separator )
				;

			// Determine the last character (one past, actually) of the value ignoring trailing spaces
			if ( beginValue < separator )
			{
				for ( endValue = separator; endValue > beginValue &&
						whiteSpace.find(value[endValue - 1]) != string::npos; --endValue )
					;
			}
			else
				endValue = beginValue;

			// Store the value
			multiValue.push_back(value.substr(beginValue, endValue - beginValue));


			beginValue = separator + 1;
		}
	}
}

string Properties::unescape(const string &s)
{
	string ns;
 	
	if (s.empty())
		return s;

 	size_t currentLen = s.length();
	size_t i = 0;

	ns.reserve(currentLen + 1);

	while (i < currentLen)
	{
		if (s[i] == escapeChar)
		{
			if (++i >= currentLen)
				break;				//	Skip the escape character. Exit the loop if it was the last character.
		}
		ns.append(1, s[i++]);		//	Otherwise, append the char from the string.
	}
//	cout << "unescape:  [" << s << "]  returns  [" << ns << "]" << endl;
	return ns;
}

string Properties::escape(const string &s)
{
	string ns;
 	
	if (s.empty())
		return s;

	ns.reserve(s.length() * 2);

	unsigned int i = 0;
	do
	{
		if ((commentMarkers.find(s[i]) != string::npos) || (s[i] == escapeChar))
		{
			ns.append(1, escapeChar);	//	Append the escape char
		}
		ns.append(1, s[i]);				//	Then append the char from the string
	} while (++i < s.length());

//	cout << "escape:  [" << s << "]  returns  [" << ns << "]" << endl;
	return ns;
}


#ifndef NO_NAMESPACES
} // namespace util_env
#endif

