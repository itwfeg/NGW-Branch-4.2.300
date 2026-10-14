#include <cstdio>



#include "collate_es.h"



#ifndef NO_NAMESPACES
namespace std
{
#endif



unsigned char collate_es::_trans_[256] =
{
	/*  NUL    0     0            */  '\x20'  /*  ' '  */ ,
	/*  SOH    1     1            */  '\x20'  /*  ' '  */ ,
	/*  STX    2     2            */  '\x20'  /*  ' '  */ ,
	/*  ETX    3     3            */  '\x20'  /*  ' '  */ ,
	/*  EOT    4     4            */  '\x20'  /*  ' '  */ ,
	/*  ENQ    5     5            */  '\x20'  /*  ' '  */ ,
	/*  ACK    6     6            */  '\x20'  /*  ' '  */ ,
	/*  BEL    7     7            */  '\x20'  /*  ' '  */ ,
	/*  BS     8     8            */  '\x20'  /*  ' '  */ ,
	/*  HT     9     9            */  '\x20'  /*  ' '  */ ,
	/*  NL     A    10            */  '\x20'  /*  ' '  */ ,
	/*  VT     B    11            */  '\x20'  /*  ' '  */ ,
	/*  NP     C    12            */  '\x20'  /*  ' '  */ ,
	/*  CR     D    13            */  '\x20'  /*  ' '  */ ,
	/*  SO     E    14            */  '\x20'  /*  ' '  */ ,
	/*  SI     F    15            */  '\x20'  /*  ' '  */ ,
	/*  DLE   10    16            */  '\x20'  /*  ' '  */ ,
	/*  DC1   11    17            */  '\x20'  /*  ' '  */ ,
	/*  DC2   12    18            */  '\x20'  /*  ' '  */ ,
	/*  DC3   13    19            */  '\x20'  /*  ' '  */ ,
	/*  DC4   14    20            */  '\x20'  /*  ' '  */ ,
	/*  NAK   15    21            */  '\x20'  /*  ' '  */ ,
	/*  SYN   16    22            */  '\x20'  /*  ' '  */ ,
	/*  ETB   17    23            */  '\x20'  /*  ' '  */ ,
	/*  CAN   18    24            */  '\x20'  /*  ' '  */ ,
	/*  EM    19    25            */  '\x20'  /*  ' '  */ ,
	/*  SUB   1A    26            */  '\x20'  /*  ' '  */ ,
	/*  ESC   1B    27            */  '\x20'  /*  ' '  */ ,
	/*  FS    1C    28            */  '\x20'  /*  ' '  */ ,
	/*  GS    1D    29            */  '\x20'  /*  ' '  */ ,
	/*  RS    1E    30            */  '\x20'  /*  ' '  */ ,
	/*  US    1F    31            */  '\x20'  /*  ' '  */ ,
	/*  SP    20    32            */  '\x20'  /*  ' '  */ ,
	/*  !     21    33            */  '\x20'  /*  ' '  */ ,
	/*  "     22    34   &quot;   */  '\x20'  /*  ' '  */ ,
	/*  #     23    35            */  '\x20'  /*  ' '  */ ,
	/*  $     24    36            */  '\x20'  /*  ' '  */ ,
	/*  %     25    37            */  '\x20'  /*  ' '  */ ,
	/*  &     26    38   &amp;    */  '\x20'  /*  ' '  */ ,
	/*  '     27    39            */  '\x20'  /*  ' '  */ ,
	/*  (     28    40            */  '\x20'  /*  ' '  */ ,
	/*  )     29    41            */  '\x20'  /*  ' '  */ ,
	/*  *     2A    42            */  '\x20'  /*  ' '  */ ,
	/*  +     2B    43            */  '\x20'  /*  ' '  */ ,
	/*  ,     2C    44            */  '\x20'  /*  ' '  */ ,
	/*  -     2D    45            */  '\x20'  /*  ' '  */ ,
	/*  .     2E    46            */  '\x20'  /*  ' '  */ ,
	/*  /     2F    47            */  '\x20'  /*  ' '  */ ,
	/*  0     30    48            */  '\x30'  /*  '0'  */ ,
	/*  1     31    49            */  '\x31'  /*  '1'  */ ,
	/*  2     32    50            */  '\x32'  /*  '2'  */ ,
	/*  3     33    51            */  '\x33'  /*  '3'  */ ,
	/*  4     34    52            */  '\x34'  /*  '4'  */ ,
	/*  5     35    53            */  '\x35'  /*  '5'  */ ,
	/*  6     36    54            */  '\x36'  /*  '6'  */ ,
	/*  7     37    55            */  '\x37'  /*  '7'  */ ,
	/*  8     38    56            */  '\x38'  /*  '8'  */ ,
	/*  9     39    57            */  '\x39'  /*  '9'  */ ,
	/*  :     3A    58            */  '\x20'  /*  ' '  */ ,
	/*  ;     3B    59            */  '\x20'  /*  ' '  */ ,
	/*  <     3C    60   &lt;     */  '\x20'  /*  ' '  */ ,
	/*  =     3D    61            */  '\x20'  /*  ' '  */ ,
	/*  >     3E    62   &gt;     */  '\x20'  /*  ' '  */ ,
	/*  ?     3F    63            */  '\x20'  /*  ' '  */ ,
	/*  @     40    64            */  '\x20'  /*  ' '  */ ,
	/*  A     41    65            */  '\x41'  /*  'A'  */ ,
	/*  B     42    66            */  '\x42'  /*  'B'  */ ,
	/*  C     43    67            */  '\x43'  /*  'C'  */ ,
	/*  D     44    68            */  '\x45'  /*  'E'  */ ,
	/*  E     45    69            */  '\x46'  /*  'F'  */ ,
	/*  F     46    70            */  '\x47'  /*  'G'  */ ,
	/*  G     47    71            */  '\x48'  /*  'H'  */ ,
	/*  H     48    72            */  '\x49'  /*  'I'  */ ,
	/*  I     49    73            */  '\x4A'  /*  'J'  */ ,
	/*  J     4A    74            */  '\x4B'  /*  'K'  */ ,
	/*  K     4B    75            */  '\x4C'  /*  'L'  */ ,
	/*  L     4C    76            */  '\x4D'  /*  'M'  */ ,
	/*  M     4D    77            */  '\x4F'  /*  'O'  */ ,
	/*  N     4E    78            */  '\x50'  /*  'P'  */ ,
	/*  O     4F    79            */  '\x52'  /*  'R'  */ ,
	/*  P     50    80            */  '\x53'  /*  'S'  */ ,
	/*  Q     51    81            */  '\x54'  /*  'T'  */ ,
	/*  R     52    82            */  '\x55'  /*  'U'  */ ,
	/*  S     53    83            */  '\x56'  /*  'V'  */ ,
	/*  T     54    84            */  '\x57'  /*  'W'  */ ,
	/*  U     55    85            */  '\x58'  /*  'X'  */ ,
	/*  V     56    86            */  '\x59'  /*  'Y'  */ ,
	/*  W     57    87            */  '\x5A'  /*  'Z'  */ ,
	/*  X     58    88            */  '\x5B'  /*  '['  */ ,
	/*  Y     59    89            */  '\x5C'  /*  '\\' */ ,
	/*  Z     5A    90            */  '\x5D'  /*  ']'  */ ,
	/*  [     5B    91            */  '\x20'  /*  ' '  */ ,
	/*  \     5C    92            */  '\x20'  /*  ' '  */ ,
	/*  ]     5D    93            */  '\x20'  /*  ' '  */ ,
	/*  ^     5E    94            */  '\x20'  /*  ' '  */ ,
	/*  _     5F    95            */  '\x20'  /*  ' '  */ ,
	/*  `     60    96            */  '\x20'  /*  ' '  */ ,
	/*  a     61    97            */  '\x61'  /*  'a'  */ ,
	/*  b     62    98            */  '\x62'  /*  'b'  */ ,
	/*  c     63    99            */  '\x63'  /*  'c'  */ ,
	/*  d     64   100            */  '\x65'  /*  'e'  */ ,
	/*  e     65   101            */  '\x66'  /*  'f'  */ ,
	/*  f     66   102            */  '\x67'  /*  'g'  */ ,
	/*  g     67   103            */  '\x68'  /*  'h'  */ ,
	/*  h     68   104            */  '\x69'  /*  'i'  */ ,
	/*  i     69   105            */  '\x6A'  /*  'j'  */ ,
	/*  j     6A   106            */  '\x6B'  /*  'k'  */ ,
	/*  k     6B   107            */  '\x6C'  /*  'l'  */ ,
	/*  l     6C   108            */  '\x6D'  /*  'm'  */ ,
	/*  m     6D   109            */  '\x6F'  /*  'o'  */ ,
	/*  n     6E   110            */  '\x70'  /*  'p'  */ ,
	/*  o     6F   111            */  '\x72'  /*  'r'  */ ,
	/*  p     70   112            */  '\x73'  /*  's'  */ ,
	/*  q     71   113            */  '\x74'  /*  't'  */ ,
	/*  r     72   114            */  '\x75'  /*  'u'  */ ,
	/*  s     73   115            */  '\x76'  /*  'v'  */ ,
	/*  t     74   116            */  '\x77'  /*  'w'  */ ,
	/*  u     75   117            */  '\x78'  /*  'x'  */ ,
	/*  v     76   118            */  '\x79'  /*  'y'  */ ,
	/*  w     77   119            */  '\x7A'  /*  'z'  */ ,
	/*  x     78   120            */  '\x7B'  /*  '{'  */ ,
	/*  y     79   121            */  '\x7C'  /*  '\\' */ ,
	/*  z     7A   122            */  '\x7D'  /*  '}'  */ ,
	/*  {     7B   123            */  '\x20'  /*  ' '  */ ,
	/*  |     7C   124            */  '\x20'  /*  ' '  */ ,
	/*  }     7D   125            */  '\x20'  /*  ' '  */ ,
	/*  ~     7E   126            */  '\x20'  /*  ' '  */ ,
	/*  DEL   7F   127            */  '\x20'  /*  ' '  */ ,
	/*        80   128            */  '\x20'  /*  ' '  */ ,
	/*        81   129            */  '\x20'  /*  ' '  */ ,
	/*        82   130            */  '\x20'  /*  ' '  */ ,
	/*        83   131            */  '\x20'  /*  ' '  */ ,
	/*        84   132            */  '\x20'  /*  ' '  */ ,
	/*        85   133            */  '\x20'  /*  ' '  */ ,
	/*        86   134            */  '\x20'  /*  ' '  */ ,
	/*        87   135            */  '\x20'  /*  ' '  */ ,
	/*        88   136            */  '\x20'  /*  ' '  */ ,
	/*        89   137            */  '\x20'  /*  ' '  */ ,
	/*        8A   138            */  '\x20'  /*  ' '  */ ,
	/*        8B   139            */  '\x20'  /*  ' '  */ ,
	/*        8C   140            */  '\x20'  /*  ' '  */ ,
	/*        8D   141            */  '\x20'  /*  ' '  */ ,
	/*        8E   142            */  '\x20'  /*  ' '  */ ,
	/*        8F   143            */  '\x20'  /*  ' '  */ ,
	/*        90   144            */  '\x20'  /*  ' '  */ ,
	/*        91   145            */  '\x20'  /*  ' '  */ ,
	/*        92   146            */  '\x20'  /*  ' '  */ ,
	/*        93   147            */  '\x20'  /*  ' '  */ ,
	/*        94   148            */  '\x20'  /*  ' '  */ ,
	/*        95   149            */  '\x20'  /*  ' '  */ ,
	/*        96   150            */  '\x20'  /*  ' '  */ ,
	/*        97   151            */  '\x20'  /*  ' '  */ ,
	/*        98   152            */  '\x20'  /*  ' '  */ ,
	/*        99   153            */  '\x20'  /*  ' '  */ ,
	/*        9A   154            */  '\x20'  /*  ' '  */ ,
	/*        9B   155            */  '\x20'  /*  ' '  */ ,
	/*        9C   156            */  '\x20'  /*  ' '  */ ,
	/*        9D   157            */  '\x20'  /*  ' '  */ ,
	/*        9E   158            */  '\x20'  /*  ' '  */ ,
	/*        9F   159            */  '\x20'  /*  ' '  */ ,
	/*        A0   160   &nbsp;   */  '\x20'  /*  ' '  */ ,
	/*  ¡     A1   161   &iexcl;  */  '\x20'  /*  ' '  */ ,
	/*        A2   162   &cent;   */  '\x20'  /*  ' '  */ ,
	/*        A3   163   &pound;  */  '\x20'  /*  ' '  */ ,
	/*        A4   164   &curren; */  '\x20'  /*  ' '  */ ,
	/*        A5   165   &yen;    */  '\x20'  /*  ' '  */ ,
	/*        A6   166   &brvbar; */  '\x20'  /*  ' '  */ ,
	/*        A7   167   &sect;   */  '\x20'  /*  ' '  */ ,
	/*        A8   168   &uml;    */  '\x20'  /*  ' '  */ ,
	/*        A9   169   &copy;   */  '\x20'  /*  ' '  */ ,
	/*  ª     AA   170   &ordf;   */  '\x20'  /*  ' '  */ ,
	/*        AB   171   &laquo;  */  '\x20'  /*  ' '  */ ,
	/*        AC   172   &not;    */  '\x20'  /*  ' '  */ ,
	/*        AD   173   &shy;    */  '\x20'  /*  ' '  */ ,
	/*        AE   174   &reg;    */  '\x20'  /*  ' '  */ ,
	/*        AF   175   &macr;   */  '\x20'  /*  ' '  */ ,
	/*  º     B0   176   &deg;    */  '\x20'  /*  ' '  */ ,
	/*        B1   177   &plusmn; */  '\x20'  /*  ' '  */ ,
	/*        B2   178   &sup2;   */  '\x20'  /*  ' '  */ ,
	/*        B3   179   &sup3;   */  '\x20'  /*  ' '  */ ,
	/*        B4   180   &acute;  */  '\x20'  /*  ' '  */ ,
	/*        B5   181   &micro;  */  '\x20'  /*  ' '  */ ,
	/*        B6   182   &para;   */  '\x20'  /*  ' '  */ ,
	/*        B7   183   &middot; */  '\x20'  /*  ' '  */ ,
	/*        B8   184   &cedil;  */  '\x20'  /*  ' '  */ ,
	/*        B9   185   &sup1;   */  '\x20'  /*  ' '  */ ,
	/*  º     BA   186   &ordm;   */  '\x20'  /*  ' '  */ ,
	/*        BB   187   &raquo;  */  '\x20'  /*  ' '  */ ,
	/*        BC   188   &frac14; */  '\x20'  /*  ' '  */ ,
	/*        BD   189   &frac12; */  '\x20'  /*  ' '  */ ,
	/*        BE   190   &frac34; */  '\x20'  /*  ' '  */ ,
	/*  ¿     BF   191   &iquest; */  '\x20'  /*  ' '  */ ,
	/*        C0   192   &Agrave; */  '\x20'  /*  ' '  */ ,
	/*  Á     C1   193   &Aacute; */  '\x41'  /*  'A'  */ ,
	/*        C2   194   &Acirc;  */  '\x20'  /*  ' '  */ ,
	/*        C3   195   &Atilde; */  '\x20'  /*  ' '  */ ,
	/*        C4   196   &Auml;   */  '\x20'  /*  ' '  */ ,
	/*        C5   197   &Aring;  */  '\x20'  /*  ' '  */ ,
	/*        C6   198   &AElig;  */  '\x20'  /*  ' '  */ ,
	/*        C7   199   &Ccedil; */  '\x20'  /*  ' '  */ ,
	/*        C8   200   &Egrave; */  '\x20'  /*  ' '  */ ,
	/*  É     C9   201   &Eacute; */  '\x46'  /*  'F'  */ ,
	/*        CA   202   &Ecirc;  */  '\x20'  /*  ' '  */ ,
	/*        CB   203   &Euml;   */  '\x20'  /*  ' '  */ ,
	/*        CC   204   &Igrave; */  '\x20'  /*  ' '  */ ,
	/*  Í     CD   205   &Iacute; */  '\x4A'  /*  'J'  */ ,
	/*        CE   206   &Icirc;  */  '\x20'  /*  ' '  */ ,
	/*        CF   207   &Iuml;   */  '\x20'  /*  ' '  */ ,
	/*        D0   208   &ETH;    */  '\x20'  /*  ' '  */ ,
	/*  Ñ     D1   209   &Ntilde; */  '\x51'  /*  'Q'  */ ,
	/*        D2   210   &Ograve; */  '\x20'  /*  ' '  */ ,
	/*  Ó     D3   211   &Oacute; */  '\x52'  /*  'R'  */ ,
	/*        D4   212   &Ocirc;  */  '\x20'  /*  ' '  */ ,
	/*        D5   213   &Otilde; */  '\x20'  /*  ' '  */ ,
	/*        D6   214   &Ouml;   */  '\x20'  /*  ' '  */ ,
	/*        D7   215   &times;  */  '\x20'  /*  ' '  */ ,
	/*        D8   216   &Oslash; */  '\x20'  /*  ' '  */ ,
	/*        D9   217   &Ugrave; */  '\x20'  /*  ' '  */ ,
	/*  Ú     DA   218   &Uacute; */  '\x58'  /*  'X'  */ ,
	/*        DB   219   &Ucirc;  */  '\x20'  /*  ' '  */ ,
	/*  Ü     DC   220   &Uuml;   */  '\x58'  /*  'X'  */ ,
	/*        DD   221   &Yacute; */  '\x20'  /*  ' '  */ ,
	/*        DE   222   &THORN;  */  '\x20'  /*  ' '  */ ,
	/*        DF   223   &szlig;  */  '\x20'  /*  ' '  */ ,
	/*        E0   224   &agrave; */  '\x20'  /*  ' '  */ ,
	/*  á     E1   225   &aacute; */  '\x61'  /*  'a'  */ ,
	/*        E2   226   &acirc;  */  '\x20'  /*  ' '  */ ,
	/*        E3   227   &atilde; */  '\x20'  /*  ' '  */ ,
	/*        E4   228   &auml;   */  '\x20'  /*  ' '  */ ,
	/*        E5   229   &aring;  */  '\x20'  /*  ' '  */ ,
	/*        E6   230   &aElig;  */  '\x20'  /*  ' '  */ ,
	/*        E7   231   &ccedil; */  '\x20'  /*  ' '  */ ,
	/*        E8   232   &egrave; */  '\x20'  /*  ' '  */ ,
	/*  é     E9   233   &eacute; */  '\x66'  /*  'f'  */ ,
	/*        EA   234   &ecirc;  */  '\x20'  /*  ' '  */ ,
	/*        EB   235   &euml;   */  '\x20'  /*  ' '  */ ,
	/*        EC   236   &igrave; */  '\x20'  /*  ' '  */ ,
	/*  í     ED   237   &iacute; */  '\x6A'  /*  'j'  */ ,
	/*        EE   238   &icirc;  */  '\x20'  /*  ' '  */ ,
	/*        EF   239   &iuml;   */  '\x20'  /*  ' '  */ ,
	/*        F0   240   &eth;    */  '\x20'  /*  ' '  */ ,
	/*  ñ     F1   241   &ntilde; */  '\x71'  /*  'q'  */ ,
	/*        F2   242   &ograve; */  '\x20'  /*  ' '  */ ,
	/*  ó     F3   243   &oacute; */  '\x72'  /*  'r'  */ ,
	/*        F4   244   &ocirc;  */  '\x20'  /*  ' '  */ ,
	/*        F5   245   &otilde; */  '\x20'  /*  ' '  */ ,
	/*        F6   246   &ouml;   */  '\x20'  /*  ' '  */ ,
	/*        F7   247   &divide; */  '\x20'  /*  ' '  */ ,
	/*        F8   248   &oslash; */  '\x20'  /*  ' '  */ ,
	/*        F9   249   &ugrave; */  '\x20'  /*  ' '  */ ,
	/*  ú     FA   250   &uacute; */  '\x78'  /*  'x'  */ ,
	/*        FB   251   &ucirc;  */  '\x20'  /*  ' '  */ ,
	/*  ü     FC   252   &uuml;   */  '\x78'  /*  'x'  */ ,
	/*        FD   253   &yacute; */  '\x20'  /*  ' '  */ ,
	/*        FE   254   &thorn;  */  '\x20'  /*  ' '  */ ,
	/*        FF   255   &yuml;   */  '\x20'  /*  ' '  */  
};


class transformIterator
{
	public:
		transformIterator(const char* p, const char* e) : ch_((const unsigned char*)p), end_((const unsigned char*)e) {}
		transformIterator(const transformIterator& iter) : ch_(iter.ch_), end_(iter.end_) {}
		~transformIterator() {}

		transformIterator& operator=(const transformIterator& rhs) { ch_ = rhs.ch_; end_ = rhs.end_; return *this; }
		char operator*() 
				{ char res; if ( ch_ + 1 != end_ && (res = doubleCharacter(ch_)) ) return res; return collate_es::_trans_[*ch_]; }
		const char operator*() const
				{ char res; if ( ch_ + 1 != end_ && (res = doubleCharacter(ch_)) ) return res; return static_cast<const char>(collate_es::_trans_[*ch_]); }
//		char operator->() { return collate_es::_trans_[*ch_]; }
//		const char& operator->() const { return static_cast<const char>(collate_es::_trans_[*ch_]); }
		transformIterator& operator++() 
				{ const unsigned char* current = ch_++; if ( ch_ != end_ && doubleCharacter(current) ) ++ ch_; return *this; }
//		const transformIterator& operator++() const { ++ch_; return *this; }
		transformIterator operator++(int) { transformIterator temp(*this); ++*this; return temp; }
//		const transformIterator operator++(int) const { transformIterator temp = *this; ++*this; return temp; }
//		transformIterator& operator--() { --ch_; return *this; }
//		const transformIterator& operator--() const { --ch_; return *this; }
//		transformIterator operator--(int) { transformIterator temp = *this; --*this; return temp; }
//		const transformIterator operator--(int) const { transformIterator temp = *this; --*this; return temp; }
		bool operator==(const transformIterator& rhs) const { return ch_ == rhs.ch_; }
		bool operator!=(const transformIterator& rhs) const { return !(*this == rhs); }
		int operator-(const transformIterator& rhs) const 
		{
			int counter = 0; 
			const unsigned char* current = rhs.ch_;
			const unsigned char* preLast = ch_ - 1;
			for ( ; current < preLast; ++current )
			{
				if ( doubleCharacter(current) )
					++current;

				++counter;
			}
		
			if ( current != ch_ )
				++counter;

			return counter; 
		}

	private:
		const unsigned char* ch_;
		const unsigned char* end_;

		static char doubleCharacter(const unsigned char* ch)     // Double characters are "Ch" and "Ll"
				{ 
					switch ( (*ch&0xdf)<<8 | *(ch + 1)&0xDF )
					{
						case 0x4348:
							return 0x44 | (*ch & 0x20);
						case 0x4C4C:
							return 0x4E | (*ch & 0x20);
					}

					return '\x00';
				}
};


int collate_es::do_compare(const char* b, const char* e, const char* b2, const char* e2) const
{
	transformIterator it1(b, e), end1(e, e);
	transformIterator it2(b2, e2), end2(e2, e2);

	while ( it1 != end1 && it2 != end2 )
	{
		if ( *it1 < *it2 )
			return -1;
		else if ( *it1 > *it2 )
			return 1;

		++it1;
		++it2;
	}

	if ( it1 != end1 )
		return 1;
	else if ( it2 != end2 )
		return -1;
	
	return 0;		
}


collate_es::string_type collate_es::do_transform(const char* b, const char* e) const
{
	return string(transformIterator(b, e), transformIterator(e, e));
}


void collate_es::do_test() const
{
	// Test comparing 'cana' y 'caña'
	test_do_compare("cana", "caña");
	test_do_compare("CANA", "CAÑA");
	test_do_compare("Cana", "Caña");
	test_do_compare("CaNa", "CaÑa");
	test_do_compare("cAnA", "cAñA");

	// Test comparing words with 'ch'
	test_do_compare("Paco", "Pacha");
	test_do_compare("PaCo", "Pacha");
	test_do_compare("Paco", "PaCha");
	test_do_compare("Paco", "PaCHa");
	test_do_compare("PaCo", "PaCHa");
	test_do_compare("PaCO", "PaCHa");
	test_do_compare("PaCo", "PaCHA");
	test_do_compare("PaCo", "PaChA");
	test_do_compare("PaCo", "PacHA");
	test_do_compare("Paco", "PacHA");
	test_do_compare("PacO", "PacHA");
	test_do_compare("PacA", "PacHA");
	test_do_compare("Paca", "PacHA");

	test_do_compare("copa", "chapa");
	test_do_compare("copa", "Chapa");
	test_do_compare("Copa", "cHapa");
	test_do_compare("COpa", "CHApa");

	test_do_compare("bc", "bch");
	test_do_compare("bd", "bch");
	test_do_compare("bca", "bch");
	test_do_compare("bda", "bch");

	// Test comparing words with 'll'
	test_do_compare("lora", "llora");
	test_do_compare("Lora", "Llora");
	test_do_compare("Lora", "LLora");
	test_do_compare("LOra", "LLOra");

	// Test accented vowels
	test_do_compare("término", "termino");
	test_do_compare("término", "terminó");
	test_do_compare("termino", "terminó");
	test_do_compare("AEIOUUaeiouu", "ÁÉÍÓÚÜáéíóúü");
	test_do_compare("aeiouuAEIOUU", "ÁÉÍÓÚÜáéíóúü");


	// Test transforming
	test_do_transform("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
}


void collate_es::test_do_compare(const string& str1, const string& str2) const
{
	const char* b1 = str1.data();
	const char* b2 = str2.data();
	int forward = do_compare(b1, b1 + str1.size(), b2, b2 + str2.size());
	int reverse = do_compare(b2, b2 + str2.size(), b1, b1 + str1.size());

	printf("The result of comparing %s and %s is\n", str1.c_str(), str2.c_str());
	printf("   Forward: %s %s %s\n", str1.c_str(), (forward == -1 ? "<" : (forward == 1 ? ">" : "=")), str2.c_str());
	printf("   Reverse: %s %s %s\n", str2.c_str(), (reverse == -1 ? "<" : (reverse == 1 ? ">" : "=")), str1.c_str());
}


void collate_es::test_do_transform(const std::string& str) const
{
	const char* b = str.data();
	string trans = do_transform(b, b + str.size());

	printf("The result of transforming\n   %s is\n   %s\n", str.c_str(), trans.c_str());
}



#ifndef NO_NAMESPACES
}
#endif

