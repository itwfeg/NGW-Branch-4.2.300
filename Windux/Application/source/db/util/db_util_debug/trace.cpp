#include "PAL_platformheader.h"

#include <fstream>
#include <iostream>



#define TRACE_NOTRACEMACRO
#include "db_util_trace.h"
#undef TRACE_NOTRACEMACRO


#if !defined ( _WIN32 )
#  include "../e_string/utilstring.h"
#  include "../thread/thread.h"
#else
#  include "..\e_string\utilstring.h"
#  include "..\thread\thread.h"
#  include "..\platform\platform.h"
#endif


#ifndef NO_NAMESPACES
using namespace util_thread;
using namespace util_platform;
#endif


using namespace std;


Mutex<> Trace::_mutex_ = Mutex<>();


Trace::Trace()
		: enabled(false), traceAll(false), traceLevel(0), debugFilename(""), out(0),
		useLock_(false)
{
}


Trace* Trace::getInstance()
{
	static Trace *_debug = 0;

	if ( _debug == 0 )
	{
		Lock< Mutex<> > lock(_mutex_);
		if ( _debug == 0 )
			_debug = new Trace();
	}

	return _debug;
}


Trace* Trace::getInstance(util_env::Properties& prop, bool useLock)
{
	string debugFile = "";
	const string* property;
	const vector<string>* properties;
	Trace* _d = getInstance();

	property = prop.getProperty("debug.traceLevel");
	if ( property && property->size() )
		_d->setTraceLevel(util_string::stringtoint(*property));

	property = prop.getProperty("debug.traceAll");
	if ( property && property->size() )
		_d->setTraceAll(util_string::stringtobool(*property));

	properties = prop.getMultiProperty("debug.traceCategories");
	if ( properties && properties->size() )
	{
		vector<string>::const_iterator iter = properties->begin();
		while ( iter != properties->end() )
			_d->addTracingCategory(*iter++);
	}

	property = prop.getProperty("debug.debugFile");
	if ( property && property->size() )
		debugFile = *property;

	property = prop.getProperty("debug.enable");
	if ( property && property->size() && util_string::stringtobool(*property) )
		_d->enable(debugFile);


	_d->useLock_ = useLock;

	
	return _d;
}


Trace::~Trace()
{
	ShutdownNicely();
	disable();
}


bool Trace::enable(const std::string& filename)
{
	bool temp = enabled;

	Lock< Mutex<> > lock(_mutex_);

	if ( enabled )
		disable();

	if ( !enabled )
	{
		if ( !filename.empty() )
		{
			out = new ofstream(filename.c_str());
			debugFilename = filename;
		}
		if ( !out )
		{
			if ( !debugFilename.empty() && debugFilename != "cout" )
				out = new ofstream(debugFilename.c_str());

			if ( !out )
			{
				out = &cout;
				debugFilename = "cout";
			}
		}

		enabled = true;
	}

	return temp;
}


bool Trace::disable()
{
	bool temp = enabled;

	Lock< Mutex<> > lock(_mutex_);

	if ( enabled )
	{
		closeDebugFile();

		enabled = false;
	}

	return temp;
}


bool Trace::isTracingCategory(const std::string& category)
{
	if ( !enabled )
		return false;

	if ( traceAll )
		return true;
	else
	{
		Lock< Mutex<> > lock(_mutex_);

		set<string>::const_iterator iter = traceCategories.begin();
		while ( iter != traceCategories.end() )
		{
			if ( category.size() >= iter->size() )
			{
#ifdef VXWORKS
				if ( !category.compare(*iter, 0, iter->size()) )
#else
				if ( !category.compare(0, iter->size(), *iter) )
#endif
					return true;
			}

			iter ++;
		}
	}

	return false;
}


void Trace::addTracingCategory(const std::string& category)
{
	Lock< Mutex<> > lock(_mutex_);

	if ( !isTracingCategory(category) )
		traceCategories.insert(category);
}


void Trace::removeTracingCategory(const std::string& category)
{
	Lock< Mutex<> > lock(_mutex_);

	traceCategories.erase(category);
}


void Trace::trace(const std::string& category, int level, const std::string& /* filename */,
		int /* lineNumber */, const std::string& message)
{
	Lock< Mutex<> > lock(_mutex_);

	if ( level <= traceLevel )
	{
		if ( traceAll || isTracingCategory(category) )
		{
			*out << "[Thread: " << Thread<>::getCurrentThreadID() << "]";
			*out << "[" << Platform::getInstance().getCurrentTime() << "]";
			*out << "[" << category << "." << level << "]";
//			*out << "[" << filename << ":" << lineNumber << "]";
			*out << " " << message << endl << flush;
		}
	}
}


void Trace::closeDebugFile()
{
	Lock< Mutex<> > lock(_mutex_);

	if ( out )
	{
		if ( out != &cout )
		{
			ofstream* temp = dynamic_cast<ofstream*>(out);
			if ( temp )
				temp->close();
			delete out;
		}

		out = 0;
	}
}

