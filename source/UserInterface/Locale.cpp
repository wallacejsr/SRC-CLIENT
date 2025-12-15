#include "StdAfx.h"
#include "Locale.h"
#include "PythonApplication.h"
#include "resource.h"
#include "../eterBase/CRC32.h"
#include "../eterpack/EterPackManager.h"
#include "../eterLocale/Japanese.h"
#include <windowsx.h>

#include <windows.h>
#include <assert.h>
#include <tlhelp32.h>
#include <imagehlp.h>
#include <psapi.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <commdlg.h>
#include "aclapi.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
//#include <VersionHelpers.h> //USE THIS WITH COMPILATION VC++ 120 WITHOUT XP FEATURE AND IT WORK.
#include "EncryptorStrings.h"
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "imagehlp.lib")
#pragma comment(lib, "comdlg32.lib")
#define JUMP_TO_OFFSET(_BaseOffset, _TargetOffset) ((LONG)(_TargetOffset) - (LONG)(_BaseOffset) - 5)
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#define STATUS_INFO_LENGTH_MISMATCH	((NTSTATUS)0xC0000004L)
#define STATUS_BUFFER_OVERFLOW ((NTSTATUS)0x80000005L)
#define SystemHandleInformation	16

//START CONFIGURATION OF PROTECTION

bool LibCheck = false;
bool YmirWorkFolderCheck = true;
//bool PsListCheck = false; //Enable it, and find the current timestamp and sizeofcode of m2bob, with logmode enable.

//APPTITLE MUST CONTAIN THIS: <Protection by Frankie Commando - www.gamesecurity.eu>
//FOR EXAMPLE: Metin2Commando - <Protection by Frankie Commando - www.gamesecurity.eu>
//std::string AppTitle = UltimateDecrypt("7d12312e362622362a2e2c63233b630730222f292a2462002e2f2e202c272e626e613534366c24202f2632272034302a353b6d24377d");
std::string AppTitle = "AEGON";
std::string WhiteList[] = {
	AppTitle,
	UltimateDecrypt("0d0d04"),
	UltimateDecrypt("0c010a61212c2c2f222f266329232d252e2a2f2563362b2d252d34"),
	UltimateDecrypt("0923202a112b28272f25626e610626352720352b2c2f"),
	UltimateDecrypt("0d233a24302625620b282627242c63162b2d252d34"),
	UltimateDecrypt("0e0010610d33242c040d620772066315272e31620724342a222763162b2d252d34"),
	UltimateDecrypt("060d0c03180c610c2635352c3329630434262f363061112a2f2963162b2d252d34"),
	UltimateDecrypt("05272520372f35620a0c07"),
	UltimateDecrypt("0c110015040a0c0763140b"),
	UltimateDecrypt("050b062c152a2f"),
	UltimateDecrypt("0021372834260c2d35282763162b2d252d34"),
	UltimateDecrypt("0c2b20332d302e243761142a3237222d62006a696313372d352b2e24620f28203120303a"),
	UltimateDecrypt("06060a6a6214282c272e35"),
	UltimateDecrypt("1212060406212836630f2737362d312a620637272d353163122b2d2a6214282c272e35"),
	UltimateDecrypt("022d2e312d2d242c37162b2d252d34"),
	UltimateDecrypt("092b2d35152d25"),
	UltimateDecrypt("092b2d35152d25"),
	UltimateDecrypt("122d13181d1608"),
	UltimateDecrypt("101313282c3a282c1608152d25"),
	UltimateDecrypt("3236223527012030"),
	UltimateDecrypt("06121a1e170a"),
	UltimateDecrypt("112b2d382b2d313737140b"),
	UltimateDecrypt("122d14031d1608"),
	UltimateDecrypt("0c000a2c27"),
	UltimateDecrypt("10131434202a140b142f26"),
	UltimateDecrypt("13011c080c1314161c140b"),
	UltimateDecrypt("282c333436012030"),
	UltimateDecrypt("1774160f0b10110b0e"),
	UltimateDecrypt("160c1318170a"),
	UltimateDecrypt("202b0203013031"),
	UltimateDecrypt("03232a25371c080f16080f0413"),
	UltimateDecrypt("03232a2537202f1d160812312e3a3a"),
	UltimateDecrypt("03232a25371318012f203130"),
	UltimateDecrypt("11232f2c0b2d313737140b"),
	UltimateDecrypt("05041608"),
	UltimateDecrypt("0a170d0d170d1e170a"),
	UltimateDecrypt("122d242e371c1511051e170a"),
};

typedef struct {
	unsigned long dwAddress;
	unsigned char pBytes[8];
} MDump;

MDump mDumps[] = {	
{0x4017D0, {0x55, 0x8b, 0xec, 0x6a, 0xff, 0x68, 0x08, 0x51}}, // Mu Window (testes)
{0xD1A35C, {0xe9, 0x70, 0xfe, 0xff, 0xff, 0xb8, 0x08, 0xce}}, // DARKTERRO
{0x40BD6E, {0xFF, 0x25, 0x00, 0x20, 0x40, 0x00, 0x00, 0x00}}, // #### Privado #### AutoPot ####  With ASLR active
{0x4A9BF6, {0xe8, 0x0E, 0xAE, 0x00, 0x00, 0xE9, 0x79, 0xFE}}, //Process Explorer Portable (testes)
{0x8BBBC0, {0x6C, 0x05, 0xE0, 0xDB, 0x8B, 0x00, 0x00, 0xE8}}, //Cheat Engine 6.4 (testes)
{0x401000, {0xEB, 0x10, 0x66, 0x62, 0x3A, 0x43, 0x2B, 0x2B}}, //OllyDbg 1.10
{0x407973, {0xF3, 0xA5, 0x8B, 0xCA, 0x83, 0xE1, 0x03, 0xF3}}, //OllyDbg 2.00
{0x610649, {0xBA, 0x04, 0x08, 0x61, 0x00, 0xE8, 0x65, 0x4D}}, //Cheat Engine 5.6.1
{0x435217, {0xFF, 0x15, 0xC4, 0xD6, 0x47, 0x00, 0x8B, 0xF0}}, //Shotbot
{0x4351E6, {0x8B, 0x15, 0x64, 0xF3, 0x49, 0x00, 0x6A, 0x00}}, //Novo Shotbot
{0x4039e3, {0x81, 0xEC, 0xD4, 0x02, 0x00, 0x00, 0x53, 0x55}}, //Process hacker portable
{0x44E08C, {0x64, 0x89, 0x25, 0x00, 0x00, 0x00, 0x00, 0x83}}, //WPE Pro
{0x4307BE, {0x75, 0x0A, 0x6A, 0x1C, 0xE8, 0x49, 0x01, 0x00}}, //WPE Pro
{0x4851C2, {0x75, 0x1C, 0x53, 0x8B, 0xCE, 0xFF, 0x75, 0xE4}}, //WPe Pro
{0x44397B, {0x75, 0x07, 0x8B, 0xCF, 0xE8, 0xF8, 0xF2, 0xFF}}, //WPe Pro
{0x401468, {0x74, 0x03, 0x8B, 0x6D, 0x04, 0x8B, 0x97, 0x30}}, //WPe Pro
{0x574EEC, {0xE8, 0x8B, 0xEA, 0xF1, 0xFF, 0x8D, 0x45, 0xEC}}, //Cheat Engine
{0x4CBD70, {0x8D, 0x85, 0x7C, 0xFE, 0xFF, 0xFF, 0xBA, 0x03}}, //Cheat Engine
{0x591FC0, {0xE8, 0x07, 0x23, 0xF0, 0xFF, 0x8D, 0x45, 0xEC}}, //Cheat Engine
{0x5839E7, {0x8D, 0x45, 0xB0, 0x50, 0x6A, 0x08, 0x8D, 0x85}}, //Cheat Engine
{0x4CBE2B, {0x8D, 0x55, 0xF0, 0xB9, 0x04, 0x00, 0x00, 0x00}}, //Cheat Engine
{0x5CF354, {0x55, 0x8B, 0xEC, 0x83, 0xC4, 0xEC, 0x53, 0x33}}, //Cheat Engine
{0x5CF440, {0xE8, 0x37, 0xA3, 0xFC, 0xFF, 0xE8, 0x8E, 0x96}}, //Chear Engine
{0x5CF43D, {0x8D, 0x45, 0xEC, 0xE8, 0x37, 0xA3, 0xFC, 0xFF}}, //Cheat Engine
{0x5FECF4, {0x55, 0x8B, 0xEC, 0x83, 0xC4, 0xEC, 0x53, 0x33}}, //Cheat Engine
{0x6105D4, {0x55, 0x8B, 0xEC, 0x83, 0xC4, 0xE8, 0x53, 0x33}}, //Cheat Engine
{0x5FED5B, {0xE8, 0x10, 0xC3, 0xE9, 0xFF, 0x8B, 0x0D, 0x64}}, //Cheat Engine
{0x434460, {0xc6, 0x05, 0x60, 0xf0, 0x43, 0x00, 0x00, 0xe8}}, //Cheat Engine
{0x5674D4, {0x55, 0x8B, 0xEC, 0x83, 0xC4, 0xEC, 0x53, 0x33}}, //Cheat Engine
{0x5AA16C, {0xE8, 0x13, 0x40, 0xFF, 0xFF, 0xE8, 0x86, 0x2C}}, //Cheat Engine
{0x591F94, {0x55, 0x8B, 0xEC, 0x83, 0xC4, 0xEC, 0x53, 0x33}}, //Cheat Engine
{0x5CF354, {0x78, 0xAA, 0x4A, 0x00, 0x48, 0xAA, 0x4A, 0x00}}, //Cheat Engine
{0x606140, {0x8C, 0x79, 0x60, 0x00, 0xE0, 0xA8, 0x60, 0x00}}, //Cheat Engine
{0x574EC0, {0x55, 0x8B, 0xEC, 0x83, 0xC4, 0xEC, 0x53, 0x33}}, //Cheat Engine
{0x40C484, {0x8B, 0x45, 0x08, 0xFF, 0x70, 0x0C, 0xFF, 0x70}}, //Cheat Engine
{0x408771, {0xEB, 0x07, 0x8B, 0x45, 0x0C, 0x33, 0xD2, 0x89}}, //Cheat Engine
{0x401704, {0x68, 0x84, 0x24, 0x40, 0x00, 0xE8, 0xEE, 0xFF}}, //MuPie
{0x401B28, {0x68, 0xD8, 0x2A, 0x40, 0x00, 0xE8, 0xF0, 0xFF}}, //MuPie
{0x4388C0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x4A, 0x49, 0x4A}}, //Mupie
{0x4380F0, {0x53, 0x00, 0x5F, 0x00, 0x56, 0x00, 0x45, 0x00}}, //MuPie
{0x401350, {0xEB, 0x10, 0x66, 0x62, 0x3A, 0x43, 0x2B, 0x2B}}, //WildProxy
{0x401401, {0xE8, 0xB6, 0xFF, 0xFF, 0xFF, 0x50, 0x50, 0xFF}}, //WildProxy
{0x4013B0, {0xE8, 0xD7, 0xFF, 0xFF, 0xFF, 0xB9, 0xB4, 0x00}}, //WildProxy
{0x40133A, {0xA3, 0x8F, 0x20, 0x47, 0x00, 0x52, 0x6A, 0x00}}, //WildProxy
{0x401320, {0xEB, 0x10, 0x66, 0x62, 0x3A, 0x43, 0x2B, 0x2B}}, //WildProxy
{0x4013F9, {0xE8, 0x1A, 0x21, 0x06, 0x00, 0xA3, 0x8B, 0x90}}, //WildProxy
{0x512134, {0x75, 0x05, 0x8A, 0x16, 0x46, 0x12, 0xD2, 0x73}}, //WildProxy
{0x512014, {0xB8, 0x00, 0x00, 0x00, 0x00, 0x60, 0x0B, 0xC0}}, //WildProxy
{0x401414, {0xEB, 0x10, 0x66, 0x62, 0x3A, 0x43, 0x2B, 0x2B}}, //WildProxy
{0x40D6EB, {0xE9, 0x0C, 0x0F, 0x00, 0x00, 0xE8, 0xAF, 0x15}}, //WildProxy
{0x41D39A, {0xEB, 0x0B, 0x0B, 0xEB, 0x0B, 0x0B, 0xEB, 0x35}}, //SpeederXP
{0x42FAA4, {0xA7, 0x62, 0x62, 0x62, 0x68, 0x68, 0x68, 0x7B}}, //SpeederXP
{0x50541A, {0xBE, 0xDE, 0xEE, 0xE2, 0x52, 0xE8, 0x6B, 0xFA}}, //SpeederXP
{0x42727A, {0x55, 0x8B, 0xEC, 0x6A, 0xFF, 0x68, 0xA8, 0x7A}}, //SpeederXP
{0x5053C8, {0x68, 0x3A, 0x38, 0x21, 0xDB, 0xE8, 0xA9, 0xAB}}, //SpeederXP
{0x410086, {0x55, 0x8B, 0xEC, 0x6A, 0xFF, 0x68, 0x98, 0x3D}}, //Game Speed Changer
{0x40FBB6, {0x55, 0x8B, 0xEC, 0x6A, 0xFF, 0x68, 0x48, 0x3D}}, //Game Speed Changer
{0x40C0B0, {0x70, 0x6C, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6F}}, //Speed Hack Simplifier
{0x40E04E, {0x53, 0x68, 0x61, 0x64, 0x6F, 0x77, 0x42, 0x65}}, //Speed Hack Simplifier
{0x402868, {0x52, 0x92, 0x3B, 0x8D, 0xD5, 0x33, 0x78, 0xB7}}, //Speed Hack Simplifier
{0x417259, {0x89, 0x42, 0xBC, 0xBA, 0x14, 0x00, 0x00, 0x80}}, //Speed Hack
{0x40134A, {0xA1, 0x8B, 0x50, 0x48, 0x00, 0xC1, 0xE0, 0x02}}, //Speed Hack
{0x40134F, {0xC1, 0xE0, 0x02, 0xA3, 0x8F, 0x40, 0x47, 0x00}}, //Speed Hack
{0x401338, {0xEB, 0x10, 0x66, 0x62, 0x3A, 0x43, 0x2B, 0x2B}}, //Speed Hack
{0x401414, { 0x68, 0xA4, 0x22, 0x40, 0x00, 0xE8, 0xEE, 0xFF }}, //Speed Hack
{ 0x4217E0, { 0x60, 0xBE, 0x00, 0xD0, 0x41, 0x00, 0x8D, 0xBE } }, // !xSpeed.net
{ 0x41F04A, { 0x8D, 0x5D, 0x5E, 0x53, 0x50, 0xFF, 0x95, 0x49 } }, // !xSpeed.net
{ 0x401100, { 0x33, 0xC0, 0x8B, 0x04, 0x9D, 0x21, 0x79, 0x40 } }, // !xSpeed.net
{ 0x41D4FE, { 0x30, 0xB3, 0x7F, 0x4E, 0xB8, 0x85, 0x54, 0xBC } }, // !xSpeed.net
{ 0x41F001, { 0x60, 0xE8, 0x03, 0x00, 0x00, 0x00, 0xE9, 0xEB } }, // !xSpeed.net
{ 0x4217E0, { 0x60, 0xBE, 0x00, 0xD0, 0x41, 0x00, 0x8D, 0xBE } }, // !xSpeed.net
{ 0x420630, { 0x60, 0xBE, 0x00, 0xC0, 0x41, 0x00, 0x8D, 0xBE } }, // !xSpeed.net
{ 0x420001, { 0x60, 0xE8, 0x03, 0x00, 0x00, 0x00, 0xE9, 0xEB } }, // !xSpeed.Pro
{ 0x426ECA, { 0x55, 0x8B, 0xEC, 0x6A, 0xFF, 0x68, 0x90, 0x7A } }, //Speed Gear
{ 0x40970E, { 0x68, 0xB4, 0x98, 0x40, 0x00, 0x64, 0xA1, 0x00 } }, //Speed Gear
{ 0x568E9A, { 0x68, 0xB8, 0xF9, 0x85, 0x13, 0xE8, 0x9D, 0x53 } }, //Speed Gear
{ 0x4011D4, { 0x68, 0x50, 0x8E, 0x40, 0x00, 0xE8, 0xF0, 0xFF } }, //HackSpeed
{ 0x416B41, { 0xE8, 0xBC, 0x57, 0x00, 0x00, 0xA3, 0xA4, 0xB2 } }, //Game Speed Adjuster
{ 0x416AB0, { 0x55, 0x8B, 0xEC, 0x6A, 0xFF, 0x68, 0xC0, 0xC0 } }, //Game Speed Adjuster
{ 0x4BCFA4, { 0x55, 0x8B, 0xEC, 0x83, 0xC4, 0xF0, 0x53, 0x56 } }, //Xelerator
{ 0x430A27, { 0xE9, 0x06, 0x01, 0x00, 0x00, 0x3B, 0xD1, 0x72 } }, //Speed Wizard
{ 0x401027, { 0x73, 0xE3, 0xBC, 0x49, 0x73, 0x62, 0x72, 0x4D } }, //Game Acelerator
{ 0x492BA6, { 0x8D, 0xBE, 0x00, 0xF0, 0xF9, 0xFF, 0xC7, 0x87 } }, //D-C SpeedHack
{ 0x4DB7C3, { 0x55, 0x8B, 0xEC, 0x6A, 0xFF, 0x68, 0x20, 0x5B } }, //Game Optimizer
{ 0x4DD014, { 0xB8, 0x00, 0x00, 0x00, 0x00, 0x60, 0x0B, 0xC0 } }, //Win Speeder
{ 0x401414, { 0x68, 0xA4, 0x22, 0x40, 0x00, 0xE8, 0xEE, 0xFF } }, //99.62t Speed Hack
{ 0x4C8A60, { 0x60, 0xBE, 0x00, 0x00, 0x48, 0x00, 0x8D, 0xBE } }, //Simple Net Speed
{ 0x492BDA, { 0x75, 0x07, 0x8B, 0x1E, 0x83, 0xEE, 0xFC, 0x11 } }, //Agility Hack By TopGun
{ 0x48A220, { 0x60, 0xBE, 0x00, 0xA0, 0x45, 0x00, 0x8D, 0xBE } }, //MU-SS4 Speed Hack
{ 0x43250B, { 0xEB, 0x02, 0x33, 0xC0, 0xC2, 0x04, 0x00, 0x83 } }, //Game Speeder
{ 0x4320F0, { 0x53, 0x00, 0x5F, 0x00, 0x56, 0x00, 0x45, 0x00 } }, //Cheat Happens
{ 0x4340F0, { 0x53, 0x00, 0x5F, 0x00, 0x56, 0x00, 0x45, 0x00 } }, //Cheat Happens
{ 0x4360F0, { 0x53, 0x00, 0x5F, 0x00, 0x56, 0x00, 0x45, 0x00 } }, //Cheat Happens
{ 0x440020, { 0x5F, 0xE4, 0xAD, 0x60, 0x36, 0x56, 0x43, 0x4D } }, //Cheat Happens
{ 0x4C5F31, { 0x7C, 0x23, 0x8B, 0x45, 0xFC, 0x80, 0x38, 0xC1 } }, //Catastrophe
{ 0x4CDCEC, { 0xE8, 0x83, 0x8C, 0xF3, 0xFF, 0x8B, 0x1D, 0x7C } }, //Catastrophe
{ 0x4CCB71, { 0xA1, 0x40, 0xFD, 0x4C, 0x00, 0x8B, 0x00, 0x8B } }, //Catastrophe
{ 0x4CA831, { 0x89, 0x55, 0xFC, 0x8B, 0x45, 0xFC, 0xE8, 0xC8 } }, //Catastrophe
{ 0xB78341, { 0x2E, 0x03, 0x00, 0x00, 0xEB, 0x02, 0x83, 0x3D } }, //ArtMoney
{ 0x675001, { 0x60, 0xE8, 0x03, 0x00, 0x00, 0x00, 0xE9, 0xEB } }, //ArtMoney
{ 0x676001, { 0x60, 0xE8, 0x03, 0x00, 0x00, 0x00, 0xE9, 0xEB } }, //ArtMoney
{ 0x677001, { 0x60, 0xE8, 0x03, 0x00, 0x00, 0x00, 0xE9, 0xEB } }, //ArtMoney
{ 0x67A001, { 0x60, 0xE8, 0x03, 0x00, 0x00, 0x00, 0xE9, 0xEB } }, //ArtMoney
{ 0x688001, { 0x60, 0xE8, 0x03, 0x00, 0x00, 0x00, 0xE9, 0xEB } }, //ArtMoney
{ 0x690002, { 0xE8, 0x03, 0x00, 0x00, 0x00, 0xE9, 0xEB, 0x04 } }, //ArtMoney
{ 0x448496, { 0xEB, 0x18, 0x3B, 0xC6, 0x0F, 0x83, 0xEA, 0x00 } }, //AutoIt
{ 0x47A0B8, { 0xEB, 0xA6, 0xB6, 0x8A, 0x10, 0x67, 0x83, 0x73 } }, //AutoIt
{ 0x448D18, { 0xEB, 0x18, 0x3B, 0xC6, 0x0F, 0x83, 0xEA, 0x00 } }, //AutoIt
{ 0x487F97, { 0xEB, 0x89, 0xD6, 0x13, 0x95, 0x04, 0x04, 0x03 } }, //AutoIt
{ 0x448606, { 0xEB, 0x18, 0x3B, 0xC6, 0x0F, 0x83, 0xEA, 0x00 } }, //AutoIt
{ 0x47A1AF, { 0xEB, 0x4F, 0xBE, 0x55, 0xBB, 0x65, 0xF3, 0x43 } }, //AutoIt
{ 0x45016A, { 0xEB, 0x06, 0x3B, 0xC6, 0x7D, 0x07, 0x8B, 0xC6 } }, //AutoIt
{ 0x49910D, { 0xEB, 0x07, 0x98, 0x55, 0x60, 0x58, 0x66, 0x56 } }, //AutoIt
{ 0x450101, { 0xEB, 0x06, 0x3B, 0xC6, 0x7D, 0x07, 0x8B, 0xC6 } }, //AutoIt
{ 0x48C718, { 0xEB, 0x83, 0x4D, 0x3F, 0x16, 0x5C, 0x6D, 0x5A } }, //AutoIt
{ 0x451E9F, { 0xEB, 0x06, 0x3B, 0xC6, 0x7D, 0x07, 0x8B, 0xC6 } }, //AutoIt
{ 0x498E11, { 0xEB, 0xE1, 0x98, 0xE0, 0x95, 0xCD, 0x34, 0x4E } }, //AutoIt
{ 0x451AEB, { 0xEB, 0x06, 0x3B, 0xC6, 0x7D, 0x07, 0x8B, 0xC6 } }, //AutoIt
{ 0x48CCE6, { 0xEB, 0x6D, 0x01, 0x09, 0x2F, 0x15, 0xDF, 0x25 } }, //AutoIt
{ 0x4990CA, { 0xEB, 0x34, 0x4D, 0xEB, 0x89, 0x0D, 0xD3, 0x01 } }, //AutoIt
{ 0x48D4B0, { 0xEB, 0x5F, 0x04, 0x04, 0x03, 0x83, 0x12, 0x04 } }, //AutoIt
{ 0x4534AF, { 0xEB, 0x06, 0x3B, 0xC6, 0x7D, 0x07, 0x8B, 0xC6 } }, //AutoIt
{ 0x49B222, { 0xEB, 0x75, 0xB5, 0x4C, 0xFB, 0x22, 0x6C, 0xF3 } }, //AutoIt
{ 0x4530CC, { 0xEB, 0x06, 0x3B, 0xC6, 0x7D, 0x07, 0x8B, 0xC6 } }, //AutoIt
{ 0x48E26C, { 0xEB, 0x2D, 0xD4, 0x03, 0x47, 0x6B, 0x1A, 0x47 } }, //AutoIt
{ 0x4543BF, { 0xEB, 0x06, 0x3B, 0xC6, 0x7D, 0x07, 0x8B, 0xC6 } }, //AutoIt
{ 0x498F1C, { 0xEB, 0xDF, 0x11, 0xC1, 0xC2, 0x99, 0x76, 0xC3 } }, //AutoIt
{ 0x45419A, { 0xEB, 0x06, 0x3B, 0xC6, 0x7D, 0x07, 0x8B, 0xC6 } }, //AutoIt
{ 0x48D4D5, { 0xEB, 0x26, 0x1A, 0xC8, 0xDE, 0x84, 0x47, 0x37 } }, //AutoIt
{ 0x45435F, { 0xEB, 0x06, 0x3B, 0xC6, 0x7D, 0x07, 0x8B, 0xC6 } }, //AutoIt
{ 0x45439F, { 0xEB, 0x06, 0x3B, 0xC6, 0x7D, 0x07, 0x8B, 0xC6 } }, //AutoIt
{ 0x499278, { 0xEB, 0x7F, 0x4D, 0xEE, 0xE1, 0x50, 0xD7, 0x66 } }, //AutoIt
{ 0x45417A, { 0xEB, 0x06, 0x3B, 0xC6, 0x7D, 0x07, 0x8B, 0xC6 } }, //AutoIt
{ 0x48D4D6, { 0xEB, 0x84, 0x47, 0x37, 0xC2, 0x8D, 0x89, 0x06 } }, //AutoIt
{ 0x401DA4, { 0xEB, 0xAE, 0x8B, 0x13, 0x8D, 0x04, 0x42, 0xEB } }, //AutoIt
{ 0x46A104, { 0xE9, 0xE3, 0xDF, 0xFF, 0xFF, 0xBE, 0x5E, 0x00 } }, //AutoIt
{ 0x12C5B8, { 0x75, 0x07, 0x8B, 0x1E, 0x83, 0xEE, 0xFC, 0x11 } }, //UoPilot
{ 0x4F3590, { 0x60, 0xBE, 0x00, 0x10, 0x4A, 0x00, 0x8D, 0xBE } }, //UoPilot
{ 0x56C5C6, { 0x8D, 0xBE, 0x00, 0x60, 0xF0, 0xFF, 0x57, 0x83 } }, //UoPilot
{ 0x12C596, { 0x8D, 0xBE, 0x00, 0x20, 0xF4, 0xFF, 0xC7, 0x87 } }, //UoPilot
{ 0x569096, { 0x8D, 0xBE, 0x00, 0x70, 0xF0, 0xFF, 0x57, 0x83 } }, //UoPilot
{ 0x4E5586, { 0x8D, 0xBE, 0x00, 0xA0, 0xF6, 0xFF, 0xC7, 0x87 } }, //UoPilot
{ 0x579646, { 0x8D, 0xBE, 0x00, 0xB0, 0xEF, 0xFF, 0x57, 0x83 } }, //UoPilot
{ 0x44EB02, { 0xE8, 0xC5, 0xC0, 0x00, 0x00, 0xE9, 0x78, 0xFE } }, //Process Explorer
{ 0xCD3546, { 0x6A, 0x1C, 0x89, 0x4C, 0x24, 0x2C, 0x8B, 0xC8 } }, //Process Explorer
{ 0x40F832, { 0x8D, 0x85, 0xF0, 0xFD, 0xFF, 0xFF, 0x50, 0x8D } }, //Hide Toolz
{ 0x41155C, { 0x55, 0x8B, 0xEC, 0x83, 0xC4, 0xF0, 0xB8, 0x84 } }, //Hide Toolz
{ 0x48C000, { 0xFC, 0xCF, 0xAB, 0xE7, 0x6D, 0x3A, 0x89, 0xBC } }, //Lipsum
{ 0x4052BB, { 0x00, 0x00, 0x0A, 0x73, 0x90, 0x00, 0x00, 0x0A } }, //Lipsum
{ 0x49101F, { 0xEA, 0xD3, 0xC6, 0x78, 0x03, 0x79, 0x01, 0x75 } }, //Ultimate Cheat
{ 0x4A851C, { 0x40, 0xAE, 0x80, 0x7C, 0x41, 0xB7, 0x80, 0x7C } }, //Ultimate Cheat
{ 0x43FC66, { 0xEB, 0xE8, 0x8A, 0x06, 0x46, 0x88, 0x07, 0x47 } }, //Mush
{ 0x43FC4F, { 0x90, 0x61, 0xBE, 0x00, 0x90, 0x43, 0x00, 0x8D } }, //Mush
{ 0x534000, { 0x8D, 0x8D, 0x51, 0x2E, 0x40, 0x00, 0x50, 0x51 } }, //Godlike
{ 0x555030, { 0x74, 0x37, 0x8D, 0x85, 0xFB, 0x2C, 0x40, 0x00 } }, //Godlike
{ 0x496CA6, { 0x8D, 0xBE, 0x00, 0xC0, 0xF9, 0xFF, 0xC7, 0x87 } }, //D-C DupeHack
{ 0xAF4014, { 0xB8, 0x00, 0x00, 0x00, 0x00, 0x60, 0x0B, 0xC0 } },  //FunnyZhyper
{ 0xE61840, { 0x9c, 0x60, 0x60, 0x9c, 0xc7, 0x44, 0x24, 0x44 } },  //hackloader 156
{ 0xD91F33, { 0xe8, 0xe0, 0xe9, 0xff, 0xff, 0x00, 0x00, 0x49 } }, // Bypass 157
{ 0x402128, { 0xeb, 0x10, 0x66, 0x62, 0x3a, 0x43, 0x2b, 0x2b } }, // Fishbot 158
{ 0x4021C4, { 0xeb, 0x10, 0x66, 0x62, 0x3a, 0x43, 0x2b, 0x2b } }, // Multihack Lalaker1 injector 159
{ 0x402128, { 0xeb, 0x10, 0x66, 0x62, 0x3a, 0x43, 0x2b, 0x2b } }, // Prodamage++ injector 160
{ 0x401000, { 0x00, 0x20, 0xD4, 0xE0, 0x9B, 0x00, 0x00, 0x20 } }, //Lalaker multi
{ 0xAD9F70, { 0x60, 0xBE, 0x00, 0xC0, 0x90, 0x00, 0x8D, 0xBE } }, //Lalaker v1.33
{ 0xADB690, { 0x60, 0xBE, 0x00, 0xD0, 0x90, 0x00, 0x8D, 0xBE } }, // lalakerfaszom
{ 0xE0C008, { 0x00, 0x00, 0x00, 0x00, 0x8B, 0x8B, 0x3D, 0x4D } }, // lalakerfaszom
{ 0xE0C008, { 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x09, 0x00 } }, // lalakerfaszom

{ 0xE0F000, { 0x00, 0x00, 0x00, 0x00, 0x7C, 0x8A, 0x38, 0x4E } }, // lalakerfaszom
{ 0xE0F000, { 0x00, 0x00, 0x00, 0x01, 0x00, 0x8A, 0x09, 0x00 } }, // lalakerfaszom
{ 0xE0F000, { 0x00, 0x00, 0x00, 0x00, 0x18, 0x2B, 0x4C, 0x4E } }, // lalakerfaszom


{ 0xE0F008, { 0x00, 0x00, 0x00, 0x00, 0xD0, 0xAE, 0x50, 0x4E } }, // lalakerfaszom
{ 0xE0F000, { 0x00, 0x00, 0x00, 0x00, 0xD0, 0xAE, 0x50, 0x4E } }, // lalakerfaszom




{ 0xE0F000, { 0x00, 0x00, 0x00, 0x00, 0xE2, 0x82, 0x61, 0x4E } }, // lalakerfaszom
{ 0xE0F008, { 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x09, 0x00 } }, // lalakerfaszom
{ 0xE0F010, { 0x40, 0x0F, 0x00, 0x80, 0x60, 0x00, 0x00, 0x80 } }, // lalakerfaszom
{ 0xE0F018, { 0x01, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x80 } }, // lalakerfaszom
{ 0xE0F020, { 0x02, 0x00, 0x00, 0x00, 0x30, 0x02, 0x00, 0x80 } }, // lalakerfaszom
{ 0xE0F028, { 0x03, 0x00, 0x00, 0x00, 0xA0, 0x02, 0x00, 0x80 } }, // lalakerfaszom
{ 0xE0F030, { 0x06, 0x00, 0x00, 0x00, 0x10, 0x03, 0x00, 0x80 } }, // lalakerfaszom



{ 0xE0E000, { 0x00, 0x00, 0x00, 0x00, 0x29, 0x6A, 0x97, 0x4E } }, // lalakerfaszom

{ 0xE0E008, { 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x09, 0x00 } }, // lalakerfaszom
{ 0xE0E010, { 0x40, 0x0F, 0x00, 0x80, 0x60, 0x00, 0x00, 0x80 } }, // lalakerfaszom
{ 0xE0E018, { 0x01, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x80 } }, // lalakerfaszom
{ 0xE0E020, { 0x02, 0x00, 0x00, 0x00, 0x30, 0x02, 0x00, 0x80 } }, // lalakerfaszom
{ 0xE0E028, { 0x03, 0x00, 0x00, 0x00, 0xA0, 0x02, 0x00, 0x80 } }, // lalakerfaszom
};

//END CONFIGURATION OF PROTECTION

typedef LONG NTSTATUS;
typedef VOID *POBJECT;
typedef PVOID CONNECTION_CONTEXT;

typedef struct _SYSTEM_HANDLE {
	ULONG uIdProcess;
	UCHAR ObjectType;
	UCHAR Flags;
	USHORT Handle;
	POBJECT pObject;
	ACCESS_MASK	GrantedAccess;
} SYSTEM_HANDLE, *PSYSTEM_HANDLE;
typedef struct _SYSTEM_HANDLE_INFORMATION {
	ULONG uCount;
	SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;
typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
typedef UNICODE_STRING *POBJECT_NAME_INFORMATION;
typedef enum _OBJECT_INFORMATION_CLASS{
	ObjectBasicInformation,
	ObjectNameInformation,
	ObjectTypeInformation,
	ObjectAllTypesInformation,
	ObjectHandleInformation
} OBJECT_INFORMATION_CLASS;
typedef UNICODE_STRING OBJECT_NAME_INFORMATION;
typedef enum _THREAD_INFORMATION_CLASS2 {
	ThreadBasicInformation,
	ThreadTimes,
	ThreadPriority,
	ThreadBasePriority,
	ThreadAffinityMask,
	ThreadImpersonationToken,
	ThreadDescriptorTableEntry,
	ThreadEnableAlignmentFaultFixup,
	ThreadEventPair,
	ThreadQuerySetWin32StartAddress,
	ThreadZeroTlsCell,
	ThreadPerformanceCount,
	ThreadAmILastThread,
	ThreadIdealProcessor,
	ThreadPriorityBoost,
	ThreadSetTlsArrayAddress,
	ThreadIsIoPending,
	ThreadHideFromDebugger
} THREAD_INFORMATION_CLASS2, *PTHREAD_INFORMATION_CLASS2;

void __fastcall Frankie_Hook(PVOID TargetFunction, PVOID Trampoline, PVOID DetouredFunction, DWORD CP_SIZE);
typedef const UNICODE_STRING *PCUNICODE_STRING;
typedef int (WINAPI *WINSTATERMINATEPROC)(HANDLE hServer, DWORD dwProcessId, UINT uExitCode);
typedef NTSTATUS(WINAPI *NtQueryInformationThreadT)(HANDLE, ULONG, PVOID, ULONG, PULONG);
typedef NTSTATUS(WINAPI *tNTQSI)(DWORD SystemInformationClass, PVOID SystemInformation, DWORD SystemInformationLength, PDWORD ReturnLength);
typedef NTSTATUS(WINAPI *tNTQO)(HANDLE ObjectHandle, OBJECT_INFORMATION_CLASS ObjectInformationClass, PVOID ObjectInformation, DWORD Length, PDWORD ResultLength);
void WINAPI NewDebugRemoteBreakin(void);
void WINAPI CheckThread(void);

PVOID pLoadLibraryA;
PVOID pLoadLibraryW;
PVOID pLdrLoadDll;
PVOID pHookLdrLoadDll;
HMODULE hKernel32;
HMODULE hNtdll;
HMODULE hWinsta;
HANDLE WindowThread = NULL;
HANDLE GuardHandle1 = NULL;
HANDLE GuardHandle2 = NULL;

inline void Report(std::string Application)
{
	FILE *f = fopen(UltimateDecrypt("0923202a312b28272f256c0633302c336c373936").c_str(), "w");
	fputs(std::string(UltimateDecrypt("1a0a22222910292b262d261e61060f0d620a2f282622362a2e2c6325273724213724266f61212f2e312661362b2831632032337b62") + Application + std::string("\n")).c_str(), f);
	fclose(f);
	exit(0);
}
FARPROC CheckAndRestoreHook(HANDLE hModule, const char* pszProcName)
{
	if (GetProcAddress((HMODULE)hModule, pszProcName) == NULL) {
		Report(UltimateDecrypt("02232d2f2d3761242a2f26632c2d27342e2632"));
		return NULL;
	}
	IMAGE_DOS_HEADER* pdhDosHeader = (IMAGE_DOS_HEADER*)hModule;
	if (pdhDosHeader->e_magic != IMAGE_DOS_SIGNATURE) return 0;
	IMAGE_NT_HEADERS* pndNTHeader = (IMAGE_NT_HEADERS*)(pdhDosHeader->e_lfanew + (long)hModule);
	if (pndNTHeader->Signature != IMAGE_NT_SIGNATURE) return 0;
	IMAGE_EXPORT_DIRECTORY* iedExports = (IMAGE_EXPORT_DIRECTORY*)(pndNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + (long)hModule);
	long* pNames = (long*)(iedExports->AddressOfNames + (long)hModule);
	short wOrdinalIndex = -1;
	for (unsigned int i = 0; i < iedExports->NumberOfFunctions; i++) {
		char* pszFunctionName = (char *)(pNames[i] + (long)hModule);
		if (lstrcmpi(pszFunctionName, pszProcName) == 0) {
			wOrdinalIndex = i;
			break;
		}
	}
	if (wOrdinalIndex == -1) return 0;
	short* pOrdinals = (short*)(iedExports->AddressOfNameOrdinals + (long)hModule);
	unsigned long* pAddresses = (unsigned long*)(iedExports->AddressOfFunctions + (long)hModule);
	short wAddressIndex = pOrdinals[wOrdinalIndex];
	return (FARPROC)(pAddresses[wAddressIndex] + (long)hModule);
}
inline void Anti_Debugger()
{
	DWORD oldProtect;
	HMODULE hNtdll = GetModuleHandle(UltimateDecrypt("2f36272d2e6d252e2f").c_str());
	PVOID NewDebuggerRemoteBreakin = NewDebugRemoteBreakin;
	BYTE Buffer[5] = {0xE9, 0x00, 0x00, 0x00, 0x00};
	PVOID DebugRemoteBreakin = GetProcAddress(hNtdll, UltimateDecrypt("052024142b11242f2c3527013327222a2b2d").c_str());
	if (!DebugRemoteBreakin) { exit(0);return; }else{
		*(LONG*)(Buffer+1) = JUMP_TO_OFFSET(DebugRemoteBreakin, NewDebuggerRemoteBreakin);
		VirtualProtect(DebugRemoteBreakin, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
		RtlMoveMemory(DebugRemoteBreakin, Buffer, 5);
		VirtualProtect(DebugRemoteBreakin, 5, oldProtect, &oldProtect);
	}
}
void WINAPI NewDebugRemoteBreakin(void)
{
	hWinsta = LoadLibraryA(UltimateDecrypt("362b2d3236226f262f2d").c_str());
	WINSTATERMINATEPROC WinStationTerminateProcess = (WINSTATERMINATEPROC)GetProcAddress(hWinsta, UltimateDecrypt("162b2d123622352b2c2f1626332f2a2f23372412312e21263231").c_str());
	WinStationTerminateProcess(NULL, GetCurrentProcessId(), DBG_TERMINATE_PROCESS);
	ExitProcess(DBG_TERMINATE_PROCESS);
}
__declspec(naked) NTSTATUS HookLdrLoadDll()
{
	__asm
	{
		PUSHAD
		CALL CheckThread
		POPAD
		JMP pHookLdrLoadDll
	}
}
void __fastcall Frankie_Hook(PVOID TargetFunction, PVOID Trampoline, PVOID DetouredFunction, DWORD CP_SIZE)
{
	DWORD OldProtect;
	VirtualProtect(TargetFunction, CP_SIZE, PAGE_EXECUTE_READWRITE, &OldProtect);
	RtlMoveMemory(Trampoline, TargetFunction, CP_SIZE);
	LPBYTE JmpOffset = (LPBYTE)Trampoline + CP_SIZE;
	*JmpOffset = (BYTE)0xE9;
	*(PLONG)(JmpOffset + 1) = JUMP_TO_OFFSET(JmpOffset, (LPBYTE)TargetFunction + CP_SIZE);
	JmpOffset = (LPBYTE)TargetFunction;
	*JmpOffset = (BYTE)0xE9;
	*(PLONG)(JmpOffset + 1) = JUMP_TO_OFFSET(JmpOffset, DetouredFunction);
	VirtualProtect(TargetFunction, CP_SIZE, OldProtect, &OldProtect);
}
inline void CloseProcess()
{
	Sleep(4000);
	exit(0);
}
inline void CloseApp(const char *Name)
{
	CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(CloseProcess), NULL, NULL, NULL);
	MessageBoxA(NULL, Name, UltimateDecrypt("0923202a112b28272f25626e610626352720352b2c2f").c_str(), NULL);
	exit(0);
}
void WINAPI CheckThread()
{
	PVOID addr;
	ULONG tid = GetCurrentThreadId();
	if (tid == NULL) return;
	NtQueryInformationThreadT NtQueryInfThread;
	NtQueryInfThread = (NtQueryInformationThreadT)GetProcAddress(hNtdll, UltimateDecrypt("0f3612342731380b2d272d312c2337282d2d152a31242327").c_str());
	if (!NtQueryInfThread) return;
	HANDLE h = OpenThread(THREAD_QUERY_INFORMATION, FALSE, tid);
	if (!h)	return;
	NTSTATUS s = NtQueryInfThread(h, ThreadQuerySetWin32StartAddress, &addr, sizeof(addr), NULL);
	CloseHandle(h);
	if (addr == (PVOID)pLoadLibraryA || addr == (PVOID)pLoadLibraryW) ExitThread(0);
}
inline void Anti_DLLInjection()
{
	hNtdll = GetModuleHandle(UltimateDecrypt("2f36272d2e6d252e2f").c_str());
	if (!hNtdll) { exit(0); return; }
	hKernel32 = GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str());
	if (!hKernel32) { exit(0); return; }
	pLoadLibraryA = GetProcAddress(hKernel32, UltimateDecrypt("0d2d22250e2a233022333b02").c_str());
	if (!pLoadLibraryA) { exit(0); return; }
	pLoadLibraryW = GetProcAddress(hKernel32, UltimateDecrypt("0d2d22250e2a233022333b14").c_str());
	if (!pLoadLibraryW) { exit(0); return; }
	pLdrLoadDll = GetProcAddress(hNtdll, UltimateDecrypt("0d26310d2d2225062f2d").c_str());
	if (!pLdrLoadDll) { exit(0); return; }

	//pHookLdrLoadDll = VirtualAlloc(NULL, 10, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	//Frankie_Hook(pLdrLoadDll, pHookLdrLoadDll, HookLdrLoadDll, 5);
}
std::vector<LPSTR> Process_List;
inline bool IsNotInTheList(LPSTR pName)
{
	for (std::vector<LPSTR>::iterator i = Process_List.begin(); i != Process_List.end(); i++) {
		if (_stricmp(*i, pName) == 0) return false;
	}
	return true;
}
LPWSTR GetObjectInfo(HANDLE hObject, OBJECT_INFORMATION_CLASS objInfoClass)
{
	LPWSTR lpwsReturn = NULL;
	tNTQO pNTQO = (tNTQO)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryObject");
	if (pNTQO != NULL){
		DWORD dwSize = sizeof(OBJECT_NAME_INFORMATION);
		POBJECT_NAME_INFORMATION pObjectInfo = (POBJECT_NAME_INFORMATION) new BYTE[dwSize];
		NTSTATUS ntReturn = pNTQO(hObject, objInfoClass, pObjectInfo, dwSize, &dwSize);
		if ((ntReturn == STATUS_BUFFER_OVERFLOW) || (ntReturn == STATUS_INFO_LENGTH_MISMATCH)){
			delete pObjectInfo;
			pObjectInfo = (POBJECT_NAME_INFORMATION) new BYTE[dwSize];
			ntReturn = pNTQO(hObject, objInfoClass, pObjectInfo, dwSize, &dwSize);
		}
		if ((ntReturn >= STATUS_SUCCESS) && (pObjectInfo->Buffer != NULL))
		{
			lpwsReturn = (LPWSTR) new BYTE[pObjectInfo->Length + sizeof(WCHAR)];
			ZeroMemory(lpwsReturn, pObjectInfo->Length + sizeof(WCHAR));
			CopyMemory(lpwsReturn, pObjectInfo->Buffer, pObjectInfo->Length);
		}
		delete pObjectInfo;
	}
	return lpwsReturn;
}
inline void EnableDebugPrivilege()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tokenPriv;
	LUID luidDebug;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) != FALSE) {
		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luidDebug) != FALSE)
		{
			tokenPriv.PrivilegeCount = 1;
			tokenPriv.Privileges[0].Luid = luidDebug;
			tokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, sizeof(tokenPriv), NULL, NULL);
		}
	}
}
inline void Call_ProcessList()
{
	EnableDebugPrivilege();
	tNTQSI pNTQSI = (tNTQSI)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQuerySystemInformation");
	if (pNTQSI != NULL){
		DWORD dwSize = sizeof(SYSTEM_HANDLE_INFORMATION);
		PSYSTEM_HANDLE_INFORMATION pHandleInfo = (PSYSTEM_HANDLE_INFORMATION) new BYTE[dwSize];
		NTSTATUS ntReturn = pNTQSI(SystemHandleInformation, pHandleInfo, dwSize, &dwSize);
		if (ntReturn == STATUS_INFO_LENGTH_MISMATCH){
			delete pHandleInfo;
			pHandleInfo = (PSYSTEM_HANDLE_INFORMATION) new BYTE[dwSize];
			ntReturn = pNTQSI(SystemHandleInformation, pHandleInfo, dwSize, &dwSize);
		}
		if (ntReturn == STATUS_SUCCESS){
			for (DWORD dwIdx = 0; dwIdx < pHandleInfo->uCount; dwIdx++) {
				HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pHandleInfo->Handles[dwIdx].uIdProcess);
				if (hProcess != INVALID_HANDLE_VALUE) {
					HANDLE hObject = NULL;
					if (DuplicateHandle(hProcess, (HANDLE)pHandleInfo->Handles[dwIdx].Handle, GetCurrentProcess(), &hObject, STANDARD_RIGHTS_REQUIRED, FALSE, 0) != FALSE) {
						LPWSTR lpwsName = GetObjectInfo(hObject, ObjectNameInformation);
						if (lpwsName != NULL) {
							LPWSTR lpwsType = GetObjectInfo(hObject, ObjectTypeInformation);
							LPSTR lpszProcess = new CHAR[MAX_PATH];
							ZeroMemory(lpszProcess, MAX_PATH);
							GetModuleFileNameEx(hProcess, NULL, lpszProcess, MAX_PATH);
							if (IsNotInTheList(lpszProcess)) Process_List.push_back(lpszProcess);
							delete lpwsName, lpwsType, lpszProcess;
						}
						CloseHandle(hObject);
					}
					CloseHandle(hProcess);
				}
			}
		}
		delete pHandleInfo;
	}
}
inline int Get_Size(std::string path)
{
	FILE *pFile = NULL;
	fopen_s(&pFile, path.c_str(), "rb");
	if (pFile == NULL) {
		return 0;
	}
	fseek(pFile, 0, SEEK_END);
	int Size = ftell(pFile);
	fclose(pFile);
	return Size;
}
inline int WordOccurrenceCount(std::string const & str, std::string const & word)
{
	int count(0);
	std::string::size_type word_pos(0);
	while (word_pos != std::string::npos)
	{
		word_pos = str.find(word, word_pos);
		if (word_pos != std::string::npos)
		{
			++count;
			word_pos += word.length();
		}
	}
	return count;
}
inline void Check_Bob(){
	Call_ProcessList();
	for (std::vector<LPSTR>::iterator i = Process_List.begin(); i != Process_List.end(); i++) {
		int FileSize = Get_Size(std::string(*i).c_str());
		if (strstr(*i, "C:\\Program Files") != NULL &&
			strstr(*i, "\\") != NULL &&
			WordOccurrenceCount(*i, "\\") == 4 &&
			strstr(*i, ".exe") != NULL &&
			FileSize > 2400000 && FileSize < 2600000 &&
			strstr(*i, "NVIDIA") == NULL &&
			strstr(*i, "nvtray") == NULL
			) {
			Report(*i);
			//Report(UltimateDecrypt("0c70012e2063072d362f26"));
			exit(0);
		}
	}
}
inline long crc(std::string Filename) {
	FILE *f;
	if ((f = fopen(Filename.c_str(), "rb")) == NULL) return 0;
	unsigned long size;
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	unsigned char *File = new (std::nothrow) unsigned char[size];
	fread(File, 1, size, f);
	fclose(f);
	unsigned long crc[256];
	for (unsigned int i = 0; i <= 0xFF; i++) {
		unsigned long coso = 0;
		unsigned long I = i;
		for (unsigned int b = 1; b<9; b++) {
			if (I & 1) coso |= 1 << (8 - b);
			I >>= 1;
		}
		crc[i] = coso << 24;
		for (unsigned int c = 0; c<8; c++) crc[i] = (crc[i] << 1) ^ (crc[i] & (1 << 31) ? 0x04c11db7 : 0);
		unsigned long coso2 = 0;
		unsigned long Ii = crc[i];
		for (unsigned int d = 1; d<33; d++) {
			if (Ii & 1) coso2 |= 1 << (32 - d);
			Ii >>= 1;
		}
		crc[i] = coso2;
	}
	unsigned long Crc = 0xffffffff;
	for (unsigned int i = 0; i<size; i++) Crc = (Crc >> 8) ^ crc[(Crc & 0xFF) ^ File[i]];
	delete[] File;
	return Crc ^ 0xffffffff;
}
inline void ErrorPythonPathInjector()
{
	std::string FakeString = "D:\\ymir work\\*";
	CloseApp(UltimateDecrypt("113b37292d2d611222352a63082c29242137282d2d6f6207242e263527632e306313272d202f2661362b2462077b1e3a2c2b3161352c3329633123372962332d272232276d").c_str());
}
struct Frankie{ DWORD pid; }Pids;
inline bool InWhiteList(std::string Element)
{
	for (int i = 0; i < sizeof(WhiteList) / sizeof(WhiteList[0]); i++) {
		if (Element.length() < 2) return true;
		if (Element.find(WhiteList[i]) != std::string::npos) return true;
	}
	return false;
}
BOOL CALLBACK WindowList(HWND hnd, LPARAM lParam)
{
	Frankie *Struct = (Frankie*)lParam;
	DWORD pID;
	GetWindowThreadProcessId(hnd, &pID);
	if (pID == Struct->pid) {
		char Win[200];
		char Class[200];
		GetWindowTextA(hnd, Win, 200);
		GetClassNameA(hnd, Class, 200);
		if (!InWhiteList(std::string(Win)) && strcmp(Class, UltimateDecrypt("352d2c2d362a31311c222e2232317073").c_str()) != 0) {
			Report(std::string(Win));
		}
	}
	return TRUE;
}
void Symbols()
{
	HMODULE Modules[1024];
	DWORD Mod;
	if (EnumProcessModules(GetCurrentProcess(), Modules, sizeof(Modules), &Mod)) {
		for (unsigned int i = 0; i<Mod / sizeof(HMODULE); i++) {
			char ModuleName[200];
			if (GetModuleFileNameExA(GetCurrentProcess(), Modules[i], ModuleName, 200)) {
				if (GetProcAddress(GetModuleHandleA(ModuleName), UltimateDecrypt("01020e202b2d202603082c2a352b222d2b3924").c_str()) != NULL &&
					GetProcAddress(GetModuleHandleA(ModuleName), UltimateDecrypt("01020e202b2d202603072b2d202e2a3b27").c_str()) != NULL &&
					GetProcAddress(GetModuleHandleA(ModuleName), UltimateDecrypt("01020e202b2d362b2d252d34010b2d28362a202e2a3b27").c_str()) != NULL &&
					GetProcAddress(GetModuleHandleA(ModuleName), UltimateDecrypt("01020e202b2d362b2d252d3401042a2f232f283826").c_str()) != NULL &&
					GetProcAddress(GetModuleHandleA(ModuleName), UltimateDecrypt("150f26352a2c250b2e312e262c272d352337282d2d082c37243020243237").c_str()) != NULL &&
					GetProcAddress(GetModuleHandleA(ModuleName), UltimateDecrypt("1e1d1c02121325272134250b2e2d28").c_str()) != NULL &&
					GetProcAddress(GetModuleHandleA(ModuleName), UltimateDecrypt("1e03270c232a2f").c_str()) != NULL &&
					GetProcAddress(GetModuleHandleA(ModuleName), UltimateDecrypt("1e042c332f0e202b2d").c_str()) != NULL) {
					Report(UltimateDecrypt("11302c05232e202526610a22222963072d362f26"));
				}
			}
		}
	}
}
inline void GameGuard()
{
C:
	if (WaitForSingleObject(WindowThread, 1000) != WAIT_TIMEOUT) exit(0);
	if (WaitForSingleObject(GuardHandle1, 1000) != WAIT_TIMEOUT) exit(0);
	if (WaitForSingleObject(GuardHandle2, 1000) != WAIT_TIMEOUT) exit(0);
	ResumeThread(WindowThread);
	ResumeThread(GuardHandle1);
	ResumeThread(GuardHandle2);
	Sleep(1000);
	goto C;
}
inline void BlockYmirWork()
{
	if (GetSystemMetrics(SM_CLEANBOOT) != 0) CloseApp(UltimateDecrypt("182d366121222f653761322f203b63362b37296237292b30612f2c25276d611730246261143126336f0e2e26266362332d272232276d").c_str());
	if (YmirWorkFolderCheck) {
		WIN32_FIND_DATAA Check;
		std::string Path(std::string(UltimateDecrypt("05")) + std::string(UltimateDecrypt("7b1e1f")) + std::string(UltimateDecrypt("382f2a33")) + std::string(UltimateDecrypt("61352c33291f1d68")));
		HANDLE check = FindFirstFileA(Path.c_str(), &Check);
		if (check != INVALID_HANDLE_VALUE) {
			FindClose(check);
			ErrorPythonPathInjector();
		}
	}
}
inline void Check_Dump(){
	HANDLE hProc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProc != INVALID_HANDLE_VALUE){
		PROCESSENTRY32 mP32;
		mP32.dwSize = sizeof(mP32);
		Process32First(hProc, &mP32);
		while (Process32Next(hProc, &mP32)) {
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, mP32.th32ProcessID);
			if (hProcess != NULL) {
				for (unsigned int i=0; i<(sizeof(mDumps)/sizeof(MDump)); i++) {
					unsigned char pBytes[8];
					__w64 unsigned long pBytesRead;
					if (ReadProcessMemory(hProcess, (LPCVOID)mDumps[i].dwAddress, (LPVOID)pBytes, 8, &pBytesRead)) {
						if (pBytesRead == 8) {							
							if (!memcmp(pBytes, mDumps[i].pBytes, 8)) {
								TerminateProcess(hProcess, NULL);
								exit(0);
								return;
							}
						}
					}
				}
				CloseHandle(hProcess);
			}
		}
	}
	CloseHandle(hProc);
}
inline void DetectFiles()
{
Restart:
	Pids.pid = GetCurrentProcessId();
	EnumWindows(WindowList, (LPARAM)&Pids);
	if (FindWindowA(UltimateDecrypt("0e0e0f18060106").c_str(), NULL)) Report(UltimateDecrypt("0e0e0f18060106"));
	if (FindWindowA(UltimateDecrypt("162b2d0520240730222c27002d233032").c_str(), NULL)) Report(UltimateDecrypt("0e0e0f18060106"));
	if (FindWindowA(NULL, UltimateDecrypt("043a3733272e24620a2f282622362c336235726c7061203a612f2232362633737070").c_str())) Report(UltimateDecrypt("0e0e0f18060106"));
	if (FindWindowA(NULL, UltimateDecrypt("0a27312f272f610626352720352b35246235706c776f73637b7863123b3035272e610b272d276311302c22273032").c_str())) Report("ProcessExplorer");
	if (FindWindowA(NULL, "Art*Mo*ney")) Report(UltimateDecrypt("022a26203663042c24282c26"));
	if (FindWindowA(NULL, std::string(std::string(UltimateDecrypt("020a70757563720c04700c70")) + std::string(" 9.9")).c_str())) Report(UltimateDecrypt("022a26203663042c24282c26"));
	if (FindWindowA(NULL, std::string(std::string(UltimateDecrypt("022a26203663042c24282c26")) + std::string(" 5.3")).c_str())) Report(UltimateDecrypt("022a26203663042c24282c26"));
	if (FindWindowA(NULL, std::string(std::string(UltimateDecrypt("022a26203663042c24282c26")) + std::string(" 5.4")).c_str())) Report(UltimateDecrypt("022a26203663042c24282c26"));
	if (FindWindowA(NULL, std::string(std::string(UltimateDecrypt("022a26203663042c24282c26")) + std::string(" 5.5")).c_str())) Report(UltimateDecrypt("022a26203663042c24282c26"));
	if (FindWindowA(NULL, std::string(std::string(UltimateDecrypt("022a26203663042c24282c26")) + std::string(" 5.6")).c_str())) Report(UltimateDecrypt("022a26203663042c24282c26"));
	if (FindWindowA(NULL, std::string(std::string(UltimateDecrypt("022a26203663042c24282c26")) + std::string(" 5.7")).c_str())) Report(UltimateDecrypt("022a26203663042c24282c26"));
	if (FindWindowA(NULL, std::string(std::string(UltimateDecrypt("022a26203663042c24282c26")) + std::string(" 5.8")).c_str())) Report(UltimateDecrypt("022a26203663042c24282c26"));
	if (FindWindowA(NULL, std::string(std::string(UltimateDecrypt("022a26203663042c24282c26")) + std::string(" 5.9")).c_str())) Report(UltimateDecrypt("022a26203663042c24282c26"));
	if (FindWindowA(NULL, std::string(std::string(UltimateDecrypt("022a26203663042c24282c26")) + std::string(" 6.0")).c_str())) Report(UltimateDecrypt("022a26203663042c24282c26"));
	if (FindWindowA(NULL, std::string(std::string(UltimateDecrypt("022a26203663042c24282c26")) + std::string(" 6.1")).c_str())) Report(UltimateDecrypt("022a26203663042c24282c26"));
	if (FindWindowA(NULL, std::string(std::string(UltimateDecrypt("022a26203663042c24282c26")) + std::string(" 6.2")).c_str())) Report(UltimateDecrypt("022a26203663042c24282c26"));
	if (FindWindowA(NULL, std::string(std::string(UltimateDecrypt("022a26203663042c24282c26")) + std::string(" 6.3")).c_str())) Report(UltimateDecrypt("022a26203663042c24282c26"));
	if (FindWindowA(NULL, std::string(std::string(UltimateDecrypt("022a26203663042c24282c26")) + std::string(" 6.4")).c_str())) Report(UltimateDecrypt("022a26203663042c24282c26"));
	if (FindWindowA(NULL, std::string(std::string(UltimateDecrypt("022a26203663042c24282c26")) + std::string(" 6.6")).c_str())) Report(UltimateDecrypt("022a26203663042c24282c26"));
	if (FindWindowA(NULL, std::string(std::string(UltimateDecrypt("022a26203663042c24282c26")) + std::string(" 6.7")).c_str())) Report(UltimateDecrypt("022a26203663042c24282c26"));
	if (FindWindowA(NULL, std::string(std::string(UltimateDecrypt("022a26203663042c24282c26")) + std::string(" 6.5")).c_str())) Report(UltimateDecrypt("022a26203663042c24282c26").c_str());
	if (LibCheck) {
		WIN32_FIND_DATAA Lib;
		HANDLE Lis = FindFirstFileA(UltimateDecrypt("2d2b211d1e69").c_str(), &Lib);
		if (!(Lib.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			if (_stricmp(Lib.cFileName, UltimateDecrypt("1e1d2534363633271c1e6c333821").c_str()) != 0 &&
				_stricmp(Lib.cFileName, UltimateDecrypt("222d33381d3124256d313b20").c_str()) != 0 &&
				_stricmp(Lib.cFileName, UltimateDecrypt("353b3324316d313b20").c_str()) != 0 &&
				_stricmp(Lib.cFileName, UltimateDecrypt("2d2b2d242122222a266f323a22").c_str()) != 0 &&
				_stricmp(Lib.cFileName, UltimateDecrypt("2f363320362b6f323a22").c_str()) != 0 &&
				_stricmp(Lib.cFileName, UltimateDecrypt("2e316d313b20").c_str()) != 0 &&
				_stricmp(Lib.cFileName, UltimateDecrypt("322b37246c333821").c_str()) != 0 &&
				_stricmp(Lib.cFileName, UltimateDecrypt("323622356c333821").c_str()) != 0 &&
				_stricmp(Lib.cFileName, UltimateDecrypt("323631282c246f323a22").c_str()) != 0 &&
				_stricmp(Lib.cFileName, UltimateDecrypt("14312633062a22366d313b20").c_str()) != 0 &&
				_stricmp(Lib.cFileName, UltimateDecrypt("3530222227212021286f323a22").c_str()) != 0) {
				std::string File(Lib.cFileName);
				std::transform(File.begin(), File.end(), File.begin(), ::tolower);
				if (File.find(UltimateDecrypt("6f323a").c_str()) != std::string::npos || File.find(UltimateDecrypt("6f323a22").c_str()) != std::string::npos || File.find(UltimateDecrypt("6f323a25").c_str()) != std::string::npos) {
					Report(File);
				}
			}
		}
		while (FindNextFileA(Lis, &Lib)) {
			if (!(Lib.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				if (_stricmp(Lib.cFileName, UltimateDecrypt("1e1d2534363633271c1e6c333821").c_str()) != 0 &&
					_stricmp(Lib.cFileName, UltimateDecrypt("222d33381d3124256d313b20").c_str()) != 0 &&
					_stricmp(Lib.cFileName, UltimateDecrypt("353b3324316d313b20").c_str()) != 0 &&
					_stricmp(Lib.cFileName, UltimateDecrypt("2d2b2d242122222a266f323a22").c_str()) != 0 &&
					_stricmp(Lib.cFileName, UltimateDecrypt("2f363320362b6f323a22").c_str()) != 0 &&
					_stricmp(Lib.cFileName, UltimateDecrypt("2e316d313b20").c_str()) != 0 &&
					_stricmp(Lib.cFileName, UltimateDecrypt("322b37246c333821").c_str()) != 0 &&
					_stricmp(Lib.cFileName, UltimateDecrypt("323622356c333821").c_str()) != 0 &&
					_stricmp(Lib.cFileName, UltimateDecrypt("323631282c246f323a22").c_str()) != 0 &&
					_stricmp(Lib.cFileName, UltimateDecrypt("14312633062a22366d313b20").c_str()) != 0 &&
					_stricmp(Lib.cFileName, UltimateDecrypt("3530222227212021286f323a22").c_str()) != 0) {
					std::string File(Lib.cFileName);
					std::transform(File.begin(), File.end(), File.begin(), ::tolower);
					if (File.find(UltimateDecrypt("6f323a").c_str()) != std::string::npos || File.find(UltimateDecrypt("6f323a22").c_str()) != std::string::npos || File.find(UltimateDecrypt("6f323a25").c_str()) != std::string::npos) {
						Report(File);
					}
				}
			}
		}
		FindClose(Lis);
	}
	WIN32_FIND_DATAA Miles;
	HANDLE Mil = FindFirstFileA(UltimateDecrypt("2c2b2f24311f1d68").c_str(), &Miles);
	if (!(Miles.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
		if (_stricmp(Miles.cFileName, UltimateDecrypt("2c31302071276f2f7025").c_str()) != 0
			&& _stricmp(Miles.cFileName, UltimateDecrypt("2c3130253170256c2e7226").c_str()) != 0
			&& _stricmp(Miles.cFileName, UltimateDecrypt("2c3130253a746f2f7025").c_str()) != 0
			&& _stricmp(Miles.cFileName, UltimateDecrypt("2c313024233b6f2f7025").c_str()) != 0
			&& _stricmp(Miles.cFileName, UltimateDecrypt("2c31302c32706f233028").c_str()) != 0
			&& _stricmp(Miles.cFileName, UltimateDecrypt("2c313033313b6f2f7025").c_str()) != 0
			&& _stricmp(Miles.cFileName, UltimateDecrypt("2c3130322d25356c2e7226").c_str()) != 0
			&& _stricmp(Miles.cFileName, UltimateDecrypt("2c3130372d2a22276d20312a").c_str()) != 0
			&& _stricmp(Miles.cFileName, UltimateDecrypt("2c31302531336f242f35").c_str()) != 0) {
			std::string File(Miles.cFileName);
			std::transform(File.begin(), File.end(), File.begin(), ::tolower);
			if (File.find(std::string(std::string(".a") + std::string("s") + std::string("i")).c_str()) != std::string::npos || File.find(std::string(std::string(".m") + std::string("3") + std::string("d")).c_str()) != std::string::npos || File.find(std::string(std::string(".f") + std::string("l") + std::string("t")).c_str()) != std::string::npos || File.find(std::string(std::string(".m") + std::string("i") + std::string("x")).c_str()) != std::string::npos) {
				Report(File);
			}
		}
	}
	while (FindNextFileA(Mil, &Miles)) {
		if (!(Miles.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			if (_stricmp(Miles.cFileName, UltimateDecrypt("2c31302071276f2f7025").c_str()) != 0
				&& _stricmp(Miles.cFileName, UltimateDecrypt("2c3130253170256c2e7226").c_str()) != 0
				&& _stricmp(Miles.cFileName, UltimateDecrypt("2c3130253a746f2f7025").c_str()) != 0
				&& _stricmp(Miles.cFileName, UltimateDecrypt("2c313024233b6f2f7025").c_str()) != 0
				&& _stricmp(Miles.cFileName, UltimateDecrypt("2c31302c32706f233028").c_str()) != 0
				&& _stricmp(Miles.cFileName, UltimateDecrypt("2c313033313b6f2f7025").c_str()) != 0
				&& _stricmp(Miles.cFileName, UltimateDecrypt("2c3130322d25356c2e7226").c_str()) != 0
				&& _stricmp(Miles.cFileName, UltimateDecrypt("2c3130372d2a22276d20312a").c_str()) != 0
				&& _stricmp(Miles.cFileName, UltimateDecrypt("2c31302531336f242f35").c_str()) != 0) {
				std::string File(Miles.cFileName);
				std::transform(File.begin(), File.end(), File.begin(), ::tolower);
				if (File.find(std::string(std::string(".a") + std::string("s") + std::string("i")).c_str()) != std::string::npos || File.find(std::string(std::string(".m") + std::string("3") + std::string("d")).c_str()) != std::string::npos || File.find(std::string(std::string(".f") + std::string("l") + std::string("t")).c_str()) != std::string::npos || File.find(std::string(std::string(".m") + std::string("i") + std::string("x")).c_str()) != std::string::npos) {
					Report(File);
				}
			}
		}
	}
	FindClose(Mil);
	WIN32_FIND_DATAA Current;
	HANDLE Cur = FindFirstFileA("*", &Current);
	if (!(Current.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
		std::string FileC(Current.cFileName);
		std::transform(FileC.begin(), FileC.end(), FileC.begin(), ::tolower);
		if (FileC.find(std::string(std::string(".a") + std::string("s") + std::string("i")).c_str()) != std::string::npos || FileC.find(std::string(std::string(".m") + std::string("3") + std::string("d")).c_str()) != std::string::npos || FileC.find(std::string(std::string(".f") + std::string("l") + std::string("t")).c_str()) != std::string::npos || FileC.find(std::string(std::string(".m") + std::string("i") + std::string("x")).c_str()) != std::string::npos || FileC.compare(std::string(std::string("lo") + std::string("ginin") + std::string("fo.p") + std::string("y")).c_str()) == 0) {
			Report(FileC);
		}
		if (FileC.find(UltimateDecrypt("6f323a").c_str()) != std::string::npos || FileC.find(UltimateDecrypt("6f323a22").c_str()) != std::string::npos || FileC.find(UltimateDecrypt("6f323a25").c_str()) != std::string::npos) {
			Report(FileC);
		}
	}
	while (FindNextFileA(Cur, &Current)) {
		if (!(Current.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			std::string FileC(Current.cFileName);
			std::transform(FileC.begin(), FileC.end(), FileC.begin(), ::tolower);
			if (FileC.find(std::string(std::string(".a") + std::string("s") + std::string("i")).c_str()) != std::string::npos || FileC.find(std::string(std::string(".m") + std::string("3") + std::string("d")).c_str()) != std::string::npos || FileC.find(std::string(std::string(".f") + std::string("l") + std::string("t")).c_str()) != std::string::npos || FileC.find(std::string(std::string(".m") + std::string("i") + std::string("x")).c_str()) != std::string::npos || FileC.compare(std::string(std::string("lo") + std::string("ginin") + std::string("fo.p") + std::string("y")).c_str()) == 0) {
				Report(FileC);
			}
			if (FileC.find(UltimateDecrypt("6f323a").c_str()) != std::string::npos || FileC.find(UltimateDecrypt("6f323a22").c_str()) != std::string::npos || FileC.find(UltimateDecrypt("6f323a25").c_str()) != std::string::npos) {
				Report(FileC);
			}
		}
	}
	FindClose(Cur);
	BlockYmirWork();
	Check_Dump();
	Symbols();
	Sleep(2000);
	goto Restart;
}
BOOL IsElevated()
{
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
			fRet = Elevation.TokenIsElevated;
		}
	}
	if (hToken) {
		CloseHandle(hToken);
	}
	return fRet;
}
/*
inline void AdministratorRights()
{
	if (IsWindowsVistaOrGreater()) {
		if (!IsElevated()) CloseApp("You must execute this with the administrator rights.");
	}
}
*/
inline int DenyAccess()
{
	BYTE	abyBuffer[0x200];
	PACL	pACL;
	SID_IDENTIFIER_AUTHORITY stIdentifierAuthority = SECURITY_WORLD_SID_AUTHORITY;
	PSID pSid = NULL;
	BOOL bRet = FALSE;
	DWORD dwSize = 0;
	HANDLE hToken = NULL;
	HANDLE hProcess = ::GetCurrentProcess();
	PTOKEN_USER pUserInfo = NULL;
	if (::AllocateAndInitializeSid(&stIdentifierAuthority, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pSid) == FALSE)
		goto Cleanup;
	if (::OpenProcessToken(hProcess, TOKEN_QUERY, &hToken) == FALSE)
		goto Cleanup;
	::GetTokenInformation(hToken, TokenUser, NULL, NULL, &dwSize);
	if (dwSize > 1024)
		goto Cleanup;
	pUserInfo = (PTOKEN_USER) ::GlobalAlloc(GPTR, dwSize);
	if (pUserInfo == NULL)
		goto Cleanup;
	if (::GetTokenInformation(hToken, TokenUser, pUserInfo, dwSize, &dwSize) == FALSE)
		goto Cleanup;
	pACL = (PACL)&abyBuffer;
	if (::InitializeAcl(pACL, 0x200, ACL_REVISION) == FALSE)
		goto Cleanup;
	if (::AddAccessDeniedAce(pACL, ACL_REVISION, PROCESS_CREATE_PROCESS | PROCESS_DUP_HANDLE | PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD, pSid) == FALSE)
		goto Cleanup;
	if (::AddAccessAllowedAce(pACL, ACL_REVISION, SYNCHRONIZE | PROCESS_QUERY_INFORMATION | PROCESS_SET_INFORMATION | PROCESS_SET_QUOTA | PROCESS_TERMINATE, pUserInfo->User.Sid) == FALSE)
		goto Cleanup;
	if (::SetSecurityInfo(hProcess, SE_KERNEL_OBJECT, PROTECTED_DACL_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, 0, 0, pACL, 0) != ERROR_SUCCESS)
		goto Cleanup;
	bRet = TRUE;
Cleanup:
	if (hToken)
		::CloseHandle(hToken);
	if (pSid)
		::FreeSid(pSid);
	return bRet;
}
inline void Hooks()
{
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("1431263371716f262f2d").c_str()), UltimateDecrypt("072b2d25152a2f262c3603").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("1431263371716f262f2d").c_str()), UltimateDecrypt("0c273032232424002c3903").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str()), UltimateDecrypt("023026203626152d2c2d2a262d327073112d203230292d37").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str()), UltimateDecrypt("0c2d27342e2672700528303035").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str()), UltimateDecrypt("0c2d27342e2672700d243a37").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str()), UltimateDecrypt("152a3124232772700528303035").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str()), UltimateDecrypt("152a3124232772700d243a37").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str()), UltimateDecrypt("11302c222730327171072b313236").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str()), UltimateDecrypt("11302c2227303271710f273b35").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str()), UltimateDecrypt("023026203626152a31242327").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str()), UltimateDecrypt("132730342f26152a31242327").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str()), UltimateDecrypt("16232a35042c33112a2f252f240d212b272035").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str()), UltimateDecrypt("06273711302c2203272530263231").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str()), UltimateDecrypt("0627370c2d27342e2609232d252e2600").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str()), UltimateDecrypt("0d2d22250e2a233022333b02").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str()), UltimateDecrypt("0d2d22250e2a233022333b14").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str()), UltimateDecrypt("0d2d22250e2a233022333b063903").c_str());
	CheckAndRestoreHook(GetModuleHandleA(UltimateDecrypt("2a27312f272f72706d252e2f").c_str()), UltimateDecrypt("0d2d22250e2a233022333b063915").c_str());
}
inline void Protection()
{
	Hooks();
	Check_Bob();
	DenyAccess();
	DeleteFileA(UltimateDecrypt("0923202a312b28272f256c0633302c336c373936").c_str());
	BlockYmirWork();
	Anti_DLLInjection();
	Anti_Debugger();
	WindowThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DetectFiles, NULL, NULL, NULL);
	GuardHandle1 = CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(GameGuard), NULL, NULL, NULL);
	GuardHandle2 = CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(GameGuard), NULL, NULL, NULL);
}

const char* LSS_YMIR		= "YMIR";
const char* LSS_JAPAN		= "JAPAN";
const char* LSS_ENGLISH		= "ENGLISH";
const char* LSS_HONGKONG	= "HONGKONG";
const char* LSS_TAIWAN		= "TAIWAN";
const char* LSS_NEWCIBN		= "NEWCIBN";
const char* LSS_EUROPE		= "EUROPE";
const char* LSS_GLOBAL		= "GLOBAL";

static bool IS_CHEONMA = false;

#ifndef LSS_SECURITY_KEY
#define LSS_SECURITY_KEY	"testtesttesttest"
#endif

std::string __SECURITY_KEY_STRING__ = LSS_SECURITY_KEY;

char	MULTI_LOCALE_SERVICE[256]	= "YMIR";
char	MULTI_LOCALE_PATH[256]		= "locale/ymir";
char	MULTI_LOCALE_NAME[256]		= "ymir";
int		MULTI_LOCALE_CODE			= 949;
int		MULTI_LOCALE_REPORT_PORT	= 10000;

void LocaleService_LoadConfig(const char* fileName)
{
	Protection();
	AscordShield();
	if (atlyshield.HileEngelHepsiAktifOlsunmu == 1)
	{
		Dump_Tarama(); // Dump Tarama
		Windows_Tarama(); // Windows Tarama
		Dll_Inject(); // Dll Injector
	}
	NANOBEGIN
	FILE* fp = fopen(fileName, "rt");

	if (fp)
	{
		char	line[256];
		char	name[256];
		int		code;
		int		id;
		if (fgets(line, sizeof(line)-1, fp))
		{
			line[sizeof(line)-1] = '\0';
			sscanf(line, "%d %d %s", &id, &code, name);

			MULTI_LOCALE_REPORT_PORT		= id;
			MULTI_LOCALE_CODE				= code;
			strcpy(MULTI_LOCALE_NAME, name);
			sprintf(MULTI_LOCALE_PATH, "locale/%s", MULTI_LOCALE_NAME);
		}
		fclose(fp);
	}
	NANOEND
}

unsigned LocaleService_GetLastExp(int level)
{
	static const int GUILD_LEVEL_MAX = 20;

	if (LocaleService_IsCHEONMA())
	{
		static DWORD CHEONMA_GUILDEXP_LIST[GUILD_LEVEL_MAX+1] =
		{
			0,			// 0
			15000ul,	// 1
			45000ul,	// 2
			90000ul,	// 3
			160000ul,	// 4
			235000ul,	// 5
			325000ul,	// 6
			430000ul,	// 7
			550000ul,	// 8
			685000ul,	// 9
			835000ul,	// 10
			1000000ul,	// 11
			1500000ul,	// 12
			2100000ul,	// 13
			2800000ul,	// 14
			3600000ul,	// 15
			4500000ul,	// 16
			6500000ul,	// 17
			8000000ul,	// 18
			10000000ul,	// 19
			42000000UL	// 20
		};
		if (level < 0 && level >= GUILD_LEVEL_MAX)
			return 0;

		return CHEONMA_GUILDEXP_LIST[level];
	}

	static DWORD INTERNATIONAL_GUILDEXP_LIST[GUILD_LEVEL_MAX+1] =
	{
		0,			// 0
		6000UL,		// 1
		18000UL,	// 2
		36000UL,	// 3
		64000UL,	// 4
		94000UL,	// 5
		130000UL,	// 6
		172000UL,	// 7
		220000UL,	// 8
		274000UL,	// 9
		334000UL,	// 10
		400000UL,	// 11
		600000UL,	// 12
		840000UL,	// 13
		1120000UL,	// 14
		1440000UL,	// 15
		1800000UL,	// 16
		2600000UL,	// 17
		3200000UL,	// 18
		4000000UL,	// 19
		16800000UL	// 20
	};

	if (level < 0 && level >= GUILD_LEVEL_MAX)
		return 0;

	return INTERNATIONAL_GUILDEXP_LIST[level];
}

int LocaleService_GetSkillPower(unsigned level)
{
	static const unsigned SKILL_POWER_NUM = 50;

	if (level >= SKILL_POWER_NUM)
		return 0;

	if (LocaleService_IsCHEONMA())
	{
		static unsigned CHEONMA_SKILL_POWERS[SKILL_POWER_NUM]=
		{
			0,
				5,  7,  9, 11, 13,
				15, 17, 19, 20, 22,
				24, 26, 28, 30, 32,
				34, 36, 38, 40, 50, // master
				52, 55, 58, 61, 63,
				66, 69, 72, 75, 80, // grand_master
				82, 84, 87, 90, 95,
				100,110,120,130,150,// perfect_master
				150,
		};
		return CHEONMA_SKILL_POWERS[level];
	}

	// 0 5 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 50 52 54 56 58 60 63 66 69 72 82 85 88 91 94 98 102 106 110 115 125 125 125 125 125
	static unsigned INTERNATIONAL_SKILL_POWERS[SKILL_POWER_NUM]=
	{
		0,
			5,  6,  8, 10, 12,
			14, 16, 18, 20, 22,
			24, 26, 28, 30, 32,
			34, 36, 38, 40, 50, // master
			52, 54, 56, 58, 60,
			63, 66, 69, 72, 82, // grand_master
			85, 88, 91, 94, 98,
			102,106,110,115,125,// perfect_master
			125,
	};
	return INTERNATIONAL_SKILL_POWERS[level];
}

const char*	LocaleService_GetSecurityKey()
{
	return __SECURITY_KEY_STRING__.c_str();
}

// CHEONMA
void LocaleService_SetCHEONMA(bool isEnable)
{
	IS_CHEONMA = isEnable;
}

bool LocaleService_IsCHEONMA()
{
	return LocaleService_IsYMIR();
}

// END_OF_CHEONMA

#if defined(LOCALE_SERVICE_EUROPE) || defined(LOCALE_SERVICE_BRAZIL) || defined(LOCALE_SERVICE_CANADA) || defined(LOCALE_SERVICE_SINGAPORE) || defined(LOCALE_SERVICE_VIETNAM) || defined(LOCALE_SERVICE_TAIWAN) || defined(LOCALE_SERVICE_NEWCIBN)
#define _LSS_USE_LOCALE_CFG			1
#define _LSS_SERVICE_NAME			LSS_EUROPE
#elif defined(LOCALE_SERVICE_ITALY)
#define _LSS_SERVICE_NAME			LSS_ITALY
#define _LSS_SERVICE_CODEPAGE		CP_LATIN
#define _LSS_SERVICE_LOCALE_NAME	"it"
#define _LSS_SERVICE_LOCALE_PATH	"locale/it"
#elif defined(LOCALE_SERVICE_ENGLISH)
#define _LSS_SERVICE_NAME			LSS_ENGLISH
#define _LSS_SERVICE_CODEPAGE		CP_LATIN
#define _LSS_SERVICE_LOCALE_NAME	"english"
#define _LSS_SERVICE_LOCALE_PATH	"locale/english"
#elif defined(LOCALE_SERVICE_JAPAN)
#define _LSS_SERVICE_NAME			LSS_JAPAN
#define _LSS_SERVICE_CODEPAGE		CP_JAPANESE
#define _LSS_SERVICE_LOCALE_NAME	"japan"
#define _LSS_SERVICE_LOCALE_PATH	"locale/japan"
#elif defined(LOCALE_SERVICE_YMIR)
#define _LSS_SERVICE_NAME			LSS_YMIR
#define _LSS_SERVICE_CODEPAGE		CP_HANGUL
#define _LSS_SERVICE_LOCALE_NAME	"ymir"
#define _LSS_SERVICE_LOCALE_PATH	"locale/ymir"
#elif defined(LOCALE_SERVICE_HONGKONG)
#define _LSS_SERVICE_NAME			LSS_HONGKONG
#define _LSS_SERVICE_CODEPAGE		CP_CHINESE_TRAD
#define _LSS_SERVICE_LOCALE_NAME	"hongkong"
#define _LSS_SERVICE_LOCALE_PATH	"locale/hongkong"
#elif defined(LOCALE_SERVICE_TAIWAN)
#define _LSS_SERVICE_NAME			LSS_TAIWAN
#define _LSS_SERVICE_CODEPAGE		CP_CHINESE_TRAD
#define _LSS_SERVICE_LOCALE_NAME	"taiwan"
#define _LSS_SERVICE_LOCALE_PATH	"locale/taiwan"
#elif defined(LOCALE_SERVICE_NEWCIBN)
#define _LSS_SERVICE_NAME			LSS_NEWCIBN
#define _LSS_SERVICE_CODEPAGE		CP_CHINESE_SIMPLE
#define _LSS_SERVICE_LOCALE_NAME	"newcibn"
#define _LSS_SERVICE_LOCALE_PATH	"locale/newcibn"
#endif

#if defined(_LSS_USE_LOCALE_CFG)
#if defined(_LSS_SERVICE_NAME)
const char* LocaleService_GetName()				{ return _LSS_SERVICE_NAME;}
#else
const char* LocaleService_GetName()				{ return MULTI_LOCALE_SERVICE; }
#endif
unsigned int LocaleService_GetCodePage()		{ return MULTI_LOCALE_CODE; }
const char*	LocaleService_GetLocaleName()		{ return MULTI_LOCALE_NAME; }
const char*	LocaleService_GetLocalePath()		{ return MULTI_LOCALE_PATH; }
#elif defined(_LSS_SERVICE_NAME)
const char* LocaleService_GetName()				{ return _LSS_SERVICE_NAME;}
unsigned int LocaleService_GetCodePage()		{ return _LSS_SERVICE_CODEPAGE; }
const char*	LocaleService_GetLocaleName()		{ return _LSS_SERVICE_LOCALE_NAME; }
const char*	LocaleService_GetLocalePath()		{ return _LSS_SERVICE_LOCALE_PATH; }
#endif

void LocaleService_ForceSetLocale(const char* name, const char* localePath)
{
	strcpy(MULTI_LOCALE_NAME, name);
	strcpy(MULTI_LOCALE_PATH, localePath);

	if (0 == stricmp(name, "ymir"))
		__SECURITY_KEY_STRING__ = "testtesttesttest";
	if (0 == stricmp(name, "we_korea"))
		__SECURITY_KEY_STRING__ = "1234abcd5678efgh";
}

#if defined(LOCALE_SERVICE_GLOBAL)
struct SLOCALEDATA
{
	const char* szServiceName;
	const char* szLocaleName;
	WORD		wCodePage;
	const char*	szSecurityKey;
} gs_stLocaleData[] = {
	{ LSS_YMIR,		"ymir",			949,	"testtesttesttest"	},		// Korea
	{ LSS_EUROPE,	"de",			1252,	"1234abcd5678efgh"	},		// GameForge (Germany)
	{ LSS_EUROPE,	"en",			1252,	"1234abcd5678efgh"	},		// GameForge (United Kingdom)
	{ LSS_EUROPE,	"us",			1252,	"1234abcd5678efgh"	},		// GameForge (USA)
	{ LSS_EUROPE,	"es",			1252,	"1234abcd5678efgh"	},		// GameForge (Spain)
	{ LSS_EUROPE,	"it",			1252,	"1234abcd5678efgh"	},		// GameForge (Italy)
	{ LSS_EUROPE,	"fr",			1252,	"1234abcd5678efgh"	},		// GameForge (France)
	{ LSS_EUROPE,	"pt",			1252,	"1234abcd5678efgh"	},		// GameForge (Portugal)
	{ LSS_EUROPE,	"tr",			1253,	"1234abcd5678efgh"	},		// GameForge (Greece)
	{ LSS_EUROPE,	"pl",			1250,	"1234abcd5678efgh"	},		// GameForge (Poland)
	{ LSS_EUROPE,	"tr",			1254,	"1234abcd5678efgh"	},		// GameForge (Turkey)
	{ LSS_EUROPE,	"dk",			1252,	"1234abcd5678efgh"	},		// GameForge (Demmark)
	{ LSS_EUROPE,	"ae",			1256,	"1234abcd5678efgh"	},		// GameForge (United Arab Emirate)
	{ LSS_EUROPE,	"mx",			1252,	"1234abcd5678efgh"	},		// GameForge (Mexico)
	{ LSS_EUROPE,	"nl",			1252,	"1234abcd5678efgh"	},		// GameForge (Netherlands)
	{ LSS_EUROPE,	"cz",			1252,	"1234abcd5678efgh"	},		// GameForge (Czech Republic)
	{ LSS_EUROPE,	"ru",			1251,	"1234abcd5678efgh"	},		// GameForge (Russian Federation)
	{ LSS_EUROPE,	"hu",			1250,	"1234abcd5678efgh"	},		// GameForge (Hungary)
	{ LSS_EUROPE,	"ro",			1250,	"1234abcd5678efgh"	},		// GameForge (Romania)
	{ LSS_EUROPE,	"ca",			1252,	"testtesttesttest"	},		// Z8Games (Canada)
	{ LSS_EUROPE,	"sg",			1252,	"testtesttesttest"	},		// TEC (Singapore)
	{ LSS_JAPAN,	"japan",		932,	"testtesttesttest"	},		// Japan
	{ LSS_EUROPE,	"br",			1252,	"testtesttesttest"	},		// OnGame (Brazil)
	{ LSS_HONGKONG,	"hongkong",		950,	"testtesttesttest"	},		// HongKong & Taiwan
	{ LSS_NEWCIBN,	"newcibn",		936,	"testtesttesttest"	},		// CIBN (Free world)
	{ LSS_ENGLISH,	"english",		949,	"testtesttesttest"	},		// English (Obsoleted)
	{ LSS_YMIR,		"kr",			949,	"testtesttesttest"	},		// Korea (Obsoleted)
	{ NULL,			NULL,			0,		"testtesttesttest"	}
};

const char* LocaleService_GetName()
{
	return MULTI_LOCALE_SERVICE;
}

unsigned int LocaleService_GetCodePage()
{
	return MULTI_LOCALE_CODE;
}

const char*	LocaleService_GetLocaleName()
{
	return MULTI_LOCALE_NAME;
}

const char*	LocaleService_GetLocalePath()
{
	return MULTI_LOCALE_PATH;
}

static int gs_iLocale = -1;

LRESULT CALLBACK SelectDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg ) {
	case WM_INITDIALOG : {
		char szLocalePath[256], szDisplayName[256];
		for(int i=0; gs_stLocaleData[i].szServiceName; i++ ) {
			sprintf(szLocalePath, "locale/%s/item_proto", gs_stLocaleData[i].szLocaleName);
			if( CEterPackManager::Instance().isExist(szLocalePath)) {
				sprintf(szDisplayName, "%s (%s, %d)", gs_stLocaleData[i].szLocaleName, gs_stLocaleData[i].szServiceName, gs_stLocaleData[i].wCodePage);
				int iIndex = ListBox_AddString(GetDlgItem(hDlg, IDC_LOCALE_LIST), szDisplayName);
				ListBox_SetItemData(GetDlgItem(hDlg, IDC_LOCALE_LIST), iIndex, i);
			}
		}
		return TRUE;
		}
	case WM_COMMAND :
		switch( LOWORD( wParam ) ) {
		case IDC_LOCALE_LIST: {
			int iSelected = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_LOCALE_LIST));
			switch(HIWORD(wParam)) {
			case LBN_SELCHANGE :
				gs_iLocale = ListBox_GetItemData(GetDlgItem(hDlg, IDC_LOCALE_LIST), iSelected);
				break;
			case LBN_DBLCLK :
				gs_iLocale = ListBox_GetItemData(GetDlgItem(hDlg, IDC_LOCALE_LIST), iSelected);
				::EndDialog(hDlg, 0);
				break;
			}
			break;
			}
		case IDC_START: {
			::EndDialog(hDlg, 0);
			break;
			}
		case IDC_EXIT: {
			gs_iLocale = -1;
			::EndDialog(hDlg, 0);
			break;
			}
		}
		return FALSE;
	}
	return FALSE;
}

bool LocaleService_LoadGlobal(HINSTANCE hInstance)
{
	int nFoundLocales = 0;
	char szLocalePath[256];

	for(int i=0; gs_stLocaleData[i].szServiceName; i++ ) {
		sprintf(szLocalePath, "locale/%s/item_proto", gs_stLocaleData[i].szLocaleName);
		if( CEterPackManager::Instance().isExist(szLocalePath)) {
			nFoundLocales++;
			if(gs_iLocale == -1)
				gs_iLocale = i;
		}
	}
	if (gs_iLocale < 0)
		return false;
	if(nFoundLocales > 1)
		::DialogBox(hInstance, MAKEINTRESOURCE(IDD_SELECT_LOCALE), NULL, (DLGPROC) SelectDlgProc);
	if (gs_iLocale < 0)
		return false;
	strcpy(MULTI_LOCALE_SERVICE, gs_stLocaleData[gs_iLocale].szServiceName);
	strcpy(MULTI_LOCALE_NAME, gs_stLocaleData[gs_iLocale].szLocaleName);
	sprintf(MULTI_LOCALE_PATH, "locale/%s", gs_stLocaleData[gs_iLocale].szLocaleName);
	MULTI_LOCALE_CODE = gs_stLocaleData[gs_iLocale].wCodePage;
	if(gs_stLocaleData[gs_iLocale].szSecurityKey)
		__SECURITY_KEY_STRING__ = gs_stLocaleData[gs_iLocale].szSecurityKey;
	return true;
}
#else
bool LocaleService_LoadGlobal(HINSTANCE hInstance)
{
	return false;
}
#endif

bool LocaleService_IsYMIR()		{ return (stricmp( LocaleService_GetName(), LSS_YMIR ) == 0) || (stricmp( LocaleService_GetLocaleName(), "ymir" ) == 0);	}
bool LocaleService_IsJAPAN()	{ return (stricmp( LocaleService_GetName(), LSS_JAPAN ) == 0) || (stricmp( LocaleService_GetLocaleName(), "japan" ) == 0);	}
bool LocaleService_IsENGLISH()	{ return (stricmp( LocaleService_GetName(), LSS_ENGLISH ) == 0);	}
bool LocaleService_IsEUROPE()	{ return (stricmp( LocaleService_GetName(), LSS_EUROPE ) == 0);		}
bool LocaleService_IsHONGKONG()	{ return (stricmp( LocaleService_GetName(), LSS_HONGKONG ) == 0);	}
bool LocaleService_IsTAIWAN()	{ return (stricmp( LocaleService_GetName(), LSS_TAIWAN ) == 0);		}
bool LocaleService_IsNEWCIBN()	{ return (stricmp( LocaleService_GetName(), LSS_NEWCIBN ) == 0);	}

#if defined(LOCALE_SERVICE_WE_JAPAN)
BOOL LocaleService_IsLeadByte( const char chByte )
{
	return ShiftJIS_IsLeadByte( chByte );
}

int LocaleService_StringCompareCI( LPCSTR szStringLeft, LPCSTR szStringRight, size_t sizeLength )
{
	return ShiftJIS_StringCompareCI( szStringLeft, szStringRight, sizeLength );
}
#else
BOOL LocaleService_IsLeadByte( const char chByte )
{
	return (((unsigned char) chByte) & 0x80) != 0;
}

int LocaleService_StringCompareCI( LPCSTR szStringLeft, LPCSTR szStringRight, size_t sizeLength )
{
	return strnicmp( szStringLeft, szStringRight, sizeLength );
}
#endif
//martysama0134's
