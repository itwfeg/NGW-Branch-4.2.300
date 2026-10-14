#ifndef CLOPTIONS_H
#define CLOPTIONS_H



#include <string>
#include <set>
#include <vector>
#include <iosfwd>



#ifndef NO_NAMESPACES
namespace util_env
{
#endif

	class CLOptions
	{
		public:
			// Types
			class Option
			{
				public:
/*					friend std::ostream& operator<<(std::ostream& os, const util_env::CLOptions::Option& rhs); (SJJC port 6/19/06) */

					// Constructors and destructor
					Option(const char * n, const char * v = "") : name(n), value(v) {}
					Option(const std::string n, const std::string v = "") : name(n), value(v) {}
//					virtual ~Option {}

					// Accessor methods
					std::string getName(void) const { return name; }
					std::string getValue(void) const { return value; }

					// Processing methods
					operator std::string() const { return value; }
					bool operator <(const Option& rhs) const { return getName() < rhs.getName(); } /* (SJJC port 6/19/06) */
					
				private:
					std::string name;
					std::string value;
			};
			typedef std::set<Option>::const_iterator option_iterator;
			typedef std::vector<std::string>::const_iterator parameter_iterator;


			// Constructors and destructor
			CLOptions(char mark) : marker(mark), executionPath("") {}
			virtual ~CLOptions() {}


			// Access methods
			std::string getExecutionPath() const { return executionPath; }

			option_iterator beginOption() const { return options.begin(); }
			option_iterator endOption() const { return options.end(); }

			parameter_iterator beginParameter() const { return parameters.begin(); }
			parameter_iterator endParameter() const { return parameters.end(); }


			// Processing methods
			bool processCommandLine(int argc, char* argv[]);

			option_iterator findOption(const std::string name) const
					{ return options.find(Option(name)); }

			void printOptions(std::ostream& out);
			void printParameters(std::ostream& out);


		private:
			char marker;
			// This is the fullpath used to execute the application
			std::string executionPath;
			// These are the options set through the command line
			std::set<Option> options;
			// These are parameters passed to the application
			std::vector<std::string> parameters;
	};


#ifndef NO_NAMESPACES
} // namespace util_env
#endif


#ifndef NO_NAMESPACES
// Need to define this operator in the global namespace or this compiler will
// generate the error
//		'operator<' not implemented in type 'util_env::CLOptions::Option' for arguments of the same type
int operator<(const util_env::CLOptions::Option& lhs, const util_env::CLOptions::Option& rhs);

// Need to define this operator in the global namespace or this compiler will
// generate the error
//		'operator<<' not implemented in type 'ostream' for arguments of type 'util_env::CLOptions::Option'
namespace util_env
{  /* (SJJC port 6/19/06) */
std::ostream& operator<<(std::ostream& os, const util_env::CLOptions::Option& rhs);
}
#else
int operator<(const CLOptions::Option& lhs, const CLOptions::Option& rhs);
std::ostream& operator<<(std::ostream& os, const CLOptions::Option& rhs);
#endif


#endif // !CLOPTIONS_H
