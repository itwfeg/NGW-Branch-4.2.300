#ifndef HOBDEPENDENCY_H
#define HOBDEPENDENCY_H

//#include "vxWorks.h"
#include <string>
#include "HobVersion.h"
#include "properties/properties.h"

class HobDependency
{

	public:

		typedef enum DependencyResult
		{
			DependencyFailed,
			DependencySucceeded,
			NoDependency
		}   DependencyResult_t;

		HobDependency();
		HobDependency(const HobDependency& dep);
		HobDependency(const std::string &dep);
		~HobDependency();

		const std::string* toString();

		HobVersion::VersionType getVersionType(){ return versionType_;}
		void setVersionType(HobVersion::VersionType newType){versionType_ = newType; buildDependencyString();}
		UINT16 getMaxMajRev(){return maxMajRev_;}
		void setMaxMajRev(UINT16 max){maxMajRev_ = max; buildDependencyString();}
		UINT16 getMinMajRev(){return minMajRev_;}
		void setMinMajRev(UINT16 min){minMajRev_ = min; buildDependencyString();}
		const std::string& getDependencyString(){return dependencyString_;}
		DependencyResult dependencyCheck(HobVersion* ver);
		void saveToPropertiesFile(util_env::Properties *pINI);

		static bool isAValidDependencyString(const std::string &dep);

	private:

		void buildDependencyString();

		HobVersion::VersionType versionType_;

		UINT16 minMajRev_, maxMajRev_;

		std::string dependencyString_;
		mutable std::string serialized_;

		const static char DEPENDENCY_SEPERATOR = '>';

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif	//HOBDEPENDENCY_H
