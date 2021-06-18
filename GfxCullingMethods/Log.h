#pragma once
#include "strutils.h"
#include "timeutils.h"
#include <fstream>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>

constexpr char LOG_FILE_OUTPUT_NAME[] = "log.txt";

enum RuntimeLogModeMasks
{
	RuntimeLogMode_None = 0u,
    RuntimeLogMode_WriteTime = 1u << 1,
    RuntimeLogMode_WriteLine = 1u << 2
};
typedef unsigned int RuntimeLogMode;

class RuntimeLogStreamBase
{
public:
	virtual void Write(const std::wstring& str) = 0;
	virtual ~RuntimeLogStreamBase() {}
	virtual bool IsValid() { return true; }
};

template<class S> // S = stream with << operator for writing
class RuntimeLogStream : public RuntimeLogStreamBase
{
public:
	RuntimeLogStream() : m_Stream(nullptr) {}
	~RuntimeLogStream()
	{}
	virtual void Write(const std::wstring& str) override
	{
		(*m_Stream) << str;
	}

	S* GetStream() const
	{
		return m_Stream;
	}
	void SetStream(S* stream)
	{
		m_Stream = stream;
	}

	virtual bool IsValid() override { return m_Stream != nullptr; }

private:
	S* m_Stream;
};

class StringRuntimeLogStream : public RuntimeLogStreamBase
{
public:
	StringRuntimeLogStream() : m_DestString(nullptr) {}
	StringRuntimeLogStream(std::wstring* dest) : m_DestString(dest) {}
	~StringRuntimeLogStream() {}

	void SetDestString(std::wstring* dest)
	{
		m_DestString = dest;
	}

	virtual void Write(const std::wstring& str) override
	{
		(*m_DestString) += str;
	}

	virtual bool IsValid() override { return m_DestString != nullptr; }
private:
	std::wstring* m_DestString;
};

class FileRuntimeLogStream : public RuntimeLogStreamBase, public std::wofstream
{
public:
	FileRuntimeLogStream() {}
	~FileRuntimeLogStream() { close(); }

	virtual void Write(const std::wstring& str) override
	{
		//(*this) << unitbuf; wtf?
		(*this) << str;
		flush();
	}

	virtual bool IsValid() override { return is_open(); }
};

class RuntimeLog
{
public:
	typedef RuntimeLogStream<std::wostream>      DefaultConsoleStreamType;
	typedef FileRuntimeLogStream                 DefaultFileStreamType;
	typedef std::vector<RuntimeLogStreamBase*>   DefaultStreamsSetType;

    static RuntimeLog& GetInstance();

	template<class T>
	void WriteUsingMode(const std::vector<RuntimeLogStreamBase*> streams, const T& str, RuntimeLogMode mode)
	{
		if (!m_Enabled)
			return;

        std::wstring compiledStr = GetWideString(str);
		CompileStr(compiledStr, mode);
		
		for (RuntimeLogStreamBase* stream : streams)
		{
			if (stream->IsValid())
				stream->Write(compiledStr);
		}
	}

	//do we need default mode(m_Mode) after refactor?
	//void Write(const std::vector<RuntimeLogStreamBase*> streams, const T& str) 
    
	RuntimeLogMode GetMode() const
	{
		return m_Mode;
	}
    void SetMode(RuntimeLogMode mode);
    void SetEnabled(bool enabled);

    bool GetEnabled();

	DefaultFileStreamType& GetDefaultFileStream()
	{
		if (!m_DefaultFileStream.is_open())
		{
			m_DefaultFileStream.open("testlog.txt");
		}
		return m_DefaultFileStream;
	}
	DefaultConsoleStreamType& GetDefaultConsoleStream()
	{
		if (m_DefaultConsoleStream.GetStream() == nullptr)
		{
			m_DefaultConsoleStream.SetStream(&std::wcout);
		}

		return m_DefaultConsoleStream;
	}

	std::vector<RuntimeLogStreamBase*>& GetDefaultRuntimeLogStreamsSet()
	{
		return m_DefaultStreamsSet;
	}
private:
    RuntimeLog();
    ~RuntimeLog();

    template<class T>
    void CompileStr(T& str, RuntimeLogMode mode) // T == string/wstring
    {
        if (mode & RuntimeLogMode_WriteTime)
        {
            T timeValueString;
            ToString(GetMilisecondsSinceProgramStart(), timeValueString);
            T timeSectionBegin;
            ToString(std::string("[TIME:"), timeSectionBegin);
            T timeSectionEnd;
            ToString(std::string("] "), timeSectionEnd);
            str = timeSectionBegin + timeValueString + timeSectionEnd + str;
        }
        if (mode & RuntimeLogMode_WriteLine)
        {
            T endlStr;
            ToString(std::string("\n"), endlStr);
            str = str + endlStr;
        }
    }
    bool m_Enabled;
    RuntimeLogMode m_Mode;

	//
	DefaultFileStreamType    m_DefaultFileStream;
	DefaultConsoleStreamType m_DefaultConsoleStream;
	DefaultStreamsSetType    m_DefaultStreamsSet;
};

#define LOG_CONSOLE_STREAM() RuntimeLog::GetInstance().GetDefaultConsoleStream()
#define LOG_FILE_STREAM()    RuntimeLog::GetInstance().GetDefaultFileStream()

#define popGetLogger() (RuntimeLog::GetInstance())
#define LOG_BUFFER(str, dest) { StringRuntimeLogStream tempStringStream(&dest); RuntimeLog::GetInstance().WriteUsingMode( { &tempStringStream }, str, RuntimeLogMode_WriteTime | RuntimeLogMode_WriteLine);}
#define LOG(str) (RuntimeLog::GetInstance().WriteUsingMode(RuntimeLog::GetInstance().GetDefaultRuntimeLogStreamsSet(), str, RuntimeLog::GetInstance().GetMode()))
#define LOG_CONS(str) (RuntimeLog::GetInstance().WriteUsingMode({ &LOG_CONSOLE_STREAM() }, str, RuntimeLogMode_WriteTime | RuntimeLogMode_WriteLine))
#define LOG_FILE(str) (RuntimeLog::GetInstance().WriteUsingMode({ &LOG_FILE_STREAM() }, str, RuntimeLogMode_WriteTime | RuntimeLogMode_WriteLine))
