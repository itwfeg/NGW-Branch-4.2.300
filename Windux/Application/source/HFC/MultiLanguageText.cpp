#include "MultiLanguageText.h"


#include "SessionManager.h"
#include "Session.h"
#include "Configuration.h"
#include "CfgInternal.h"

#include "util/filesys/filename.h"
#include "util/thread/lock.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int MultiLanguageText::_dataType_ = 0;
		std::string MultiLanguageText::_dataTypeName_("MultiLanguageText"); 
	#endif


	MultiLanguageText::MultiLanguageText()
		: singleton_(MultiLanguageSingleton::getInstance()), language_(InvalidLanguage)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

		language_ = singleton_.getLanguageSelection();
	}


	MultiLanguageText::~MultiLanguageText()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	MultiLanguageSingleton * MultiLanguageSingleton::_instance_ = 0;
	util_thread::Mutex<> MultiLanguageSingleton::_singletonMutex_ = util_thread::Mutex<>();
	std::string MultiLanguageSingleton::_notFoundProperty_ = "NotFound";
	std::string MultiLanguageSingleton::_emptyString_ = "";


	MultiLanguageSingleton & MultiLanguageSingleton::getInstance()
	{
		if ( _instance_ == 0 )
		{
			util_thread::Lock< util_thread::Mutex<> > lock(_singletonMutex_);
			if ( _instance_ == 0 )
			{
				_instance_ = new MultiLanguageSingleton();
			}
		}

		return *_instance_;
	}


	// The constructor may call init directly without using a critical section. This is because no
	// function member (with the exception of getInstance()) is static and therefore it is necessary
	// to call getInstance(), which is protected, before being able to call any function member.
	void MultiLanguageSingleton::init()
	{
		Configuration* config = Configuration::getInstance();

		// Determine primary language
		if ( config->getPrimaryLanguageCode().size() && config->getPrimaryCountryCode().size() )
		{
			std::string languageFilename(generateLanguageFilename(config->getPrimaryLanguageCode(), config->getPrimaryCountryCode()));

			if ( languageFilename != primaryLanguageFilename_ )
			{
				primaryLanguageFilename_ = languageFilename;
				loadTextFile(PrimaryLanguage, primaryLanguageFilename_);
			}
		}
		else
		{
			primaryLanguageFilename_.clear();
			unloadTextFile(PrimaryLanguage);
		}

		// Determine secondary language
		if ( config->getSecondaryLanguageCode().size() && config->getSecondaryCountryCode().size() )
		{
			std::string languageFilename(generateLanguageFilename(config->getSecondaryLanguageCode(), config->getSecondaryCountryCode()));

			if ( languageFilename != secondaryLanguageFilename_ )
			{
				secondaryLanguageFilename_ = languageFilename;
				loadTextFile(SecondaryLanguage, secondaryLanguageFilename_);
			}
		}
		else
		{
			secondaryLanguageFilename_.clear();
			unloadTextFile(SecondaryLanguage);
		}
	}


	const std::string& MultiLanguageSingleton::generateLanguageFilename(const std::string language, const std::string country)
	{
		languageFilename_.assign("Text.");
		languageFilename_.append(language).append(1, '_');
		languageFilename_.append(country).append(".txt");

		return languageFilename_;
	}


	LanguageSelection MultiLanguageSingleton::getLanguageSelection() const
	{
		util_thread::Thread<>::ThreadIDType tid = util_thread::Thread<>::getCurrentThreadID();

		util_thread::Lock< util_thread::Mutex<> > lock(registryMutex_);

		LanguageSelectionsContainerType::const_iterator iter = registry_.find(tid);
		if ( iter != registry_.end() )
			return iter->second;
		else
			return PrimaryLanguage;
	}


	bool MultiLanguageSingleton::setLanguageSelection(LanguageSelection language)
	{
		util_thread::Thread<>::ThreadIDType tid = util_thread::Thread<>::getCurrentThreadID();

		util_thread::Lock< util_thread::Mutex<> > lock(registryMutex_);

		LanguageSelectionsContainerType::iterator iter = registry_.find(tid);
		if ( iter != registry_.end() )
		{
			iter->second = language;
			return true;
		}
		else
		{
			std::pair<LanguageSelectionsContainerType::iterator, bool> result = registry_.insert(std::make_pair(tid, language));
			return result.second;
		}
	}


	// This function returns a reference to a string stored in the Properties class. There is a very small chance of
	// having a problem here if, once the string is return but not quite processed yet, another thread asks to update
	// the language files, which would remove the current Properties instance and its strings. The way to deal with
	// this would be by having the Properties class store and return smart pointers to those strings. In this way, if
	// the properties object and its strings are to be deleted then the string being used will remain valid until it
	// is not longer needed. The chances of this happening are very small since the language settings would rarely
	// change and the language strings are only used for reports (at the present time).
	const std::string& MultiLanguageSingleton::getText(const std::string& key, LanguageSelection currentLanguage) const
	{
		util_thread::Lock< util_thread::Mutex<> > lock(accessorMutex_);

		if ( currentLanguage == PrimaryLanguage )
		{
			const std::string* value = primaryLanguageStrings_->getProperty(key);
			
			if ( !value )
				value = primaryLanguageStrings_->getProperty(_notFoundProperty_);

			return value ? *value : _emptyString_;
		}
		else
		{
			const std::string* value = secondaryLanguageStrings_->getProperty(key);
			
			if ( !value )
				value = secondaryLanguageStrings_->getProperty(_notFoundProperty_);

			return value ? *value : _emptyString_;
		}
	}


	void MultiLanguageSingleton::updateLanguageFiles()
	{
		util_thread::Lock< util_thread::Mutex<> > lock(accessorMutex_);

		init();
	}


	void MultiLanguageSingleton::loadTextFile(LanguageSelection type, const std::string& name)
	{
		util_filesystem::Filename filename(Configuration::getInstance()->toStringCfgI(CfgInternal::LANGUAGEFILESDIR), name);

		if ( type == PrimaryLanguage )
			primaryLanguageStrings_ = new util_env::Properties(filename.getFullPath());
		else
			secondaryLanguageStrings_ = new util_env::Properties(filename.getFullPath());
	}


	void MultiLanguageSingleton::unloadTextFile(LanguageSelection type)
	{
		if ( type == PrimaryLanguage )
			primaryLanguageStrings_ = 0;
		else
			secondaryLanguageStrings_ = 0;
	}



}
