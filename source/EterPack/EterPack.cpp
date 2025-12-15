#include "StdAfx.h"
#include "EterPack.h"
#include "Inline.h"

#include "../EterBase/utils.h"
#include "../EterBase/Debug.h"
#include "../EterBase/CRC32.h"

const TeaKey& s_adwEterPackKey()
{
	static const TeaKey key
	{
		std::array <DWORD, 4> {{
				841894746,
				959330102,
				594031435,
				2033789782,
			}
		}
	};
	return key;
}

const TeaKey& s_adwEterPackSecurityKey()
{
	static const TeaKey key
	{
		std::array <DWORD, 4> {{
				1934304049,
				1178152487,
				590759265,
				606817078,
			}
		}
	};
	return key;
}

CEterPack::CEterPack(): m_indexCount (0), m_FragmentSize (0), m_bEncrypted (false), m_bReadOnly (false)
{
}

CEterPack::~CEterPack()
{
	Destroy();
}

void CEterPack::Destroy()
{
	m_bReadOnly = false;
	m_bEncrypted = false;
	m_indexCount = 0;
	m_DataPositionMap.clear();

	for (int i = 0; i < FREE_INDEX_MAX_SIZE + 1; ++i)
	{
		m_FreeIndexList[i].clear();
	}

	m_indexData.clear();
	m_FragmentSize = 0;

	memset (m_dbName, 0, sizeof (m_dbName));
	memset (m_indexFileName, 0, sizeof (m_indexFileName));
}

bool CEterPack::Create (CEterFileDict& rkFileDict, const char* dbname, const char* pathName, bool bReadOnly)
{
	m_stPathName = pathName;
	strncpy (m_dbName, dbname, DBNAME_MAX_LEN);

	strncpy (m_indexFileName, dbname, MAX_PATH);
	strcat (m_indexFileName, ".eix");

	m_stDataFileName = dbname;
	m_stDataFileName += ".epk";

	m_bReadOnly = bReadOnly;

	if (!CreateIndexFile())
	{
		return false;
	}

	if (!CreateDataFile())
	{
		return false;
	}

	BuildIndex (rkFileDict);
	return true;
}

bool CEterPack::BuildIndex (CEterFileDict& rkFileDict, bool bOverwrite)
{
	CMappedFile file;
	LPCVOID pvData;
	CLZObject zObj;

	if (!file.Create (m_indexFileName, &pvData, 0, 0))
	{
		TraceError ("Cannot open pack index file! %s", m_dbName);
		return false;
	}

	if (file.Size() < eterpack::c_HeaderSize)
	{
		TraceError ("Pack index file header error! %s", m_dbName);
		return false;
	}

	DWORD fourcc = * (DWORD*) pvData;

	BYTE* pbData;
	unsigned int uiFileSize;

	if (fourcc == eterpack::c_IndexCC)
	{
		pbData = (BYTE*) pvData;
		uiFileSize = file.Size();
	}
	else if (fourcc == CLZObject::ms_dwFourCC)
	{
		m_bEncrypted = true;

		if (!CLZO::Instance().Decompress (zObj, (const BYTE*) pvData, s_adwEterPackKey()))
		{
			return false;
		}

		if (zObj.GetSize() < eterpack::c_HeaderSize)
		{
			return false;
		}

		pbData = zObj.GetBuffer();
		uiFileSize = zObj.GetSize();
	}
	else
	{
		TraceError ("Pack index file fourcc error! %s", m_dbName);
		return false;
	}

	pbData += sizeof (DWORD);
	DWORD ver = * (DWORD*) pbData;
	pbData += sizeof (DWORD);

	if (ver != eterpack::c_Version)
	{
		TraceError ("Pack index file version error! %s", m_dbName);
		return false;
	}

	m_indexCount = * (long*) pbData;
	pbData += sizeof (long);

	if (uiFileSize < eterpack::c_HeaderSize + sizeof (TEterPackIndex) * m_indexCount)
	{
		TraceError ("Pack index file size error! %s, indexCount %d", m_dbName, m_indexCount);
		return false;
	}

	m_indexData.resize (m_indexCount);
	memcpy (m_indexData.data(), pbData, sizeof (TEterPackIndex) * m_indexCount);

	for (TEterPackIndex& index : m_indexData)
	{
		if (!index.filename_crc)
		{
			PushFreeIndex (&index);
		}
		else
		{
			if (index.real_data_size > index.data_size)
			{
				m_FragmentSize += index.real_data_size - index.data_size;
			}

			m_DataPositionMap.insert (TDataPositionMap::value_type (index.filename_crc, &index));

			if (bOverwrite)
			{
				rkFileDict.UpdateItem (this, &index);
			}
			else
			{
				rkFileDict.InsertItem (this, &index);
			}
		}
	}

	return true;
}

bool CEterPack::Get (CMappedFile& out_file, const char* filename, LPCVOID* data)
{
	TEterPackIndex* index = FindIndex (filename);

	if (!index)
	{
		return false;
	}

	out_file.Create (m_stDataFileName.c_str(), data, index->data_position, index->data_size);

	if (COMPRESSED_TYPE_COMPRESS == index->compressed_type)
	{
		CLZObject* zObj = new CLZObject;

		if (!CLZO::Instance().Decompress (*zObj, static_cast<const BYTE*> (*data)))
		{
			TraceError ("Failed to decompress : %s", filename);
			delete zObj;
			return false;
		}

		out_file.BindLZObject (zObj);
		*data = zObj->GetBuffer();
	}
	else if (COMPRESSED_TYPE_SECURITY == index->compressed_type)
	{
		CLZObject* zObj = new CLZObject;

		if (!CLZO::Instance().Decompress (*zObj, static_cast<const BYTE*> (*data), s_adwEterPackSecurityKey()))
		{
			TraceError ("Failed to encrypt : %s", filename);
			delete zObj;
			return false;
		}

		out_file.BindLZObject (zObj);
		*data = zObj->GetBuffer();
	}

	return true;
}

bool CEterPack::Get (CMappedFile& out_file, const char* filename, TEterPackIndex* index, LPCVOID* data)
{
	if (!index)
	{
		return false;
	}

	out_file.Create (m_stDataFileName.c_str(), data, index->data_position, index->data_size);

	if (COMPRESSED_TYPE_COMPRESS == index->compressed_type)
	{
		CLZObject* zObj = new CLZObject;

		if (!CLZO::Instance().Decompress (*zObj, static_cast<const BYTE*> (*data)))
		{
			TraceError ("Failed to decompress : %s", filename);
			delete zObj;
			return false;
		}

		out_file.BindLZObject (zObj);
		*data = zObj->GetBuffer();
	}
	else if (COMPRESSED_TYPE_SECURITY == index->compressed_type)
	{
		CLZObject* zObj = new CLZObject;

		if (!CLZO::Instance().Decompress (*zObj, static_cast<const BYTE*> (*data), s_adwEterPackSecurityKey()))
		{
			TraceError ("Failed to encrypt : %s", filename);
			delete zObj;
			return false;
		}

		out_file.BindLZObject (zObj);
		*data = zObj->GetBuffer();
	}

	return true;
}

bool CEterPack::CreateIndexFile()
{
	FILE* fp;

	if (NULL != (fp = fopen (m_indexFileName, "rb")))
	{
		fclose (fp);
		return true;
	}
	else if (m_bReadOnly)
	{
		return false;
	}

	fp = fopen (m_indexFileName, "wb");

	if (!fp)
	{
		return false;
	}

	fwrite (&eterpack::c_IndexCC, sizeof (DWORD), 1, fp);
	fwrite (&eterpack::c_Version, sizeof (DWORD), 1, fp);
	fwrite (&m_indexCount, sizeof (long), 1, fp);
	fclose (fp);
	return true;
}

void CEterPack::WriteIndex (CFileBase& file, TEterPackIndex* index)
{
	file.Seek (sizeof (DWORD) + sizeof (DWORD));
	file.Write (&m_indexCount, sizeof (long));
	file.Seek (eterpack::c_HeaderSize + (index->id * sizeof (TEterPackIndex)));

	if (!file.Write (index, sizeof (TEterPackIndex)))
	{
		assert (!"WriteIndex: fwrite failed");
		return;
	}
}

int CEterPack::GetFreeBlockIndex (long size)
{
	return min (FREE_INDEX_MAX_SIZE, size / FREE_INDEX_BLOCK_SIZE);
}

void CEterPack::PushFreeIndex (TEterPackIndex* index)
{
	if (index->filename_crc != 0)
	{
		auto it = m_DataPositionMap.find (index->filename_crc);

		if (it != m_DataPositionMap.end())
		{
			m_DataPositionMap.erase (it);
		}

		index->filename_crc = 0;
		memset (index->filename, 0, sizeof (index->filename));
	}

	int blockidx = GetFreeBlockIndex (index->real_data_size);
	m_FreeIndexList[blockidx].push_back (index);
	m_FragmentSize += index->real_data_size;
}

long CEterPack::GetNewIndexPosition (CFileBase& file)
{
	long pos = (file.Size() - eterpack::c_HeaderSize) / sizeof (TEterPackIndex);
	++m_indexCount;
	return (pos);
}

TEterPackIndex* CEterPack::NewIndex (CFileBase& file, const char* filename, long size)
{
	TEterPackIndex* index = NULL;
	int block_size = size + (DATA_BLOCK_SIZE - (size % DATA_BLOCK_SIZE));
	int blockidx = GetFreeBlockIndex (block_size);

	for (auto it = m_FreeIndexList[blockidx].begin(); it != m_FreeIndexList[blockidx].end(); ++it)
	{
		if ((*it)->real_data_size >= size)
		{
			index = *it;
			m_FreeIndexList[blockidx].erase (it);
			assert (index->filename_crc == 0);
			break;
		}
	}

	if (!index)
	{
		index = new TEterPackIndex;
		index->real_data_size = block_size;
		index->id = GetNewIndexPosition (file);
	}

	strncpy (index->filename, filename, FILENAME_MAX_LEN);
	index->filename[FILENAME_MAX_LEN] = '\0';
	inlineConvertPackFilename (index->filename);

	index->filename_crc = GetCRC32 (index->filename, strlen (index->filename));

	m_DataPositionMap.insert (TDataPositionMap::value_type (index->filename_crc, index));
	return index;
}

TEterPackIndex* CEterPack::FindIndex (const char* filename)
{
	static char tmpFilename[MAX_PATH + 1];
	strncpy (tmpFilename, filename, MAX_PATH);
	inlineConvertPackFilename (tmpFilename);
	DWORD filename_crc = GetCRC32 (tmpFilename, strlen (tmpFilename));
	auto it = m_DataPositionMap.find (filename_crc);

	if (it == m_DataPositionMap.end())
	{
		return NULL;
	}

	return (it->second);
}

bool CEterPack::IsExist (const char* filename)
{
	return FindIndex (filename) ? true : false;
}

bool CEterPack::CreateDataFile()
{
	FILE* fp;

	if (NULL != (fp = fopen (m_stDataFileName.c_str(), "rb")))
	{
		fclose (fp);
		return true;
	}
	else if (m_bReadOnly)
	{
		return false;
	}

	fp = fopen (m_stDataFileName.c_str(), "wb");

	if (!fp)
	{
		return false;
	}

	fclose (fp);
	return true;
}

long CEterPack::GetNewDataPosition (CFileBase& file)
{
	return file.Size();
}

bool CEterPack::WriteNewData (CFileBase& file, TEterPackIndex* index, LPCVOID data)
{
	file.Seek (index->data_position);

	if (!file.Write (data, index->data_size))
	{
		assert (!"WriteData: fwrite data failed");
		return false;
	}

	int empty_size = index->real_data_size - index->data_size;

	if (empty_size < 0)
	{
		printf ("SYSERR: WriteNewData(): CRITICAL ERROR: empty_size lower than 0!\n");
		exit (1);
	}

	if (empty_size == 0)
	{
		return true;
	}

	char* empty_buf = (char*) calloc (empty_size, sizeof (char));

	if (!file.Write (empty_buf, empty_size))
	{
		assert (!"WriteData: fwrite empty data failed");
		return false;
	}

	free (empty_buf);
	return true;
}

TDataPositionMap& CEterPack::GetIndexMap()
{
	return m_DataPositionMap;
}

void CEterFileDict::InsertItem (CEterPack* pkPack, TEterPackIndex* pkInfo)
{
	Item item;
	item.pkPack = pkPack;
	item.pkInfo = pkInfo;
	m_dict.insert (TDict::value_type (pkInfo->filename_crc, item));
}

void CEterFileDict::UpdateItem (CEterPack* pkPack, TEterPackIndex* pkInfo)
{
	Item item;
	item.pkPack = pkPack;
	item.pkInfo = pkInfo;

	auto it = m_dict.find (pkInfo->filename_crc);
	if (it == m_dict.end())
	{
		m_dict.insert (TDict::value_type (pkInfo->filename_crc, item));
	}
	else
	{
		if (strcmp (it->second.pkInfo->filename, item.pkInfo->filename) == 0)
		{
			it->second = item;
		}
		else
		{
			TraceError ("NAME_COLLISION: OLD: %s NEW: %s", it->second.pkInfo->filename, item.pkInfo->filename);
		}
	}
}

CEterFileDict::Item* CEterFileDict::GetItem (DWORD dwFileNameHash, const char* c_pszFileName)
{
	auto iter_pair = m_dict.equal_range (dwFileNameHash);
	auto iter = iter_pair.first;

	while (iter != iter_pair.second)
	{
		Item& item = iter->second;
		if (0 == strcmp (c_pszFileName, item.pkInfo->filename))
		{
			return &item;
		}
		++iter;
	}

	return NULL;
}