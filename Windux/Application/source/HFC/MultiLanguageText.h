#ifndef MULTILANGUAGETEXT_H
#define MULTILANGUAGETEXT_H



#include <map>


#include "evolution.h"

#include "util/properties/properties.h"
#include "util/thread/mutex.h"
#include "util/thread/thread.h"

#include "CSmartPtr.h"


namespace hfc
{


	class MultiLanguageSingleton
	{
		friend class MultiLanguageText;


		public:
			// --- Constructors and destructor ---
			static MultiLanguageSingleton & getInstance();


			// --- Instance function members ---


		private:
			// --- Types and subtypes ---
			typedef std::map< util_thread::Thread<>::ThreadIDType, LanguageSelection > LanguageSelectionsContainerType;


			// --- Instance data members ---
			mutable std::string languageFilename_;
			std::string primaryLanguageFilename_, secondaryLanguageFilename_;
			Util::CSmartPtr<util_env::Properties> primaryLanguageStrings_, secondaryLanguageStrings_;
			mutable util_thread::Mutex<> accessorMutex_;

			LanguageSelectionsContainerType registry_;                 // Registry for Language Selections
			mutable util_thread::Mutex<> registryMutex_;               // Mutex to control access to registry


			// --- Constructors and destructor ---
			MultiLanguageSingleton() { init(); }
			~MultiLanguageSingleton() { }


			// --- Instance function members ---
			void init();
			const std::string& generateLanguageFilename(const std::string language, const std::string country);
			void updateLanguageFiles();
			void loadTextFile(LanguageSelection type, const std::string& language);
			void unloadTextFile(LanguageSelection type);

			LanguageSelection getLanguageSelection() const;
			bool setLanguageSelection(LanguageSelection language);

			const std::string& getText(const std::string& key, LanguageSelection currentLanguage) const;


			// --- Class data members ---
			// Singleton data members
			static MultiLanguageSingleton * _instance_;
			static util_thread::Mutex<> _singletonMutex_;
			static std::string _notFoundProperty_, _emptyString_;
	};



	class MultiLanguageText
	{
		public:
			// --- Constructors and destructor ---
			MultiLanguageText();
			~MultiLanguageText();


			// --- Instance function members ---
			const std::string& operator()(const std::string& key) const { return singleton_.getText(key, language_); }
			const std::string& operator()(const std::string& key1, const std::string& key2) const
				{ return singleton_.getText(key1 + "." + key2, language_); }
			const std::string& operator()(const std::string& key, LanguageSelection language) const { return singleton_.getText(key, language); }


			// --- Class function members ---
			static bool setLanguageSelection(LanguageSelection language) { return MultiLanguageSingleton::getInstance().setLanguageSelection(language); }
			static void updateLanguageFiles() { MultiLanguageSingleton::getInstance().updateLanguageFiles(); }


		private:
			// --- Instance data members ---
			MultiLanguageSingleton& singleton_;
			LanguageSelection language_;


			// --- Instance function members ---
			void init();


			// --- Class data members ---

			// Accountant data members
	#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
	#endif
	};


}



#endif //MULTILANGUAGETEXT_H
