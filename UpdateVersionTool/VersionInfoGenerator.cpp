#include "stdafx.h"
#include "VersionInfoGenerator.h"
#include "VersionInfo.h"

CBufferNode* CVersionInfoGenerator::GetByte()
{
	CBufferNode *pBufNode = new CBufferNode();
	if (!pBufNode)
	{
		return NULL;
	}
		
	if (!(m_pStringFileInfoGenerator && m_pVarFileInfoGenerator && m_pVersionInfo && m_pConfigParser))
	{
		return NULL;
	}
	int iByteTotal = _GetValueOffset() + m_pStringFileInfoGenerator->GetNodeByteLength() + m_pVarFileInfoGenerator->GetNodeByteLength();


	BYTE* pBuff = new BYTE[iByteTotal];
	if (pBuff)
	{
		DWORD dwOffset = 0;
		ZeroMemory(pBuff, iByteTotal);
		CopyMemory((pBuff + dwOffset), m_pVersionInfo, sizeof(CVersionInfo));

		dwOffset += sizeof(CVersionInfo);
		if (m_pStringFileInfoGenerator)
		{
			m_pStringFileInfoGenerator->SetByteDate(pBuff + dwOffset);
			dwOffset += m_pStringFileInfoGenerator->GetNodeByteLength();
		}

		if (m_pVarFileInfoGenerator)
		{
			m_pVarFileInfoGenerator->SetByteDate(pBuff + dwOffset);
			dwOffset += m_pVarFileInfoGenerator->GetNodeByteLength();
		}
	}

	pBufNode->pData = pBuff;
	pBufNode->size = iByteTotal;

	return pBufNode;
}

void CVersionInfoGenerator::SetConfigParser(CVersionConfig* pConfigParser)
{
	m_pConfigParser = pConfigParser; 
	if (!m_pVersionInfo)
	{
		m_pVersionInfo = new CVersionInfo();
	}

	/*CStringFileInfoGenerator* m_pStringFileInfoGenerator = NULL;
	CVarFileInfoGenerator* m_pVarFileInfoGenerator = NULL;*/

	if (!m_pStringFileInfoGenerator)
	{
		m_pStringFileInfoGenerator = new CStringFileInfoGenerator();
	}
	if (m_pStringFileInfoGenerator)
	{
		m_pStringFileInfoGenerator->SetConfigParser(m_pConfigParser, _GetValueOffset());
	}


	if (!m_pVarFileInfoGenerator)
	{
		m_pVarFileInfoGenerator = new CVarFileInfoGenerator();
	}
	if (m_pVarFileInfoGenerator && m_pStringFileInfoGenerator)
	{
		m_pVarFileInfoGenerator->SetConfigParser(m_pConfigParser, _GetValueOffset() + m_pStringFileInfoGenerator->GetNodeByteLength());
	}


	if (m_pVarFileInfoGenerator && m_pStringFileInfoGenerator)
	{
		if (m_pVersionInfo)
		{
			m_pVersionInfo->wLength = WORD(_GetValueOffset() + m_pStringFileInfoGenerator->GetNodeByteLength() + m_pVarFileInfoGenerator->GetNodeByteLength());
			m_pVersionInfo->wValueLength = sizeof(VS_FIXEDFILEINFO);
			m_pVersionInfo->wType = ValueType_Binary;
			
			m_pVersionInfo->Value.dwSignature = 0xFEEF04BD;
			m_pVersionInfo->Value.dwStrucVersion = 0x10000;
			m_pVersionInfo->Value.dwFileVersionMS = m_pConfigParser->GetFileVersionMS();
			m_pVersionInfo->Value.dwFileVersionLS = m_pConfigParser->GetFileVersionLS();
			m_pVersionInfo->Value.dwProductVersionMS = m_pConfigParser->GetProductVersionMS();
			m_pVersionInfo->Value.dwProductVersionLS = m_pConfigParser->GetProductVersionLS();
			m_pVersionInfo->Value.dwFileFlagsMask = m_pConfigParser->GetDword_FileFlagsMask();
			m_pVersionInfo->Value.dwFileFlags = m_pConfigParser->GetDword_FileFlags();
			m_pVersionInfo->Value.dwFileOS = m_pConfigParser->GetDword_FileOS();
			m_pVersionInfo->Value.dwFileType = m_pConfigParser->GetDword_FileType();
			m_pVersionInfo->Value.dwFileSubtype = m_pConfigParser->GetDword_FileSubtype();
			m_pVersionInfo->Value.dwFileDateMS = m_pConfigParser->GetDword_FileDateMS();
			m_pVersionInfo->Value.dwFileDateLS = m_pConfigParser->GetDword_FileDateLS();

		}
	}



}

//CByteNode CVersionInfoGenerator::_GenerateFixFileInfoByte()
//{	
//	CByteNode   node;
//
//	if (!(m_pConfigParser && m_pVersionInfo) )
//	{
//		return node;
//	}
//		
//	VS_FIXEDFILEINFO* pBuff = new VS_FIXEDFILEINFO;
//	if (pBuff)
//	{
//		*pBuff = m_pVersionInfo->Value;
//		node.pData = (BYTE*)pBuff;
//		node.size = sizeof(VS_FIXEDFILEINFO);
//	}
//
//	return node;
//}

void CStringFileInfoGenerator::SetConfigParser(CVersionConfig* pConfigParser, DWORD dwOffset)
{
	__super::SetConfigParser(pConfigParser, dwOffset);


	if (!m_pStringTableGenerator)
	{
		m_pStringTableGenerator = new CStringTableGenerator();
	}

	if (m_pStringTableGenerator)
	{
		m_pStringTableGenerator->SetConfigParser(pConfigParser, m_dwOffset + GetValueOffset());
	}


	if (m_pDataHead)
	{
		m_pDataHead->wLength = (WORD)(sizeof(CDataHead) + (wcslen(m_szKey) + 1) * 2 + m_Padding + m_pStringTableGenerator->GetNodeByteLength());
		m_pDataHead->wValueLength = 0;
		m_pDataHead->wType = ValueType_Text;
	}
}

void CStringFileInfoGenerator::SetByteDate(BYTE* pData)
{
	DWORD dwCurOffset = SetHeadByteDate(pData);


	if (m_pStringTableGenerator)
	{
		m_pStringTableGenerator->SetByteDate(pData + dwCurOffset);
	}
}

void CDataBaseGenerator::SetConfigParser(CVersionConfig* pConfigParser, DWORD dwOffset)
{
	m_pDataHead = new CDataHead();
	m_pConfigParser = pConfigParser;
	m_dwOffset = dwOffset;

	m_paddingPre = GetAlignPadding(dwOffset);
	
	//调用子类来设置
	SetKey();

	m_Padding = GetAlignPadding(dwOffset + m_paddingPre + GetHeadByteLength());

}
DWORD CDataBaseGenerator::SetHeadByteDate(BYTE* pData)
{
	DWORD dwCurOffset = m_paddingPre;

	CopyMemory(pData + dwCurOffset, m_pDataHead, sizeof(CDataHead));
	dwCurOffset += sizeof(CDataHead);

	CopyMemory(pData + dwCurOffset, m_szKey, getKeyLength());
	dwCurOffset += getKeyLength();

	dwCurOffset += m_Padding;

	return dwCurOffset;
}

void CStringTableGenerator::SetConfigParser(CVersionConfig* pConfigParser, DWORD dwOffset)
{
	__super::SetConfigParser(pConfigParser, dwOffset);

	int dwValueOffset = m_dwOffset + GetValueOffset();
	m_dwValueLength = 0;
	vector<CStringDataNode> vecNode = m_pConfigParser->GetStringDataNodeList();
	for (vector<CStringDataNode>::iterator it = vecNode.begin(); it < vecNode.end(); ++it)
	{
		CStringDataGenerator* pStringDataGenerator = new CStringDataGenerator();
		if (pStringDataGenerator)
		{
			m_vecStringData.push_back(pStringDataGenerator);

			pStringDataGenerator->SetConfigParser(pConfigParser, *it, dwValueOffset + m_dwValueLength);

			m_dwValueLength += pStringDataGenerator->GetNodeByteLength();
		}
	}


	if (m_pDataHead)
	{
		m_pDataHead->wLength = sizeof(CDataHead) + (wcslen(m_szKey) + 1) * 2 + m_Padding + m_dwValueLength;
		m_pDataHead->wValueLength = 0;
		m_pDataHead->wType = ValueType_Text;
	}

}


void CStringTableGenerator::SetByteDate(BYTE* pData)
{
	DWORD dwCurOffset = SetHeadByteDate(pData);

	for (vector<CStringDataGenerator*>::iterator it = m_vecStringData.begin(); it < m_vecStringData.end(); ++it)
	{
		if (*it)
		{
			(*it)->SetByteDate(pData + dwCurOffset);
			dwCurOffset += (*it)->GetNodeByteLength();
		}
	}
}

void CStringDataGenerator::SetConfigParser(CVersionConfig* pConfigParser, CStringDataNode& node, DWORD dwOffset)
{
	m_node = node;
	m_szKey = m_node.szKey;
	m_value = m_node.szValue;

	__super::SetConfigParser(pConfigParser, dwOffset);

	if (m_pDataHead)
	{
		m_pDataHead->wLength = sizeof(CDataHead) + (wcslen(m_szKey) + 1) * 2 + m_Padding + (wcslen(m_value) + 1) * 2;
		m_pDataHead->wValueLength = (wcslen(m_value) + 1);
		m_pDataHead->wType = ValueType_Text;
	}



	//wprintf(L"StringData: %s:%s\n", m_szKey, m_value);

}

void CStringDataGenerator::SetByteDate(BYTE* pData)
{
	DWORD dwCurOffset = SetHeadByteDate(pData);
	
	CopyMemory(pData + dwCurOffset, m_value, getValueLength());
	dwCurOffset += getValueLength();
}

void CVarFileInfoGenerator::SetConfigParser(CVersionConfig* pConfigParser, DWORD dwOffset)
{
	__super::SetConfigParser(pConfigParser, dwOffset);

	if (!m_pVarData)
	{
		m_pVarData = new CVarDataGenerator();
	}

	if (m_pVarData)
	{
		m_pVarData->SetConfigParser(pConfigParser, dwOffset + GetValueOffset());
	}

	if (m_pDataHead && m_pVarData)
	{
		m_pDataHead->wLength = sizeof(CDataHead) + (wcslen(m_szKey) + 1) * 2 + m_Padding + m_pVarData->GetNodeByteLength();
		m_pDataHead->wValueLength = 0;
		m_pDataHead->wType = ValueType_Text;
	}
}

void CVarFileInfoGenerator::SetByteDate(BYTE* pData)
{
	DWORD dwCurOffset = SetHeadByteDate(pData);
	if (m_pVarData)
	{
		m_pVarData->SetByteDate(pData + dwCurOffset);
	}
}

void CVarDataGenerator::SetConfigParser(CVersionConfig* pConfigParser, DWORD dwOffset)
{
	__super::SetConfigParser(pConfigParser, dwOffset);

	if (pConfigParser)
	{
		m_pLangAndCodepage.SetLang(pConfigParser->GetWord_Lang());
		m_pLangAndCodepage.SetCodepage(pConfigParser->GetWord_Codepage());
	}

	if (m_pDataHead)
	{
		m_pDataHead->wLength = sizeof(CDataHead) + (wcslen(m_szKey) + 1) * 2 + m_Padding + sizeof(CLangAndCodepage);
		m_pDataHead->wValueLength = sizeof(CLangAndCodepage);
		m_pDataHead->wType = ValueType_Binary;
	}

}

void CVarDataGenerator::SetByteDate(BYTE* pData)
{
	DWORD dwCurOffset = SetHeadByteDate(pData);

	CopyMemory(pData + dwCurOffset, &m_pLangAndCodepage, sizeof(CLangAndCodepage));
	dwCurOffset += sizeof(CLangAndCodepage);
}
