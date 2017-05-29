/*
 ** $Id: charset-convert.c 1116 2010-12-02 04:03:35Z dongjunjie $
 **
 ** charset-convert.c: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: dongjunjie 
 **  
 ** Create date: 2010-06-09 
 */

static int single_retrieve_char(const char*word)
{
	return 1;
}


#if !defined(_MGNCS_USE_ICONV) || defined(WIN32)

#ifdef _MGCHARSET_UNICODE
#define MAX_ENCODING_SUPPORT   8
PLOGFONT  _encoding_fonts[MAX_ENCODING_SUPPORT];


static PLOGFONT find_matched_logfont(const char* charset, int (*cmp)(const char*,const char*))
{
    int i;
    PLOGFONT plogfont = NULL;
    if (charset == NULL)
        return NULL;

    for(i = 0; i < MAX_ENCODING_SUPPORT && _encoding_fonts[i]; i++)
    {
        if(cmp(_encoding_fonts[i]->charset, charset) == 0)
            return _encoding_fonts[i];
    }

    if( i >= MAX_ENCODING_SUPPORT)
        return NULL;

    //try find from sysfont
    for(i = 0; i < NR_SYSLOGFONTS; i ++)
    {
        if(cmp(g_SysLogFont[i]->charset, charset) == 0)
        {
            plogfont = g_SysLogFont[i];
            break;
        }
    }

    if(!plogfont) //try create one
    {
        plogfont = CreateLogFont (NULL, "", charset,
                  FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
                  FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
                  12, 0);
        if(plogfont && strcasecmp(charset, plogfont->charset)!= 0)
        {
            DestroyLogFont(plogfont);
            plogfont = NULL;
        }
    }

    if(plogfont)
    {
        //save the plogfont to array
        for(i = 0 ; i < MAX_ENCODING_SUPPORT && _encoding_fonts[i]; i ++);
        _encoding_fonts[i] = plogfont;
    }


    return plogfont;
}

#define LOWER(c)  (((c) >='A' && (c)<='Z')?((c)-'A' + 'a') : (c))
#define CASE_EQUAL(a,b)  ((a == b) || (LOWER(a) == LOWER(b)))
static inline int charset_closed_cmp(const char* charset_target, const char* charset)
{
    /*
     * fg. ISO8859 match ISO8859-2 , ISO8859-3, ....
     * fg. GB* match all gb start with GB18030, GB2312 and so on
     */
    
    while(*charset && *charset_target && CASE_EQUAL(*charset, *charset_target)) {
        charset ++;
        charset_target ++;
    }
    
    if(*charset == '*')
        return 0; //matched
    if(*charset == 0 && *charset_target == '-')
        return 0; //matched

    return *charset_target - *charset;
}

//try find list
static const char * try_find_list [] = {
    "GB18030",
    "GBK",
    "GB2312-0",
    "GB2312",
    NULL,
    NULL
};

const char** find_sub_charset(const char* charset, int *pat)
{
    int i = 0;
    int begin = 0;
    for(i = 0; i < sizeof(try_find_list)/sizeof(char*); i ++)
    {
        if(try_find_list[i] == NULL)
        {
            begin = i + 1;
            continue;
        }

        if(strcasecmp(charset, try_find_list[i]) == 0)
        {
            break;
        }
    }

    if(pat)
        *pat = i - begin;
    return begin >= sizeof(try_find_list)/sizeof(char*) ? NULL : &try_find_list[begin];
}

static PLOGFONT try_find_closed_font(const char* charset)
{
    //find a list in try list
    int charset_at = 0;
    PLOGFONT plogfont;
    const char**  try_at = find_sub_charset(charset, &charset_at);
    if(try_at)
    {
        int i; 
        for( i = 0 ; try_at[i]; i ++)
        {
            if ( i != charset_at) 
            {
                if ((plogfont = find_matched_logfont(try_at[i], strcasecmp)))
                    return plogfont;
            }
        }
    }

    return find_matched_logfont(charset, charset_closed_cmp);
}

static PLOGFONT find_logfont_by_charset(const char* charset)
{

    PLOGFONT plogfont ;
    BOOL     closed_cmp = FALSE;
    //int i;

    if(!charset)
        return NULL;

    closed_cmp = strrchr(charset, '*') != NULL;

    plogfont = find_matched_logfont(charset, closed_cmp?charset_closed_cmp:strcasecmp);

    if(!plogfont && !closed_cmp) //try find a closed charset font
        plogfont = try_find_closed_font(charset);
    

    return plogfont;
}

/*static void return_encoding_logfont(PLOGFONT plogfont)
{
    //Do Nothing, only a interface
}*/
#endif

#define SET_FROM_TO_FONTS(converter, pfromfont, ptofont) do { \
    (converter)->conv_info = (void*)(ptofont); \
    (converter)->reserved = (void*)(pfromfont); \
}while(0)
#define GET_FROM_TO_FONTS(converter, pfromfont, ptofont) do { \
    pfromfont = (PLOGFONT)((converter)->reserved); \
    ptofont   = (PLOGFONT)((converter)->conv_info); \
}while(0)
/////
BOOL ncsIMInitCharsetConv(CHARSET_CONVERTER* converter, const char* from_charset, const char* to_charset, NCSCB_RETRIEVE_CHAR retrieve_char)
{
    PLOGFONT pfromfont = NULL, ptofont = NULL;
    if(!converter || !from_charset || !to_charset || !retrieve_char)
        return FALSE;
    //converter init
    memset(converter, 0, sizeof(CHARSET_CONVERTER));

    converter->retrieve_char = retrieve_char;

    if (strcasecmp(from_charset, to_charset) == 0)
    {
        return TRUE;
    }

#ifdef _MGCHARSET_UNICODE
    //from font
    if (strcasecmp(from_charset, "unicode"))
    {
        pfromfont = find_logfont_by_charset(from_charset);
        if(!pfromfont)
            return FALSE;
    }
    //to logfont
    if (strcasecmp(to_charset, "unicode"))
    {
        ptofont = find_logfont_by_charset(to_charset);
        if(!ptofont)
            return FALSE;
    }

    SET_FROM_TO_FONTS(converter, pfromfont, ptofont);
#else
    SET_FROM_TO_FONTS(converter, NULL, NULL);
#endif
    return TRUE;
}

void ncsIMFreeCharsetConv(CHARSET_CONVERTER *conv)
{
    if(conv)
        SET_FROM_TO_FONTS(conv, NULL, NULL);
}

unsigned int ncsIMCharsetConv(CHARSET_CONVERTER* conv, const char ** pwords)
{
#ifdef _MGCHARSET_UNICODE
    PLOGFONT pfromfont, ptofont;
#endif

    if(!pwords || !pwords[0])
        return 0;
    
#ifdef _MGCHARSET_UNICODE
    GET_FROM_TO_FONTS(conv, pfromfont, ptofont); 
    if(!pfromfont && !ptofont)
#endif
    {
        int ch = pwords[0][0];
        (*pwords) ++;
        return ch;
    }
#ifdef _MGCHARSET_UNICODE
    else
    {
        int ch_number = conv->retrieve_char?conv->retrieve_char(pwords[0]):1;
        UChar32 wc;
        unsigned char mb[sizeof(int)+1];
        int mb_len = 0;
        unsigned int dst_char;
        int i;
        //from Mutil-byte->Uincode byte
        if (pfromfont)
            ch_number = MB2WCEx(pfromfont, (void*)&wc, TRUE, (unsigned char*)pwords[0], ch_number);
        else
            wc = (UChar32)(*((wchar_t*)pwords[0]));
    

        //Uncode byte -> To Mutil-byte
        if (ptofont)
            mb_len = WC2MBEx(ptofont, mb, wc);
        else
            return wc; //return unicode directly
        
        if(mb_len <= 0)
            return 0;

        dst_char = mb[0];
        for (i = 1; i < mb_len; i ++)
        {
            dst_char |= mb[i] << (i*8);
        }
        return dst_char;
    }
#endif
}

char * ncsIMConvertWord(CHARSET_CONVERTER* conv, const char* word, char *out, int len)
{
    if(!word || !out || len <= 0)
        return NULL;

#ifdef _MGCHARSET_UNICODE
    if(conv == NULL || (conv->conv_info == NULL && conv->reserved == NULL))
#endif
    {
        strncpy(out, word, len);
        out[len] = 0;
        return out;
    }
#ifdef _MGCHARSET_UNICODE
    else
    {
        PLOGFONT pfromfont, ptofont;
        GET_FROM_TO_FONTS(conv, pfromfont, ptofont);

        if (!ptofont) //multi-byte to uincode
        {
            int len = 0;
            len = MBS2WCS(pfromfont, (void*)out, (unsigned char*)word, len, len); 
            ((wchar_t*)out)[len] = 0;
        }
        else if (!pfromfont) //uincode to multi-byte
        {
            int len = 0;
            len = WCS2MBS(ptofont, (unsigned char*)out, (void*)word, len, len * 4); //FIXEDME: should pass the buffer size of out
            out[len] = 0;
        }
        else
        {
            //MultiByte-UincodeByte-MultiByte
            int i = 0;
            int j = 0;
            UChar32 wc;

            while (i < len && word[i])
            {
                int mlen;
                i += MB2WCEx(pfromfont, (void*)&wc, TRUE, (unsigned char*)(word + i), len - i);
                mlen = WC2MBEx(ptofont, (unsigned char*)&out[j], wc);
                if(mlen <= 0)
                    break;
                j += mlen;
            }
            out[j] = 0;
        }
        return out;
    }
#endif
}

static int sb_len_first_char (const unsigned char* mstr, int len)
{
    if (len < 1) return 0;
    if (*mstr != '\0')  return 1;
    return 0;
}

static int unicode_len_first_char (const unsigned char* mstr, int len)
{
    if (len < 1) return 0;
    if (*((wchar_t*)mstr) != 0)  return sizeof(wchar_t);
    return 0;
}

//FIXEDME: I must get CHARSETOPS' len_first_char's pointer
/** The character set operation structure. */
typedef struct _CHARSETOPS_INFO
{
	/** The character number of the character set. */
    int nr_chars;

	/** The byte number of the max length character. */
    int bytes_maxlen_char;

	/** The name of the character set. */
    const char* name;

	/** Default character. */
    Glyph32 def_glyph_value;

	/** The method to get the length of the first character function. */
    int (*len_first_char) (const unsigned char* mstr, int mstrlen);
}CHARSETOPS_INFO;

static inline NCSCB_LEN_FIRST_CHAR get_lenfirstchar_callback_from_logfont(PLOGFONT plogfont)
{
    if(plogfont->mbc_devfont 
            && plogfont->mbc_devfont->charset_ops
            && ((CHARSETOPS_INFO*)(plogfont->mbc_devfont->charset_ops))->len_first_char)
        return ((CHARSETOPS_INFO*)(plogfont->mbc_devfont->charset_ops))->len_first_char;

    return plogfont->sbc_devfont 
            ? (plogfont->sbc_devfont->charset_ops 
                ? ((CHARSETOPS_INFO*)(plogfont->mbc_devfont->charset_ops))->len_first_char
                : NULL ) 
            : NULL;
}


NCSCB_LEN_FIRST_CHAR ncsIMGetFirstCharCallback(const char* encoding)
{
    PLOGFONT plogfont;
    if(!encoding)
        return sb_len_first_char;
    if(strcasecmp(encoding, "unicode") == 0)
        return unicode_len_first_char;

    plogfont = find_logfont_by_charset(encoding);
    if(!plogfont)
        return sb_len_first_char;
    
    //return the charset_ops's len_first_char
    return get_lenfirstchar_callback_from_logfont(plogfont);
}

#else
#include <iconv.h>

BOOL ncsIMInitCharsetConv(CHARSET_CONVERTER* converter, const char* from_charset, const char* to_charset, NCSCB_RETRIEVE_CHAR retrieve_char)
{
	//converter init
	memset(converter, 0, sizeof(CHARSET_CONVERTER));
	iconv_t cb = iconv_open(to_charset, from_charset);
	if(!cb)
		return FALSE;

	converter->conv_info = (void*)cb;
	converter->retrieve_char = retrieve_char;

	if(converter->retrieve_char == NULL)
		converter->retrieve_char = single_retrieve_char;
	return TRUE;
}

void ncsIMFreeCharsetConv(CHARSET_CONVERTER* conv)
{
	if(conv)
	{
		iconv_close(conv->conv_info);
	}
}

unsigned int ncsIMCharsetConv(CHARSET_CONVERTER* conv, const char ** pwords)
{
	if(!pwords || !pwords[0])
		return 0;
	if(conv && conv->conv_info)
	{
		int ch_number = conv->retrieve_char?conv->retrieve_char(pwords[0]):1;
		unsigned char word[sizeof(int)];
		unsigned char *pword = word;
		unsigned char** word_out = &pword;
		int word_len = sizeof(word);
		int in_len = ch_number;
		int i;
		unsigned int dst_char;
		memset(word, 0, sizeof(word));
		int len = iconv((iconv_t)(conv->conv_info), pwords, &in_len,word_out, &word_len);
		if(in_len > 0)
		{
			//(*pwords) += ch_number;
			return 0;
		}

		word_len = sizeof(word) - word_len;

		dst_char =  word[0];
		for(i=1; i < word_len; i++)
		{
			dst_char |= word[i]<<(i*8);
		}
		//(*pwords) += ch_number;
		return dst_char;
	}
	else
	{
		int ch = pwords[0][0];
		(*pwords) ++;
		return ch;
	}
	return 0;
}


char*  ncsIMConvertWord(CHARSET_CONVERTER* conv, const char* word, char* out, int len)
{
	if(!word || !out || len <= 0)
		return NULL;

	if(conv == NULL || conv->conv_info == NULL)
	{
		strncpy(out, word, len);
		out[len] = 0;
		return out;
	}
	else
	{
		char * tmp_word = word;
		char * tmp_out = out;
		char** in_word = &tmp_word;
		int    in_left = strlen(word);
		char** out_word = &tmp_out;
		int    out_left = len;

		iconv((iconv_t)(conv->conv_info), in_word, &in_left, out_word, &out_left);
		len -= out_left;
		if(len > 0)
			out[len] = 0;
		return out;
	}
}


////////////////////////////////////////////////////////////

static int sb_len_first_char (const unsigned char* mstr, int len)
{
    if (len < 1) return 0;
    if (*mstr != '\0')  return 1;
    return 0;
}

#define IS_GB2312_CHAR(ch1, ch2) \
        if (((ch1 >= 0xA1 && ch1 <= 0xA9) || (ch1 >= 0xB0 && ch1 <= 0xF7)) \
                        && ch2 >= 0xA1 && ch2 <= 0xFE)

static int gb2312_0_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    IS_GB2312_CHAR (ch1, ch2)
        return 2;

    return 0;
}

#define IS_GBK_CHAR(ch1, ch2) \
    if (ch1 >= 0x81 && ch1 <= 0xFE && ch2 >= 0x40 && ch2 <= 0xFE && ch2 != 0x7F)

static int gbk_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    IS_GBK_CHAR(ch1, ch2)
        return 2;

    return 0;
}

static int gb18030_0_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;
    unsigned char ch3;
    unsigned char ch4;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    if (ch1 >= 0x81 && ch1 <= 0xFE && ch2 >= 0x40 && ch2 <= 0xFE && ch2 != 0x7F)
        return 2;

    if (len < 4) return 0;

    ch3 = mstr [2];
    ch4 = mstr [3];
    if (ch2 >= 0x30 && ch2 <= 0x39 && ch4 >= 0x30 && ch4 <= 0x39
            && ch1 >= 0x81 && ch1 <= 0xFE && ch3 >= 0x81 && ch3 <= 0xFE)
        return 4;

    return 0;
}

static int big5_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    if (ch1 >= 0xA1 && ch1 <= 0xFE &&
            ((ch2 >=0x40 && ch2 <= 0x7E) || (ch2 >= 0xA1 && ch2 <= 0xFE)))
        return 2;

    return 0;
}

static int utf8_len_first_char (const unsigned char* mstr, int len)
{
    int t, c = *((unsigned char *)(mstr++));
    int n = 1, ch_len = 0;

    /*for ascii character*/
    if (c < 0x80) {
        return 1;
    }

    if (c & 0x80) {
        while (c & (0x80 >> n))
            n++;

        if (n > len)
            return 0;

        ch_len = n;
        while (--n > 0) {
            t = *((unsigned char *)(mstr++));

            if ((!(t & 0x80)) || (t & 0x40))
                return 0;
        }
    }

    return ch_len;
}

static int utf16le_len_first_char (const unsigned char* mstr, int len)
{
    UChar16 w1, w2;

    if (len < 2)
        return 0;

    w1 = MAKEWORD (mstr[0], mstr[1]);

    if (w1 < 0xD800 || w1 > 0xDFFF)
        return 2;

    if (w1 >= 0xD800 && w1 <= 0xDBFF) {
        if (len < 4)
            return 0;
        w2 = MAKEWORD (mstr[2], mstr[3]);
        if (w2 < 0xDC00 || w2 > 0xDFFF)
            return 0;
    }

    return 4;
}

static int utf16be_len_first_char (const unsigned char* mstr, int len)
{
    UChar16 w1, w2;

    if (len < 2)
        return 0;

    w1 = MAKEWORD (mstr[1], mstr[0]);

    if (w1 < 0xD800 || w1 > 0xDFFF)
        return 2;

    if (w1 >= 0xD800 && w1 <= 0xDBFF) {
        if (len < 4)
            return 0;
        w2 = MAKEWORD (mstr[3], mstr[2]);
        if (w2 < 0xDC00 || w2 > 0xDFFF)
            return 0;
    }

    return 4;
}

#define _C(encoding, func)  \
	{encoding, (NCSCB_LEN_FIRST_CHAR)(func)}

struct {
	const char* encoding;
	NCSCB_LEN_FIRST_CHAR len_first_char;
} _ime_charset_char_infos[] =
{
	_C("GB2312",gb2312_0_len_first_char),
	_C("GBK", gbk_len_first_char),
	_C("GB18030", gb18030_0_len_first_char),
	_C("BIG5", big5_len_first_char),
	_C("UTF-8", utf8_len_first_char),
	_C("UTF-16LE", utf16le_len_first_char),
	_C("UTF-16BE", utf16be_len_first_char),
	{NULL, NULL}
};

#undef _C
NCSCB_LEN_FIRST_CHAR ncsIMGetFirstCharCallback(const char* encoding)
{
	int i;
	if(!encoding)
		return sb_len_first_char;
	for( i= 0; _ime_charset_char_infos[i].encoding; i++)
	{
		if(strcasecmp(encoding, _ime_charset_char_infos[i].encoding) == 0)
			return _ime_charset_char_infos[i].len_first_char;
	}
	return sb_len_first_char;
}
#endif
