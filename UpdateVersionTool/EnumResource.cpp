#include "stdafx.h"
#include <strsafe.h>

#include "EnumResource.h"
#include "VersionInfoStruct.h"


BOOL EnumTypesFunc(HMODULE hModule, LPTSTR lpType, LONG lParam)
{
	// Write the resource type to a resource information file.
	// The type may be a string or an unsigned decimal
	// integer, so test before printing.
	if (IS_INTRESOURCE(lpType))
	{
		InfoLog(L"[%s] [%d]  lpType=%u  ", __FUNCTIONW__, __LINE__, (USHORT)lpType);
		if (((WORD)((ULONG_PTR)RT_VERSION)) == (USHORT)lpType)
		{
			InfoLog(L"[%s] [%d]  type=RT_VERSION", __FUNCTIONW__, __LINE__);
			EnumResourceNames(hModule,	lpType,	(ENUMRESNAMEPROC)EnumNamesFunc,	lParam);
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		return TRUE;
	}

	return TRUE;
}


BOOL EnumNamesFunc(HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LONG lParam)
{
	TCHAR szBuffer[80];  // print buffer for info file
	DWORD cbWritten;     // number of bytes written to resource info file
	size_t cbString;
	HRESULT hResult;

	// Write the resource name to a resource information file.
	// The name may be a string or an unsigned decimal
	// integer, so test before printing.
	if (!IS_INTRESOURCE(lpName))
	{
		hResult = StringCchPrintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), TEXT("Name: %s"), lpName);
		if (FAILED(hResult))
		{
			// Add code to fail as securely as possible.
			return FALSE;
		}
	}
	else
	{
		hResult = StringCchPrintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), TEXT("Name: %u"), (USHORT)lpName);
		if (FAILED(hResult))
		{
			// Add code to fail as securely as possible.
			return FALSE;
		}
	}

	InfoLog(L"[%s] [%d]  %s", __FUNCTIONW__, __LINE__, szBuffer);

	//WriteFile(g_hFile, szBuffer, (DWORD)cbString, &cbWritten, NULL);
	// Find the languages of all resources of type
	// lpType and name lpName.
	EnumResourceLanguages(hModule, lpType, lpName, (ENUMRESLANGPROC)EnumLangsFunc, lParam);

	return TRUE;
}



BOOL EnumLangsFunc(HMODULE hModule, LPCTSTR lpType, LPCTSTR lpName, WORD wLang, LONG lParam)
{
	CVersionNameAndLang lang;
	//lang.wstrNameID = lpName;
	lang.dwLangID = wLang;
	InfoLog(L"[%s] [%d]  wLang=%d", __FUNCTIONW__, __LINE__, wLang);

	vector<CVersionNameAndLang>* pVecVersionNameAndLang = (vector<CVersionNameAndLang>*)lParam;
	if (pVecVersionNameAndLang)
	{
		pVecVersionNameAndLang->push_back(lang);
	}
	
	return TRUE;
}
