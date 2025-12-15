#include "StdAfx.h"
#include <string.h>

#include "PropertyManager.h"
#include "Property.h"

CProperty::CProperty (const char* c_pszFileName) : mc_pFileName (NULL), m_dwCRC (0)
{
	m_stFileName = c_pszFileName;
	StringPath (m_stFileName);

	mc_pFileName = strrchr (m_stFileName.c_str(), '/');

	if (!mc_pFileName)
	{
		mc_pFileName = m_stFileName.c_str();
	}
	else
	{
		++mc_pFileName;
	}
}

DWORD CProperty::GetCRC()
{
	return m_dwCRC;
}

const char* CProperty::GetFileName()
{
	return (m_stFileName.c_str());
}

bool CProperty::GetString (const char* c_pszKey, const char** c_ppString)
{
	std::string stTempKey = c_pszKey;
	stl_lowers (stTempKey);
	CTokenVectorMap::iterator it = m_stTokenMap.find (stTempKey.c_str());

	if (m_stTokenMap.end() == it)
	{
		return false;
	}

	*c_ppString = it->second[0].c_str();
	return true;
}

DWORD CProperty::GetSize()
{
	return m_stTokenMap.size();
}

bool CProperty::GetVector (const char* c_pszKey, CTokenVector& rTokenVector)
{
	std::string stTempKey = c_pszKey;
	stl_lowers (stTempKey);
	CTokenVectorMap::iterator it = m_stTokenMap.find (stTempKey.c_str());

	if (m_stTokenMap.end() == it)
	{
		return false;
	}

	CTokenVector& rSourceTokenVector = it->second;
	CTokenVector::iterator itor = rSourceTokenVector.begin();
	for (; itor != rSourceTokenVector.end(); ++itor)
	{
		rTokenVector.push_back (*itor);
	}

	return true;
}

void CProperty::PutString (const char* c_pszKey, const char* c_pszString)
{
	std::string stTempKey = c_pszKey;
	stl_lowers (stTempKey);

	CTokenVectorMap::iterator itor = m_stTokenMap.find (stTempKey);

	if (itor != m_stTokenMap.end())
	{
		m_stTokenMap.erase (itor);
	}

	CTokenVector tokenVector;
	tokenVector.push_back (c_pszString);

	m_stTokenMap.insert (CTokenVectorMap::value_type (stTempKey, tokenVector));
}

void CProperty::PutVector (const char* c_pszKey, const CTokenVector& c_rTokenVector)
{
	std::string stTempKey = c_pszKey;
	stl_lowers (stTempKey);

	m_stTokenMap.insert (CTokenVectorMap::value_type (stTempKey, c_rTokenVector));
}

bool CProperty::ReadFromMemory (const void* c_pvData, int iLen, const char* c_pszFileName)
{
	const char* pcData = (const char*) c_pvData;

	if (* (DWORD*) pcData != MAKEFOURCC ('Y', 'P', 'R', 'T'))
	{
		return false;
	}

	pcData += sizeof (DWORD);

	if (*pcData != '\r' || * (pcData + 1) != '\n')
	{
		TraceError ("CProperty::ReadFromMemory: File format error after FourCC: %s\n", c_pszFileName);
		return false;
	}

	pcData += 2;

	CTokenVector stTokenVector;
	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind (iLen - (sizeof (DWORD) + 2), pcData);

	m_stCRC = textFileLoader.GetLineString (0);
	m_dwCRC = _atoi64 (m_stCRC.c_str());

	for (DWORD i = 1; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine (i, &stTokenVector))
		{
			continue;
		}

		stl_lowers (stTokenVector[0]);
		std::string stKey = stTokenVector[0];

		stTokenVector.erase (stTokenVector.begin());
		PutVector (stKey.c_str(), stTokenVector);
	}

	return true;
}

void CProperty::Clear()
{
	m_stTokenMap.clear();
}