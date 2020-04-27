// UpdateVersionTool.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

#include "UpdateVersionTool.h"
#include "VersionInfo.h"
#include <locale.h>
#include "EnumResource.h"
#include "VersionInfoGenerator.h"
#include "ConfigParser.h"
#include "ChangeVersionInfo.h"
#include <imagehlp.h>

#pragma  comment(lib, "Version.lib")
#pragma  comment(lib, "dbghelp.lib")

static LPTOP_LEVEL_EXCEPTION_FILTER g_pfnOldUnhandledExceptionFilter = NULL;



void getVersionInfo(LPCTSTR szpath)
{
	int iErrorCode = 0;

	int iVersionInfoSize = GetFileVersionInfoSize(szpath, 0);
	if (iVersionInfoSize == 0)
	{
		iErrorCode = GetLastError();
		return;
	}

	char* lpVersionInfoData = new char[iVersionInfoSize];
	ZeroMemory(lpVersionInfoData, iVersionInfoSize);

	if (!GetFileVersionInfo(szpath, 0, iVersionInfoSize, lpVersionInfoData))
	{
		iErrorCode = GetLastError();
		return;
	}

	CVersionInfoParser versionInfoParser;
	versionInfoParser.Parse((BYTE*)lpVersionInfoData);

}




LONG WINAPI MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	if (pExceptionInfo)
	{
		SYSTEMTIME time;
		GetLocalTime(&time);
		CAtlString strTime;
		WCHAR szFile[MAX_PATH + 1] = { 0 };
		wsprintf(szFile, L"%d_%d_%d_%d_%d_%d_%d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

		wstring strAppdata = GetAppdataPath();
		WCHAR szPath[MAX_PATH] = { 0 };
		wcscpy_s(szPath, strAppdata.c_str());
		PathAppend(szPath, szFile);

		wprintf(L"%s", szPath);

		HANDLE hFile = CreateFile(szPath, FILE_ALL_ACCESS, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			DWORD i = GetLastError();
			return EXCEPTION_CONTINUE_SEARCH;
		}

		MINIDUMP_EXCEPTION_INFORMATION mei;
		mei.ThreadId = GetCurrentThreadId();
		mei.ClientPointers = TRUE;
		mei.ExceptionPointers = pExceptionInfo;

		InfoLog(szPath);
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, &mei, NULL, NULL);
		CloseHandle(hFile);
	}
		
	return EXCEPTION_CONTINUE_SEARCH;
}

int main(int argc, char** argv)
{
	InfoLog(L"main enter");
	setlocale(LC_ALL, "chs");

	//设置崩溃回调
	g_pfnOldUnhandledExceptionFilter = ::SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

	CConfigParser configParser;
	configParser.SetConfigFile(L"VersionInfo.json");

	int iGroupCount = configParser.GetGroupCount();
	for (int index = 0; index < iGroupCount; ++index)
	{
		CChangeVersionInfo* pChangeVersionInfo = new CChangeVersionInfo();
		if (pChangeVersionInfo)
		{
			CGroupInfo* pGroupInfo = configParser.GetGroupInfoByIndex(index);
			if (pGroupInfo)
			{
				pChangeVersionInfo->SetGroupInfo(pGroupInfo);
				pChangeVersionInfo->StartChangeVersionInfo();
			}

			delete pChangeVersionInfo;
			pChangeVersionInfo = NULL;
		}
	}


	if (argc == 2)
	{
		
	}


	if (g_pfnOldUnhandledExceptionFilter)
	{
		::SetUnhandledExceptionFilter(g_pfnOldUnhandledExceptionFilter);
		g_pfnOldUnhandledExceptionFilter = NULL;
	}

  	system("pause");

    return 0;
}

