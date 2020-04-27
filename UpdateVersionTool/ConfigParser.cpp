#include "stdafx.h"
#include "ConfigParser.h"
#include "json\reader.h"
#include <memory>
#include <iostream>
#include <fstream>

using namespace Json;
using namespace std;





void CConfigParser::SetConfigFile(TCHAR* strFile)
{
	m_wstrFilePath = strFile;

	_ParseConfig();

}

int CConfigParser::GetGroupCount()
{
	return m_vecGroupInfo.size();		 
}

CGroupInfo* CConfigParser::GetGroupInfoByIndex(int index)
{
	CGroupInfo* pGroupInfo = NULL;

	if (index >= 0 && index < m_vecGroupInfo.size())
	{
		pGroupInfo = new CGroupInfo();
		if (pGroupInfo)
		{
			*pGroupInfo = m_vecGroupInfo[index];
		}
	}

	return pGroupInfo;		 
}

bool CConfigParser::_ParseConfig()
{
	bool bret = true;

	if (m_wstrFilePath.empty())
	{
		return false;
	}

	ifstream in(m_wstrFilePath, ios::in);

	if (!in.is_open())
	{
		return false;
	}


	CharReaderBuilder builder;
	builder["collectComments"] = false;
	Value root;
	JSONCPP_STRING errs;
	
	
	do 
	{
		if (false == parseFromStream(builder, in, &root, &errs))
		{
			bret = false;
			break;
		}


		if (!root.isObject())
		{
			return false;
		}


		Value::Members  vecNames = root.getMemberNames();

		for (Value::Members::iterator it=vecNames.begin();it!=vecNames.end();++it)
		{
			CGroupInfo groupInfo;
			groupInfo.m_wstrGroupName = Utf8ToWS(it->c_str());


			Value objGroupInfo = root[*it];
			if (objGroupInfo.isObject())
			{
				Value objVersionInfo = objGroupInfo[WSToUtf8(key_JsonConfig_VersionInfo)];
				if (objVersionInfo.isObject())
				{
					Value::Members  vecStringNames = objVersionInfo.getMemberNames();
					for (Value::Members::iterator itStringName = vecStringNames.begin(); itStringName!= vecStringNames.end(); ++itStringName)
					{
						groupInfo.m_mapStringInfo.insert(make_pair(Utf8ToWS(itStringName->c_str()), Utf8ToWS(objVersionInfo[*itStringName].asCString())));
					}
				}
				else
				{
					// to do


				}

				Value objFileDir = objGroupInfo[WSToUtf8(key_JsonConfig_FileDir)];
				if (objFileDir.isString())
				{
					groupInfo.m_FileDir = Utf8ToWS(objFileDir.asCString());
				}


				Value objFileList = objGroupInfo[WSToUtf8(key_JsonConfig_FileList)];
				if (objFileList.isArray())
				{
					for (Value::ArrayIndex i=0; i<objFileList.size(); ++i)
					{
						groupInfo.m_vecFileList.push_back(Utf8ToWS(objFileList[i].asCString()));
					}
				}
				else
				{
					// to do 
				}
			
				m_vecGroupInfo.push_back(groupInfo);
			}
		}

	} while (0);


	in.close();

	return bret;
}
