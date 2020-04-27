#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <windows.h>

using namespace std;
using namespace ATL;

//重要等级排序：Debug < Info < Error < Crit

#define ErrLog(fmt, ...)	do \
{\
	WriteLog(LEVEL_ERROR, fmt, __VA_ARGS__);\
} while (0)

#define InfoLog(fmt, ...)	do \
{\
	WriteLog(LEVEL_INFO, fmt, __VA_ARGS__);\
} while (0)

#define CritLog(fmt, ...)	do \
{\
	WriteLog(LEVEL_CRITICAL, fmt, __VA_ARGS__);\
} while (0)

#define DebugLog(fmt, ...)	do \
{\
	WriteLog(LEVEL_DEBUG, fmt, __VA_ARGS__);\
} while (0)


#if defined(UNICODE) || defined(_UNICODE)
#define _tstring wstring
#else
#define _tstring string
#endif

typedef map<_tstring, _tstring> mapLogParam;

//日志等级，可以逻辑或使用
enum
{
	LEVEL_NOLOG = 0,
	LEVEL_DEBUG = 1,
	LEVEL_INFO = 1<<1,
	LEVEL_ERROR = 1<<2,
	LEVEL_CRITICAL = 1<<3,

	LEVEL_ALL = LEVEL_DEBUG|LEVEL_INFO|LEVEL_ERROR|LEVEL_CRITICAL
};

//记录的载体类型
enum
{
	TYPE_NONE = 0,
	TYPE_FILE = 1,
	TYPE_DEBUGER = 1<<1,
	TYPE_PIPE = 1<<2,
	TYPE_MEMORY = 1<<3, 

	TYPE_ALL = TYPE_FILE|TYPE_DEBUGER|TYPE_PIPE|TYPE_MEMORY
};

void WriteLog(DWORD dwLogLV, LPCTSTR lpszFormat, ...);
void ChangeLogLevel(DWORD dwLoglv);
void ChangeLogType(DWORD dwLogType);

struct LogConfig
{
public:

	LogConfig(DWORD dwLogLV = LEVEL_NOLOG, DWORD dwLogType = TYPE_NONE)
	{
		m_dwLogLevel = dwLogLV;
		m_dwLogType = dwLogType;
	}

	inline void SetLogParam(_tstring key, _tstring val)
	{
		//m_mapLogParam.insert(mapLogParam::value_type(key, val));  //这种方式无法覆盖原有值
		m_mapLogParam[key] = val;
	}

	inline void GetLogParam(_tstring key, _tstring &val)
	{
		val = m_mapLogParam[key];
	}

	inline void SetLogFilePath(_tstring strFilePath)
	{
		SetLogParam(_T("logfile"), strFilePath);
	}

	inline void GetLogFilePath(_tstring &strFilePath)
	{
		GetLogParam(_T("logfile"), strFilePath);
	}

	inline void SetLogPipeName(_tstring strPipeName)
	{
		SetLogParam(_T("logpipe"), strPipeName);
	}

	inline void GetLogPipeName(_tstring &strPipeName)
	{
		GetLogParam(_T("logpipe"), strPipeName);
	}

	inline void SetLogMemoryName(_tstring strMemoryName)
	{
		SetLogParam(_T("logmemory"), strMemoryName);
	}

	inline void GetLogMemoryName(_tstring &strMemoryName)
	{
		GetLogParam(_T("logmemory"), strMemoryName);
	}

	DWORD m_dwLogLevel;
	DWORD m_dwLogType;

private:
	mapLogParam m_mapLogParam;
};

class LogProvider
{
public:
	virtual BOOL Initialize(LogConfig*)	
	{
		return TRUE;
	}
	virtual inline BOOL Write(LPCTSTR szbuf, size_t buflen)
	{
		return TRUE;
	}
	virtual void Close(){}
	LogProvider(DWORD dwLogType)
	{
		m_dwLogType = dwLogType;
	}
	virtual ~LogProvider(){}

	inline DWORD GetLogType()
	{
		return m_dwLogType;
	}

protected:
	DWORD m_dwLogType;
};

class FileLogProvider : public LogProvider
{
public:
	virtual BOOL Initialize(LogConfig*);
	virtual inline BOOL Write(LPCTSTR szbuf, size_t buflen);
	virtual void Close();
	FileLogProvider(DWORD dwLogType);
	virtual ~FileLogProvider(){}
protected:
	FILE* m_File;
}; 

class DebugerLogProvider : public LogProvider
{
public:
	virtual BOOL Initialize(LogConfig*);
	virtual inline BOOL Write(LPCTSTR szbuf, size_t buflen);
	virtual void Close();
	DebugerLogProvider(DWORD dwLogType);
	virtual ~DebugerLogProvider(){}
}; 

class PipeLogProvider : public LogProvider
{
public:
	virtual BOOL Initialize(LogConfig*);
	virtual inline BOOL Write(LPCTSTR szbuf, size_t buflen);
	virtual void Close();
	PipeLogProvider(DWORD dwLogType);
	virtual ~PipeLogProvider(){}

protected:
	HANDLE m_hpipe;
}; 

class MemoryLogProvider : public LogProvider
{
public:
	virtual BOOL Initialize(LogConfig*);
	virtual inline BOOL Write(LPCTSTR szbuf, size_t buflen);
	virtual void Close();
	MemoryLogProvider(DWORD dwLogType);
	virtual ~MemoryLogProvider(){}

protected:
	HANDLE m_hFileMapping;
	LPVOID m_lpMapPoint;

	char* m_cBuf;
	UINT* m_puiOffset;
	BOOL* m_pbLoop;

	CRITICAL_SECTION m_cs;
};

class Loger
{
public:
	Loger();
	~Loger();

	void Initialize();
	void ChangeLogLevel(DWORD dwLoglv);
	void ChangeLogType(DWORD dwLogType);
	void SetLogFilePath(LPCTSTR lpszFilePath);
	void SetLogPipeName(LPCTSTR lpszPipeName);
	void SetLogMemoryName(LPCTSTR lpszLogMemoryName);
	inline void WriteLog(DWORD dwlogLv, LPCTSTR lpszModuleName, LPCTSTR lpszFormat, va_list args);
	inline void WriteLog(DWORD dwlogLv, LPCTSTR lpszFormat, va_list args);

protected:
	void _WriterLogInternal(CAtlString& strOutput);
private:

//	static unsigned int __stdcall ListenforChangeLogConfig(void*);
	LogConfig m_LogConfig;
	vector<LogProvider*> m_vecLogProvider;
	DWORD m_dwNotifyThreadID;
	HANDLE m_hNotifyThread;
	BOOL	m_bInited;
};
