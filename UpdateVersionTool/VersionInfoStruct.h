#pragma once
#include <wtypes.h>
#include <vector>
#include <string>

#include "ConstantDefine.h"

using namespace std;




int GetAlignPadding(BYTE* pData);
int GetAlignPadding(DWORD dwOffset);
void SplitString(std::wstring& wstr, vector<std::wstring>& vecWstr, std::wstring& token);
std::wstring Utf8ToWS(const char * str, int nLen = -1);
std::string WSToUtf8(const std::wstring& wide);
BOOL CheckFileExist(LPCTSTR strFile);
void ErrorHandler(TCHAR* pinfo); 
std::wstring  GetAppdataPath();





struct  CBufferNode
{
	BYTE*            pData = NULL;
	DWORD            size = 0;
};



struct  CVersionNameAndLang
{
	wstring wstrNameID ;
	DWORD   dwLangID = 0;
};

typedef enum _enValueType
{
	ValueType_Binary = 0,
	ValueType_Text = 1
}ValueType;


struct  CVersionInfo
{
	WORD             wLength = 0;        // 00 
	WORD             wValueLength = 0;   // 02 
	WORD             wType = 0;          // 04 
	WCHAR            szKey[16] = KEY_VsVersionInfo;      // 06 
	WORD             Padding1 = 0;       // 38 Value 4btye align ռλ2byte
	VS_FIXEDFILEINFO Value;				 // 40    13 DWORD 
	//BYTE*			 pData = NULL;     // 40+13*4 = 92
};


struct CDataHead
{
	WORD        wLength = 0;			// 0
	WORD        wValueLength = 0;		//2
	WORD        wType = 0;				//4
};


class CLangAndCodepage
{
public:
	CLangAndCodepage() {}
	void SetLangAndCodepage(WORD wLanguage, WORD wCodePage)
	{
		m_wLanguage = wLanguage;
		m_wCodePage = wCodePage;
	}

	~CLangAndCodepage() {}

	WORD GetLang() { return m_wLanguage; }
	void SetLang(WORD value) { m_wLanguage = value; }
	WORD GetCodepage() { return m_wCodePage; }
	void SetCodepage(WORD value) { m_wCodePage = value; }
	wstring GetLangAndCodepage();
private:
	WORD m_wLanguage = 0;
	WORD m_wCodePage = 0;
};


class CVersionFormat
{
public:
	CVersionFormat() {}
	void SetVersionFormat(wstring& wstrVersion);

	DWORD GetMSVersion() 
	{
		return (m_MSVersionHeight << 16) + m_MSVersionLow;
	}
	DWORD GetLSVersion()
	{
		return (m_LSVersionHeight << 16) + m_LSVersionLow;
	}

	wstring GetVersionString();

private:
	wstring m_wstrVersion;

	WORD m_MSVersionHeight = 0;
	WORD m_MSVersionLow = 0;
	WORD m_LSVersionHeight = 0;
	WORD m_LSVersionLow = 0;
};



struct  CStringDataNode
{
	WCHAR            *szKey;
	WCHAR            *szValue ;
};