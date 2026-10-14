/**
 * @file WeigherValueMaxAssembly.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max assembly number
 * Created on: Sept 20, 2017
 *
 **/
#ifndef WEIGHERVALUEMAXASSEMBLY_H
#define WEIGHERVALUEMAXASSEMBLY_H
#include <string>
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class WeigherValueMaxAssembly : public EvolMessage
{
	public:
	WeigherValueMaxAssembly( std::string str, Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherValueMaxAssembly( const char *buffer );
	~WeigherValueMaxAssembly();

	static const unsigned int _AssemblyNumbSize_	= 10;

	std::string getAssemblyNumber() { return assemblyNumber; }
	const char *serialize()const;

	private:
	std::string  assemblyNumber;

};
#endif
