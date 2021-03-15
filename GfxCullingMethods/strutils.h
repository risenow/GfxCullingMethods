#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <codecvt>
#include <algorithm>
#include <assert.h>

std::wstring strtowstr_fast(const std::string& str); 
std::string wstrtostr_fast(const std::wstring& wstr);

std::wstring strtowstr(const std::string& str);
std::string wstrtostr(const std::wstring& wstr);

bool StrIsRealNumber(const std::string& str);
bool StrIsInt(const std::string& str);
bool StrIsUnsignedInt(const std::string& str);

typedef char* cptr;
typedef wchar_t* wcptr;

template<class T>
void ToString(const T& in, std::string& out)
{
    out = std::to_string(in);
}
template<>
void ToString(const std::wstring& in, std::string& out);
template<>
void ToString(const std::string& in, std::string& out);
//template<>
//void ToString(const wcptr in, std::string& out);

template<class T>
void ToString(const T& in, std::wstring& out)
{
    out = std::to_wstring(in);
}
template<>
void ToString(const std::string& in, std::wstring& out);
template<>
void ToString(const std::wstring& in, std::wstring& out);
//template<>
//void ToString(const cptr in, std::wstring& out);

template<class T>
std::wstring GetWideString(const T& str)
{
    return str.c_str();
}
template<>
std::wstring GetWideString(const wcptr& str);
template<>
std::wstring GetWideString(const std::string& str);
template<>
std::wstring GetWideString(const cptr& str);

template<class T>
cptr GetStringContent(const T& str)
{
    return (cptr)str.c_str();
}
template<> 
cptr GetStringContent(const cptr& str);

template<class T>
void WriteToConsole(const T& str)
{
    std::cout << GetStringContent(str);
}
template<> 
void WriteToConsole(const std::wstring& str);

template<class T>
bool IsSpace(T c) // T == char
{
    return c == ' ';
}
template<>
bool IsSpace(wchar_t c); // T == wchar

template<class T>
void EraseAllSpacesFromString(T& str)
{
    str.erase(std::remove_if(str.begin(), str.end(), IsSpace<T::value_type>), str.end());
}

void ExtractWords(const std::string& src, std::vector<std::string>& words);
void FirstWord(const std::string& src, std::string& word);