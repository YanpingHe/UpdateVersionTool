#pragma once

#include <string>
#include "VersionInfoStruct.h"
#include "ConfigParser.h"
#include "EnumResource.h"
#include "VersionConfig.h"

using namespace std;



class CChangeVersionInfo
{
public:
	CChangeVersionInfo() {}
	~CChangeVersionInfo() {}
	void SetGroupInfo(CGroupInfo* pGroupInfo);
	void StartChangeVersionInfo();


private:
	wstring _GetFilePath(wstring& wstrFileName);
	bool _DeleteVersionInfo(wstring& wstrFileName);
	DWORD _GetVersionLang(wstring& wstrFileName);
	bool _IsExistVersionInfo(wstring& wstrFileName);
	bool _AddVersionInfo(wstring& wstrFileName);


private:
	CGroupInfo* m_pGroupInfo = NULL;
	
	CVersionConfig* m_pVersionInfo = NULL;
	wstring m_wstrModuleFolder;

};




