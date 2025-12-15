#pragma once

#include "EterPack.h"
#include "../eterBase/Singleton.h"
#include "../eterBase/Stl.h"

class CEterPackManager: public CSingleton<CEterPackManager>
{
	public:
		struct SCache
		{
			BYTE* m_abBufData;
			DWORD m_dwBufSize;
		};

	public:
		using TEterPackList = std::list<CEterPack*>;
		using TEterPackMap = std::unordered_map<std::string, CEterPack*>;

	public:
		CEterPackManager();
		virtual~CEterPackManager();

		bool Get (CMappedFile& rMappedFile, const char* c_szFileName, LPCVOID* pData);

		bool GetFromPack (CMappedFile& rMappedFile, const char* c_szFileName, LPCVOID* pData);
		bool GetFromFile (CMappedFile& rMappedFile, const char* c_szFileName, LPCVOID* pData);

		bool isExist (const char* c_szFileName);
		bool isExistInPack (const char* c_szFileName);

		bool RegisterPack (const char* c_szName, const char* c_szDirectory);
		void RegisterRootPack (const char* c_szName);

	protected:
		int ConvertFileName (const char* c_szFileName, std::string& rstrFileName);
		SCache* FindCache (DWORD dwFileNameHash);
		void ClearCacheMap();

	protected:
		CEterFileDict m_FileDict;
		CEterPack m_RootPack;
		TEterPackList m_PackList;
		TEterPackMap m_PackMap;
		TEterPackMap m_DirPackMap;

		std::unordered_map<DWORD, SCache> m_kMap_dwNameKey_kCache;
		CRITICAL_SECTION m_csFinder;
};