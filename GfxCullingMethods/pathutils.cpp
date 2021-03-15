#include "pathutils.h"

std::string ExcludeFileFromPath(const std::string& path)
{
    return ReducePathRank(path, GetPathRank(path) - 1);
}

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
std::wstring ReducePathRank(const std::wstring& path, unsigned long rank)
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

std::wstring ReducePathToRankFromStart(const std::wstring& path, unsigned long rank)
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

unsigned long GetPathRank(const std::wstring& path)
{
	unsigned long rank = 0;
	for (long i = 0; i < path.size(); i++)
	{
		if (path[i] == '/')
			rank++;
	}
	return rank;
}

void NormalizePath(std::wstring& path)
{
	//if (!IsPathToFile(path) && path[path.size() - 1] != '/')
	for (unsigned long i = 0; i < path.size(); i++)
	{
		if (path[i] == '\\')
			path[i] = '/';
	}
}

void NormalizePathPlain(wchar_t* path)
{
	for (unsigned long i = 0; path[i] != '\0'; i++)
	{
		if (path[i] == '\\')
			path[i] = '/';
	}
}