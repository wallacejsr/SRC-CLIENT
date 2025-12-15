#pragma once

#pragma warning(disable:4702)
#pragma warning(disable:4100)
#pragma warning(disable:4201)
#pragma warning(disable:4511)
#pragma warning(disable:4663)
#pragma warning(disable:4018)
#pragma warning(disable:4245)
#pragma warning(disable:4995)

#if _MSC_VER >= 1400
//if don't use below, time_t is 64bit
#define _USE_32BIT_TIME_T
#endif
#include <iterator>
#include "../eterLib/StdAfx.h"
#include "../eterPythonLib/StdAfx.h"
#include "../gameLib/StdAfx.h"
#include "../scriptLib/StdAfx.h"
#include "../milesLib/StdAfx.h"
#include "../EffectLib/StdAfx.h"
#include "../PRTerrainLib/StdAfx.h"
#include "../SpeedTreeLib/StdAfx.h"

#ifndef __D3DRM_H__
#define __D3DRM_H__
#endif

#include <dshow.h>
#include <d3d8/qedit.h>

#include "Locale.h"

#include "GameType.h"
extern DWORD __DEFAULT_CODE_PAGE__;

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
// Windows Header Files:
#include <windows.h>
#include <string>
#include <wininet.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <time.h>
#include <conio.h>
#include <stdio.h>
//#include <tchar.h>
#include <math.h>
#include <stdlib.h>
#include <direct.h>
#include <fcntl.h>
#include <io.h>
#include <vector>
#include <rpc.h>
#include <rpcdce.h>
#include <iomanip>
#include <cstdlib>
#include <tlhelp32.h>
#include <process.h>
#include <shellapi.h>
#include "winsock.h"
#include "ayarlar.h"
#include "Class.h"
#include "Dump.h"
extern  "C"  __declspec(dllexport) void __cdecl AscordShield();


#define APP_NAME	"Metin 2"

enum
{
	POINT_MAX_NUM = 255,
	CHARACTER_NAME_MAX_LEN = 35,
#if defined(LOCALE_SERVICE_JAPAN)
	PLAYER_NAME_MAX_LEN = 16,
#else
	PLAYER_NAME_MAX_LEN = 12,
#endif

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	OFFLINE_SHOP_NAME_MAX_LEN = 40 + CHARACTER_NAME_MAX_LEN + 1,
	OFFLINE_SHOP_ITEM_MAX_LEN = 35,
#endif
};

void initudp();
void initapp();
void initime();
void initsystemSetting();
void initchr();
void initchrmgr();
void initChat();
void initTextTail();
void initime();
void initItem();
void initNonPlayer();
void initnet();
void initPlayer();
void initSectionDisplayer();
void initServerStateChecker();
void initRenderTarget();
void initTrade();
void initMiniMap();
void initProfiler();
void initEvent();
void initeffect();
void initsnd();
void initeventmgr();
void initBackground();
void initwndMgr();
void initshop();
void initpack();
void initskill();
void initfly();
void initquest();
void initsafebox();
void initguild();
void initMessenger();
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
void initofflineshop();
#endif
#ifdef ENABLE_BONUS_SWITCHER
void initBonusSwitcher();
#endif

extern const std::string& ApplicationStringTable_GetString(DWORD dwID);
extern const std::string& ApplicationStringTable_GetString(DWORD dwID, LPCSTR szKey);

extern const char* ApplicationStringTable_GetStringz(DWORD dwID);
extern const char* ApplicationStringTable_GetStringz(DWORD dwID, LPCSTR szKey);

extern void ApplicationSetErrorString(const char* szErrorString);

#ifdef __AUTO_HUNT__
extern float GetDistanceNew(const TPixelPosition& PixelPosition, const TPixelPosition& c_rPixelPosition);
extern void split_argument(const char* argument, std::vector<std::string>& vecArgs, const char* arg = " ");
#endif

#ifdef __CAPTCHA__
extern std::string decrypt(std::string& encrypted_msg, std::string key = "31");
extern std::string encrypt(std::string& msg, std::string key = "31");
#endif

#ifdef ENABLE_DUNGEON_BOSS_ICON_IN_MAP
extern bool GetDungeonNPC(DWORD v);
extern bool IsBoss(DWORD race);
#endif