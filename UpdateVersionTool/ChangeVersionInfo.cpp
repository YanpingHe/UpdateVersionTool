#include "stdafx.h"
#include "ChangeVersionInfo.h"
#include <shlwapi.h>
#include "VersionInfoGenerator.h"
#include "VersionConfig.h"


void CChangeVersionInfo::SetGroupInfo(CGroupInfo* pGroupInfo)
{
	m_pGroupInfo = pGroupInfo;
}

void CChangeVersionInfo::StartChangeVersionInfo()
{
	if (!m_pGroupInfo)
	{
		ErrLog(L"[%s] [%d]  m_pGroupInfo=NULL", __FUNCTIONW__, __LINE__);
		return;
	}

	int iCount = m_pGroupInfo->m_vecFileList.size();
	for (int index = 0; index < iCount; ++index)
	{

		wstring wstrFilePath = _GetFilePath(m_pGroupInfo->m_vecFileList[index]);
		InfoLog(L"[%s] [%d] intdex=%d iCount=%d wstrFilePath=%s", __FUNCTIONW__, __LINE__, index, iCount, wstrFilePath.c_str());

		if (!CheckFileExist(wstrFilePath.c_str()))
		{
			ErrLog(L"[%s] [%d] not exist", __FUNCTIONW__, __LINE__);
			continue;
		}

		if (_IsExistVersionInfo(wstrFilePath))
		{
			InfoLog(L"[%s] [%d] version info already exist", __FUNCTIONW__, __LINE__);
			_DeleteVersionInfo(wstrFilePath);
		}
		else
		{
			InfoLog(L"[%s] [%d] version info not exist", __FUNCTIONW__, __LINE__);
		}

		_AddVersionInfo(wstrFilePath);
	}
}
 
std::wstring CChangeVersionInfo::_GetFilePath(wstring& wstrFileName)
{
	if (!m_pGroupInfo)
	{
		return L""; 
	}

	wstring wstrFilePath;

	if (m_wstrModuleFolder.empty())
	{
		//先用exe当前目录
		WCHAR szPath[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, szPath, MAX_PATH);
		PathRemoveFileSpec(szPath);
		m_wstrModuleFolder = szPath;

		//配置中有目录，就用配置中的，配置路径可以为相对路径
		if (m_pGroupInfo->m_FileDir.size() > 0)
		{
			wstrFilePath = m_pGroupInfo->m_FileDir;
			if (PathIsRelative(m_pGroupInfo->m_FileDir.c_str()))
			{
				WCHAR szPath[MAX_PATH] = { 0 };
				wcscpy_s(szPath, m_wstrModuleFolder.c_str());
				PathAppend(szPath, m_pGroupInfo->m_FileDir.c_str());
				m_wstrModuleFolder = szPath;
			}
			else
			{
				m_wstrModuleFolder = m_pGroupInfo->m_FileDir;
			}
		}
	}

	WCHAR szPath[MAX_PATH] = { 0 };
	wcscpy_s(szPath, m_wstrModuleFolder.c_str());
	PathAppend(szPath, wstrFileName.c_str());
	wstrFilePath = szPath;

	return wstrFilePath;
}

bool CChangeVersionInfo::_DeleteVersionInfo(wstring& wstrFileName)
{
	InfoLog(L"[%s] [%d] enter", __FUNCTIONW__, __LINE__);

	DWORD dwLang = _GetVersionLang(wstrFileName);
	InfoLog(L"[%s] [%d] dwLang:%u", __FUNCTIONW__, __LINE__, dwLang);

	if (0 != dwLang)
	{
		// Open the file to which you want to add the dialog box resource.
		HANDLE	hUpdateRes = BeginUpdateResource(wstrFileName.c_str(), FALSE);
		if (hUpdateRes == NULL)
		{
			ErrLog(L"[%s] [%d]  BeginUpdateResource ret NULL  errorcode:%d", __FUNCTIONW__, __LINE__, GetLastError());
			return 0;
		}

		// Add the dialog box resource to the update list.
		BOOL  result = UpdateResource(hUpdateRes,    // update resource handle
			RT_VERSION,                         // change dialog box resource
			MAKEINTRESOURCE(VS_VERSION_INFO),         // dialog box id
			MAKELANGID(PRIMARYLANGID(dwLang), SUBLANGID(dwLang)),  // neutral language
			NULL,                         // ptr to resource info
			0);       // size of resource info

		if (result == FALSE)
		{
			ErrLog(L"[%s] [%d]  UpdateResource ret NULL  errorcode:%d", __FUNCTIONW__, __LINE__, GetLastError());
			return 0;
		}

		// Write changes to FOOT.EXE and then close it.
		if (!EndUpdateResource(hUpdateRes, FALSE))
		{
			ErrLog(L"[%s] [%d]  EndUpdateResource ret NULL  errorcode:%d", __FUNCTIONW__, __LINE__, GetLastError());
			return 0;
		}

		//wprintf(L"_DeleteVersionInfo : %s", wstrFileName.c_str());

		InfoLog(L"[%s] [%d]  _DeleteVersionInfo  success ", __FUNCTIONW__, __LINE__);
		return true;
	}

	return false;
}

DWORD CChangeVersionInfo::_GetVersionLang(wstring& wstrFileName)
{
	HMODULE hExe;        // handle to .EXE file
	hExe = LoadLibraryEx(wstrFileName.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE);
	if (hExe == NULL)
	{
		ErrLog(L"[%s] [%d]  LoadLibraryEx ret NULL  errorcode:%d", __FUNCTIONW__, __LINE__, GetLastError());
		return 0;
	}

	vector<CVersionNameAndLang> vecVersionNameAndLang;
	EnumResourceTypes(hExe, (ENUMRESTYPEPROC)EnumTypesFunc, LONG_PTR(&vecVersionNameAndLang));
	FreeLibrary(hExe);

	if (vecVersionNameAndLang.size() > 0)
	{
		return vecVersionNameAndLang[0].dwLangID;
	}
	
	return 0;
}


bool CChangeVersionInfo::_IsExistVersionInfo(wstring& wstrFileName)
{
	int iVersionInfoSize = GetFileVersionInfoSize(wstrFileName.c_str(), 0);
	if (iVersionInfoSize == 0)
	{
		InfoLog(L"[%s] [%d] errorcode:%d", __FUNCTIONW__, __LINE__, GetLastError());
		return false;
	}

	return true;
}



bool CChangeVersionInfo::_AddVersionInfo(wstring& wstrFileName)
{
	InfoLog(L"[%s] [%d] enter", __FUNCTIONW__, __LINE__);

	CBufferNode* pBufNode = NULL;
	CVersionInfoGenerator  *pVersionInfoGenerator = new CVersionInfoGenerator();
	if (pVersionInfoGenerator)
	{
		//整个group的文件用一个配置
		if (!m_pVersionInfo)
		{
			m_pVersionInfo = new CVersionConfig();
			if (m_pVersionInfo)
			{
				m_pVersionInfo->SetConfig(wstrFileName, m_pGroupInfo->m_mapStringInfo);
			}
		}

		if (!m_pVersionInfo)
		{
			ErrLog(L"[%s] [%d] m_pVersionInfo==NULL", __FUNCTIONW__, __LINE__);
			return false;
		}

		m_pVersionInfo->ResetInternalName(wstrFileName);
		pVersionInfoGenerator->SetConfigParser(m_pVersionInfo);
		pBufNode = pVersionInfoGenerator->GetByte();
	}
	else
	{
		ErrLog(L"[%s] [%d] new fail", __FUNCTIONW__, __LINE__);
	}

	if (!(pBufNode && pBufNode->size > 0))
	{
		ErrLog(L"[%s] [%d] pBufNode==%d  pBufNode->size=%d", __FUNCTIONW__, __LINE__, pBufNode, pBufNode ? pBufNode->size : 0);
		return 0;
	}

	InfoLog(L"[%s] [%d]  pBufNode->size=%d", __FUNCTIONW__, __LINE__, pBufNode->size);


	//解析序列化数据是否正确
	//CVersionInfoParser versionInfoParser;
	//versionInfoParser.Parse((BYTE*)(pBufNode->pData));

	// Open the file to which you want to add the dialog box resource.
	HANDLE hUpdateRes = BeginUpdateResource(wstrFileName.c_str(), FALSE);
	if (hUpdateRes == NULL)
	{
		ErrLog(L"[%s] [%d]  BeginUpdateResource ret NULL  errorcode:%d", __FUNCTIONW__, __LINE__, GetLastError());
		delete pBufNode->pData;
		delete pBufNode;
		pBufNode = NULL;

		return 0;
	}

   // Add the dialog box resource to the update list.
	BOOL result = UpdateResource(hUpdateRes,    // update resource handle
		RT_VERSION,                         // change dialog box resource
		MAKEINTRESOURCE(VS_VERSION_INFO),         // dialog box id
		MAKELANGID(LANG_CHINESE_SIMPLIFIED, SUBLANG_CHINESE_SIMPLIFIED),  // neutral language
		pBufNode->pData,                         // ptr to resource info
		pBufNode->size);       // size of resource info


	if (result == FALSE)
	{
		ErrLog(L"[%s] [%d]  UpdateResource ret NULL  errorcode:%d", __FUNCTIONW__, __LINE__, GetLastError());
		delete pBufNode->pData;
		delete pBufNode;
		pBufNode = NULL;

		return 0;
	}

	// Write changes to FOOT.EXE and then close it.
	if (!EndUpdateResource(hUpdateRes, FALSE))
	{
		ErrLog(L"[%s] [%d]  EndUpdateResource ret NULL  errorcode:%d", __FUNCTIONW__, __LINE__, GetLastError());
		delete pBufNode->pData;
		delete pBufNode;
		pBufNode = NULL;
		return 0;
	}

	InfoLog(L"[%s] [%d]  _AddVersionInfo success", __FUNCTIONW__, __LINE__);
	
	delete pBufNode->pData;
	delete pBufNode;
	pBufNode = NULL;

	return true;

}
