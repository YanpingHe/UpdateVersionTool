#include "stdafx.h"
#include "VersionConfig.h"
#include <shlwapi.h>


CVersionConfig::CVersionConfig()
{


}

CVersionConfig::~CVersionConfig()
{



}



void  CVersionConfig::SetConfig(wstring& strFile, map<wstring, wstring>& m_mapStringInfo)
{
	m_wstrFilePath = strFile;
	
	m_wstrComments = L"";
	m_wstrCompanyName = L"腾讯科技";
	m_wstrFileDescription = L"测试文件版本222";
	m_wstrFileVersion = L"10.20.4022.0";
	m_wstrInternalName = L"Test1.exe";
	m_wstrLegalCopyright = L"Copyright (C) 2019－2019";
	m_wstrOriginalFilename = L"Test1.exe";
	m_wstrProductName = L"腾讯视频";
	m_wstrProductVersion = L"10.20.4022.0";
	m_wstrSpecialBuild = L"";
	m_wstrPrivateBuild = L"";
	m_wstrLegalTrademarks = L"";

	m_dwFileFlagsMask = 63;
	m_dwFileFlags = 0;
	m_dwFileOS = VOS_NT_WINDOWS32;

	/*VFT_APP
	VFT_DLL
	VFT_STATIC_LIB*/
	//m_dwFileType = VFT_DLL; VFT_APP
	m_dwFileType = VFT_APP;
	m_dwFileSubtype = 0;
	m_dwFileDateMS = 0;
	m_dwFileDateLS = 0;


	for (map<wstring, wstring>::iterator it = m_mapStringInfo.begin(); it!= m_mapStringInfo.end(); ++it)
	{
		if (it->first.size() > 0)
		{
			if (it->first == key_String_Comments)
			{
				m_wstrComments = it->second;
			}
			else if (it->first == key_String_CompanyName)
			{
				m_wstrCompanyName = it->second;
			}
			else if (it->first == key_String_FileDescription)
			{
				m_wstrFileDescription = it->second;
			}
			else if (it->first == key_String_FileVersion)
			{
				m_wstrFileVersion = it->second;
			}
			else if (it->first == key_String_InternalName)
			{
				m_wstrInternalName = it->second;
			}
			else if (it->first == key_String_LegalCopyright)
			{
				m_wstrLegalCopyright = it->second;

				//%Year%替换成当前年
				
				wstring strYear = L"%Year%";
				int iPos = m_wstrLegalCopyright.find(strYear);
				if (iPos != -1)
				{
					SYSTEMTIME time;
					GetLocalTime(&time);
					time.wYear;

					wstring str = to_wstring(time.wYear);
					m_wstrLegalCopyright.replace(iPos, strYear.length(), str);
				}
			}
			else if (it->first == key_String_OriginalFilename)
			{
				m_wstrOriginalFilename = it->second;
			}
			else if (it->first == key_String_ProductName)
			{
				m_wstrProductName = it->second;
			}
			else if (it->first == key_String_ProductVersion)
			{
				m_wstrProductVersion = it->second;
			}
			else if (it->first == key_String_SpecialBuild)
			{
				m_wstrSpecialBuild = it->second;
			}
			else if (it->first == key_String_PrivateBuild)
			{
				m_wstrPrivateBuild = it->second;

			}
			else if (it->first == key_String_LegalTrademarks)
			{
				m_wstrLegalTrademarks = it->first;
			}
			else if (it->first == key_String_LangAndCode)
			{
				DWORD dwLangAndCode = std::stoul(it->second, 0, 16);
				WORD dwLang = dwLangAndCode >> 16;
				WORD dwCode = dwLangAndCode & 0x0000FFFF;
				m_LangAndCodepage.SetLangAndCodepage(dwLang, dwCode);
			}			
		}
	}

	if (m_wstrFileVersion.empty() && !m_wstrProductVersion.empty())
	{
		m_wstrFileVersion = m_wstrProductVersion;
	}

	if (m_wstrProductVersion.empty() && !m_wstrFileVersion.empty())
	{
		m_wstrProductVersion = m_wstrFileVersion;
	}

	m_FileVersionFormat.SetVersionFormat(m_wstrFileVersion);
	m_ProductVersionFormat.SetVersionFormat(m_wstrProductVersion);

	ResetInternalName(m_wstrFilePath);
}

void CVersionConfig::ResetInternalName(wstring& strFile)
{
	m_wstrFilePath = strFile;

	//generate file type
	if (!m_wstrFilePath.empty())
	{
		WCHAR* strFileName = PathFindFileName(m_wstrFilePath.c_str());
		m_wstrOriginalFilename = strFileName;
		if (m_wstrOriginalFilename.length() > 0)
		{
			WCHAR* strExt = PathFindExtension(strFileName);
			if (strExt)
			{
				m_dwFileType = _GetFileType(strExt);
			}
		}
		else
		{
			// to do 
		}
	}
	else
	{
		//to do 
	}
}

vector<CStringDataNode> CVersionConfig::GetStringDataNodeList()
{
	vector<CStringDataNode>   vecNode;

	if (!m_wstrComments.empty())
	{
		CStringDataNode node = { key_String_Comments , (WCHAR*)m_wstrComments.c_str() };
		vecNode.push_back(node);
	}

	if (!m_wstrCompanyName.empty())
	{
		CStringDataNode node = { key_String_CompanyName , (WCHAR*)m_wstrCompanyName.c_str() };
		vecNode.push_back(node);
	}

	if (!m_wstrFileDescription.empty())
	{
		CStringDataNode node = { key_String_FileDescription , (WCHAR*)m_wstrFileDescription.c_str() };
		vecNode.push_back(node);
	}

	if (!m_wstrFileVersion.empty())
	{
		CStringDataNode node = { key_String_FileVersion , (WCHAR*)m_wstrFileVersion.c_str() };
		vecNode.push_back(node);
	}

	if (!m_wstrInternalName.empty())
	{
		CStringDataNode node = { key_String_InternalName , (WCHAR*)m_wstrInternalName.c_str() };
		vecNode.push_back(node);
	}

	if (!m_wstrLegalCopyright.empty())
	{
		CStringDataNode node = { key_String_LegalCopyright , (WCHAR*)m_wstrLegalCopyright.c_str() };
		vecNode.push_back(node);
	}

	if (!m_wstrOriginalFilename.empty())
	{
		CStringDataNode node = { key_String_OriginalFilename , (WCHAR*)m_wstrOriginalFilename.c_str() };
		vecNode.push_back(node);
	}

	if (!m_wstrProductName.empty())
	{
		CStringDataNode node = { key_String_ProductName , (WCHAR*)m_wstrProductName.c_str() };
		vecNode.push_back(node);
	}

	if (!m_wstrProductVersion.empty())
	{
		CStringDataNode node = { key_String_ProductVersion , (WCHAR*)m_wstrProductVersion.c_str() };
		vecNode.push_back(node);
	}
	
	if (!m_wstrSpecialBuild.empty())
	{
		CStringDataNode node = { key_String_SpecialBuild , (WCHAR*)m_wstrSpecialBuild.c_str() };
		vecNode.push_back(node);
	}

	if (!m_wstrPrivateBuild.empty())
	{
		CStringDataNode node = { key_String_PrivateBuild , (WCHAR*)m_wstrPrivateBuild.c_str() };
		vecNode.push_back(node);
	}

	if (!m_wstrLegalTrademarks.empty())
	{
		CStringDataNode node = { key_String_LegalTrademarks , (WCHAR*)m_wstrLegalTrademarks.c_str() };
		vecNode.push_back(node);
	}

	return vecNode;
}

int CVersionConfig::_GetFileType(wstring strExt)
{
	int itype = VFT_APP;
	if (_wcsicmp(strExt.c_str(), L".exe"))
	{
		itype = VFT_APP;
	}
	else if (_wcsicmp(strExt.c_str(), L".dll"))
	{
		itype = VFT_DLL;
	}
	else if (_wcsicmp(strExt.c_str(), L".lib"))
	{
		itype = VFT_STATIC_LIB;
	}

	return itype;
}


