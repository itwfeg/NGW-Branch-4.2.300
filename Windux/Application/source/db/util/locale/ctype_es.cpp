#include <cstdio>



#include "ctype_es.h"



#ifndef NO_NAMESPACES
namespace std
{
#endif



int ctype_es::_class_[256] =
	{
	/*  NUL    0     0            */                                             ctype::cntrl,
	/*  SOH    1     1            */                                             ctype::cntrl,
	/*  STX    2     2            */                                             ctype::cntrl,
	/*  ETX    3     3            */                                             ctype::cntrl,
	/*  EOT    4     4            */                                             ctype::cntrl,
	/*  ENQ    5     5            */                                             ctype::cntrl,
	/*  ACK    6     6            */                                             ctype::cntrl,
	/*  BEL    7     7            */                                             ctype::cntrl,
	/*  BS     8     8            */                                             ctype::cntrl,
	/*  HT     9     9            */                                ctype::space|ctype::cntrl,
	/*  NL    0A    10            */                                ctype::space|ctype::cntrl,
	/*  VT    0B    11            */                                             ctype::cntrl,
	/*  NP    0C    12            */                                             ctype::cntrl,
	/*  CR    0D    13            */                                             ctype::cntrl,
	/*  SO    0E    14            */                                             ctype::cntrl,
	/*  SI    0F    15            */                                             ctype::cntrl,
	/*  DLE   10    16            */                                             ctype::cntrl,
	/*  DC1   11    17            */                                             ctype::cntrl,
	/*  DC2   12    18            */                                             ctype::cntrl,
	/*  DC3   13    19            */                                             ctype::cntrl,
	/*  DC4   14    20            */                                             ctype::cntrl,
	/*  NAK   15    21            */                                             ctype::cntrl,
	/*  SYN   16    22            */                                             ctype::cntrl,
	/*  ETB   17    23            */                                             ctype::cntrl,
	/*  CAN   18    24            */                                             ctype::cntrl,
	/*  EM    19    25            */                                             ctype::cntrl,
	/*  SUB   1A    26            */                                             ctype::cntrl,
	/*  ESC   1B    27            */                                             ctype::cntrl,
	/*  FS    1C    28            */                                             ctype::cntrl,
	/*  GS    1D    29            */                                             ctype::cntrl,
	/*  RS    1E    30            */                                             ctype::cntrl,
	/*  US    1F    31            */                                             ctype::cntrl,
	/*  SP    20    32            */                                ctype::space|ctype::cntrl,
	/*  !     21    33            */                                ctype::punct|ctype::print,
	/*  "     22    34   &quot;   */                                ctype::punct|ctype::print,
	/*  #     23    35            */                                             ctype::print,
	/*  $     24    36            */                                             ctype::print,
	/*  %     25    37            */                                             ctype::print,
	/*  &     26    38   &amp;    */                                             ctype::print,
	/*  '     27    39            */                                ctype::punct|ctype::print,
	/*  (     28    40            */                                ctype::punct|ctype::print,
	/*  )     29    41            */                                ctype::punct|ctype::print,
	/*  *     2A    42            */                                             ctype::print,
	/*  +     2B    43            */                                             ctype::print,
	/*  ,     2C    44            */                                             ctype::print,
	/*  -     2D    45            */                                             ctype::print,
	/*  .     2E    46            */                                ctype::punct|ctype::print,
	/*  /     2F    47            */                                             ctype::print,
	/*  0     30    48            */     ctype::xdigit|             ctype::digit|ctype::print,
	/*  1     31    49            */     ctype::xdigit|             ctype::digit|ctype::print,
	/*  2     32    50            */     ctype::xdigit|             ctype::digit|ctype::print,
	/*  3     33    51            */     ctype::xdigit|             ctype::digit|ctype::print,
	/*  4     34    52            */     ctype::xdigit|             ctype::digit|ctype::print,
	/*  5     35    53            */     ctype::xdigit|             ctype::digit|ctype::print,
	/*  6     36    54            */     ctype::xdigit|             ctype::digit|ctype::print,
	/*  7     37    55            */     ctype::xdigit|             ctype::digit|ctype::print,
	/*  8     38    56            */     ctype::xdigit|             ctype::digit|ctype::print,
	/*  9     39    57            */     ctype::xdigit|             ctype::digit|ctype::print,
	/*  :     3A    58            */                                ctype::punct|ctype::print,
	/*  ;     3B    59            */                                ctype::punct|ctype::print,
	/*  <     3C    60   &lt;     */                                             ctype::print,
	/*  =     3D    61            */                                             ctype::print,
	/*  >     3E    62   &gt;     */                                             ctype::print,
	/*  ?     3F    63            */                                ctype::punct|ctype::print,
	/*  @     40    64            */                                             ctype::print,
	/*  A     41    65            */     ctype::xdigit|ctype::upper|ctype::alpha|ctype::print,
	/*  B     42    66            */     ctype::xdigit|ctype::upper|ctype::alpha|ctype::print,
	/*  C     43    67            */     ctype::xdigit|ctype::upper|ctype::alpha|ctype::print,
	/*  D     44    68            */     ctype::xdigit|ctype::upper|ctype::alpha|ctype::print,
	/*  E     45    69            */     ctype::xdigit|ctype::upper|ctype::alpha|ctype::print,
	/*  F     46    70            */     ctype::xdigit|ctype::upper|ctype::alpha|ctype::print,
	/*  G     47    71            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  H     48    72            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  I     49    73            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  J     4A    74            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  K     4B    75            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  L     4C    76            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  M     4D    77            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  N     4E    78            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  O     4F    79            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  P     50    80            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  Q     51    81            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  R     52    82            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  S     53    83            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  T     54    84            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  U     55    85            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  V     56    86            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  W     57    87            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  X     58    88            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  Y     59    89            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  Z     5A    90            */                   ctype::upper|ctype::alpha|ctype::print,
	/*  [     5B    91            */                                             ctype::print,
	/*  \     5C    92            */                                             ctype::print,
	/*  ]     5D    93            */                                             ctype::print,
	/*  ^     5E    94            */                                             ctype::print,
	/*  _     5F    95            */                                             ctype::print,
	/*  `     60    96            */                                             ctype::print,
	/*  a     61    97            */     ctype::xdigit|ctype::lower|ctype::alpha|ctype::print,
	/*  b     62    98            */     ctype::xdigit|ctype::lower|ctype::alpha|ctype::print,
	/*  c     63    99            */     ctype::xdigit|ctype::lower|ctype::alpha|ctype::print,
	/*  d     64   100            */     ctype::xdigit|ctype::lower|ctype::alpha|ctype::print,
	/*  e     65   101            */     ctype::xdigit|ctype::lower|ctype::alpha|ctype::print,
	/*  f     66   102            */     ctype::xdigit|ctype::lower|ctype::alpha|ctype::print,
	/*  g     67   103            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  h     68   104            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  i     69   105            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  j     6A   106            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  k     6B   107            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  l     6C   108            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  m     6D   109            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  n     6E   110            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  o     6F   111            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  p     70   112            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  q     71   113            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  r     72   114            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  s     73   115            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  t     74   116            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  u     75   117            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  v     76   118            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  w     77   119            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  x     78   120            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  y     79   121            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  z     7A   122            */                   ctype::lower|ctype::alpha|ctype::print,
	/*  {     7B   123            */                                             ctype::print,
	/*  |     7C   124            */                                             ctype::print,
	/*  }     7D   125            */                                             ctype::print,
	/*  ~     7E   126            */                                             ctype::print,
	/*  DEL   7F   127            */                                             ctype::cntrl,
	/*        80   128            */                                             ctype::cntrl,
	/*        81   129            */                                             ctype::cntrl,
	/*        82   130            */                                             ctype::cntrl,
	/*        83   131            */                                             ctype::cntrl,
	/*        84   132            */                                             ctype::cntrl,
	/*        85   133            */                                             ctype::cntrl,
	/*        86   134            */                                             ctype::cntrl,
	/*        87   135            */                                             ctype::cntrl,
	/*        88   136            */                                             ctype::cntrl,
	/*        89   137            */                                             ctype::cntrl,
	/*        8A   138            */                                             ctype::cntrl,
	/*        8B   139            */                                             ctype::cntrl,
	/*        8C   140            */                                             ctype::cntrl,
	/*        8D   141            */                                             ctype::cntrl,
	/*        8E   142            */                                             ctype::cntrl,
	/*        8F   143            */                                             ctype::cntrl,
	/*        90   144            */                                             ctype::cntrl,
	/*        91   145            */                                             ctype::cntrl,
	/*        92   146            */                                             ctype::cntrl,
	/*        93   147            */                                             ctype::cntrl,
	/*        94   148            */                                             ctype::cntrl,
	/*        95   149            */                                             ctype::cntrl,
	/*        96   150            */                                             ctype::cntrl,
	/*        97   151            */                                             ctype::cntrl,
	/*        98   152            */                                             ctype::cntrl,
	/*        99   153            */                                             ctype::cntrl,
	/*        9A   154            */                                             ctype::cntrl,
	/*        9B   155            */                                             ctype::cntrl,
	/*        9C   156            */                                             ctype::cntrl,
	/*        9D   157            */                                             ctype::cntrl,
	/*        9E   158            */                                             ctype::cntrl,
	/*        9F   159            */                                             ctype::cntrl,
	/*        A0   160   &nbsp;   */                                             ctype::cntrl,
	/*  ¡     A1   161   &iexcl;  */                                ctype::punct|ctype::print,
	/*        A2   162   &cent;   */                                             ctype::cntrl,
	/*        A3   163   &pound;  */                                             ctype::cntrl,
	/*        A4   164   &curren; */                                             ctype::cntrl,
	/*        A5   165   &yen;    */                                             ctype::cntrl,
	/*        A6   166   &brvbar; */                                             ctype::cntrl,
	/*        A7   167   &sect;   */                                             ctype::cntrl,
	/*        A8   168   &uml;    */                                             ctype::cntrl,
	/*        A9   169   &copy;   */                                             ctype::cntrl,
	/*  ª     AA   170   &ordf;   */                                             ctype::print,
	/*        AB   171   &laquo;  */                                             ctype::cntrl,
	/*        AC   172   &not;    */                                             ctype::cntrl,
	/*        AD   173   &shy;    */                                             ctype::cntrl,
	/*        AE   174   &reg;    */                                             ctype::cntrl,
	/*        AF   175   &macr;   */                                             ctype::cntrl,
	/*  º     B0   176   &deg;    */                                             ctype::print,
	/*        B1   177   &plusmn; */                                             ctype::cntrl,
	/*        B2   178   &sup2;   */                                             ctype::cntrl,
	/*        B3   179   &sup3;   */                                             ctype::cntrl,
	/*        B4   180   &acute;  */                                             ctype::cntrl,
	/*        B5   181   &micro;  */                                             ctype::cntrl,
	/*        B6   182   &para;   */                                             ctype::cntrl,
	/*        B7   183   &middot; */                                             ctype::cntrl,
	/*        B8   184   &cedil;  */                                             ctype::cntrl,
	/*        B9   185   &sup1;   */                                             ctype::cntrl,
	/*  º     BA   186   &ordm;   */                                             ctype::print,
	/*        BB   187   &raquo;  */                                             ctype::cntrl,
	/*        BC   188   &frac14; */                                             ctype::cntrl,
	/*        BD   189   &frac12; */                                             ctype::cntrl,
	/*        BE   190   &frac34; */                                             ctype::cntrl,
	/*  ¿     BF   191   &iquest; */                                ctype::punct|ctype::print,
	/*        C0   192   &Agrave; */                                             ctype::cntrl,
	/*  Á     C1   193   &Aacute; */                   ctype::upper|ctype::alpha|ctype::print,
	/*        C2   194   &Acirc;  */                                             ctype::cntrl,
	/*        C3   195   &Atilde; */                                             ctype::cntrl,
	/*        C4   196   &Auml;   */                                             ctype::cntrl,
	/*        C5   197   &Aring;  */                                             ctype::cntrl,
	/*        C6   198   &AElig;  */                                             ctype::cntrl,
	/*        C7   199   &Ccedil; */                                             ctype::cntrl,
	/*        C8   200   &Egrave; */                                             ctype::cntrl,
	/*  É     C9   201   &Eacute; */                   ctype::upper|ctype::alpha|ctype::print,
	/*        CA   202   &Ecirc;  */                                             ctype::cntrl,
	/*        CB   203   &Euml;   */                                             ctype::cntrl,
	/*        CC   204   &Igrave; */                                             ctype::cntrl,
	/*  Í     CD   205   &Iacute; */                   ctype::upper|ctype::alpha|ctype::print,
	/*        CE   206   &Icirc;  */                                             ctype::cntrl,
	/*        CF   207   &Iuml;   */                                             ctype::cntrl,
	/*        D0   208   &ETH;    */                                             ctype::cntrl,
	/*  Ñ     D1   209   &Ntilde; */                   ctype::upper|ctype::alpha|ctype::print,
	/*        D2   210   &Ograve; */                                             ctype::cntrl,
	/*  Ó     D3   211   &Oacute; */                   ctype::upper|ctype::alpha|ctype::print,
	/*        D4   212   &Ocirc;  */                                             ctype::cntrl,
	/*        D5   213   &Otilde; */                                             ctype::cntrl,
	/*        D6   214   &Ouml;   */                                             ctype::cntrl,
	/*        D7   215   &times;  */                                             ctype::cntrl,
	/*        D8   216   &Oslash; */                                             ctype::cntrl,
	/*        D9   217   &Ugrave; */                                             ctype::cntrl,
	/*  Ú     DA   218   &Uacute; */                   ctype::upper|ctype::alpha|ctype::print,
	/*        DB   219   &Ucirc;  */                                             ctype::cntrl,
	/*  Ü     DC   220   &Uuml;   */                   ctype::upper|ctype::alpha|ctype::print,
	/*        DD   221   &Yacute; */                                             ctype::cntrl,
	/*        DE   222   &THORN;  */                                             ctype::cntrl,
	/*        DF   223   &szlig;  */                                             ctype::cntrl,
	/*        E0   224   &agrave; */                                             ctype::cntrl,
	/*  á     E1   225   &aacute; */                   ctype::lower|ctype::alpha|ctype::print,
	/*        E2   226   &acirc;  */                                             ctype::cntrl,
	/*        E3   227   &atilde; */                                             ctype::cntrl,
	/*        E4   228   &auml;   */                                             ctype::cntrl,
	/*        E5   229   &aring;  */                                             ctype::cntrl,
	/*        E6   230   &aElig;  */                                             ctype::cntrl,
	/*        E7   231   &ccedil; */                                             ctype::cntrl,
	/*        E8   232   &egrave; */                                             ctype::cntrl,
	/*  é     E9   233   &eacute; */                   ctype::lower|ctype::alpha|ctype::print,
	/*        EA   234   &ecirc;  */                                             ctype::cntrl,
	/*        EB   235   &euml;   */                                             ctype::cntrl,
	/*        EC   236   &igrave; */                                             ctype::cntrl,
	/*  í     ED   237   &iacute; */                   ctype::lower|ctype::alpha|ctype::print,
	/*        EE   238   &icirc;  */                                             ctype::cntrl,
	/*        EF   239   &iuml;   */                                             ctype::cntrl,
	/*        F0   240   &eth;    */                                             ctype::cntrl,
	/*  ñ     F1   241   &ntilde; */                   ctype::lower|ctype::alpha|ctype::print,
	/*        F2   242   &ograve; */                                             ctype::cntrl,
	/*  ó     F3   243   &oacute; */                   ctype::lower|ctype::alpha|ctype::print,
	/*        F4   244   &ocirc;  */                                             ctype::cntrl,
	/*        F5   245   &otilde; */                                             ctype::cntrl,
	/*        F6   246   &ouml;   */                                             ctype::cntrl,
	/*        F7   247   &divide; */                                             ctype::cntrl,
	/*        F8   248   &oslash; */                                             ctype::cntrl,
	/*        F9   249   &ugrave; */                                             ctype::cntrl,
	/*  ú     FA   250   &uacute; */                   ctype::lower|ctype::alpha|ctype::print,
	/*        FB   251   &ucirc;  */                                             ctype::cntrl,
	/*  ü     FC   252   &uuml;   */                   ctype::lower|ctype::alpha|ctype::print,
	/*        FD   253   &yacute; */                                             ctype::cntrl,
	/*        FE   254   &thorn;  */                                             ctype::cntrl,
	/*        FF   255   &yuml;   */                                             ctype::cntrl
	};


char ctype_es::do_toupper(char c) const
{
	if ( _class_[(unsigned char)c]&ctype::lower )
		return c&'\xDF';
	else
		return c;
}


const char* ctype_es::do_toupper(char* b, const char* e) const
{
	for ( ; b < e; ++b )
		if ( _class_[*(unsigned char*)b]&ctype::lower )
			*b &= '\xDF';

	return e;
}


char ctype_es::do_tolower(char c) const
{
	if ( _class_[(unsigned char)c]&ctype::upper )
		return c|'\x20';
	else
		return c;
}


const char* ctype_es::do_tolower(char* b, const char* e) const
{
	for ( ; b < e; ++b )
		if ( _class_[*(unsigned char*)b]&ctype::upper )
			*b |= '\x20';

	return e;
}


void ctype_es::do_test() const
{
	// Testing character classification
	test_do_is("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzÁÉÍÓÚÜÑáéíóúüñ", cntrl);
	test_do_is("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzÁÉÍÓÚÜÑáéíóúüñ", print);
	test_do_is("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzÁÉÍÓÚÜÑáéíóúüñ", space);
	test_do_is("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzÁÉÍÓÚÜÑáéíóúüñ", punct);
	test_do_is("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzÁÉÍÓÚÜÑáéíóúüñ", alpha);
	test_do_is("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzÁÉÍÓÚÜÑáéíóúüñ", digit);
	test_do_is("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzÁÉÍÓÚÜÑáéíóúüñ", xdigit);
	test_do_is("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzÁÉÍÓÚÜÑáéíóúüñ", upper);
	test_do_is("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzÁÉÍÓÚÜÑáéíóúüñ", lower);
	test_do_is("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzÁÉÍÓÚÜÑáéíóúüñ", alnum);
	test_do_is("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzÁÉÍÓÚÜÑáéíóúüñ", graph);

	// Testing upper-casing
	test_do_toupper("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzÁÉÍÓÚÜÑáéíóúüñ");

	// Testing upper-casing
	test_do_tolower("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzÁÉÍÓÚÜÑáéíóúüñ");
}


void ctype_es::test_do_is(const std::string& str, mask m) const
{
	printf("Character classification for characters '%s' and mask %s\n", str.c_str(), maskString(m));
	for ( unsigned int i = 0; i < str.size(); ++i )
		printf("   Character %c	(%x) %s of type %s\n", str[i], str[i], (do_is(m, str[i]) ? "is" : "is not"), maskString(m));
}


void ctype_es::test_do_toupper(const std::string& str) const
{
	string s(str);
	printf("Upper-casing characters '%s' produces:\n", s.c_str());
	printf("   Individually: '");
	for ( unsigned int i = 0; i < s.size(); ++i )
		printf("%c", do_toupper(s[i]));
	printf("'\n");
	do_toupper(&s[0],&s[0] + s.size());
	printf("   As a whole:   '%s'\n", s.c_str());
}


void ctype_es::test_do_tolower(const std::string& str) const
{
	string s(str);
	printf("Lower-casing characters '%s' produces:\n", s.c_str());
	printf("   Individually: '");
	for ( unsigned int i = 0; i < s.size(); ++i )
		printf("%c", do_tolower(s[i]));
	printf("'\n");
	do_tolower(&s[0],&s[0] + s.size());
	printf("   As a whole:   '%s'\n", s.c_str());
}


const char* ctype_es::maskString(mask m) const
{
	switch ( m )
	{
		case cntrl:
			return "cntrl";
		case print:
			return "print";
		case space:
			return "space";
		case punct:
			return "punct";
		case alpha:
			return "alpha";
		case digit:
			return "digit";
		case xdigit:
			return "xdigit";
		case upper:
			return "upper";
		case lower:
			return "lower";
		case alnum:
			return "alnum";
		case graph:
			return "graph";
		default :
			return "";
	}
}



#ifndef NO_NAMESPACES
}
#endif

