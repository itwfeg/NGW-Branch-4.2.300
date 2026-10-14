/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#define HOBTOKENIZER_MAIN

#ifdef HOBTOKENIZER_MAIN
#include <iostream>
using namespace std;
#include "HobString.h"
#include "HobTokenizer.h"

void testTokenizer(char *data, char sep)
{
	if (!data)
		return;
	HobString h(data);

	cout << "input is : *" << h << "*   separator is : *" << sep << "*" << endl;
	HobTokenizer *ht;
	if (data == (char *)NULL)
		ht = new HobTokenizer((HobString *)NULL, sep);
	else
		ht = new HobTokenizer(&h, sep);
	HobString s;
	ht->first(&s);
	cout << "first is: *" << s << "*" << endl;
	while (ht->hasMoreTokens())
	{
		ht->next(&s);
		cout << "next is: *" << s << "*" << endl;
	}
	ht->first(&s);
	cout << "first is: *" << s << "*" << endl;
	delete ht;
}

void testTokenizer_nextOnly(char *data, char sep)
{
	HobString h(data);
	cout << "next only - input is : *" << h << "*   separator is : *" << sep << "*" << endl;
	HobTokenizer ht(&h, sep);
	HobString s;
	while (ht.hasMoreTokens())
	{
		ht.next(&s);
		cout << "next is: *" << s << "*" << endl;
	}
	ht.first(&s);
	cout << "first is: *" << s << "*" << endl;
}

void testTokenizer(char *data, char *sep)
{
	HobString h(data);
	cout << "input is : *" << h << "*   separators are : *" << sep << "*" << endl;
	HobTokenizer ht(&h, sep);
	HobString s;
	ht.first(&s);
	cout << "first is: *" << s << "*" << endl;
	while (ht.hasMoreTokens())
	{
		ht.next(&s);
		cout << "next is: *" << s << "*" << endl;
	}
	ht.first(&s);
	cout << "first is: *" << s << "*" << endl;
}

void testTokenizerWithNum(char *data, char *sep)
{
	HobString h(data);
	cout << "input is : *" << h << "*   separators are : *" << sep << "*" << endl;
	HobTokenizer ht(&h, sep);
	HobString s;
	ht.first(&s);
	cout << "first is: *" << s << "*" << endl;
	while (ht.hasMoreTokens())
	{
		ht.next(&s);
		cout << "next is: *" << s << "*" << endl;
	}
	ht.first(&s);
	cout << "first is: *" << s << "*" << endl;

	s = "Unknown";
	ht.getTokenNum(&s, 3);
	cout << "token 3 is: *" << s << "*" << endl;

	s = "Unknown";
	ht.getTokenNum(&s, 1);
	cout << "token 1 is: *" << s << "*" << endl;

	s = "Unknown";
	ht.getTokenNum(&s, 4);
	cout << "token 4 is: *" << s << "*" << endl;

	s = "Unknown";
	ht.getTokenNum(&s, 20);
	cout << "token 20 is: *" << s << "*" << endl;

	s = "Unknown";
	ht.getTokenNum(&s, 2);
	cout << "token 2 is: *" << s << "*" << endl;

	s = "Unknown";
	ht.getTokenNum(&s, 0);
	cout << "token 0 is: *" << s << "*" << endl;

	s = "Unknown";
	ht.getTokenNum(&s, 1);
	cout << "token 1 is: *" << s << "*" << endl;

	s = "Unknown";
	ht.getTokenNum(&s, -15);
	cout << "token -15 is: *" << s << "*" << endl;

	s = "Unknown";
	ht.getTokenNum(&s, 1);
	cout << "token 1 is: *" << s << "*" << endl;

	s = "Unknown";
	ht.getTokenNum(&s, 3);
	cout << "token 3 is: *" << s << "*" << endl;

	s = "Unknown";
	ht.getTokenNum(&s, 4);
	cout << "token 4 is: *" << s << "*" << endl;

	s = "Unknown";
	ht.getTokenNum(&s, 2);
	cout << "token 2 is: *" << s << "*" << endl;

}

void testTokenizer(char *data, char *sep, int numTokens)
{
	HobString h(data);
	cout << "input is : *" << h << "*   separators are : *" << sep << "*" << endl;
	HobTokenizer ht(&h, sep, numTokens);
	HobString s;
	ht.first(&s);
	cout << "first is: *" << s << "*" << endl;
	while (ht.hasMoreTokens())
	{
		ht.next(&s);
		cout << "next is: *" << s << "*" << endl;
	}
	ht.first(&s);
	cout << "first is: *" << s << "*" << endl;
}

int hobtokenizer_main(void)
{

	cout << "This test tries to create a new HobTokenizer with both parameters 'null'." << endl;
	cout << "It then tries to read the non-existant tokens." << endl;
	testTokenizer((char *)NULL, (char)NULL);

	cout << endl << endl;
	cout << "This test tries to create a new HobTokenizer with input string being 'null'." << endl;
	cout << "It then tries to read the non-existant tokens." << endl;
	testTokenizer((char *)NULL, ',');

	cout << endl << endl;
	cout << "This test tries to create a new HobTokenizer with separator char being 'null'." << endl;
	cout << "It then tries to read the token(s)." << endl;
	testTokenizer("abc", (char)NULL);

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer with comma separated tokens (token is char)." << endl;
	cout << "The separator is not found in the input, so the entire input string is the one and only token." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("abcdefghi", ',');

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer with comma separated tokens (token is char)." << endl;
	cout << "It then tries to read the tokens by only using next_token," << endl;
	cout << "It should still find the first one." << endl;
	testTokenizer_nextOnly("abc,def,ghi", ',');
	
	cout << endl << endl;
	cout << "This test creates a new HobTokenizer with comma separated tokens (token is char *)." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("abc,def,ghi", ",");

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer with comma separated tokens (token is char *)." << endl;
	cout << "Each separator is doubled." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("abc,,def,,ghi", ",");

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer with comma or semi-colon separated tokens." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("abc,def;ghi", ",;");

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer with comma or semi-colon separated tokens." << endl;
	cout << "Each separator is doubled with a different separator." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("abc,;def;,ghi", ",;");

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer with data containing spaces, but no quotes." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("a bc,de f,g h i, jkl,mno , p q r ,last", ",");

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer with data that are quoted strings." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("\"a bc\",\"de f\",\"g h i\",\" jkl\",\"mno \",\" p q r \",\"last\"", ",");

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer with data that are quoted strings with embedded quotes." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("\"\"\"a bc\",\"de f\"\"\",\"g \"\"h i\",\"\"\" jkl\"\"\",\"\"\"mno \"\"\",\"\"\" p q r \"\"\",\"\"\"last\"\"\"", ",");

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer with MALFORMED data that are quoted strings with embedded quotes." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("\"\"a bc\",\"de f\"\",\"g \"\"h i\",\"\" \" jkl\" \"\",\"\"\"mno \"\"\",\"\"\" p q r \"\"\",\"\"\"last\"\"\",,", ",");


	cout << endl << endl;
	cout << "This test creates a new HobTokenizer for the first 0 of the 6 tokens (should do all tokens)." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("abc,def,ghi,jkl,mno,pqr", ",", 0);

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer for the first 1 of the 6 tokens." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("abc,def,ghi,jkl,mno,pqr", ",", 1);

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer for the first 2 of the 6 tokens." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("abc,def,ghi,jkl,mno,pqr", ",", 2);

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer for the first 5 of the 6 tokens." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("abc,def,ghi,jkl,mno,pqr", ",", 5);

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer for the first 6 of the 6 tokens (should do all tokens)." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("abc,def,ghi,jkl,mno,pqr", ",", 6);

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer for the first 7 of the 6 tokens (should do all tokens)." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("abc,def,ghi,jkl,mno,pqr", ",", 7);

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer for the first -1 of the 6 tokens (should do all tokens)." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("abc,def,ghi,jkl,mno,pqr", ",", -1);

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer for the first 0 of the 6 tokens (should do all tokens)." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("abc,def,ghi,jkl,mno,pqr,", ",", 0);

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer for the first 2 of the 6 tokens (should do all tokens)." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("abc,def,ghi,jkl,mno,pqr,", ",", 2);

	cout << endl << endl;
	cout << "These tests create a new HobTokenizer with comma separated tokens (token is char)." << endl;
	cout << "The separator is also found in the quoted string, in which case it should not be treated as a separator." << endl;
	cout << "It then tries to read the tokens." << endl;
	testTokenizer("\"abc,def,ghi\"", ",");
	cout << endl;
	testTokenizer("\"\"abc", ",");
	cout << endl;
	testTokenizer("\"a,bc\"", ",");
	cout << endl;
	testTokenizer("\"a,bc", ",");
	cout << endl;
	testTokenizer("\"a\",bc", ",");
	cout << endl;
	testTokenizer("\"\",abc", ",");
	cout << endl;
	testTokenizer("\"a,b\"c", ",");
	cout << endl;
	testTokenizer("\"a,b\",c", ",");
	cout << endl;
	testTokenizer("\"a\"\",b,c", ",");
	cout << endl;
	testTokenizer("\"a\"b,c", ",");
	cout << endl;

	cout << endl << endl;
	cout << "This test creates a new HobTokenizer with comma separated tokens (token is char *)." << endl;
	cout << "It then tries to read the tokens, skipping around and accessing different tokens by number." << endl;
	testTokenizerWithNum("a1bc,d2ef,g3hi,j4kl,m5no", ",");

	return 0;
}

#endif //HOBTOKENIZER_MAIN
