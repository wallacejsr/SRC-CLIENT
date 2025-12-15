#include "StdAfx.h"
#include "EterPackManager.h"

#include "../eterBase/Debug.h"
#include "../eterBase/CRC32.h"
//#include <io.h> // 2k13
#include <corecrt_io.h> // 2k19

int CEterPackManager::ConvertFileName (const char* c_szFileName, std::string& rstrFileName)
{
	rstrFileName = c_szFileName;
	stl_lowers (rstrFileName);
	int iCount = 0;

	for (DWORD i = 0; i < rstrFileName.length(); ++i)
	{
		if (rstrFileName[i] == '/')
		{
			++iCount;
		}
		else if (rstrFileName[i] == '\\')
		{
			rstrFileName[i] = '/';
			++iCount;
		}
	}

	return iCount;
}

CEterPackManager::SCache* CEterPackManager::FindCache (DWORD dwFileNameHash)
{
	auto it = m_kMap_dwNameKey_kCache.find (dwFileNameHash);
	if (m_kMap_dwNameKey_kCache.end() == it)
	{
		return NULL;
	}

	return &it->second;
}

void CEterPackManager::ClearCacheMap()
{
	for (auto it = m_kMap_dwNameKey_kCache.begin(); it != m_kMap_dwNameKey_kCache.end(); ++it)
	{
		delete [] it->second.m_abBufData;
	}

	m_kMap_dwNameKey_kCache.clear();
}

bool CEterPackManager::Get (CMappedFile& rMappedFile, const char* c_szFileName, LPCVOID* pData)
{
	if (GetFromPack (rMappedFile, c_szFileName, pData))
	{
		return true;
	}

	#ifndef FILES_ONLY_FROM_PACK
	if (GetFromFile (rMappedFile, c_szFileName, pData))
	{
		return true;
	}
	#endif

	#ifdef _DEBUG
	TraceError ("CANNOT_FIND_FILE [%s]", c_szFileName);
	#endif

	return false;
}

struct FinderLock
{
	FinderLock (CRITICAL_SECTION& cs) : p_cs (&cs)
	{
		EnterCriticalSection (p_cs);
	}

	~FinderLock()
	{
		LeaveCriticalSection (p_cs);
	}

	CRITICAL_SECTION* p_cs;
};

bool CEterPackManager::GetFromPack (CMappedFile& rMappedFile, const char* c_szFileName, LPCVOID* pData)
{
	FinderLock lock (m_csFinder);
	static std::string strFileName;

	if (!ConvertFileName (c_szFileName, strFileName))
	{
		return m_RootPack.Get (rMappedFile, strFileName.c_str(), pData);
	}
	else
	{
		DWORD dwFileNameHash = GetCRC32 (strFileName.c_str(), strFileName.length());
		SCache* pkCache = FindCache (dwFileNameHash);

		if (pkCache)
		{
			rMappedFile.Link (pkCache->m_dwBufSize, pkCache->m_abBufData);
			return true;
		}

		CEterFileDict::Item* pkFileItem = m_FileDict.GetItem (dwFileNameHash, strFileName.c_str());
		if (pkFileItem)
		{
			if (pkFileItem->pkPack)
			{
				return pkFileItem->pkPack->Get (rMappedFile, strFileName.c_str(), pkFileItem->pkInfo, pData);
			}
		}
	}

	return false;
}

bool CEterPackManager::GetFromFile (CMappedFile& rMappedFile, const char* c_szFileName, LPCVOID* pData)
{
	return rMappedFile.Create (c_szFileName, pData, 0, 0) ? true : false;
}

bool CEterPackManager::isExistInPack (const char* c_szFileName)
{
	std::string strFileName;

	if (!ConvertFileName (c_szFileName, strFileName))
	{
		return m_RootPack.IsExist (strFileName.c_str());
	}
	else
	{
		DWORD dwFileNameHash = GetCRC32 (strFileName.c_str(), strFileName.length());
		CEterFileDict::Item* pkFileItem = m_FileDict.GetItem (dwFileNameHash, strFileName.c_str());

		if (pkFileItem)
		{
			if (pkFileItem->pkPack)
			{
				return pkFileItem->pkPack->IsExist (strFileName.c_str());
			}
		}
	}

	return false;
}

bool CEterPackManager::isExist (const char* c_szFileName)
{
	if (isExistInPack (c_szFileName))
	{
		return true;
	}

	return _access (c_szFileName, 0) == 0 ? true : false;
}

void CEterPackManager::RegisterRootPack (const char* c_szName)
{
	if (!m_RootPack.Create (m_FileDict, c_szName, ""))
	{
		TraceError ("%s: Pack file does not exist", c_szName);
	}
}

bool CEterPackManager::RegisterPack (const char* c_szName, const char* c_szDirectory)
{
	CEterPack* pEterPack = NULL;
	{
		auto itor = m_PackMap.find (c_szName);

		if (m_PackMap.end() == itor)
		{
			pEterPack = new CEterPack;
			if (pEterPack->Create (m_FileDict, c_szName, c_szDirectory, true))
			{
				m_PackMap.insert (TEterPackMap::value_type (c_szName, pEterPack));
			}
			else
			{
				#ifdef _DEBUG
				Tracef ("The eterpack doesn't exist [%s]\n", c_szName);
				#endif
				delete pEterPack;
				pEterPack = NULL;
				return false;
			}
		}
		else
		{
			pEterPack = itor->second;
		}
	}

	if (c_szDirectory && c_szDirectory[0] != '*')
	{
		auto itor = m_DirPackMap.find (c_szDirectory);
		if (m_DirPackMap.end() == itor)
		{
			m_PackList.push_front (pEterPack);
			m_DirPackMap.insert (TEterPackMap::value_type (c_szDirectory, pEterPack));
		}
	}

	return true;
}

CEterPackManager::CEterPackManager()
{
	InitializeCriticalSection (&m_csFinder);
}

CEterPackManager::~CEterPackManager()
{
	ClearCacheMap();
	auto it = m_PackMap.begin();
	auto et = m_PackMap.end();

	while (it != et)
	{
		delete it->second;
		it++;
	}

	DeleteCriticalSection (&m_csFinder);
}