#include "stdafx.h"
#include <string>
#include "UTF.h"

bool UTF::wchartoutf8(const wchar_t* in, std::string &out)
{
	out.clear();
	while (*in)
	{
		unsigned short muh = *in;
		if (!(muh & 0xFF80))
		{
			out += (char)muh;
		}
		else if (!(muh & 0xF800))
		{
			out += 0xC0 | (muh >> 6);
			out += 0x80 | (muh & 0x3F);
		}
		else
		{
			out += 0xE0 | (muh >> 12);
			out += 0x80 | ((muh >> 6) & 0x3F);
			out += 0x80 | (muh & 0x3F);
		}
		in++;
	}
	return true;
}

bool UTF::utf8towstring(const char* in, std::wstring &out)
{
	return utf8towstring((const unsigned char*) in, out);
}

bool UTF::utf8towstring(const unsigned char* in, std::wstring &out)
{
	out.clear();
	while (*in)
	{
		char muh = *in;
		if ((muh & 0xF0) == 0xE0)
		{
			char muh2 = *(in+1);
			if ((muh2 & 0xC0) != 0x80)
				return false;
			char muh3 = *(in+2);
			if ((muh3 & 0xC0) != 0x80)
				return false;
			out += ((muh & 0x0F) << 12) | ((muh2 & 0x3F) << 6) | (muh3 & 0x3F);
			in += 3;
		}
		else if ((muh & 0xE0) == 0xC0)
		{
			char muh2 = *(in+1);
			if ((muh2 & 0xC0) != 0x80)
				return false;
			out += ((muh & 0x1F) << 6) | (muh2 & 0x3F);
			in += 2;
		}
		else if ((muh & 0x80) == 0x00)
		{
			out += muh;
			in++;
		}
		else
			return false;
	}
	return true;
}
