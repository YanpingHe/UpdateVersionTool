#include "stdafx.h"

#include "VersionInfo.h"
#include "VersionInfoStruct.h"



CVersionInfoParser::CVersionInfoParser()
{
}

CVersionInfoParser::~CVersionInfoParser()
{
}


void CVersionInfoParser::Parse(BYTE* pData)
{
	if (!pData)
	{
		return;
	}

	m_paddingPre = GetAlignPadding(pData);
	m_pVsVersionInfoData = pData+m_paddingPre;
	m_pVersionInfo = (CVersionInfo*)(pData+ m_paddingPre);
	
	if (wcscmp(m_pVersionInfo->szKey, KEY_VsVersionInfo) != 0)
	{
		return;
	}


	ParseFixedFileInfo();

	m_pData = pData + m_paddingPre + sizeof(CVersionInfo);


	int iTotalSize = m_pVersionInfo->wLength;
	int iReadSize = m_paddingPre + sizeof(CVersionInfo);


	while (iReadSize < iTotalSize)
	{
		int padding = GetAlignPadding(pData + iReadSize);
		iReadSize += padding;
		CDataHead* pHead = (CDataHead*)(pData + iReadSize);
		WCHAR* szKey = (WCHAR*)(pData + iReadSize + sizeof(CDataHead));

		if (wcscmp(szKey, KEY_StringFileInfo) == 0)
		{
			CStringFileInfo* pStringFileInfo = new CStringFileInfo();
			if (pStringFileInfo)
			{
				//m_vecStringFileInfo.push_back(pStringFileInfo);
				pStringFileInfo->Parse((BYTE*)pHead);
			}

			iReadSize += pStringFileInfo->GetNodeByteLength();
		}

		if (wcscmp(szKey, key_VarFileInfo) == 0)
		{
			CVarFileInfo* pVarFileInfo = new CVarFileInfo();
			if (pVarFileInfo)
			{

				//m_vecVarFileInfo.push_back(pVarFileInfo);
				pVarFileInfo->Parse((BYTE*)pHead);
			}

			iReadSize += pVarFileInfo->GetNodeByteLength();
		}

	}
}

void CVersionInfoParser::ParseFixedFileInfo()
{
	if (!m_pVersionInfo)
	{
		return;
	}

	/*typedef struct tagVS_FIXEDFILEINFO {
		DWORD dwSignature;
		DWORD dwStrucVersion;
		DWORD dwFileVersionMS;
		DWORD dwFileVersionLS;
		DWORD dwProductVersionMS;
		DWORD dwProductVersionLS;
		DWORD dwFileFlagsMask;
		DWORD dwFileFlags;
		DWORD dwFileOS;
		DWORD dwFileType;
		DWORD dwFileSubtype;
		DWORD dwFileDateMS;
		DWORD dwFileDateLS;
	} VS_FIXEDFILEINFO;*/


	VS_FIXEDFILEINFO info = m_pVersionInfo->Value;
	wprintf(L"dwSignature: %x \n", info.dwSignature);
	wprintf(L"dwStrucVersion: %x \n", info.dwStrucVersion);
	wprintf(L"dwFileVersionMS: %d.%d.%d.%d\n", info.dwFileVersionMS >> 16, info.dwFileVersionMS & 0x0000FFFF, info.dwFileVersionLS>>16, info.dwFileVersionLS & 0x0000FFFF);
	wprintf(L"dwProductVersionMS: %d.%d.%d.%d\n", info.dwProductVersionMS >> 16, info.dwProductVersionMS & 0x0000FFFF, info.dwProductVersionLS >> 16, info.dwProductVersionLS & 0x0000FFFF);
	
	wprintf(L"dwFileFlagsMask: %d \n", info.dwFileFlagsMask);
	wprintf(L"dwFileFlags: %d \n", info.dwFileFlags);
	wprintf(L"dwFileOS: %d \n", info.dwFileOS);
	wprintf(L"dwFileType: %d \n", info.dwFileType);
	wprintf(L"dwFileSubtype: %d \n", info.dwFileSubtype);
	wprintf(L"dwFileDateMS: %d-%d \n", info.dwFileDateMS, info.dwFileDateLS);


}

void CStringFileInfo::Parse(BYTE* pData)
{
	__super::Parse(pData);

	//int iTotalSize = m_pDataHead->wLength;
	//int iReadSize = m_paddingPre + GetHeadByteLength() + m_Padding;

	//while (iReadSize < iTotalSize)
	//{
	//	CStringTable* pStringTable = new CStringTable;
	//	if (pStringTable)
	//	{
	//		m_vecStringTable.push_back(pStringTable);

	//		
	//		pStringTable->Parse((BYTE*)(pData + iReadSize));
	//		iReadSize += pStringTable->GetNodeByteLength();
	//	}
	//}


	m_pStringTable = new CStringTable;
	if (m_pStringTable)
	{
		int iReadSize = m_paddingPre + GetHeadByteLength() + m_Padding;
		m_pStringTable->Parse((BYTE*)(pData + iReadSize));
	}
}

void CVarStringData::Parse(BYTE* pData)
{
	__super::Parse(pData);

	m_value = (WCHAR*)(pData + m_paddingPre + GetHeadByteLength() + m_Padding);
}

void CDataBase::Parse(BYTE* pData)
{
	m_paddingPre = GetAlignPadding(pData);
	m_pDataHead = (CDataHead*)(pData+m_paddingPre);
	m_pData = pData;

	m_szKey = (WCHAR*)(pData + m_paddingPre + sizeof(CDataHead));
	
	int size = GetHeadByteLength();
	m_Padding = GetAlignPadding(pData + m_paddingPre + size);



	wprintf(L"key:%s  type:%d\n", m_szKey, m_pDataHead->wType);

}



void CStringTable::Parse(BYTE* pData)
{
	__super::Parse(pData);

	int iTotalSize = m_pDataHead->wLength;
	int iReadSize = GetHeadByteLength() + m_Padding;

	wprintf(L"StringTable: %s\n", m_szKey);

	while (iReadSize < iTotalSize)
	{
		CVarStringData* pStringData = new CVarStringData();
		if (pStringData)
		{
			m_vecStringData.push_back(pStringData);
			pStringData->Parse((BYTE*)(pData + m_paddingPre + iReadSize));

			wprintf(L"%-20s:%s \n", pStringData->GetKey(), pStringData->GetValue());

			iReadSize += pStringData->GetNodeByteLength();
		}
	}
}

void CVarFileInfo::Parse(BYTE* pData)
{
	__super::Parse(pData);

	wprintf(L"StringTable: %s\n", m_szKey);

	m_pVarData = new CVarData();
	if (m_pVarData)
	{
		int iReadSize = m_paddingPre + GetHeadByteLength() + m_Padding;
		m_pVarData->Parse((BYTE*)(pData + iReadSize));
	}
}

void CVarData::Parse(BYTE* pData)
{
	__super::Parse(pData);

	int iTotalSize = m_pDataHead->wLength;
	int iReadSize =  GetHeadByteLength() + m_Padding;

	wprintf(L"StringTable: %s\n", m_szKey);

	while (iReadSize < iTotalSize)
	{
		CLangAndCodepage* pLangAndCodepage = new CLangAndCodepage();
		if (pLangAndCodepage)
		{
			m_vecLangAndCodepage.push_back(pLangAndCodepage);
			pLangAndCodepage->SetLang(*(WORD*)(pData + m_paddingPre + iReadSize));
			iReadSize += sizeof(WORD);
			pLangAndCodepage->SetCodepage(*(WORD*)(pData + m_paddingPre + iReadSize));
			iReadSize += sizeof(WORD);

			wprintf(L"%04x %04x \n", pLangAndCodepage->GetLang(), pLangAndCodepage->GetCodepage());
		}
	}
}
