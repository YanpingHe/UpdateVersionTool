#include "stdafx.h"
#include "VersionInfoStruct.h"
#include <string>
#include <shlobj.h>

int GetAlignPadding(BYTE* pData)
{
	int padding = 0;
	if ((int)pData % KAlign_4Byte != 0)
	{
		padding = KAlign_4Byte - (int)pData % KAlign_4Byte;
	}
	else
	{
		padding = 0;
	}

	return padding;
}

int GetAlignPadding(DWORD dwOffset)
{
	int padding = 0;
	if (dwOffset % KAlign_4Byte != 0)
	{
		padding = KAlign_4Byte - dwOffset % KAlign_4Byte;
	}
	else
	{
		padding = 0;
	}

	return padding;
}

void SplitString(std::wstring& wstr, vector<std::wstring>& vecWstr, std::wstring& token)
{
	if (token.empty() || wstr.empty())
	{
		return;
	}

	int iStartPos = 0;
	int iEndPos = 0;
	while (iStartPos < wstr.size())
	{
		iEndPos = wstr.find(token, iStartPos);
		if (iEndPos != std::string::npos)
		{
			vecWstr.push_back(wstr.substr(iStartPos, iEndPos - iStartPos));
			iStartPos = iEndPos + 1;
		}
		else
		{
			if (iStartPos <= wstr.size())
			{
				vecWstr.push_back(wstr.substr(iStartPos, wstr.size() - iStartPos + 1));
				iStartPos = wstr.size() + 1;
			}
			break;
		}
	}
}

std::wstring Utf8ToWS(const char * str, int nLen/* = -1*/)
{
	std::wstring wstr;
	if (str == 0) return wstr;
	if (nLen < 0) nLen = (int)strlen(str);
	if (nLen == 0) return wstr;
	int nLength = MultiByteToWideChar(CP_UTF8, 0, str, nLen, 0, 0);

	wstr = std::wstring(nLength, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, str, nLen, &wstr[0], nLength);

	return wstr;
}


std::string WSToUtf8(const std::wstring& wide)
{
	int wide_length = static_cast<int>(wide.length());
	if (wide_length == 0)
		return std::string();

	// Compute the length of the buffer we'll need.
	int charcount = WideCharToMultiByte(CP_UTF8, 0, wide.data(), wide_length,
		NULL, 0, NULL, NULL);
	if (charcount == 0)
		return std::string();

	std::string mb;
	mb.resize(charcount);
	WideCharToMultiByte(CP_UTF8, 0, wide.data(), wide_length,
		&mb[0], charcount, NULL, NULL);

	return mb;
}

BOOL CheckFileExist(LPCTSTR strFile)
{
	WIN32_FILE_ATTRIBUTE_DATA attrs = { 0 };
	return ::GetFileAttributesEx(strFile, ::GetFileExInfoStandard, &attrs);
}

void ErrorHandler(TCHAR* pinfo)
{

}


std::wstring GetAppdataPath()
{
	static wstring strCommonAppDataPath;

	if (strCommonAppDataPath.empty())
	{
		WCHAR szPath[MAX_PATH] = { 0 };
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, szPath)))
		{
			strCommonAppDataPath = szPath;
		}
	}

	return strCommonAppDataPath;
}

void CVersionFormat::SetVersionFormat(wstring& wstrVersion)
{
	m_wstrVersion = wstrVersion;


	wstring wstrList = m_wstrVersion;
	vector<wstring> vecAttrName;
	wstring wstrToken = L".";
	SplitString(wstrList, vecAttrName, wstrToken);

	if (vecAttrName.size() == 4)
	{
		m_MSVersionHeight = (WORD)stoul(vecAttrName[0]);
		m_MSVersionLow = (WORD)stoul(vecAttrName[1]);
		m_LSVersionHeight = (WORD)stoul(vecAttrName[2]);
		m_LSVersionLow = (WORD)stoul(vecAttrName[3]);
	}
}

std::wstring CVersionFormat::GetVersionString()
{
	wstring wstr = L"1.0.0.0";
	
	WCHAR wstrBuff[MAX_PATH] = {0};
	wsprintf(wstrBuff, L"%d.%d.%d.%d", m_MSVersionHeight, m_MSVersionLow, m_LSVersionHeight, m_LSVersionLow);
	wstr = wstrBuff;

	return wstr;		 
}

std::wstring CLangAndCodepage::GetLangAndCodepage()
{
	wstring wstr;

	WCHAR wstrBuff[MAX_PATH] = { 0 };
	wsprintf(wstrBuff, L"%04x%04x", m_wLanguage, m_wCodePage);
	wstr = wstrBuff;

	return wstr;
}