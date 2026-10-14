//#define HOBSTRING_MAIN

#ifdef HOBSTRING_MAIN
#include "HobString.h"

void test_stripDup(HobString h, char c)
{
	cout << "input is : *" << h << "*" << endl;
	h.stripDuplicateChar(c);
	const char *x = h.c_str();
	cout << "     removing duplicate '" << c << "'  result is : *" << x << "*" << endl;
}

void test_dup(HobString h, char c)
{
	cout << "input is : *" << h << "*" << endl;
	h.duplicateChar(c);
	const char *x = h.c_str();
	cout << "     duplicating '" << c << "'  result is : *" << x << "*" << endl;
}


void test_stripWS(HobString h, int pos)
{
	cout << "input is : *" << h << "*" << endl;
	h.stripWhiteSpace(pos);
	const char *x = h.c_str();
	cout << "     removing white-space '" << pos << "'  result is :*" << x << "*" << endl;
}

void test_quote(HobString h)
{
	cout << "input is : *" << h << "*" << endl;
	h.quote();
	const char *x = h.c_str();
	cout << "     quoting result is : *" << x << "*" << endl;
}


int hobstring_main(void)
{
	HobString h;

	cout << endl << "Testing removing duplicate characters" << endl;

	h = "abcdefg";
	test_stripDup(h, '"');
	test_stripDup(h, 'd');
	test_stripDup(h, 'a');
	test_stripDup(h, 'g');

	h="abcddefg";
	test_stripDup(h, '"');
	test_stripDup(h, 'a');
	test_stripDup(h, 'g');
	test_stripDup(h, 'd');

	h="abcdefgg";
	test_stripDup(h, '"');
	test_stripDup(h, 'd');
	test_stripDup(h, 'a');
	test_stripDup(h, 'g');

	h="aabcdefg";
	test_stripDup(h, '"');
	test_stripDup(h, 'd');
	test_stripDup(h, 'g');
	test_stripDup(h, 'a');

	h="";
	test_stripDup(h, 0);
	test_stripDup(h, 'd');

	h="a";
	test_stripDup(h, 'd');
	test_stripDup(h, 'a');

	h="aa";
	test_stripDup(h, 'd');
	test_stripDup(h, 'a');

	h="aaa";
	test_stripDup(h, 'd');
	test_stripDup(h, 'a');

	h="aaaa";
	test_stripDup(h, 'd');
	test_stripDup(h, 'a');

	cout << endl << "Testing stripping white-space" << endl;

	h = "abc";
	test_stripWS(h, HobString::LEADING);
	test_stripWS(h, HobString::TRAILING);
	test_stripWS(h, HobString::LEADING_TRAILING);

	h = "  abc";
	test_stripWS(h, HobString::LEADING);
	test_stripWS(h, HobString::TRAILING);
	test_stripWS(h, HobString::LEADING_TRAILING);

	h = "\t\tabc";
	test_stripWS(h, HobString::LEADING);
	test_stripWS(h, HobString::TRAILING);
	test_stripWS(h, HobString::LEADING_TRAILING);

	h = " \tabc";
	test_stripWS(h, HobString::LEADING);
	test_stripWS(h, HobString::TRAILING);
	test_stripWS(h, HobString::LEADING_TRAILING);

	h = " abc  ";
	test_stripWS(h, HobString::LEADING);
	test_stripWS(h, HobString::TRAILING);
	test_stripWS(h, HobString::LEADING_TRAILING);

	h = "abc  ";
	test_stripWS(h, HobString::LEADING);
	test_stripWS(h, HobString::TRAILING);
	test_stripWS(h, HobString::LEADING_TRAILING);

	h = "abc\t\t";
	test_stripWS(h, HobString::LEADING);
	test_stripWS(h, HobString::TRAILING);
	test_stripWS(h, HobString::LEADING_TRAILING);

	h = "abc \t";
	test_stripWS(h, HobString::LEADING);
	test_stripWS(h, HobString::TRAILING);
	test_stripWS(h, HobString::LEADING_TRAILING);

	h = " a\tb c  ";
	test_stripWS(h, HobString::LEADING);
	test_stripWS(h, HobString::TRAILING);
	test_stripWS(h, HobString::LEADING_TRAILING);


	cout << endl << "Testing duplicating characters" << endl;

	h = "";
	test_dup(h, 0);
	test_dup(h, 'a');

	h = "abc";
	test_dup(h, 'd');

	h = "abc";
	test_dup(h, 'a');

	h = "abc";
	test_dup(h, 'b');

	h = "abc";
	test_dup(h, 'c');

	h="abaca";
	test_dup(h, 'a');

	h="a";
	test_dup(h, 'a');

	h="aa";
	test_dup(h, 'a');

	h="aaa";
	test_dup(h, 'a');


	cout << endl << "Testing quoting a string" << endl;

	h = "";
	test_quote(h);

	h = "a";
	test_quote(h);

	h = "abc";
	test_quote(h);

	h = "a\"b";
	test_quote(h);

	h = "\"ab";
	test_quote(h);

	h = "ab\"";
	test_quote(h);

	h = "  ";
	test_quote(h);


	return 0;
}

#endif //HOBSTRING_MAIN