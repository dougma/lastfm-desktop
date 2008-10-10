#pragma once

class UTF
{
public:
	static bool wchartoutf8(const wchar_t* in, std::string &out);
	static bool utf8towstring(const unsigned char* in, std::wstring &out);
	static bool utf8towstring(const char* in, std::wstring &out);
};
