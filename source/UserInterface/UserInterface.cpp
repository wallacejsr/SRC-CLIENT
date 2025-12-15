
#include "StdAfx.h"
#include <chrono>
#include <future>
#include "PythonApplication.h"
#include "ProcessScanner.h"
#include "PythonExceptionSender.h"
#include "resource.h"
#include "Version.h"
#include <windows.h>
#include <cstring>
#include <xutility>
#include "string"
#include <fstream>
#include <cstdint>
#include <iostream>
#include <psapi.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include "../eterPack/EterPackManager.h"
#include "../eterLib/Util.h"
#include "../CWebBrowser/CWebBrowser.h"
#include "../eterBase/CPostIt.h"

#include "CheckLatestFiles.h"

#ifdef ENABLE_HWID_BAN_SYSTEM
#include "HWIDManager.h"
#endif

#define MAX_ROWS_MD5 19

extern "C" {
extern int _fltused;
volatile int _AVOID_FLOATING_POINT_LIBRARY_BUG = _fltused;
};
extern "C" { FILE __iob_func[3] = { *stdin, *stdout, *stderr }; }

#pragma comment(linker, "/NODEFAULTLIB:libci.lib")

#pragma comment( lib, "version.lib" )

//#ifdef _DEBUG
//#pragma comment( lib, "python27_d.lib" )
//#else
#pragma comment( lib, "python27.lib" )
//#endif
#pragma comment( lib, "imagehlp.lib" )
#pragma comment( lib, "devil.lib" )

#if GrannyProductMinorVersion==4
#pragma comment( lib, "granny2.4.0.10.lib" )
#elif GrannyProductMinorVersion==7
#pragma comment( lib, "granny2.7.0.30.lib" )
#elif GrannyProductMinorVersion==8
#pragma comment( lib, "granny2.8.49.0.lib" )
#elif GrannyProductMinorVersion==9
#pragma comment( lib, "granny2.9.12.0.lib" )
#elif GrannyProductMinorVersion==11
#pragma comment( lib, "granny2.11.8.0.lib" )
#else
#error "unknown granny version"
#endif
#pragma comment( lib, "mss32.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "imm32.lib" )
#pragma comment( lib, "oldnames.lib" )
#pragma comment( lib, "SpeedTreeRT.lib" )
#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "ws2_32.lib" )
#pragma comment( lib, "strmiids.lib" )
#pragma comment( lib, "ddraw.lib" )
#pragma comment( lib, "dmoguids.lib" )
//#pragma comment( lib, "wsock32.lib" )
#include <stdlib.h>
#include <cryptopp/cryptoppLibLink.h>
bool __IS_TEST_SERVER_MODE__=false;

// #define __USE_CYTHON__
#ifdef __USE_CYTHON__
// don't include these two files .h .cpp if you're implementing cython via .pyd
// #include "PythonrootlibManager.h"
// it would be better including such file in the project, but this is easier at this moment:
// #include "PythonrootlibManager.cpp"
#endif

// #define __USE_EXTRA_CYTHON__
#ifdef __USE_EXTRA_CYTHON__
// don't include these two files .h .cpp if you're implementing cython via .pyd
// #include "PythonuiscriptlibManager.h"
// it would be better including such file in the project, but this is easier at this moment:
// #include "PythonuiscriptlibManager.cpp"
#endif

// #define ENABLE_FILES_CHECK
#define ENABLE_PYLIB_CHECK
#define ENABLE_MILES_CHECK
// #define ENABLE_DAEMONPROTECTION
// #define ENABLE_ARGUMENT_LAUNCHER

#ifdef ENABLE_DAEMONPROTECTION
#define DPDLL_FILENAME	"game.dll"
#define DPDLL_CRC32		0xB9929FE7
#define DPDLL_FILESIZE	607744
#endif

#ifdef BLOCK_WRITE_PROCESS_MEMORY
PVOID* find (const char* szFunc, HMODULE hModule)
{
	if (!hModule)
	{
		hModule = GetModuleHandle (0);
	}

	PIMAGE_DOS_HEADER img_dos_headers = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS img_nt_headers = (PIMAGE_NT_HEADERS) ((byte*)img_dos_headers + img_dos_headers->e_lfanew);
	PIMAGE_IMPORT_DESCRIPTOR img_import_desc = (PIMAGE_IMPORT_DESCRIPTOR) ((byte*)img_dos_headers + img_nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	if (img_dos_headers->e_magic != IMAGE_DOS_SIGNATURE)
	{
		printf ("e_magic dos sig\n");
	}

	for (IMAGE_IMPORT_DESCRIPTOR* iid = img_import_desc; iid->Name != 0; iid++)
	{
		for (int func_idx = 0; * (func_idx + (void**) (iid->FirstThunk + (size_t)hModule)) != nullptr; func_idx++)
		{
			char* mod_func_name = (char*) (* (func_idx + (size_t*) (iid->OriginalFirstThunk + (size_t)hModule)) + (size_t)hModule + 2);
			const intptr_t nmod_func_name = (intptr_t)mod_func_name;
			if (nmod_func_name >= 0)
			{
				if (!::strcmp (szFunc, mod_func_name))
				{
					return func_idx + (void**) (iid->FirstThunk + (size_t)hModule);
				}
			}
		}
	}
	return 0;
}

std::uint32_t detour_ptr (const char* szFunc, PVOID newfunction, HMODULE module)
{
	void**&& func_ptr = find (szFunc, module);
	if (*func_ptr == newfunction || *func_ptr == nullptr)
	{
		return 0;
	}

	DWORD old_rights;
	DWORD new_rights = PAGE_READWRITE;
	VirtualProtect (func_ptr, sizeof (uintptr_t), new_rights, &old_rights);
	uintptr_t ret = (uintptr_t) * func_ptr;
	*func_ptr = newfunction;
	VirtualProtect (func_ptr, sizeof (uintptr_t), old_rights, &new_rights);
	return ret;
}

using WriteProcessMemoryFn = bool (__stdcall*) (HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T*);
WriteProcessMemoryFn oWriteProcessMemory;
bool __stdcall hkWriteProcessMemory (HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten)
{
	return oWriteProcessMemory (nullptr, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
}
#endif

#ifdef ENABLE_CHECK_FILES_MD5
#include "../eterPack/md5.h"
#include "../eterPack/ffd.h"

static char* ar_szMD5FileNames[MAX_ROWS_MD5][2] =
{
	{ "miles/mss32.dll", "6400e224b8b44ece59a992e6d8233719" },
	{ "miles/mssa3d.m3d", "e089ce52b0617a6530069f22e0bdba2a" },
	{ "miles/mssds3d.m3d", "85267776d45dbf5475c7d9882f08117c" },
	{ "miles/mssdsp.flt", "cb71b1791009eca618e9b1ad4baa4fa9" },
	{ "miles/mssdx7.m3d", "2727e2671482a55b2f1f16aa88d2780f" },
	{ "miles/msseax.m3d", "788bd950efe89fa5166292bd6729fa62" },
	{ "miles/mssmp3.asi", "189576dfe55af3b70db7e3e2312cd0fd" },
	{ "miles/mssrsx.m3d", "7fae15b559eb91f491a5f75cfa103cd4" },
	{ "miles/msssoft.m3d", "bdc9ad58ade17dbd939522eee447416f" },
	{ "miles/mssvoice.asi", "3d5342edebe722748ace78c930f4d8a5" },
	{ "devil.dll", "8df4d4324e5755f1a0567db3c5be4c58" },
	{ "granny2.dll", "6f403e1359bbc6b5f23b2def240e4c3c" },
	{ "mss32.dll", "6400e224b8b44ece59a992e6d8233719" },
	{ "msvcp60.dll", "cb21d826d9c39aed19dd431c1880f5de" },
	{ "msvcp100.dll", "32e390954b2c6b1583a969ed0e7c8a9d" },
	{ "msvcr100.dll", "2b92a88e329f4845d31941967a3baa90" },
	{ "msvcr120.dll", "034ccadc1c073e4216e9466b720f9849" },
	{ "python27.dll", "3db410bfe27c77b76e6f8978f9361f0a" },
	{ "speedtreert.dll", "1ac3d612389fa679f5ca3c6bab855145" },
};

void CheckMD5Filenames()
{
	MD5 md5;
	for (int it = 0; it < _countof (ar_szMD5FileNames); it++)
	{
		if (strcmp (md5.digestFile (ar_szMD5FileNames[it][0]), ar_szMD5FileNames[it][1]))
		{
			char szBuf[512 + 1];
			snprintf (szBuf, sizeof (szBuf), "O arquivo [%s] foi modificado! Use o arquivo original.", ar_szMD5FileNames[it][0]);
			MessageBoxA (NULL, szBuf, "metin2client", NULL);
			exit (0);
		}
	}
}
#endif

extern bool SetDefaultCodePage(DWORD codePage);

static const char * sc_apszPythonLibraryFilenames[] =
{
	"UserDict.pyc",
	"__future__.pyc",
	"copy_reg.pyc",
	"linecache.pyc",
	"ntpath.pyc",
	"os.pyc",
	"site.pyc",
	"stat.pyc",
	"string.pyc",
	"traceback.pyc",
	"types.pyc",
	"\n",
};

#ifdef ENABLE_PYLIB_CHECK
constexpr int PrintLevel = 0;
#define PYFOLD "./lib"

template <class ...Args>
void PrintMe(int level, const Args& ...Arguments)
{
	if (PrintLevel >= level) TraceError(Arguments...);
}

#define PYFORCE

typedef struct PyLibFiles_s
{
	std::string sFileName;
	size_t stSize;
	DWORD dwCRC32;
	bool SkipCheck;
} PyLibFiles_t;

PyLibFiles_t PyLibFilesTable[] =
{
#if PY_VERSION_HEX==0x020710f0
	{ PYFOLD"/abc.pyc", 6187, 3834771731, false},
	{ PYFOLD"/bisect.pyc", 3236, 3116899751, false },
	{ PYFOLD"/codecs.pyc", 36978, 2928014693, false },
	{ PYFOLD"/collections.pyc", 26172, 385366131, false },
	{ PYFOLD"/copy.pyc", 13208, 1091298715, false },
	{ PYFOLD"/copy_reg.pyc", 5157, 536292604, false },
	{ PYFOLD"/encodings/aliases.pyc", 8803, 3888310600, false },
	{ PYFOLD"/encodings/cp949.pyc", 2009, 1824094431, false },
	{ PYFOLD"/encodings/__init__.pyc", 4510, 2926961588, false },
	{ PYFOLD"/fnmatch.pyc", 3732, 4270526278, false },
	{ PYFOLD"/functools.pyc", 6193, 3257285433, false },
	{ PYFOLD"/genericpath.pyc", 3303, 1652596334, false },
	{ PYFOLD"/heapq.pyc", 13896, 2948659214, false },
	{ PYFOLD"/keyword.pyc", 2169, 2178546341, false },
	{ PYFOLD"/linecache.pyc", 3235, 4048207604, false },
	{ PYFOLD"/locale.pyc", 49841, 4114662314, false },
	{ PYFOLD"/ntpath.pyc", 11961, 2765879465, false },
	{ PYFOLD"/os.pyc", 25769, 911432770, false },
	{ PYFOLD"/pyexpat.pyd", 127488, 2778492911, false },
	{ PYFOLD"/pyexpat_d.pyd", 194560, 2589182738, false },
	{ PYFOLD"/re.pyc", 13178, 1671609387, false },
	{ PYFOLD"/shutil.pyc", 19273, 1873281015, false },
	{ PYFOLD"/site.pyc", 20019, 3897044925, false },
	{ PYFOLD"/sre_compile.pyc", 11107, 1620746411, false },
	{ PYFOLD"/sre_constants.pyc", 6108, 3900811275, false },
	{ PYFOLD"/sre_parse.pyc", 19244, 1459430047, false },
	{ PYFOLD"/stat.pyc", 2791, 1375966108, false },
	{ PYFOLD"/string.pyc", 13514, 648740690, false },
	{ PYFOLD"/sysconfig.pyc", 17571, 1529083148, false },
	{ PYFOLD"/traceback.pyc", 11703, 3768933732, false },
	{ PYFOLD"/types.pyc", 2530, 920695307, false },
	{ PYFOLD"/UserDict.pyc", 9000, 1431875928, false },
	{ PYFOLD"/warnings.pyc", 13232, 3752454002, false },
	{ PYFOLD"/weakref.pyc", 16037, 2124701469, false },
	{ PYFOLD"/xml/dom/domreg.pyc", 3506, 2127674645, false },
	{ PYFOLD"/xml/dom/expatbuilder.pyc", 36698, 316034696, false },
	{ PYFOLD"/xml/dom/minicompat.pyc", 4144, 747596376, false },
	{ PYFOLD"/xml/dom/minidom.pyc", 74704, 1543233763, false },
	{ PYFOLD"/xml/dom/minidomer.pyc", 73, 3191306169, false },
	{ PYFOLD"/xml/dom/nodefilter.pyc", 1243, 3805409468, false },
	{ PYFOLD"/xml/dom/xmlbuilder.pyc", 18659, 4118801318, false },
	{ PYFOLD"/xml/dom/__init__.pyc", 7337, 343751384, false },
	{ PYFOLD"/xml/parsers/expat.pyc", 326, 2425747752, false },
	{ PYFOLD"/xml/parsers/invers.pyc", 16, 2721462394, false },
	{ PYFOLD"/xml/parsers/__init__.pyc", 353, 1691127318, false },
	{ PYFOLD"/xml/__init__.pyc", 1117, 3531597556, false },
	{ PYFOLD"/_abcoll.pyc", 22339, 2365844594, false },
	{ PYFOLD"/_locale.pyc", 49841, 4114662314, false },
	{ PYFOLD"/_weakrefset.pyc", 10490, 1576811346, false },
	{ PYFOLD"/__future__.pyc", 4431, 2857792867, false },
	{ PYFOLD"/random.pyc", 0, 0, true },
	{ PYFOLD"/hashlib.pyc", 0, 0, true },
	{ PYFOLD"/pyexpat_d.pdb", 0, 0, true },
	{ PYFOLD"/ACP_HS_KGO.license", 0, 0, true },
#elif PY_VERSION_HEX==0x020203f0
#else
#error "unknown python version"
#endif
};

bool checkPyLibDir(const string szDirName)
{
	bool HasHack = false;

	char szDirNamePath[MAX_PATH];
	sprintf(szDirNamePath, "%s\\*", szDirName.c_str());

	WIN32_FIND_DATA f;
	HANDLE h = FindFirstFile(szDirNamePath, &f);

	if (h == INVALID_HANDLE_VALUE) { return HasHack; }

	do
	{
		if (HasHack)
			break;
		const char * name = f.cFileName;

		if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) { continue; }

		if (f.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			char filePath[MAX_PATH];
			sprintf(filePath, "%s%s%s", szDirName.c_str(), "\\", name);
			PrintMe(1, "sub %s", filePath);
            checkPyLibDir(filePath);
		}
		else
		{
			// start processing file
			PrintMe(1, "starting %s", name);
			std::string sName(name);
			std::string sPathName(szDirName+"/"+name);
			// change \\ to /
			std::replace(sPathName.begin(), sPathName.end(), '\\', '/');
			PrintMe(1, "path %s", sPathName.c_str());
			// lower file name
			std::transform(sName.begin(), sName.end(), sName.begin(), ::tolower);
			{
				PrintMe(1, "verify %s", sName.c_str());
				bool isPyLibFound = false;
				for (PyLibFiles_t *i1=std::begin(PyLibFilesTable), *e1=std::end(PyLibFilesTable); i1<e1; i1++)
				{
					if (!i1->sFileName.compare(sPathName))
					{
						if (i1->SkipCheck == true)
						{
							isPyLibFound = true;
							break;
						}
						PrintMe(1, "found %s==%s", i1->sFileName.c_str(), sName.c_str());
						DWORD dwCrc32 = GetFileCRC32(sPathName.c_str());
						// assert(dwCrc32);
						DWORD dwFileSize = f.nFileSizeLow;
						if (i1->stSize!=dwFileSize)
						{
							PrintMe(1, "wrong size %u==%u", i1->stSize, dwFileSize);
							HasHack = true;
							PrintMe(0, "wrong size %u for %s", dwFileSize, sPathName.c_str());
							return HasHack;
						}
						else if (i1->dwCRC32 != dwCrc32)
						{
							PrintMe(1, "wrong crc32 %u==%u", i1->dwCRC32, dwCrc32);
							HasHack = true;
							PrintMe(0, "wrong crc32 %u for %s", dwCrc32, sPathName.c_str());
							return HasHack;
						}
						PrintMe(1, "right size %u==%u", i1->stSize, dwFileSize);
						PrintMe(1, "right crc32 %u==%u", i1->dwCRC32, dwCrc32);
						PrintMe(2, "{ \"%s\", %u, %u},", sPathName.c_str(), dwFileSize, dwCrc32);
						isPyLibFound = true;
						break;
					}
				}
				// block ambiguous pyc/d files
				if (!isPyLibFound)
				{
					PrintMe(1, "not found %s", sName.c_str());
#ifdef PYFORCE
					HasHack = true;
					PrintMe(0, "ambiguous file for %s", sPathName.c_str());
					return HasHack;
#endif
				}
				PrintMe(1, "skipping file(%s) hack(%u) found(%u)", sName.c_str(), HasHack, isPyLibFound);
			}
		}

	} while (FindNextFile(h, &f));
	FindClose(h);
	return HasHack;
}

bool __CheckPyLibFiles()
{
	PrintMe(1, "__CheckPyLibFiles processing " PYFOLD);
	if (checkPyLibDir(PYFOLD))
		return false;
	return true;
}
#endif

#ifdef ENABLE_MILES_CHECK
#include <algorithm>
#include "../EterBase/Filename.h"
// #include "../EterBase/CRC32.h"

typedef struct MilesFiles_s
{
	std::string sFileName;
	size_t stSize;
	DWORD dwCRC32;
} MilesFiles_t;

typedef struct MilesExten_s
{
	std::string ExtName;
	bool IsUni;
} MilesExten_t;

MilesExten_t MilesExtenTable[] = {
	{"dll", false},
	{"asi", true},
	{"flt", true},
	{"m3d", true},
	{"mix", true},
};

MilesFiles_t MilesFilesTable[] =
{
	{"mss32.dll", 349696, 1817711331},
	{"mssa3d.m3d", 83456, 1812642892},
	{"mssds3d.m3d", 70656, 2704596484},
	{"mssdsp.flt", 93696, 3364819387},
	{"mssdx7.m3d", 80896, 236402185},
	{"msseax.m3d", 103424, 3195814903},
	{"mssmp3.asi", 125952, 1219814613},
	{"mssrsx.m3d", 354816, 550946743},
	{"msssoft.m3d", 67072, 4284421368},
	{"mssvoice.asi", 197120, 1407967464},
};

bool checkMilesDir(const string szDirName)
{
	bool HasHack = false;

	char szDirNamePath[MAX_PATH];
	sprintf(szDirNamePath, "%s\\*", szDirName.c_str());

	WIN32_FIND_DATA f;
	HANDLE h = FindFirstFile(szDirNamePath, &f);

	if (h == INVALID_HANDLE_VALUE) { return HasHack; }

	do
	{
		if (HasHack)
			break;
		const char * name = f.cFileName;

		if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) { continue; }

		if (f.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			// do nothing if it's a folder
		}
		else
		{
			// start processing file
			PrintMe(1, "starting %s", name);
			std::string sName(name);
			std::string sPathName(szDirName+"/"+name);
			PrintMe(1, "path %s", sPathName.c_str());
			// lower file name
			std::transform(sName.begin(), sName.end(), sName.begin(), ::tolower);
			// file or symlink; check for asi flt m3d mix exploit
			string sNameExt = CFileNameHelper::GetExtension(sName);
			PrintMe(1, "ext %s", sNameExt.c_str());
			// workaround instead of std::find
			bool isMilesFile = false, isMilesUnique = false;
			for (MilesExten_t *i1=std::begin(MilesExtenTable), *e1=std::end(MilesExtenTable); i1<e1; i1++)
			{
				PrintMe(1, "is %s ? %s", i1->ExtName.c_str(), sNameExt.c_str());
				if (!sNameExt.compare(0, i1->ExtName.length(), i1->ExtName))
				{
					isMilesFile = true;
					isMilesUnique = i1->IsUni;
				}
			}
			if (isMilesFile)
			{
				PrintMe(1, "verify %s -> %s", sName.c_str(), sNameExt.c_str());
				bool isMilesFound = false;
				for (MilesFiles_t *i1=std::begin(MilesFilesTable), *e1=std::end(MilesFilesTable); i1<e1; i1++)
				{
					if (!i1->sFileName.compare(sName))
					{
						PrintMe(1, "found %s==%s", i1->sFileName.c_str(), sName.c_str());
						DWORD dwCrc32 = GetFileCRC32(sPathName.c_str());
						// assert(dwCrc32);
						DWORD dwFileSize = f.nFileSizeLow;
						if (i1->stSize!=dwFileSize)
						{
							PrintMe(1, "wrong size %u==%u", i1->stSize, dwFileSize);
							HasHack = true;
							PrintMe(0, "wrong size %u for %s", dwFileSize, sPathName.c_str());
							return HasHack;
						}
						else if (i1->dwCRC32 != dwCrc32)
						{
							PrintMe(1, "wrong crc32 %u==%u", i1->dwCRC32, dwCrc32);
							HasHack = true;
							PrintMe(0, "wrong crc32 %u for %s", dwCrc32, sPathName.c_str());
							return HasHack;
						}
						PrintMe(1, "right size %u==%u", i1->stSize, dwFileSize);
						PrintMe(1, "right crc32 %u==%u", i1->dwCRC32, dwCrc32);
						isMilesFound = true;
						break;
					}
				}
				// only mss32.dll is checked, and there's no need to check the others
				if (!isMilesFound && isMilesUnique)
				{
					PrintMe(1, "not found %s", sName.c_str());
					HasHack = true;
					PrintMe(0, "ambiguous file for %s", sPathName.c_str());
					return HasHack;
				}
				PrintMe(1, "skipping file(%s) hack(%u) found(%u) uni(%u)", sName.c_str(), HasHack, isMilesFound, isMilesUnique);
			}
		}

	} while (FindNextFile(h, &f));
	FindClose(h);
	return HasHack;
}

bool __CheckMilesFiles()
{
	PrintMe(1, "__CheckMilesFiles processing . and .\\miles");
	if (checkMilesDir(".") || checkMilesDir(".\\miles"))
		return false;
	return true;
}
#endif


#ifdef ENABLE_FILES_CHECK
#define PRINT_LEVEL 0
#define PRINTME(level, ...) if(PRINT_LEVEL>=level) TraceError(__VA_ARGS__);

typedef struct FolderFiles_s
{
	std::string sFileName;
	size_t stSize;
	DWORD dwCRC32;
	bool SkipCheck;
} FolderFiles_t;

FolderFiles_t FolderFilesTable[] =
{
	{ "channel.inf", 0, 0, true },
	{ "config.exe", 0, 0, true },
	{ "config.ini", 0, 0, true },
	{ "locale.cfg", 0, 0, true },
	{ "metin2.cfg", 0, 0, true },
	{ "mouse.cfg", 0, 0, true },
	{ "syserr.txt", 0, 0, true },
	{ "errorlog.txt", 0, 0, true },
	{ "log.txt", 0, 0, true },
	{ "log.txt", 0, 0, true },
	{ "language.cfg", 0, 0, true },
	{ "mscprotect.dll", 0, 0, true },
	// launcher dll
	{ "svside.dll", 0, 0, true },
	// launcher dll
	{ "microsoft.windowsapicodepack.dll", 0, 0, true },
	{ "microsoft.windowsapicodepack.shell.dll", 0, 0, true },
	{ "winformanimation.dll", 0, 0, true },
	{ "winformanimation.dll", 0, 0, true },
	{ "winformanimation.xml", 0, 0, true },
	// protecao
	{ "hackshield.config.error.txt", 0, 0, true },
	{ "c0d3xshield.log", 0, 0, true },
	{ "lexiashield.log", 0, 0, true },
	// launcher rename
	{ "metin2client.bin", 0, 0, true },
	{ "metin2client2.exe", 0, 0, true },
	// launcher novo
	{ ".xtreme2launcherv2.exe", 0, 0, true },
	// launcher antigo
	// { "metin2release.exe", 0, 0, true },
	// { "_xtreme2patcher.exe", 0, 0, true },
	// launcher antigo
	{ "devil.dll", 269312, 2431682744, false },
	{ "granny2.dll", 672256, 1015743114, false },
	{ "mss32.dll", 349696, 1817711331, false },
	{ "msvcp60.dll", 401462, 2134402409, false },
	{ "msvcp100.dll", 421200, 2990310814, false },
	{ "msvcr100.dll", 768848, 2111302386, false },
	{ "msvcr120.dll", 970912, 2922629643, false },
	{ "python27.dll", 2649600, 3015492484, false },
	{ "speedtreert.dll", 1806336, 2487135219, false },
};

bool checkFilesDir(const string szDirName)
{
	bool HasHack = false;

	char szDirNamePath[MAX_PATH];
	sprintf(szDirNamePath, "%s\\*", szDirName.c_str());

	WIN32_FIND_DATA f;
	HANDLE h = FindFirstFile(szDirNamePath, &f);

	if (h == INVALID_HANDLE_VALUE) { return HasHack; }

	do
	{
		if (HasHack)
			break;
		const char * name = f.cFileName;

		if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) { continue; }

		if (f.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			// do nothing if it's a folder
		}
		else
		{
			// start processing file
			PRINTME(1, "starting %s", name);
			std::string sName(name);
			std::string sPathName(szDirName + "/" + name);
			PRINTME(1, "path %s", sPathName.c_str());
			// lower file name
			std::transform(sName.begin(), sName.end(), sName.begin(), ::tolower);
			// file or symlink;
			string sNameExt = CFileNameHelper::GetExtension(sName);
			PRINTME(1, "ext %s", sNameExt.c_str());
			// workaround instead of std::find
			{
				PRINTME(1, "verify %s -> %s", sName.c_str(), sNameExt.c_str());
				bool isFilesFound = false;
				for (FolderFiles_t *i1 = std::begin(FolderFilesTable), *e1 = std::end(FolderFilesTable); i1<e1; i1++)
				{
					if (!i1->sFileName.compare(sName))
					{
						if (i1->SkipCheck == true)
						{
							isFilesFound = true;
							break;
						}
						PRINTME(1, "found %s==%s", i1->sFileName.c_str(), sName.c_str());
						DWORD dwCrc32 = GetFileCRC32(sPathName.c_str());
						// assert(dwCrc32);
						DWORD dwFileSize = f.nFileSizeLow;
						if (i1->stSize != dwFileSize)
						{
							PRINTME(1, "wrong size %u==%u", i1->stSize, dwFileSize);
							HasHack = true;
							PRINTME(0, "wrong size %u for %s", dwFileSize, sPathName.c_str());
							return HasHack;
						}
						else if (i1->dwCRC32 != dwCrc32)
						{
							PRINTME(1, "wrong crc32 %u==%u", i1->dwCRC32, dwCrc32);
							HasHack = true;
							PRINTME(0, "wrong crc32 %u for %s", dwCrc32, sPathName.c_str());
							return HasHack;
						}
						PRINTME(1, "right size %u==%u", i1->stSize, dwFileSize);
						PRINTME(1, "right crc32 %u==%u", i1->dwCRC32, dwCrc32);
						isFilesFound = true;
						break;
					}
				}
				// only mss32.dll is checked, and there's no need to check the others
				if (!isFilesFound)
				{
					DWORD dwCrc32 = GetFileCRC32(sPathName.c_str());
					DWORD dwFileSize = f.nFileSizeLow;
					PRINTME(1, "not found %s", sName.c_str());
					HasHack = true;
					PRINTME(0, "Por favor localize e apague esse arquivo:: %s %d %d", sPathName.c_str(), dwFileSize, dwCrc32);
					return HasHack;
				}
				PRINTME(1, "skipping file(%s) hack(%u) found(%u)", sName.c_str(), HasHack, isFilesFound);
			}
		}

	} while (FindNextFile(h, &f));
	FindClose(h);
	return HasHack;
}

bool __CheckFiles()
{
	PRINTME(1, "__CheckFiles processing .");
	if (checkFilesDir("."))
		return false;
	return true;
}
#endif

char gs_szErrorString[512] = "";

void ApplicationSetErrorString(const char* szErrorString)
{
	strcpy(gs_szErrorString, szErrorString);
}

bool CheckPythonLibraryFilenames()
{
	for (int i = 0; *sc_apszPythonLibraryFilenames[i] != '\n'; ++i)
	{
		std::string stFilename = "lib\\";
		stFilename += sc_apszPythonLibraryFilenames[i];

		if (_access(stFilename.c_str(), 0) != 0)
		{
			return false;
		}

		MoveFile(stFilename.c_str(), stFilename.c_str());
	}

	return true;
}

struct ApplicationStringTable
{
	HINSTANCE m_hInstance;
	std::map<DWORD, std::string> m_kMap_dwID_stLocale;
} gs_kAppStrTable;

void ApplicationStringTable_Initialize(HINSTANCE hInstance)
{
	gs_kAppStrTable.m_hInstance=hInstance;
}

const std::string& ApplicationStringTable_GetString(DWORD dwID, LPCSTR szKey)
{
	char szBuffer[512];
	char szIniFileName[256];
	char szLocale[256];

	::GetCurrentDirectory(sizeof(szIniFileName), szIniFileName);
	if(szIniFileName[lstrlen(szIniFileName)-1] != '\\')
		strcat(szIniFileName, "\\");
	strcat(szIniFileName, "metin2client.dat");

	strcpy(szLocale, LocaleService_GetLocalePath());
	if(strnicmp(szLocale, "locale/", strlen("locale/")) == 0)
		strcpy(szLocale, LocaleService_GetLocalePath() + strlen("locale/"));
	::GetPrivateProfileString(szLocale, szKey, NULL, szBuffer, sizeof(szBuffer)-1, szIniFileName);
	if(szBuffer[0] == '\0')
		LoadString(gs_kAppStrTable.m_hInstance, dwID, szBuffer, sizeof(szBuffer)-1);
	if(szBuffer[0] == '\0')
		::GetPrivateProfileString("en", szKey, NULL, szBuffer, sizeof(szBuffer)-1, szIniFileName);
	if(szBuffer[0] == '\0')
		strcpy(szBuffer, szKey);

	std::string& rstLocale=gs_kAppStrTable.m_kMap_dwID_stLocale[dwID];
	rstLocale=szBuffer;

	return rstLocale;
}

const std::string& ApplicationStringTable_GetString(DWORD dwID)
{
	char szBuffer[512];

	LoadString(gs_kAppStrTable.m_hInstance, dwID, szBuffer, sizeof(szBuffer)-1);
	std::string& rstLocale=gs_kAppStrTable.m_kMap_dwID_stLocale[dwID];
	rstLocale=szBuffer;

	return rstLocale;
}

const char* ApplicationStringTable_GetStringz(DWORD dwID, LPCSTR szKey)
{
	return ApplicationStringTable_GetString(dwID, szKey).c_str();
}

const char* ApplicationStringTable_GetStringz(DWORD dwID)
{
	return ApplicationStringTable_GetString(dwID).c_str();
}

////////////////////////////////////////////

int Setup(LPSTR lpCmdLine); // Internal function forward

bool PackInitialize (const char* c_pszFolder)
{
	std::string strFolder = c_pszFolder;
	strFolder += "/";
	#ifdef __INDEX_FROM_SOURCE__
	CEterPackManager::Instance().RegisterPack ("pack/bgm", "*");
	CEterPackManager::Instance().RegisterPack ("pack/effect", "*");
	CEterPackManager::Instance().RegisterPack ("pack/patch_ork", "*");
	CEterPackManager::Instance().RegisterPack ("pack/patch_natal", "*");
	CEterPackManager::Instance().RegisterPack ("pack/patch_halloween", "*");
	CEterPackManager::Instance().RegisterPack ("pack/patch_updates01", "*");
	CEterPackManager::Instance().RegisterPack ("pack/patch_updates02", "*");
	CEterPackManager::Instance().RegisterPack ("pack/patch_updates03", "*");
	CEterPackManager::Instance().RegisterPack ("pack/patch_updates04", "*");
	CEterPackManager::Instance().RegisterPack ("pack/patch_updates05", "*");
	CEterPackManager::Instance().RegisterPack ("pack/patch_updates06", "*");
	CEterPackManager::Instance().RegisterPack ("pack/patch_updates06.2", "*");
	CEterPackManager::Instance().RegisterPack ("pack/patch_updates07", "*");
	CEterPackManager::Instance().RegisterPack ("pack/etc", "*");
	CEterPackManager::Instance().RegisterPack ("pack/guild", "*");
	CEterPackManager::Instance().RegisterPack ("pack/icon", "*");
	CEterPackManager::Instance().RegisterPack ("pack/item", "*");
	CEterPackManager::Instance().RegisterPack ("pack/locale", "*");
	CEterPackManager::Instance().RegisterPack ("pack/maps", "*");
	CEterPackManager::Instance().RegisterPack ("pack/monster", "*");
	CEterPackManager::Instance().RegisterPack ("pack/monster2", "*");
	CEterPackManager::Instance().RegisterPack ("pack/npc", "*");
	CEterPackManager::Instance().RegisterPack ("pack/npc_mount", "*");
	CEterPackManager::Instance().RegisterPack ("pack/npc_pet", "*");
	CEterPackManager::Instance().RegisterPack ("pack/npc2", "*");
	CEterPackManager::Instance().RegisterPack ("pack/pc", "*");
	CEterPackManager::Instance().RegisterPack ("pack/pc_s_w", "*");
	CEterPackManager::Instance().RegisterPack ("pack/pc2", "*");
	CEterPackManager::Instance().RegisterPack ("pack/pc2_s_w", "*");
	CEterPackManager::Instance().RegisterPack ("pack/pc_plechito", "*");
	CEterPackManager::Instance().RegisterPack ("pack/property", "*");
	CEterPackManager::Instance().RegisterPack ("pack/season", "*");
	CEterPackManager::Instance().RegisterPack ("pack/sound", "*");
	CEterPackManager::Instance().RegisterPack ("pack/terrain", "*");
	CEterPackManager::Instance().RegisterPack ("pack/textureset", "*");
	CEterPackManager::Instance().RegisterPack ("pack/tree", "*");
	CEterPackManager::Instance().RegisterPack ("pack/zone", "*");
	#else

	#ifdef OBFUSCATE_ROT47
	std::string strIndex = strFolder + rot47 (":?56I]EIE");
	#else
	std::string strIndex = strFolder + "index.txt";
	#endif

	std::ifstream kIndex (strIndex, std::ios_base::in);

	if (!kIndex.good())
	{
		return false;
	}

	std::string strPack;

	while (std::getline (kIndex, strPack))
	{
		std::string strFileName = strFolder + strPack;
		CEterPackManager::Instance().RegisterPack (strFileName.c_str(), "*");
	}
	#endif

	#ifdef OBFUSCATE_ROT47
	std::string strRoot = strFolder + rot47 ("C@@E");
	#else
	std::string strRoot = strFolder + "root";
	#endif
	CEterPackManager::Instance().RegisterRootPack (strRoot.c_str());
	CSoundData::SetPackMode();
	return true;
}

bool RunMainScript(CPythonLauncher& pyLauncher, const char* lpCmdLine)
{
	initpack();
	initdbg();
	initime();
	initgrp();
	initgrpImage();
	initgrpText();
	initwndMgr();
	/////////////////////////////////////////////
	initudp();
	initapp();
	initsystemSetting();
	initchr();
	initchrmgr();
	initPlayer();
	initItem();
	initNonPlayer();
	initTrade();
	initChat();
	initTextTail();
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	initofflineshop();
#endif
	initnet();
	initMiniMap();
	initProfiler();
	initEvent();
	initeffect();
	initfly();
	initsnd();
	initeventmgr();
	initshop();
	initskill();
	initquest();
	initBackground();
	initMessenger();
#ifdef ENABLE_BONUS_SWITCHER
	initBonusSwitcher();
#endif
	initsafebox();
	initguild();
	initServerStateChecker();
	initRenderTarget();

#ifdef __USE_CYTHON__
	// don't add this line if you're implementing cython via .pyd:
	// initrootlibManager();
#endif
#ifdef __USE_EXTRA_CYTHON__
	// don't add this line if you're implementing cython via .pyd:
	// inituiscriptlibManager();
#endif

    PyObject * builtins = PyImport_ImportModule("__builtin__");
#ifdef NDEBUG // @warme601 _DISTRIBUTE -> NDEBUG
	PyModule_AddIntConstant(builtins, "__DEBUG__", 0);
#else
	PyModule_AddIntConstant(builtins, "__DEBUG__", 1);
#endif
#ifdef __USE_CYTHON__
	PyModule_AddIntConstant(builtins, "__USE_CYTHON__", 1);
#else
	PyModule_AddIntConstant(builtins, "__USE_CYTHON__", 0);
#endif
#ifdef __USE_EXTRA_CYTHON__
	PyModule_AddIntConstant(builtins, "__USE_EXTRA_CYTHON__", 1);
#else
	PyModule_AddIntConstant(builtins, "__USE_EXTRA_CYTHON__", 0);
#endif

	// RegisterCommandLine
	{
		std::string stRegisterCmdLine;

		const char * loginMark = "-cs";
		const char * loginMark_NonEncode = "-ncs";
		const char * seperator = " ";

		std::string stCmdLine;
		const int CmdSize = 3;
		vector<std::string> stVec;
		SplitLine(lpCmdLine,seperator,&stVec);
		if (CmdSize == stVec.size() && stVec[0]==loginMark)
		{
			char buf[MAX_PATH];
			base64_decode(stVec[2].c_str(),buf);
			stVec[2] = buf;
			string_join(seperator,stVec,&stCmdLine);
		}
		else if (CmdSize <= stVec.size() && stVec[0]==loginMark_NonEncode)
		{
			stVec[0] = loginMark;
			string_join(" ",stVec,&stCmdLine);
		}
		else
			stCmdLine = lpCmdLine;

		PyModule_AddStringConstant(builtins, "__COMMAND_LINE__", stCmdLine.c_str());
	}
	{
		vector<std::string> stVec;
		SplitLine(lpCmdLine," " ,&stVec);

		if (stVec.size() != 0 && "--pause-before-create-window" == stVec[0])
		{
#ifdef XTRAP_CLIENT_ENABLE
			if (!XTrap_CheckInit())
				return false;
#endif
			system("pause");
		}
#ifdef ENABLE_DAEMONPROTECTION
		DWORD dwCrc32, dwSize;
		if (!((dwCrc32 = GetFileCRC32(DPDLL_FILENAME))==DPDLL_CRC32))
		{
			TraceError("game wrong crc32 %d", dwCrc32);
			return false;
		}
		if (!((dwSize = GetFileSize(DPDLL_FILENAME))==DPDLL_FILESIZE))
		{
			TraceError("game wrong size %d", dwSize);
			return false;
		}
		if (!LoadLibraryA(DPDLL_FILENAME))
		{
			TraceError("game not loaded");
			return false;
		}
#endif

#ifdef __USE_CYTHON__
		if (!pyLauncher.RunLine("import rootlib\nrootlib.moduleImport('system')"))
#else
		if (!pyLauncher.RunFile("system.py"))
#endif
		{
			TraceError("RunMain Error");
			return false;
		}
	}

	return true;
}

bool Main(HINSTANCE hInstance, LPSTR lpCmdLine)
{
#ifdef BLOCK_WRITE_PROCESS_MEMORY
	oWriteProcessMemory = (WriteProcessMemoryFn)detour_ptr ("WriteProcessMemory", (PVOID)hkWriteProcessMemory, GetModuleHandleA ("Kernel32.dll"));
#endif

#ifdef LOCALE_SERVICE_YMIR
	extern bool g_isScreenShotKey;
	g_isScreenShotKey = true;
#endif

	DWORD dwRandSeed=time(NULL)+DWORD(GetCurrentProcess());
	srandom(dwRandSeed);
	srand(random());

	SetLogLevel(1);

#ifdef LOCALE_SERVICE_VIETNAM_MILD
	extern BOOL USE_VIETNAM_CONVERT_WEAPON_VNUM;
	USE_VIETNAM_CONVERT_WEAPON_VNUM = true;
#endif

	if (_access("perf_game_update.txt", 0)==0)
	{
		DeleteFile("perf_game_update.txt");
	}

	if (_access("newpatch.exe", 0)==0)
	{
		system("patchupdater.exe");
		return false;
	}
#ifndef __VTUNE__
	ilInit();
#endif
	if (!Setup(lpCmdLine))
		return false;

#ifdef _DEBUG
	OpenConsoleWindow();
	OpenLogFile(true); // true == uses syserr.txt and log.txt
#else
	OpenLogFile(false); // false == uses syserr.txt only
#endif

	static CLZO				lzo;
	static CEterPackManager	EterPackManager;
#ifdef ENABLE_HWID_BAN_SYSTEM
	static HWIDMANAGER		hwidManager;
#endif

	if (!PackInitialize("pack"))
	{
		LogBox("Pack Initialization failed. Check log.txt file..");
		return false;
	}

	if(LocaleService_LoadGlobal(hInstance))
		SetDefaultCodePage(LocaleService_GetCodePage());

#ifdef ENABLE_PYLIB_CHECK
	if (!__CheckPyLibFiles())
		return false;
#endif
#ifdef ENABLE_MILES_CHECK
	if (!__CheckMilesFiles())
		return false;
#endif
#ifdef ENABLE_FILES_CHECK
	if (!__CheckFiles())
		return false;
#endif
	CPythonApplication * app = new CPythonApplication;

	app->Initialize(hInstance);

	bool ret=false;
	{
		CPythonLauncher pyLauncher;
		CPythonExceptionSender pyExceptionSender;
		SetExceptionSender(&pyExceptionSender);

		if (pyLauncher.Create())
		{
			ret=RunMainScript(pyLauncher, lpCmdLine);
		}

		//ProcessScanner_ReleaseQuitEvent();

		app->Clear();

		timeEndPeriod(1);
		pyLauncher.Clear();
	}

	app->Destroy();
	delete app;

	return ret;
}

HANDLE CreateMetin2GameMutex()
{
	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));
	sa.nLength				= sizeof(sa);
	sa.lpSecurityDescriptor	= NULL;
	sa.bInheritHandle		= FALSE;

	return CreateMutex(&sa, FALSE, "Metin2GameMutex");
}

void DestroyMetin2GameMutex(HANDLE hMutex)
{
	if (hMutex)
	{
		ReleaseMutex(hMutex);
		hMutex = NULL;
	}
}

void __ErrorPythonLibraryIsNotExist()
{
	LogBoxf("FATAL ERROR!! Python Library file not exist!");
}

bool __IsTimeStampOption(LPSTR lpCmdLine)
{
	const char* TIMESTAMP = "/timestamp";
	return (strncmp(lpCmdLine, TIMESTAMP, strlen(TIMESTAMP))==0);
}

void __PrintTimeStamp()
{
#ifdef	_DEBUG
	if (__IS_TEST_SERVER_MODE__)
		LogBoxf("METIN2 BINARY TEST DEBUG VERSION %s  ( MS C++ %d Compiled )", __TIMESTAMP__, _MSC_VER);
	else
		LogBoxf("METIN2 BINARY DEBUG VERSION %s ( MS C++ %d Compiled )", __TIMESTAMP__, _MSC_VER);

#else
	if (__IS_TEST_SERVER_MODE__)
		LogBoxf("METIN2 BINARY TEST VERSION %s  ( MS C++ %d Compiled )", __TIMESTAMP__, _MSC_VER);
	else
		LogBoxf("METIN2 BINARY DISTRIBUTE VERSION %s ( MS C++ %d Compiled )", __TIMESTAMP__, _MSC_VER);
#endif
}

bool __IsLocaleOption(LPSTR lpCmdLine)
{
	return (strcmp(lpCmdLine, "--locale") == 0);
}

bool __IsLocaleVersion(LPSTR lpCmdLine)
{
	return (strcmp(lpCmdLine, "--perforce-revision") == 0);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef ENABLE_ARGUMENT_LAUNCHER
	if (strstr(lpCmdLine, "--39NS7AP9RXMBVXO49VTHNO79RCHREF60") != 0)
	{
		true;
	}
	else
	{
		LogBox ("O cliente so podera ser aberto pelo metin2client!");
		system("start .xtreme2launcherv2.exe");
		return false;
	}

	// if (strstr(lpCmdLine, "--rrtfvrqww") == 0)
	// {
		// system ("start .xtreme2launcherv2.exe");
		// return 0;
	// }
#endif

	LoadLibraryA("svside.dll");
	if (strstr(lpCmdLine, "--hackshield") != 0)
		return 0;

#ifdef ENABLE_CHECK_FILES_MD5
	CheckMD5Filenames();
#endif

#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc( 110247 );
#endif

	ApplicationStringTable_Initialize(hInstance);

	LocaleService_LoadConfig("locale.cfg");
	SetDefaultCodePage(LocaleService_GetCodePage());

#if defined(CHECK_LATEST_DATA_FILES)
	if (!CheckLatestFiles())
		return 0;
#endif

	bool bQuit = false;
	bool bAuthKeyChecked = false;
	int nArgc = 0;
	PCHAR* szArgv = CommandLineToArgv( lpCmdLine, &nArgc );

	for( int i=0; i < nArgc; i++ ) {
		if(szArgv[i] == 0)
			continue;
		if (__IsLocaleVersion(szArgv[i]))
		{
			char szModuleName[MAX_PATH];
			char szVersionPath[MAX_PATH];
			GetModuleFileName(NULL, szModuleName, sizeof(szModuleName));
			sprintf(szVersionPath, "%s.version", szModuleName);
			FILE* fp = fopen(szVersionPath, "wt");
			if (fp)
			{
				extern int METIN2_GET_VERSION();
				fprintf(fp, "r%d\n", METIN2_GET_VERSION());
				fclose(fp);
			}
			bQuit = true;
		} else if (__IsLocaleOption(szArgv[i]))
		{
			FILE* fp=fopen("locale.txt", "wt");
			fprintf(fp, "service[%s] code_page[%d]",
				LocaleService_GetName(), LocaleService_GetCodePage());
			fclose(fp);
			bQuit = true;
		} else if (__IsTimeStampOption(szArgv[i]))
		{
			__PrintTimeStamp();
			bQuit = true;
		} else if ((strcmp(szArgv[i], "--force-set-locale") == 0))
		{
			if (nArgc <= i + 2)
			{
				MessageBox(NULL, "Invalid arguments", ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);
				goto Clean;
			}

			const char* localeName = szArgv[++i];
			const char* localePath = szArgv[++i];

			LocaleService_ForceSetLocale(localeName, localePath);
		}
	}

	if(bQuit)
		goto Clean;

#if defined(NEEDED_COMMAND_ARGUMENT)
	if (strstr(lpCmdLine, NEEDED_COMMAND_ARGUMENT) == 0) {
		MessageBox(NULL, ApplicationStringTable_GetStringz(IDS_ERR_MUST_LAUNCH_FROM_PATCHER, "ERR_MUST_LAUNCH_FROM_PATCHER"), ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);
			goto Clean;
	}
#endif

#if defined(NEEDED_COMMAND_CLIPBOARD)
	{
		CHAR szSecKey[256];
		CPostIt cPostIt( "VOLUME1" );

		if( cPostIt.Get( "SEC_KEY", szSecKey, sizeof(szSecKey) ) == FALSE ) {
			MessageBox(NULL, ApplicationStringTable_GetStringz(IDS_ERR_MUST_LAUNCH_FROM_PATCHER, "ERR_MUST_LAUNCH_FROM_PATCHER"), ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);
			goto Clean;
		}
		if( strstr(szSecKey, NEEDED_COMMAND_CLIPBOARD) == 0 ) {
			MessageBox(NULL, ApplicationStringTable_GetStringz(IDS_ERR_MUST_LAUNCH_FROM_PATCHER, "ERR_MUST_LAUNCH_FROM_PATCHER"), ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);
			goto Clean;
		}
		cPostIt.Empty();
	}
#endif

	WebBrowser_Startup(hInstance);

#ifndef ENABLE_PYLIB_CHECK
	if (!CheckPythonLibraryFilenames())
	{
		__ErrorPythonLibraryIsNotExist();
		goto Clean;
	}
#endif

	Main(hInstance, lpCmdLine);

	WebBrowser_Cleanup();

	::CoUninitialize();

	if(gs_szErrorString[0])
		MessageBox(NULL, gs_szErrorString, ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);

Clean:
	SAFE_FREE_GLOBAL(szArgv);

	return 0;
}

#if GrannyProductMinorVersion==4 || GrannyProductMinorVersion==7
static void GrannyError(granny_log_message_type Type,
						granny_log_message_origin Origin,
						char const *Error,
						void *UserData)
{
	TraceError("GRANNY: %s", Error);
}
#elif GrannyProductMinorVersion==11 || GrannyProductMinorVersion==9 || GrannyProductMinorVersion==8
static void GrannyError(granny_log_message_type Type,
						granny_log_message_origin Origin,
						char const*  File,
						granny_int32x Line,
						char const *Error,
						void *UserData)
{
	//Origin==GrannyFileReadingLogMessage for granny run-time tag& revision warning (Type==GrannyWarningLogMessage)
	//Origin==GrannyControlLogMessage for miss track_group on static models as weapons warning (Type==GrannyWarningLogMessage)
	//Origin==GrannyMeshBindingLogMessage for miss bone ToSkeleton on new ymir models error (Type==GrannyErrorLogMessage)
	// if (Type == GrannyWarningLogMessage)
	if (Origin==GrannyFileReadingLogMessage || Origin==GrannyControlLogMessage || Origin==GrannyMeshBindingLogMessage)
		return;
	TraceError("GRANNY: %s(%d): ERROR: %s --- [%d] %s --- [%d] %s", File, Line, Error, Type, GrannyGetLogMessageTypeString(Type), Origin, GrannyGetLogMessageOriginString(Origin));
}
#else
#error "unknown granny version"
#endif

int Setup(LPSTR lpCmdLine)
{
	TIMECAPS tc;
	UINT wTimerRes;

	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
		return 0;

	wTimerRes = MINMAX(tc.wPeriodMin, 1, tc.wPeriodMax);
	timeBeginPeriod(wTimerRes);

	granny_log_callback Callback;
    Callback.Function = GrannyError;
    Callback.UserData = 0;
    GrannySetLogCallback(&Callback);
	return 1;
}
