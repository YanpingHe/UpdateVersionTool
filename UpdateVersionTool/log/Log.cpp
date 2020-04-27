#include "StdAfx.h"
#include "Log.h"
#include <strsafe.h>
#include <Shlobj.h>
#include <atlstr.h>
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")

Loger g_TestLog;

void WriteLog(DWORD eLogLV, LPCTSTR lpszFormat, ... )
{
	va_list args;
	va_start(args, lpszFormat);
	g_TestLog.WriteLog(eLogLV, lpszFormat, args);
	va_end(args);
}

void ChangeLogLevel( DWORD dwLoglv )
{
	g_TestLog.ChangeLogLevel(dwLoglv);
}

void ChangeLogType( DWORD dwLogType )
{
	g_TestLog.ChangeLogType(dwLogType);
}


BOOL FileLogProvider::Initialize(LogConfig* pLogConfig)
{
	_tstring strFilePath;
	pLogConfig->GetLogFilePath(strFilePath);
	ASSERT_RET(strFilePath.length(), FALSE);

	//首先确保目录存在
	_tstring::size_type st = strFilePath.find_last_of(_T('\\'));
	if(st != _tstring::npos)
	{
		_tstring strFileDir = strFilePath.substr(0, st);
		SHCreateDirectoryEx(NULL, strFileDir.c_str(), NULL);
	}

	m_File = _tfsopen(strFilePath.c_str(), _T("wb"), _SH_DENYWR);

	return (m_File == NULL)?FALSE:TRUE;
}

BOOL FileLogProvider::Write( LPCTSTR szbuf, size_t size)
{
	if(m_File == NULL)
	{
		return FALSE;
	}

	if(szbuf == NULL || size == 0)
	{
		return FALSE;
	}

	// 每次定位到文件尾
	// 这种方法在多个进程同时写的时候可能有问题，暂时没有简易的解决办法
	fseek(m_File, 0, SEEK_END);
	fwrite(szbuf, size*sizeof(TCHAR), 1, m_File);
	fflush(m_File);

	return TRUE;
}

void FileLogProvider::Close()
{
	if(m_File != NULL)
	{
		fclose(m_File);
		m_File = NULL;
	}
}

FileLogProvider::FileLogProvider( DWORD dwLogType ):LogProvider(dwLogType)
{
	m_File = NULL;
}

BOOL DebugerLogProvider::Initialize(LogConfig*)
{
	return TRUE;
}

BOOL DebugerLogProvider::Write( LPCTSTR szbuf, size_t size)
{
	if(szbuf == NULL || size == 0)
	{
		return FALSE;
	}

	OutputDebugString(szbuf);
	return TRUE;
}

void DebugerLogProvider::Close()
{
}

DebugerLogProvider::DebugerLogProvider( DWORD dwLogType ):LogProvider(dwLogType)
{

}

inline void Loger::WriteLog( DWORD logLv, LPCTSTR lpszModuleName, LPCTSTR lpszFormat, va_list args )
{
	ASSERT_RET(lpszModuleName && lpszFormat, NORET);

	if((logLv & m_LogConfig.m_dwLogLevel) == 0)
	{
		return;
	}

	TCHAR szData[1024 * 200] = {0};
	//日志太长的话会被截断
	StringCchVPrintf(szData, _countof(szData), lpszFormat, args);

	SYSTEMTIME time;
	GetLocalTime(&time);
	CAtlString strTime;
	strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d:%03d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

	TCHAR chLogLV;
	switch(logLv)
	{
	case LEVEL_CRITICAL:
		chLogLV = _T('C');
		break;
	case LEVEL_ERROR:
		chLogLV = _T('E');
		break;
	case LEVEL_INFO:
		chLogLV = _T('I');
		break;
	case LEVEL_DEBUG:
	default:
		chLogLV = _T('D');
		break;
	}

	CAtlString strOutput;
	strOutput.Format(_T("[%c]\t%s\t%u\t%s\t%s\n"), chLogLV, lpszModuleName, GetCurrentThreadId(), (LPCTSTR)strTime, szData);

	_WriterLogInternal(strOutput);
}

inline void Loger::WriteLog( DWORD logLv, LPCTSTR lpszFormat, va_list args )
{
	ASSERT_RET(lpszFormat, NORET);

	if((logLv & m_LogConfig.m_dwLogLevel) == 0)
	{
		return;
	}

	HMODULE hCaller = NULL;
	TCHAR szCallerModuleName[MAX_PATH*2] = {0};
	if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, lpszFormat, &hCaller))
		GetModuleBaseName(GetCurrentProcess(), hCaller, szCallerModuleName, _countof(szCallerModuleName) - 1);

	TCHAR szData[1024 * 200] = {0};
	StringCchVPrintf(szData, _countof(szData), lpszFormat, args);

	SYSTEMTIME time;
	GetLocalTime(&time);
	CAtlString strTime;
	strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d:%03d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

	TCHAR chLogLV;
	switch(logLv)
	{
	case LEVEL_CRITICAL:
		chLogLV = _T('C');
		break;
	case LEVEL_ERROR:
		chLogLV = _T('E');
		break;
	case LEVEL_INFO:
		chLogLV = _T('I');
		break;
	case LEVEL_DEBUG:
	default:
		chLogLV = _T('D');
		break;
	}

	CAtlString strOutput;
	strOutput.Format(_T("[%c]\t%s\t%u\t%s\t%s\n"), chLogLV, szCallerModuleName, GetCurrentThreadId(), (LPCTSTR)strTime, szData);

	//_WriterLogInternal(strOutput);

	wprintf(_T("%s"), strOutput.GetString());


}


void Loger::_WriterLogInternal( CAtlString& strOutput )
{
	if (!m_bInited)
	{
		Initialize();
	}

	for(vector<LogProvider*>::iterator it = m_vecLogProvider.begin(); it != m_vecLogProvider.end(); it++)
	{
		if(((*it)->GetLogType() & m_LogConfig.m_dwLogType) == 0)
		{
			continue;
		}

		(*it)->Write((LPCTSTR)strOutput, strOutput.GetLength());
	}
}

void inline DeleteFileFunc(const CAtlString &strFindFilePath, LARGE_INTEGER &liDirSize, vector<CAtlString> &vecOldFilePaths)
{
	CAtlString strFindFiles = strFindFilePath + _T("\\*");
	WIN32_FIND_DATA fd;
	BOOL bFound = FALSE;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	static SYSTEMTIME nowtime;
	static BOOL bFirst = TRUE;
	if(bFirst)
	{
		bFirst = FALSE;
		GetLocalTime(&nowtime);
	}

	if((hFind = FindFirstFile(strFindFiles, &fd)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			CAtlString strFindFile = fd.cFileName;
			if(strFindFile == _T("..") || strFindFile == _T("."))
			{
				continue;
			}

			CAtlString strSubFile = strFindFilePath + _T("\\") + strFindFile;
			if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				DeleteFileFunc(strSubFile, liDirSize, vecOldFilePaths);
				//删除文件夹，但是不用担心，文件夹里有文件的话，不会删除成功
				RemoveDirectory(strSubFile);
			}
			else
			{
				//大小为0的文件干掉
				if(fd.nFileSizeLow == 0 && fd.nFileSizeHigh == 0)
				{
					::DeleteFile(strSubFile);
				}
				else
				{
					//10天以上的文件干掉
					BOOL bDel = FALSE;
					SYSTEMTIME filewritetime;
					if(FileTimeToSystemTime(&fd.ftLastWriteTime, &filewritetime))
					{
						if(filewritetime.wYear > nowtime.wYear)
						{
							bDel = FALSE;
						}
						else if(filewritetime.wYear < nowtime.wYear ||  filewritetime.wMonth < nowtime.wMonth)
						{
							bDel = TRUE;
						}
						else if(filewritetime.wMonth > nowtime.wMonth)
						{
							bDel = FALSE;
						}
						else if(nowtime.wDay - filewritetime.wDay > 10)
						{
							bDel = TRUE;
						}
						else if(nowtime.wDay > filewritetime.wDay) //如果之后发现日志文件夹大小之和超过1G，则删除不是今天写过的文件
						{
							vecOldFilePaths.push_back(strSubFile);
							bDel = FALSE;
						}

						if(bDel)
						{
							::DeleteFile(strSubFile);
						}
						else
						{
							//计算文件夹大小
							liDirSize.LowPart += fd.nFileSizeLow;
							liDirSize.HighPart += fd.nFileSizeHigh;
						}
					}
				}
			}

		}while(FindNextFile(hFind, &fd));

		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}
}

void Loger::Initialize()
{
	if (m_bInited)
	{
		return;
	}

	m_bInited = TRUE;
	LogProvider* oFileLogProvider = new(std::nothrow)FileLogProvider(TYPE_FILE);
//	LogProvider* oDebugerLogProvider = new(std::nothrow)DebugerLogProvider(TYPE_DEBUGER);
//	LogProvider* oPipeLogProvider = new(std::nothrow)PipeLogProvider(TYPE_PIPE);
//	LogProvider* oMemoryLogProvider = new(std::nothrow)MemoryLogProvider(TYPE_MEMORY);

//	ASSERT_RET(oFileLogProvider && oDebugerLogProvider && oPipeLogProvider && oMemoryLogProvider, NORET);
	ASSERT_RET(oFileLogProvider, NORET);

	m_vecLogProvider.push_back(oFileLogProvider);
	//m_vecLogProvider.push_back(oDebugerLogProvider);
	//m_vecLogProvider.push_back(oPipeLogProvider);
	//m_vecLogProvider.push_back(oMemoryLogProvider);

	for(vector<LogProvider*>::iterator it = m_vecLogProvider.begin(); it != m_vecLogProvider.end(); it++)
	{
		(*it)->Initialize(&m_LogConfig);
	}
}

void Loger::ChangeLogLevel( DWORD eLoglv )
{
	m_LogConfig.m_dwLogLevel = eLoglv;
}

void Loger::ChangeLogType( DWORD eLogType )
{
	m_LogConfig.m_dwLogType = eLogType;
}

void Loger::SetLogFilePath( LPCTSTR lpszFilePath )
{
	m_LogConfig.SetLogFilePath(lpszFilePath);
}

Loger::Loger():
m_bInited(FALSE)
{
	ChangeLogLevel(LEVEL_ALL);
	ChangeLogType(TYPE_FILE);
	
	//设置日志文件名
	TCHAR szExeName[MAX_PATH] = _T("UpdateVersionTool");

	TCHAR szPath[MAX_PATH] = { 0 };
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, FALSE);
	CAtlString strLogFilePath;
	strLogFilePath.Format(_T("%s\\Test\\Log\\"), szPath);

	SYSTEMTIME sysTime;
	ZeroMemory(&sysTime, sizeof(sysTime));
	::GetLocalTime(&sysTime);

	ATL::CString strLogFileName;
	strLogFileName.Format(L"%s%s[%u-%u-%u %u-%u-%u-%u][%u].log"
		, strLogFilePath
		, szExeName
		, (UINT)sysTime.wYear
		, (UINT)sysTime.wMonth
		, (UINT)sysTime.wDay
		, (UINT)sysTime.wHour
		, (UINT)sysTime.wMinute
		, (UINT)sysTime.wSecond
		, (UINT)sysTime.wMilliseconds
		, ::GetCurrentProcessId());

	SetLogFilePath((LPCTSTR)strLogFileName);

	
	CAtlString strLogPipeName;
	strLogPipeName.Format(_T("\\\\.\\pipe\\%sLogPipe"), szExeName);
	SetLogPipeName((LPCTSTR)strLogPipeName);

	CAtlString strLogMemroyName;
	strLogMemroyName.Format(_T("%s%u_MemoryLog"), szExeName, GetCurrentProcessId());
	SetLogMemoryName((LPCTSTR)strLogMemroyName);
}

Loger::~Loger()
{
	if(m_hNotifyThread)
	{
		PostThreadMessage(m_dwNotifyThreadID, WM_QUIT, 0, 0);
		DWORD dwRet = WaitForSingleObject(m_hNotifyThread, 3000);
		if(dwRet == WAIT_TIMEOUT)
		{
			TerminateThread(m_hNotifyThread, 0);
		}
		CloseHandle(m_hNotifyThread);
		m_hNotifyThread = NULL;
		m_dwNotifyThreadID = 0;
	}

	for(vector<LogProvider*>::iterator it = m_vecLogProvider.begin(); it != m_vecLogProvider.end();)
	{
		LogProvider* pProvider = *it;
		pProvider->Close();
		it = m_vecLogProvider.erase(it);
		delete pProvider;
		pProvider = NULL;
	}
}

void Loger::SetLogPipeName( LPCTSTR lpszPipeName )
{
	m_LogConfig.SetLogPipeName(lpszPipeName);
}

void Loger::SetLogMemoryName(LPCTSTR lpszLogMemoryName)
{
	m_LogConfig.SetLogMemoryName(lpszLogMemoryName);
}
/*
unsigned int Loger::ListenforChangeLogConfig(void* pParam)
{
	ASSERT_RET(pParam, 0);

	Loger* loger = (Loger*)pParam;
	HANDLE hEvent = NULL;
	HKEY   hKey = NULL;
	DWORD dwValue = 0;
	DWORD dwSizeofValue = sizeof(dwValue);

	do 
	{
		RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Test"), 0, KEY_NOTIFY|KEY_QUERY_VALUE, &hKey);
		ASSERT_OP(hKey, break);

		hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		ASSERT_OP(hEvent, break);

		DWORD dwRet;
		LONG lErrorCode;
		while(true)
		{
			lErrorCode = RegNotifyChangeKeyValue(hKey, 
				TRUE, 
				REG_NOTIFY_CHANGE_LAST_SET|REG_NOTIFY_CHANGE_NAME, 
				hEvent, 
				TRUE);
			ASSERT_RET(lErrorCode == ERROR_SUCCESS, 0);

			dwRet = MsgWaitForMultipleObjects(1, &hEvent, FALSE, INFINITE, QS_ALLPOSTMESSAGE);
			if(dwRet == WAIT_OBJECT_0 + 1)
			{
				break;
			}
			else if(dwRet == WAIT_FAILED)
			{
				ASSERT_OP(0, break);
			}
			else if(dwRet == WAIT_OBJECT_0)
			{
				//检查键值
				DWORD dwValue = 0;
				DWORD dwSizeofValue = sizeof(dwValue);
				LONG nRet = 0;
				if(ERROR_SUCCESS == RegQueryValueEx(hKey, _T("LogLevel"), NULL, NULL, (LPBYTE)&dwValue, &dwSizeofValue))
				{
					dwValue &= LEVEL_ALL;
					loger->ChangeLogLevel(dwValue);
				}

				if(ERROR_SUCCESS == RegQueryValueEx(hKey, _T("LogType"), NULL, NULL, (LPBYTE)&dwValue, &dwSizeofValue))
				{
					dwValue &= TYPE_ALL;
					loger->ChangeLogType(dwValue);
				}
			}
		}

	}while(0);

	if(hKey)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}

	if(hEvent)
	{
		CloseHandle(hEvent);
		hEvent = NULL;
	}

	return 0;
}*/


BOOL PipeLogProvider::Initialize( LogConfig* pLogConfig)
{
	_tstring strPipeame;
	pLogConfig->GetLogPipeName(strPipeame);
	ASSERT_RET(strPipeame.length(), FALSE);

	m_hpipe = CreateNamedPipe(strPipeame.c_str(), PIPE_ACCESS_OUTBOUND,
		PIPE_TYPE_MESSAGE|PIPE_NOWAIT, PIPE_UNLIMITED_INSTANCES, 10240 * 4, 1024, 0, NULL);

	ASSERT_RET(m_hpipe != INVALID_HANDLE_VALUE, FALSE);

	ConnectNamedPipe(m_hpipe, NULL);

	return TRUE;
}

PipeLogProvider::PipeLogProvider( DWORD dwLogType ):LogProvider(dwLogType)
{
	m_hpipe = INVALID_HANDLE_VALUE;
}

void PipeLogProvider::Close()
{
	if(m_hpipe != INVALID_HANDLE_VALUE || m_hpipe != NULL)
	{
		DisconnectNamedPipe(m_hpipe);
		CloseHandle(m_hpipe);
		m_hpipe = INVALID_HANDLE_VALUE;
	}
}

BOOL PipeLogProvider::Write( LPCTSTR szbuf, size_t buflen )
{
	DWORD dwWrited = 0;
	if(!WriteFile(m_hpipe, szbuf, buflen*sizeof(TCHAR), &dwWrited, NULL))
	{
		if(ERROR_NO_DATA == GetLastError()) //客户端关闭了管道
		{
			DisconnectNamedPipe(m_hpipe);
			ConnectNamedPipe(m_hpipe, NULL);
		}
	}

	return TRUE;
}

//memory logprovider
#define LOG_MEMORY_LEN	(4*1024*1024)

BOOL MemoryLogProvider::Initialize( LogConfig* pLogConfig)
{
	m_hFileMapping = NULL;
	m_lpMapPoint = NULL;
	m_puiOffset = NULL;
	m_pbLoop = NULL;
	m_cBuf = NULL;

	_tstring strMemoryName;
	pLogConfig->GetLogMemoryName(strMemoryName);
	ASSERT_RET(strMemoryName.length(), FALSE);

	//先设置为4M，有需要再改
	//额外的sizeof(UINT*)*2 是用来保存当前偏移和是否已经循环这两个变量的
	m_hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(UINT*)*2 + LOG_MEMORY_LEN, strMemoryName.c_str());
	ASSERT_RET(m_hFileMapping, FALSE);

	m_lpMapPoint = MapViewOfFile(m_hFileMapping, FILE_MAP_WRITE|FILE_MAP_READ, 0, 0, sizeof(UINT*)*2 + LOG_MEMORY_LEN); 
	ASSERT_RET(m_lpMapPoint, FALSE);

	m_puiOffset = (UINT*)m_lpMapPoint;
	m_pbLoop = (BOOL*)m_lpMapPoint + 1;

	m_cBuf = (char*)m_lpMapPoint + sizeof(UINT*)*2;
	*m_puiOffset = 0;
	*m_pbLoop = FALSE;
	return TRUE;
}

MemoryLogProvider::MemoryLogProvider( DWORD dwLogType ):LogProvider(dwLogType)
{
	InitializeCriticalSection(&m_cs);
}

void MemoryLogProvider::Close()
{
	if(m_lpMapPoint)
	{
		UnmapViewOfFile(m_lpMapPoint);
		m_lpMapPoint = NULL;
	}

	if(m_hFileMapping)
	{
		CloseHandle(m_hFileMapping);
		m_hFileMapping = NULL;
	}

	DeleteCriticalSection(&m_cs);
}

//为了节省内存空间，写入去之前全部转换为多字节字符
BOOL MemoryLogProvider::Write( LPCTSTR szbuf, size_t buflen )
{
#if defined(_UNICODE) || defined(UNICODE)
	char *lpBuf = new(std::nothrow)char[buflen*sizeof(WCHAR)];
	ASSERT_RET(lpBuf, FALSE);
	memset(lpBuf, 0, buflen*sizeof(WCHAR));

	int nChars = ::WideCharToMultiByte(CP_ACP, 0, szbuf, buflen, lpBuf, buflen*sizeof(WCHAR), NULL, NULL);
#else
	int nChars = buflen*sizeof(TCHAR);
	char* lpBuf = (char*)szbuf;
#endif

	EnterCriticalSection(&m_cs);

	if(nChars > LOG_MEMORY_LEN)
	{
		nChars = LOG_MEMORY_LEN;
	}

	if(*m_puiOffset + nChars <= LOG_MEMORY_LEN)
	{
		memcpy(m_cBuf+*m_puiOffset, lpBuf, nChars);

		*m_puiOffset += nChars;
		if(*m_puiOffset == LOG_MEMORY_LEN)
		{
			*m_puiOffset = 0;
			*m_pbLoop = TRUE;
		}
	}
	else
	{
		DWORD dwLast = LOG_MEMORY_LEN-*m_puiOffset;
		memcpy(m_cBuf+*m_puiOffset, lpBuf, dwLast);
		memcpy(m_cBuf, lpBuf+dwLast, nChars-dwLast);
		*m_puiOffset = nChars-dwLast;
		*m_pbLoop = TRUE;
	}

	LeaveCriticalSection(&m_cs);

#if defined(_UNICODE) || defined(UNICODE)
	delete []lpBuf;
#endif

	return TRUE;
}