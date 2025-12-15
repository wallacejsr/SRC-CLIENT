#pragma once

#include <unordered_map>
#include <cstdint>
#include <array>

#include "../EterBase/MappedFile.h"

struct TeaKey
{
		TeaKey (std::array<DWORD, 4>src): data_ (src), copy (src) {}

		operator const std::uint8_t* ()const
		{
			return reinterpret_cast<const std::uint8_t*> (data_.data());
		}

		operator const DWORD* ()const
		{
			return data_.data();
		}

		operator DWORD* ()const
		{
			return copy.data();
		}

	private:
		std::array<DWORD, 4>data_;
		mutable std::array<DWORD, 4>copy;
};

const TeaKey& s_adwEterPackKey();
const TeaKey& s_adwEterPackSecurityKey();

namespace eterpack
{
	const DWORD c_PackCC = MAKEFOURCC ('E', 'P', 'K', 'D');
	const DWORD c_IndexCC = MAKEFOURCC ('E', 'P', 'K', 'D');
	const DWORD c_Version = 2;
	const DWORD c_HeaderSize = sizeof (DWORD) + sizeof (DWORD) + sizeof (long);
};

enum EEterPackTypes
{
	DBNAME_MAX_LEN = 255,
	FILENAME_MAX_LEN = 160,
	FREE_INDEX_BLOCK_SIZE = 32768,
	FREE_INDEX_MAX_SIZE = 512,
	DATA_BLOCK_SIZE = 256,

	COMPRESSED_TYPE_NONE = 0,
	COMPRESSED_TYPE_COMPRESS = 1,
	COMPRESSED_TYPE_SECURITY = 2,
	COMPRESSED_TYPE_COUNT = 3,
};

#pragma pack(push, 4)
typedef struct SEterPackIndex
{
	long id;
	char filename[FILENAME_MAX_LEN + 1];
	DWORD filename_crc;
	long real_data_size;
	long data_size;
	DWORD data_crc;
	long data_position;
	char compressed_type;
} TEterPackIndex;
#pragma pack(pop)

class CEterPack;

class CEterFileDict
{
	public:
		struct Item
		{
			Item(): pkPack (NULL), pkInfo (NULL)
			{
			}

			CEterPack* pkPack;
			TEterPackIndex* pkInfo;
		};

		using TDict = std::unordered_multimap<DWORD, Item>;

	public:
		void InsertItem (CEterPack* pkPack, TEterPackIndex* pkInfo);
		void UpdateItem (CEterPack* pkPack, TEterPackIndex* pkInfo);

		Item* GetItem (DWORD dwFileNameHash, const char* c_pszFileName);

	private:
		TDict m_dict;
};

using TDataPositionMap = std::unordered_map<DWORD, TEterPackIndex*>;
using TFreeIndexList = std::list<TEterPackIndex*>;

class CEterPack
{
	public:
		CEterPack();
		virtual~CEterPack();

		void Destroy();
		bool Create (CEterFileDict& rkFileDict, const char* dbname, const char* pathName, bool bReadOnly = true);

		bool Get (CMappedFile& mappedFile, const char* filename, LPCVOID* data);
		bool Get (CMappedFile& mappedFile, const char* filename, TEterPackIndex* index, LPCVOID* data);

		bool IsExist (const char* filename);

		TDataPositionMap& GetIndexMap();

	private:
		bool BuildIndex (CEterFileDict& rkFileDict, bool bOverwirte = false);

		bool CreateIndexFile();
		TEterPackIndex* FindIndex (const char* filename);
		long GetNewIndexPosition (CFileBase& file);
		TEterPackIndex* NewIndex (CFileBase& file, const char* filename, long size);
		void WriteIndex (CFileBase& file, TEterPackIndex* index);
		int GetFreeBlockIndex (long size);
		void PushFreeIndex (TEterPackIndex* index);

		bool CreateDataFile();
		long GetNewDataPosition (CFileBase& file);
		bool WriteNewData (CFileBase& file, TEterPackIndex* index, LPCVOID data);

	protected:
		CMappedFile m_file;

		long m_indexCount;
		bool m_bEncrypted;

		char m_dbName[DBNAME_MAX_LEN + 1];
		char m_indexFileName[MAX_PATH + 1];

		std::vector<TEterPackIndex> m_indexData;

		long m_FragmentSize;
		bool m_bReadOnly;

		TDataPositionMap m_DataPositionMap;
		TFreeIndexList m_FreeIndexList[FREE_INDEX_MAX_SIZE + 1];

		std::string m_stDataFileName;
		std::string m_stPathName;
};