#ifndef SYSTEMFONTSLIST_H
#define SYSTEMFONTSLIST_H
//#include <ugl/uglfont.h>
#include <list>

class SystemFontsList
{

	public:

	SystemFontsList();
	~SystemFontsList();

	 char *getFontName();



	


	private:

		std::list<char *> *fontList;
		std::list<char *>::iterator iter;

		//lcg rev 0.1 UGL_FONT_DESC fontDescription;
		std::string fontDescription;//lcg rev 0.1
		
		bool listEmpty;					//true if no available system / user fonts
		char empty;
	


};

#endif //SYSTEMFONTSLIST_H
