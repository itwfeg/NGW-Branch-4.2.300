/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HOBSTRING_H
#define HOBSTRING_H

#include <string>
#include <iterator>


class HobString : public std::string
{
    public:

        HobString();
        HobString( const HobString& );
        ~HobString();
        HobString(const std::string& str);
        HobString(const std::string& str, std::string::size_type str_idx);
        HobString(const std::string& str, std::string::size_type str_idx, std::string::size_type str_num);
        HobString(const char* cstr);
        HobString(const char* cstr, std::string::size_type chars_len);
        HobString(std::string::size_type num, char c);
        //HobString(InputIterator beg, InputIterator end);

        HobString& operator= (char c) { std::string::operator=(c); return *this; };


        // Replace duplicate characters with only a single instance of the char.
        void stripDuplicateChar(char c);
        void duplicateChar(char c);
        bool unQuote();
        void quote();
        void quoteForCSV();
        void unQuoteForCSV();

        std::string getFromEnd(std::string::size_type str_idx);
        int countConsecutiveChars(char c);
        int countConsecutiveChars(char c, std::string::size_type str_idx);

        static const short LEADING = 0;
        static const short TRAILING = 1;
        static const short LEADING_TRAILING = 2;

        // Remove leading, trailing, or both leading and trailing white-space (' ', '\t').
        void stripWhiteSpace(short position);

        bool isNumeric();

        bool isUpper();
        void toUpper();

        bool isLower();
        void toLower();

        void copyIntoString(std::string::size_type maxChars, std::string::size_type startIndex, HobString copyFromStr);
        void makeBinary();
        void makeAscii();
        void getCharsWithPadding(int numChars);

        void convertToEvolFontSequences();
        void convertToUltimaFontSequences();
        void stripFontSequences();

        void stripChar(char c);

		void removeTrailingNewlinesAndSpaces();
		void removeSpacesBeforeNewlines();
		void replaceChar(char cOrig, char cNew);

    private:
#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};
#endif //HOBSTRING_H
