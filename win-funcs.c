///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <windows.h>
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <io.h>

#define LOWCASE(ch)   (((ch)>='A' && (ch)<='Z')?(ch)-'A'+'a':(ch))

static int mystrcasecmp(const char* s1, const char* s2)
{
	if(s1 == NULL && s2 == NULL)
		return 0;
	if(s1 == NULL)
		return -1;
	if(s2 == NULL)
		return 1;
	while(*s1 && *s2 &&  LOWCASE(*s1) == LOWCASE(*s2)){
		s1 ++;
		s2 ++;
	}

	if(!*s1 && !*s2)
		return 0;
	if(!*s1)
		return -1;
	if(!*s2)
		return 1;
	if(LOWCASE(*s1) > LOWCASE(*s2))
		return 1;
	return -1;
}

void win_truncate_file(FILE *fp, int size)
{
	int fd = _fileno(fp);
	HANDLE hFile = (HANDLE)_get_osfhandle(fd);

	fseek(fp, size, SEEK_SET);
	SetEndOfFile(hFile);
}

//////////////////////////////////////////////////////
//convert
static int get_codepage(const char* charset)
{
	if(charset == NULL)
		return CP_ACP;
	if(mystrcasecmp(charset,"utf-8") == 0)
		return CP_UTF8;
	else if(mystrcasecmp(charset, "unicode") == 0)
		return -1; //-1 is unicode
	
	return CP_ACP;
}

static int Utf8ToUnicode(const char* in_words, char* out_words, int in_len, int out_len)
{
	return MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED,(LPCTSTR)in_words, in_len, (LPWSTR)out_words, out_len);
}

static int UnicodeToUtf8(const char* in_words, char* out_words, int in_len, int out_len)
{
	return WideCharToMultiByte(CP_UTF8, MB_PRECOMPOSED, (LPCWSTR)in_words, in_len, (LPTSTR)out_words, out_len,NULL,NULL);
}

static int AcpToUtf8(const char* in_words, char* out_words, int in_len, int out_len)
{
	wchar_t szBuffer[1024];
	int len =  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,(LPCTSTR)in_words, in_len, (LPCTSTR)szBuffer, out_len);
	if(len <= 0)
		return 0;
	return WideCharToMultiByte(CP_UTF8, MB_PRECOMPOSED, (LPCWSTR)szBuffer, len, (LPTSTR)out_words, out_len,NULL,NULL);
}

static int Utf8ToAcp(const char* in_words, char* out_words, int in_len, int out_len)
{
	wchar_t szBuffer[1024];
	int len =  MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED,(LPCTSTR)in_words, in_len, (LPCTSTR)szBuffer, out_len);
	if(len <= 0)
		return 0;
	return WideCharToMultiByte(CP_ACP, MB_PRECOMPOSED, (LPCWSTR)szBuffer, len, (LPTSTR)out_words, out_len,NULL,NULL);
}

static int AcpToUnicode(const char* in_words, char* out_words, int in_len, int out_len)
{
	return MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,(LPCTSTR)in_words, in_len, (LPCTSTR)out_words, out_len);
}

static int UnicodeToAcp(const char* in_words, char* out_words, int in_len, int out_len)
{
	return WideCharToMultiByte(CP_ACP, MB_PRECOMPOSED, (LPCWSTR)in_words, in_len, (LPTSTR)out_words, out_len,NULL,NULL);
}

static int no_trans(const char* in_words, char* out_words, int in_len, int out_len)
{
	int len = (in_len > out_len ? out_len : in_len) -1;
	strncpy(out_words, in_words, len);
	out_words[len] = 0;
	return len;
}

void* win_get_convert_cb(const char* from_charset, const char* to_charset)
{
	int cp_from, cp_to;
	cp_from = get_codepage(from_charset);
	cp_to   = get_codepage(to_charset);

	if(cp_from == cp_to)
		return no_trans;

	if(cp_from == CP_UTF8)
	{
		if(cp_to == CP_ACP)
			return Utf8ToAcp;
		else if(cp_to == -1) //unicode
			return Utf8ToUnicode;
	}
	else if(cp_from == -1)//unicode
	{
		if(cp_to == CP_ACP)
			return UnicodeToAcp;
		else if(cp_to == CP_UTF8)
			return UnicodeToUtf8;
	}
	else if(cp_from == CP_ACP)
	{
		if(cp_to == CP_UTF8)
			return AcpToUtf8;
		else if(cp_to == -1)
			return AcpToUnicode;
	}

	return no_trans;
}

#endif