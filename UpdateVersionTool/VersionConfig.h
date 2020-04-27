#pragma once

#include <string>
#include <map>
#include "VersionInfoStruct.h"

using namespace std;

class CVersionConfig
{
public:
	CVersionConfig();
	~CVersionConfig();
	void SetConfig(wstring& strFile, map<wstring, wstring>& m_mapStringInfo);
	void ResetInternalName(wstring& strFile);

	wstring GetString_Comments() { return m_wstrComments; }
	wstring GetString_CompanyName() { return m_wstrCompanyName; }
	wstring GetString_FileDescription() { return m_wstrFileDescription; }
	wstring GetString_FileVersion() { return m_wstrFileVersion; }
	wstring GetString_InternalName() { return m_wstrInternalName; }
	wstring GetString_LegalCopyright() { return m_wstrLegalCopyright; }
	wstring GetString_OriginalFilename() { return m_wstrOriginalFilename; }
	wstring GetString_ProductName() { return m_wstrProductName; }
	wstring GetString_ProductVersion() { return m_wstrProductVersion; }
	wstring GetString_SpecialBuild() { return m_wstrComments; }
	wstring GetString_PrivateBuild() { return m_wstrSpecialBuild; }
	wstring GetString_LegalTrademarks() { return m_wstrLegalTrademarks; }
	wstring GetString_LangAndCodepage() { return m_LangAndCodepage.GetLangAndCodepage(); }

	WORD GetWord_Lang() { return m_LangAndCodepage.GetLang(); }
	WORD GetWord_Codepage() { return m_LangAndCodepage.GetCodepage(); }

	DWORD GetDword_FileFlagsMask() {return m_dwFileFlagsMask;	}
	DWORD GetDword_FileFlags() {	return m_dwFileFlags;	}
	DWORD GetDword_FileOS() {		return m_dwFileOS;	}
	DWORD GetDword_FileType() {		return m_dwFileType;	}
	DWORD GetDword_FileSubtype() {	return m_dwFileSubtype;	}
	DWORD GetDword_FileDateMS() {	return m_dwFileDateMS;	}
	DWORD GetDword_FileDateLS() {	return m_dwFileDateLS;	}

	DWORD GetFileVersionMS() {		return m_FileVersionFormat.GetMSVersion();	}
	DWORD GetFileVersionLS() {		return m_FileVersionFormat.GetLSVersion();	}
	DWORD GetProductVersionMS() {	return m_FileVersionFormat.GetMSVersion();	}
	DWORD GetProductVersionLS() {	return m_FileVersionFormat.GetLSVersion();	}


	vector<CStringDataNode> GetStringDataNodeList();



private:
	int _GetFileType(wstring strExt);
private:
	wstring m_wstrFilePath;

	wstring m_wstrComments;
	wstring m_wstrCompanyName;
	wstring m_wstrFileDescription;
	wstring m_wstrFileVersion;
	wstring m_wstrInternalName;
	wstring m_wstrLegalCopyright;
	wstring m_wstrOriginalFilename;
	wstring m_wstrProductName;
	wstring m_wstrProductVersion;
	wstring m_wstrSpecialBuild;
	wstring m_wstrPrivateBuild;
	wstring m_wstrLegalTrademarks;
	



	DWORD m_dwFileFlagsMask = 0;
	DWORD m_dwFileFlags = 0;
	DWORD m_dwFileOS = 0;
	DWORD m_dwFileType = 0;
	DWORD m_dwFileSubtype = 0;
	DWORD m_dwFileDateMS = 0;
	DWORD m_dwFileDateLS = 0;
	

	CVersionFormat m_FileVersionFormat;
	CVersionFormat m_ProductVersionFormat;

	CLangAndCodepage  m_LangAndCodepage;
};