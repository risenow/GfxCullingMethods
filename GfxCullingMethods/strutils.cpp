#include "strutils.h"

std::wstring strtowstr_fast(const std::string& str)
{
    std::wstring wstr = std::wstring(str.begin(), str.end());
    return wstr;
}
std::string wstrtostr_fast(const std::wstring& wstr)
{
    std::string str = std::string(wstr.begin(), wstr.end());
    return str;
}

std::wstring strtowstr(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wstr = converter.from_bytes(str);
    return wstr;
}
std::string wstrtostr(const std::wstring& wstr)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string str = converter.to_bytes(wstr);
    return str;
}

bool StrIsRealNumber(const std::string& str)
{
    return (!str.empty() && str.find_first_not_of("-0123456789.") == std::string::npos);
}
bool StrIsInt(const std::string& str)
{
    return (!str.empty() && str.find_first_not_of("-0123456789") == std::string::npos);
}
bool StrIsUnsignedInt(const std::string& str)
{
    return (!str.empty() && str.find_first_not_of("0123456789") == std::string::npos);
}

template<>
void ToString(const std::wstring& in, std::string& out)
{
    out = wstrtostr_fast(in);
}
template<>
void ToString(const std::string& in, std::string& out)
{
    out = in;
}

template<>
void ToString(const std::string& in, std::wstring& out)
{
    out = strtowstr_fast(in);
}
template<>
void ToString(const std::wstring& in, std::wstring& out)
{
    out = in;
}

template<>
std::wstring GetWideString(const wcptr& str)
{
    return std::wstring(str);
}
template<>
std::wstring GetWideString(const std::string& str)
{
    return strtowstr_fast(str);
}
template<>
std::wstring GetWideString(const cptr& str)
{
    return strtowstr_fast(std::string(str));
}
/*template<>
std::wstring GetWideString(wcptr str)
{
    return std::wstring(str);
}
template<>
std::wstring GetWideString(cptr str)
{
    return strtowstr(std::string(str));
}*/

template<>
cptr GetStringContent(const cptr& str)
{
    return (cptr)str;
}

void ExtractWords(const std::string& src, std::vector<std::string>& words)
{
    int pl = 1;
    size_t i = 0;
    while (pl && src[i])
    {
        size_t cbi = i;
        while (src[i] && src[i] != ' ') i++;
        pl = src[i];
        words.push_back(src.substr(cbi, i - cbi));
        i++;
    }
}

void FirstWord(const std::string& src, std::string& word)
{
    assert(src.size() != 0);
    int pl = 1;
    size_t i = 0;
    //while (pl && src[i])
    //{
    size_t cbi = i;
    while (src[i] && src[i] != ' ') i++;
    pl = src[i];
    word = src.substr(cbi, i - cbi);
    return;
    //words.push_back();
  //  i++;
//}
}

template<>
void WriteToConsole(const std::wstring& str)
{
    std::wcout << str;
}

template<>
bool IsSpace(wchar_t c) // T== wchar
{
    return c == L' ';
}

