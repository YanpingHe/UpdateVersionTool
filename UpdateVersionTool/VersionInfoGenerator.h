#pragma once
#include <wtypes.h>
#include <vector>
#include "ConstantDefine.h"
#include "VersionInfo.h"
#include "VersionConfig.h"

using namespace std;









class CDataBaseGenerator
{
public:
	void SetConfigParser(CVersionConfig* pConfigParser, DWORD dwOffset);
	virtual void SetKey()=0;
	int GetHeadByteLength()
	{
		return sizeof(CDataHead) + (wcslen(m_szKey) + 1) * 2;
	}

	int GetValueOffset()
	{
		return m_paddingPre + GetHeadByteLength() + m_Padding;
	}
	DWORD SetHeadByteDate(BYTE* pData);
	DWORD getKeyLength() { return (wcslen(m_szKey) + 1) * 2; }

		

	//int GetNodeByteLength()
	//{
	//	//GetHeadByteLength() + m_Padding + GetValueLength()  == m_pDataHead->wLength
	//	if (m_pDataHead)
	//	{
	//		return m_pDataHead->wLength + m_paddingPre;
	//	}
	//	return 0;
	//}

	//virtual int GetValueLength() { return 0; }


	//WCHAR* GetKey() { return m_szKey; }
	//void GetKey(WCHAR* szKey) { m_szKey = szKey; }

protected:
	WORD		m_paddingPre = 0;
	CDataHead*  m_pDataHead = NULL;
	WCHAR*      m_szKey = NULL;  //这个字段不能放CDataHead
	WORD		m_Padding = 0;
	BYTE*	    m_pData = NULL;
	DWORD       m_dwOffset = 0;

	CVersionConfig* m_pConfigParser = NULL;
};


class CStringDataGenerator : public CDataBaseGenerator
{
public:
	void SetConfigParser(CVersionConfig* pConfigParser, CStringDataNode& node, DWORD dwOffset);
	
	virtual void SetKey() {


		m_szKey = m_node.szKey;
	}

	int GetNodeByteLength()
	{
		//GetHeadByteLength() + m_Padding + GetValueLength()  == m_pDataHead->wLength
		if (m_pDataHead)
		{
			return m_paddingPre + sizeof(CDataHead) + (wcslen(m_szKey) + 1) * 2 + m_Padding + (wcslen(m_value) + 1) * 2;
		}
		return 0;
	}
	DWORD getValueLength() { return (wcslen(m_value) + 1) * 2; }

	void SetByteDate(BYTE* pData);


	WCHAR* GetValue() { return m_value; }
	void   SetValue(WCHAR* value) { m_value = value; }
private:
	WCHAR*  m_value = NULL;
	CStringDataNode m_node;
};


class CStringTableGenerator : public CDataBaseGenerator
{
public:
	void SetConfigParser(CVersionConfig* pConfigParser, DWORD dwOffset);
	virtual void SetKey() {
		if (m_pConfigParser)
		{
			wstring str = m_pConfigParser->GetString_LangAndCodepage();

			m_szKey = new WCHAR[str.size()+1];

			if (m_szKey)
			{
				wcscpy_s(m_szKey, str.size()+1, str.c_str());
			}


			//m_szKey = (WCHAR*)m_pConfigParser->GetString_LangAndCodepage().c_str();
		}
	}

	int GetNodeByteLength()
	{
		//GetHeadByteLength() + m_Padding + GetValueLength()  == m_pDataHead->wLength
		if (m_pDataHead)
		{
			return m_paddingPre + sizeof(CDataHead) + (wcslen(m_szKey) + 1) * 2 + m_Padding + m_dwValueLength;
		}
		return 0;
	}
	void SetByteDate(BYTE* pData);
private:
	vector<CStringDataGenerator*> m_vecStringData;
	DWORD  m_dwValueLength = 0;
};



class CStringFileInfoGenerator : public CDataBaseGenerator
{
public:
	void SetConfigParser(CVersionConfig* pConfigParser, DWORD dwOffset);
	virtual void SetKey() { m_szKey = KEY_StringFileInfo; }
	int GetNodeByteLength()
	{
		//GetHeadByteLength() + m_Padding + GetValueLength()  == m_pDataHead->wLength
		if (m_pDataHead && m_pStringTableGenerator)
		{
			return m_paddingPre + sizeof(CDataHead) + (wcslen(m_szKey) + 1) * 2 + m_Padding + m_pStringTableGenerator->GetNodeByteLength();
		}
		return 0;
	}
	void SetByteDate(BYTE* pData);
private:
	CStringTableGenerator* m_pStringTableGenerator = NULL;
};





class CVarDataGenerator : public CDataBaseGenerator
{
public:
	void SetConfigParser(CVersionConfig* pConfigParser, DWORD dwOffset);
	virtual void SetKey() { m_szKey = key_Var; }
	int GetNodeByteLength()
	{
		//GetHeadByteLength() + m_Padding + GetValueLength()  == m_pDataHead->wLength
		if (m_pDataHead)
		{
			return m_paddingPre + sizeof(CDataHead) + (wcslen(m_szKey) + 1) * 2 + m_Padding + sizeof(CLangAndCodepage);
		}
		return 0;
	}
	void SetByteDate(BYTE* pData);

private:
	CLangAndCodepage m_pLangAndCodepage;
};


class CVarFileInfoGenerator : public CDataBaseGenerator
{
public:
	void SetConfigParser(CVersionConfig* pConfigParser, DWORD dwOffset);
	virtual void SetKey() { m_szKey = key_VarFileInfo; }
	int GetNodeByteLength()
	{
		//GetHeadByteLength() + m_Padding + GetValueLength()  == m_pDataHead->wLength
		if (m_pDataHead && m_pVarData)
		{
			return m_paddingPre + sizeof(CDataHead) + (wcslen(m_szKey) + 1) * 2 + m_Padding + m_pVarData->GetNodeByteLength();
		}
		return 0;
	}

	void SetByteDate(BYTE* pData);

private:
	CVarDataGenerator* m_pVarData = NULL;
};



class CVersionInfoGenerator
{
public:
	CVersionInfoGenerator() {}
	~CVersionInfoGenerator() {}

	CBufferNode* GetByte();

	//void SetFixedFileInfo(vector<CDataNode>  vecDataNode) { m_vecFixedFileInfoDataNode = vecDataNode; }
	//void SetStringFileInfo(vector<CDataNode>  vecDataNode) { m_vecStringFileInfoDataNode = vecDataNode; }
	void SetVarFileInfo(CLangAndCodepage  langAndCodepage) { m_langAndCodepage = langAndCodepage; }
	void SetConfigParser(CVersionConfig* pConfigParser);




protected:
	//CByteNode _GenerateFixFileInfoByte();
	DWORD _GetValueOffset() {
		return m_paddingPre + sizeof(CVersionInfo);
	}


private:
	WORD			 m_paddingPre = 0;
	CVersionInfo*    m_pVersionInfo = NULL;
	BYTE*			 m_pVsVersionInfoData = NULL;
	BYTE*			 m_pData = NULL;     // 40+13*4 = 92

	CStringFileInfoGenerator* m_pStringFileInfoGenerator = NULL;
	CVarFileInfoGenerator* m_pVarFileInfoGenerator = NULL;

	CLangAndCodepage   m_langAndCodepage;
	CVersionConfig*     m_pConfigParser = NULL;
};