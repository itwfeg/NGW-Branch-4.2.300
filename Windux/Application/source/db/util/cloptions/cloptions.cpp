#include <iostream>
#include <iterator>



#include "cloptions.h"


#ifndef NO_NAMESPACES
namespace util_env
{
#endif


using namespace std;


bool CLOptions::processCommandLine(int argc, char* argv[])
{
	// Retrieve the execution path first
	executionPath = argv[0];

	// Process the rest of the command line
	for ( int i = 1; i < argc; ++i )
	{
		if ( *argv[i] == marker )
		{
			if ( i + 1 >= argc || *argv[i+1] == marker )
			{	// Option without value
				options.insert(Option(argv[i] + 1));
			}
			else
			{	// Option with value
				options.insert(Option(argv[i] + 1, argv[i+1]));
				++i;  // Skip value too
			}
		}
		else
		{
			parameters.push_back(argv[i]);
		}
	}

	return true;
}


void CLOptions::printOptions(ostream& out)
{
	copy(options.begin(), options.end(), ostream_iterator<Option>(out, "\n"));
}


void CLOptions::printParameters(ostream& out)
{
	copy(parameters.begin(), parameters.end(), ostream_iterator<string>(out, "\n"));
}


#ifndef NO_NAMESPACES
} // namespace util_env
#endif


#ifndef NO_NAMESPACES
// The following operators are defined outside the 'namespace util_env' in order
// to prevent this compiler from reporting errors. See the include file for more
// explanations of this issue. Also see test cases in problems/problem1 directory.
int operator<(const util_env::CLOptions::Option& lhs, const util_env::CLOptions::Option &rhs)
{
	return lhs.getName() < rhs.getName();
}


namespace util_env
{  /* (SJJC port 6/19/06) */
std::ostream& operator<<(std::ostream& os, const util_env::CLOptions::Option& rhs)
{
	return os << rhs.getName() << " " << rhs.getValue();
}
}
#else
int operator<(const CLOptions::Option& lhs, const CLOptions::Option &rhs)
{
	return lhs.getName() < rhs.getName();
}


std::ostream& operator<<(std::ostream& os, const CLOptions::Option& rhs)
{
	return os << rhs.getName() << " " << rhs.getValue();
}
#endif



#ifdef CLOPTIONS_DRIVER


#include "cloptions.h"

#include <iostream>

using namespace std;


int main (int argc, char* argv[])
{
	CLOptions opts('-');

	opts.processCommandLine(argc, argv);


	cout << "Path of execution: " << opts.getExecutionPath() << endl;


	CLOptions::option_iterator op = opts.beginOption();
	cout << "Options:" << endl;
	while ( op != opts.endOption() )
	{
		cout << "  " << *op << endl;

		++ op;
	}
	cout << "All options:" << endl;
	opts.printOptions();


	CLOptions::parameter_iterator param = opts.beginParameter();
	cout << "Parameters:" << endl;
	while ( param != opts.endParameter() )
	{
		cout << "  " << *param << endl;

		++ param;
	}
	cout << "All parameters:" << endl;
	opts.printParameters();
}


#endif // CLOPTIONS_DRIVER

