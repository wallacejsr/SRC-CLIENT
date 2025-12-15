#include "StdAfx.h"
#include "../eterPack/EterPackManager.h"

#include "PropertyManager.h"
#include "Property.h"

CPropertyManager::CPropertyManager() : m_isFileMode (true)
{
}

CPropertyManager::~CPropertyManager()
{
	Clear();
}

bool CPropertyManager::Initialize (const char* c_pszPackFileName)
{
	if (c_pszPackFileName)
	{
		if (!m_pack.Create (m_fileDict, c_pszPackFileName, "", true))
		{
			LogBoxf ("Cannot open property pack file (filename %s)", c_pszPackFileName);
			return false;
		}

		m_isFileMode = false;

		TDataPositionMap& indexMap = m_pack.GetIndexMap();

		TDataPositionMap::iterator itor = indexMap.begin();

		typedef std::map<DWORD, TEterPackIndex*> TDataPositionMap;

		while (indexMap.end() != itor)
		{
			TEterPackIndex* pIndex = itor->second;
			++itor;

			if (!_stricmp ("property/reserve", pIndex->filename))
			{
				LoadReservedCRC (pIndex->filename);
				continue;
			}

			if (!Register (pIndex->filename))
			{
				continue;
			}
		}
	}
	else
	{
		m_isFileMode = true;
	}

	return true;
}

bool CPropertyManager::LoadReservedCRC (const char* c_pszFileName)
{
	CMappedFile file;
	LPCVOID c_pvData;

	if (!CEterPackManager::Instance().Get (file, c_pszFileName, &c_pvData))
	{
		return false;
	}

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind (file.Size(), c_pvData);

	for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		const char* pszLine = textFileLoader.GetLineString (i).c_str();

		if (!pszLine || !*pszLine)
		{
			continue;
		}

		ReserveCRC (_atoi64 (pszLine));
	}

	return true;
}

void CPropertyManager::ReserveCRC (DWORD dwCRC)
{
	m_ReservedCRCSet.insert (dwCRC);
}

DWORD CPropertyManager::GetUniqueCRC (const char* c_szSeed)
{
	std::string stTmp = c_szSeed;

	while (1)
	{
		DWORD dwCRC = GetCRC32 (stTmp.c_str(), stTmp.length());

		if (m_ReservedCRCSet.find (dwCRC) == m_ReservedCRCSet.end() &&
				m_PropertyByCRCMap.find (dwCRC) == m_PropertyByCRCMap.end())
		{
			return dwCRC;
		}

		char szAdd[2];
		_snprintf (szAdd, sizeof (szAdd), "%d", random() % 10);
		stTmp += szAdd;
	}
}

bool CPropertyManager::Register (const char* c_pszFileName, CProperty** ppProperty)
{
	CMappedFile file;
	LPCVOID		c_pvData;

	if (!CEterPackManager::Instance().Get (file, c_pszFileName, &c_pvData))
	{
		return false;
	}

	CProperty* pProperty = new CProperty (c_pszFileName);

	if (!pProperty->ReadFromMemory (c_pvData, file.Size(), c_pszFileName))
	{
		delete pProperty;
		return false;
	}

	DWORD dwCRC = pProperty->GetCRC();

	TPropertyCRCMap::iterator itor = m_PropertyByCRCMap.find (dwCRC);

	if (m_PropertyByCRCMap.end() != itor)
	{
		Tracef ("Property already registered, replace %s to %s\n",
				itor->second->GetFileName(),
				c_pszFileName);

		delete itor->second;
		itor->second = pProperty;
	}
	else
	{
		m_PropertyByCRCMap.insert (TPropertyCRCMap::value_type (dwCRC, pProperty));
	}

	if (ppProperty)
	{
		*ppProperty = pProperty;
	}

	return true;
}

bool CPropertyManager::Get (const char* c_pszFileName, CProperty** ppProperty)
{
	return Register (c_pszFileName, ppProperty);
}

bool CPropertyManager::Get (DWORD dwCRC, CProperty** ppProperty)
{
	TPropertyCRCMap::iterator itor = m_PropertyByCRCMap.find (dwCRC);

	if (m_PropertyByCRCMap.end() == itor)
	{
		return false;
	}

	*ppProperty = itor->second;
	return true;
}

void CPropertyManager::Clear()
{
	stl_wipe_second (m_PropertyByCRCMap);
}