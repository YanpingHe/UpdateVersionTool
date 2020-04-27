#pragma once
#include <wtypes.h>
#include <vector>
#include "ConstantDefine.h"
#include "VersionInfoStruct.h"

using namespace std;

class CVersionInfoParser
{
public:
	CVersionInfoParser();
	~CVersionInfoParser();

	void Parse(BYTE* pData);
	void ParseFixedFileInfo();
private:
	WORD			 m_paddingPre = 0;
	CVersionInfo*    m_pVersionInfo = NULL;
	BYTE*			 m_pVsVersionInfoData = NULL;
	BYTE*			 m_pData = NULL;     // 40+13*4 = 92
};





//typedef struct {
//	WORD   wLength;
//	WORD   wValueLength;
//	WORD   wType;
//	WCHAR  szKey;
//	WORD   Padding;
//	String Children;
//} StringTable;




class CDataBase
{
public:
	void Parse(BYTE* pData);
	int GetHeadByteLength()
	{
		return sizeof(CDataHead) + (wcslen(m_szKey) + 1) * 2;
	}
	int GetNodeByteLength()
	{
		//GetHeadByteLength() + m_Padding + GetValueLength()  == m_pDataHead->wLength
		if (m_pDataHead)
		{
			return m_pDataHead->wLength + m_paddingPre;
		}
		return 0;
	}

	virtual int GetValueLength() { return 0; }
	

	WCHAR* GetKey() { return m_szKey; }
	void GetKey(WCHAR* szKey) { m_szKey = szKey; }

protected:
	WORD		m_paddingPre = 0;
	CDataHead*  m_pDataHead = NULL;
	WCHAR*      m_szKey = NULL;  //这个字段不能放CDataHead
	WORD		m_Padding = 0;
	BYTE*	    m_pData = NULL;
};


class CVarStringData : public CDataBase
{
public:
	void Parse(BYTE* pData);
	virtual int GetValueLength()
	{
		return (wcslen(m_value) + 1) * 2;
	}

	WCHAR* GetValue() { return m_value; }
	void   SetValue(WCHAR* value) { m_value = value; }
private:
	WCHAR*  m_value = NULL;
};

class CStringTable : public CDataBase
{
public:
	void Parse(BYTE* pData);

private:
	vector<CVarStringData*> m_vecStringData;
};

class CStringFileInfo : public CDataBase
{
public:
	void Parse(BYTE* pData);

private:
	CStringTable* m_pStringTable;
};


class CVarData : public CDataBase
{
public:
	void Parse(BYTE* pData);
	virtual int GetValueLength()
	{
		return m_vecLangAndCodepage.size() * sizeof(CLangAndCodepage);
	}

private:
	vector<CLangAndCodepage*> m_vecLangAndCodepage;
};


class CVarFileInfo : public CDataBase
{
public:
	void Parse(BYTE* pData);

private:
	CVarData* m_pVarData = NULL;
};
