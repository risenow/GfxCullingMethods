#pragma once
#include <string>
#include "typesalias.h"

template<class T>
bool IsPathToFile(const std::wstring& path)
{
    for (long i = path.size() - 1; i >= 0; i--)
    {
        if (path[i] == '/')
            return false;
        if (path[i] == '.')
            return true;
    }
    return false;
}

template<class T>
T ReducePathRank(const T& path, unsigned long rank)
{
    long lastSlashFoundIndex = -1;
    long lastSlashFoundRank = 0;
    for (long i = 0; (i < path.size()) && (lastSlashFoundRank < rank); i++)
    {
        if (path[i] == '/')
        {
            lastSlashFoundIndex = i;
            lastSlashFoundRank++;
        }
    }

    if (lastSlashFoundIndex != -1)
    {
        unsigned long beginIndex = lastSlashFoundIndex + 1;
        return path.substr(beginIndex, path.size() - beginIndex);
    }
    return path;
}
template<class T>
T ReducePathToRankFromStart(const T& path, unsigned long rank)
{
    long lastSlashFoundIndex = -1;
    long lastSlashFoundRank = 0;
    for (long i = 0; (i < path.size()) && (lastSlashFoundRank < rank); i++)
    {
        if (path[i] == '/')
        {
            lastSlashFoundIndex = i;
            lastSlashFoundRank++;
        }
    }

    if (lastSlashFoundIndex != -1)
        return path.substr(0, lastSlashFoundIndex);
    return path;
}

template<class T>
unsigned long GetPathRank(const T& path)
{
    unsigned long rank = 0;
    for (long i = 0; i < path.size(); i++)
    {
        if (path[i] == '/')
            rank++;
    }
    return rank;
}

template<class T>
T ExcludeFileFromPath(const T& path)
{
    if (!path.size())
        return path;
    T p = path;
    int i = p.size() - 1;
    while (i >= 0 && p[i] != '/') i--;
    
    if (i < 0)
        return path;

    return p.substr(0, i + 1);

    //return ReducePathRank(path, GetPathRank(path) - 1);
}

template<class T>
void NormalizePath(T& path)
{
    //if (!IsPathToFile(path) && path[path.size() - 1] != '/')
    for (unsigned long i = 0; i < path.size(); i++)
    {
        if (path[i] == '\\')
            path[i] = '/';
    }
}
//template<>
void NormalizePathPlain(wchar_t* path);

//template<class T>
//void NormalizePath(wchar_t* path);