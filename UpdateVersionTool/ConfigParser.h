#pragma once

#include <string>
#include <map>
#include "VersionInfoStruct.h"

using namespace std;


struct CGroupInfo
{
	wstring m_wstrGroupName;
	map<wstring, wstring> m_mapStringInfo;
	wstring			m_FileDir;
	vector<wstring> m_vecFileList;
};




class CConfigParser
{
public:
	CConfigParser() {}
	~CConfigParser() {}
	void SetConfigFile(TCHAR* strFile);
	int GetGroupCount();
	CGroupInfo* GetGroupInfoByIndex(int index);

private:
	bool _ParseConfig();


private:
	wstring m_wstrFilePath;
	vector<CGroupInfo>  m_vecGroupInfo;
};