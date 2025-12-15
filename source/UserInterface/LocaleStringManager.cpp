#include "StdAfx.h"
#include "LocaleStringManager.h"
#include "../eterPack/EterPackManager.h"
#include "PythonNetworkStream.h"

LocaleStringManager::LocaleStringManager()
{
	localeStringMap.clear();
	SetLoaded(false);
}

LocaleStringManager::~LocaleStringManager()
{
}

LocaleStringManager::TFormat LocaleStringManager::FormatTable[FORMAT_TYPE_MAX_NUM] =
{
	{FORMAT_TYPE_INT,			"d",		sizeof(int) },
	{FORMAT_TYPE_UNSIGNED_INT,		"u",		sizeof(unsigned int) },
	{FORMAT_TYPE_SHORT_INT,			"h",		sizeof(short int) },
	{FORMAT_TYPE_SIGNED_CHAR,		"hh",		sizeof(signed char) },

	{FORMAT_TYPE_LONG,			"l",		sizeof(long) },
	{FORMAT_TYPE_UNSIGNED_LONG,		"lu",		sizeof(unsigned long) },
	{FORMAT_TYPE_LONG_INT,			"ld",		sizeof(long int) },
	{FORMAT_TYPE_LONG_LONG_INT,		"lld",		sizeof(long long int) },

	{FORMAT_TYPE_FLOAT,			"f",		STRING_SIZE },

	{FORMAT_TYPE_STRING,			"s",		STRING_SIZE },
};

bool LocaleStringManager::IsFormatSpecifier(const char* specifier)
{
	for (const auto& format : FormatTable)
	{
		int result = strcmp(format.szName, specifier);
		if (result == 0)
			return true;
	}

	return false;
}

int LocaleStringManager::GetFormatSpecifierType(const char* specifier)
{
	for (const auto& format : FormatTable)
	{
		int result = strcmp(format.szName, specifier);
		if (result == 0)
			return format.iType;
	}

	return -1;
}

UINT LocaleStringManager::GetFormatSpecifierSize(int type)
{
	return FormatTable[type].iSize;
}

int LocaleStringManager::GetFormatSpecifierStringSize(int type)
{
	return strlen(FormatTable[type].szName);
}

void LocaleStringManager::LocateFormatSpecifier(const char* c_szFormat, size_t iSize, int& iSpecifierType, int& iEndPos)
{
	std::string specifier = "";
	std::string specifierCopy = "";
	int specifierType = 0;
	int formatLenght = 0;

	for (int iPos = 0; iPos < iSize; iPos++)
	{
		if (!isalpha(c_szFormat[iPos]))
		{
			formatLenght++;
			continue;
		}

		if (!isdigit(c_szFormat[iPos]))
		{
			specifierCopy += c_szFormat[iPos];

			if (IsFormatSpecifier(specifierCopy.c_str()))
			{
				specifier = specifierCopy;
				specifierType = GetFormatSpecifierType(specifier.c_str());

				int specifierStrSize = GetFormatSpecifierStringSize(specifierType);

				if (formatLenght > specifierStrSize)
					iEndPos = formatLenght;
				else
					iEndPos = specifierStrSize;

				formatLenght++;
			}
		}
	}

	iSpecifierType = specifierType;
}

bool LocaleStringManager::FillLocaleString(DWORD dwStringIndex, std::string& strLocaleString)
{
	std::string localeString = FindLocaleString(dwStringIndex);
	if (localeString.empty())
	{
		CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
		rns.ClearRecvBuffer();

		strLocaleString = "";

		return true;
	}

	std::stringstream ss;
	size_t formatSize = localeString.size();

	char stringBuf[512 + 1];
	memcpy(stringBuf, localeString.c_str(), localeString.size());


	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();

	for (int iPos = 0; iPos < formatSize; iPos++)
	{
		if (stringBuf[iPos] == '%')
		{
			if (stringBuf[iPos + 1] == '%')
			{
				ss << '%';

				iPos++;
				continue;
			}
			else
			{
				char stringCopyBuf[512 + 1];
				memcpy(stringCopyBuf, stringBuf + iPos, formatSize);

				int specifierType, endPos = 0;

				LocateFormatSpecifier(stringCopyBuf, formatSize, specifierType, endPos);

				CPythonNetworkStream& rns = CPythonNetworkStream::Instance();

				if (endPos > 0)
				{
					UINT valueSize = GetFormatSpecifierSize(specifierType);

					char variableBuf[64 + 1];
					bool bRet = false;
					std::string variableStr = "";

					switch (specifierType)
					{
						case FORMAT_TYPE_INT:
						{
							bRet = rns.Recv(valueSize, &variableBuf);
							variableStr = std::to_string(*(int*)variableBuf);
						} break;

						case FORMAT_TYPE_UNSIGNED_INT:
						{
							bRet = rns.Recv(valueSize, &variableBuf);
							variableStr = std::to_string(*(unsigned int*)variableBuf);
						} break;

						case FORMAT_TYPE_SHORT_INT:
						{
							bRet = rns.Recv(valueSize, &variableBuf);
							variableStr = std::to_string(*(short*)variableBuf);
						} break;

						case FORMAT_TYPE_SIGNED_CHAR:
						{
							bRet = rns.Recv(valueSize, &variableBuf);
							variableStr = std::to_string(*(signed char*)variableBuf);
						} break;

						case FORMAT_TYPE_LONG:
						{
							bRet = rns.Recv(valueSize, &variableBuf);
							variableStr = std::to_string(*(long*)variableBuf);
						} break;

						case FORMAT_TYPE_UNSIGNED_LONG:
						{
							bRet = rns.Recv(valueSize, &variableBuf);
							variableStr = std::to_string(*(unsigned long*)variableBuf);
						} break;

						case FORMAT_TYPE_LONG_INT:
						{
							bRet = rns.Recv(valueSize, &variableBuf);
							variableStr = std::to_string(*(int*)variableBuf);
						} break;

						case FORMAT_TYPE_LONG_LONG_INT:
						{
							bRet = rns.Recv(valueSize, &variableBuf);
							variableStr = std::to_string(*(long long*)variableBuf);
						} break;

						case FORMAT_TYPE_FLOAT:
						{
							char stringVariableBuf[STRING_SIZE];
							if (!rns.Recv(valueSize, &stringVariableBuf))
								return false;

							ss << stringVariableBuf;
							iPos += endPos;
							continue;
						} break;

						case FORMAT_TYPE_STRING:
						{
							char stringVariableBuf[STRING_SIZE];
							if (!rns.Recv(valueSize, &stringVariableBuf))
								return false;

							ss << stringVariableBuf;
							iPos += endPos;
							continue;
						} break;
					}

					if (!bRet)
						return false;

					ss << variableStr;
					iPos += endPos;
				}
			}
		}
		else
			ss << stringBuf[iPos];
	}

	strLocaleString = ss.str();

	return true;
}

std::string LocaleStringManager::FindLocaleString(DWORD dwStringIndex)
{
	auto it = localeStringMap.find(dwStringIndex);

	if (it == localeStringMap.end())
	{
		TraceError("CPythonLocaleString::FindLocaleString: cannot find \"%d\";", dwStringIndex);
		return "";
	}
	return it->second.c_str();
}

bool LocaleStringManager::RegisterLocaleStringTable(const char * c_szFileName)
{
	if (IsLoaded())
		return true;

	const VOID* pvData;
	CMappedFile kFile;
	if (!CEterPackManager::Instance().Get(kFile, c_szFileName, &pvData))
		return false;

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(kFile.Size(), pvData);

	CTokenVector TokenVector;
	int lineIndex = 0;
	for (DWORD i = 0; i < textFileLoader.GetLineCount() - 1; ++i)
	{
		lineIndex++;

		if (!textFileLoader.SplitLineByTab(i, &TokenVector))
		{
			TraceError("LocaleStringManager::RegisterLocaleStringTable: Strange Token Strucutre [Line %d]\n", lineIndex);
			continue;
		}

		if (TokenVector.size() != 2)
		{
			TraceError("LocaleStringManager::RegisterLocaleStringTable(%s) - Strange Token Count [Line:%d / TokenCount:%d]\n", c_szFileName, lineIndex, TokenVector.size());
			continue;
		}

		uint16_t dwStringIndex = atoi(TokenVector[0].c_str());
		std::string strLocaleString = TokenVector[1];

		localeStringMap[dwStringIndex] = strLocaleString;
	}

	SetLoaded(true);

	return true;
}
