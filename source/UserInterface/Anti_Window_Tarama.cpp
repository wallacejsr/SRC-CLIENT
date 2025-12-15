//CODED BY AscordSHÝELD - AHMET Ascord
#include "stdafx.h"
#include "ayarlar.h"
#include <fstream>
#include <string>

using namespace std;

string __VOCABLES = "aAbBeEiIoOuUyY";
string __Consonants = "bBcCdDfFgGhHjJkKlLmMnNpPqQrRsStTvVwWxXyYzZ";

void TraceError(const char* message) {
    cout << "[ERROR] " << message << endl;
}

void vKillProcess(const string& processName) {
    cout << "Terminating process: " << processName << endl;
    // Add code to terminate the process
}

bool IsValidTitleChar(char c) {
    return (__VOCABLES.find(c) != string::npos || __Consonants.find(c) != string::npos);
}

bool IsSeparator(char c) {
    return (c == '.' || c == ' ' || c == '_');
}

void LogDetectedWindow(const string& WindowTitle) {
    if (WindowTitle.substr(0, 5) == "cheat")
	{
        // TraceError("[c0d3xShield] Cheat detected!");
        return;
    }

    int digitCount = 0;
    for (char c : WindowTitle) {
        if (isdigit(c)) {
            digitCount++;
        }

        if (digitCount >= 6)
		{
            // TraceError("[c0d3xShield] Too many digits detected!");
            PostQuitMessage(0);
        }
    }

    bool bProgramSystem = false;
    int iCountFake = 0;

    for (int i = 0; i < WindowTitle.size(); i++) {
        char currentChar = WindowTitle[i];

        if (IsSeparator(currentChar) || !IsValidTitleChar(currentChar)) {
            bProgramSystem = true;
            break;
        }
    }

    if (!bProgramSystem) {
        iCountFake++;

        if (iCountFake >= 4 && WindowTitle.size() >= 10 && WindowTitle.size() <= 14) {
            int iTmp = 0;

            while (iTmp < WindowTitle.size()) {
                if (IsSeparator(WindowTitle[iTmp])) {
                    break;
                }

                iTmp++;

                if (iTmp >= 7) {
                    vKillProcess("metin2client.bin");
                }
            }
        }
    }
}

void TitleWindow(LPCSTR WindowTitle){
	HWND WinTitle = FindWindowA(NULL,WindowTitle);
	if( WinTitle > 0){

		{
			using namespace std;
			fstream textfile;
			textfile.open(atlyshield.LogTutmatxtismineolsun, ios::out | ios::app);
			textfile << WindowTitle << " - c0d3xShield Log detected." << endl;
			exit(0);
		}
	}
}

void TitleCheckWindow(){
TitleWindow("ArtMoney PRO v7.27");
TitleWindow("ArtMoney SE v7.31");
TitleWindow("ArtMoney SE v7.32");
TitleWindow("lalaker1");
TitleWindow("Bypass");
TitleWindow("Cheat Engine 5.0");
TitleWindow("Cheat Engine 5.1");
TitleWindow("Cheat Engine 5.1.1");
TitleWindow("Cheat Engine 5.2");
TitleWindow("Cheat Engine 5.3");
TitleWindow("Cheat Engine 5.4");
TitleWindow("Cheat Engine 5.5");
TitleWindow("Cheat Engine 5.6");
TitleWindow("Cheat Engine 5.6.1");
TitleWindow("Cheat Engine 6.0");
TitleWindow("Cheat Engine 6.1");
TitleWindow("Cheat Engine 6.3");
TitleWindow("Cheat Engine 6.4");
TitleWindow("Cheat Engine");
TitleWindow("HiDeToolz");
TitleWindow("HideToolz");
TitleWindow("1nj3ct0r");
TitleWindow("injector");
TitleWindow("Injector");
TitleWindow("Olly Debugger");
TitleWindow("Process Explorer 11.33");
TitleWindow("Process Explorer");
TitleWindow("Process Hacker");
TitleWindow("T Search");
TitleWindow("WPE PRO");
TitleWindow("WPePro 0.9a");
TitleWindow("WPePro 1.3");
TitleWindow("ZhyperMu Packet Editor");
TitleWindow("hacker");	
TitleWindow("rPE - rEdoX Packet Editor");
TitleWindow("7x");
TitleWindow("wallhack");
TitleWindow("hile");
TitleWindow("7x , hile");
TitleWindow("lalaker2");
TitleWindow("lalaker hile");
TitleWindow("injectör");
TitleWindow("mt2 hileleri");
TitleWindow("megadosya");
TitleWindow("hileler");
TitleWindow("baypass");
TitleWindow("metin2mod");
TitleWindow("bot");
TitleWindow("hilesi indir");
TitleWindow("megadosya.com");
TitleWindow("Injector Gadget");
TitleWindow("[BETA] Hack Loader v1.0.3");
TitleWindow("[BETA] Hack Loader");
TitleWindow("Module loaded!");
TitleWindow("Okcu Hilesi");
TitleWindow("Mobber Hack");
TitleWindow("IrcRest Python Loader");
TitleWindow("Python Loader");
TitleWindow("CurrPorts");
TitleWindow("Moo0 Connection Watcher");
TitleWindow("Open Ports Scanner");
TitleWindow("PortExpert v1.6.1.9");
TitleWindow("ProcNetMonitor");
TitleWindow("TcpLogView");
TitleWindow("TCPMonitor");
TitleWindow("System Explorer");
TitleWindow("GhostMouse 3.2.3");
TitleWindow("Windows Görev Yöneticisi");
TitleWindow("[M24 Pro]");
TitleWindow("MultiHack Injector");
TitleWindow("N-Jector by NOOB");
TitleWindow("Success!!");
TitleWindow("Core hack 1.2");
TitleWindow("Metin2 PServers Bypass by lalaker1");
TitleWindow("Komut Ýstemi");
TitleWindow("Shaiya Launcher");
TitleWindow("Metin2 Launcher by NoviceHacker");
TitleWindow("Chestille Metin2 PvP Bot");
TitleWindow("[DF] Beta 2.0.0");
TitleWindow("VMware vSphere Client");
TitleWindow("Oto Tuþ V3");
TitleWindow("Multi Macro Bot 1.0");
TitleWindow("Extreme Injector v3.5 by master131");
TitleWindow("Extreme Injector v3.7.2 by master131");
TitleWindow("PaidBot");
TitleWindow("Dll-Injectorv2 by Padmak");
TitleWindow("Cheat Engine 6.8.1");
TitleWindow("Cheat Engine 6.8.0");
TitleWindow("Cheat Engine 6.7.0");
TitleWindow("Cheat Engine 6.7");
TitleWindow("Cheat Engine 6.5");
TitleWindow("Cheat Engine 6.6");
TitleWindow("Cheat Engine 6.2");
TitleWindow("Cheat Engine 6.5.1");
TitleWindow("Cheat Engine 5.4.0");
TitleWindow("Cheat Engine 6.2.0");
TitleWindow("lalaker1.app");
TitleWindow("lalaker1.app.injector");
TitleWindow("lalaker1.app.v2");
TitleWindow("lalaker1.app - www.lalaker1.app/app.v2");
TitleWindow("lalaker1.app BETA ver.106.2");
TitleWindow("lalaker1.app.v3.0.4_9");
TitleWindow("kagb22.dll");
TitleWindow("kagb27.dll");
TitleWindow("by master131");
TitleWindow("st_mod_universal");
TitleWindow("st_mod_universal.rar");
TitleWindow("ST Mod - Patcher.exe");
TitleWindow("ST Mod - Patcher");
TitleWindow("STMod");
TitleWindow("STMod.exe");
TitleWindow("Resources");
TitleWindow("M24Pro_4_51");
TitleWindow("M24Pro_4_52");
TitleWindow("M24Pro_4_53");
TitleWindow("M24Pro_4_54");
TitleWindow("M24Pro_4_55");
TitleWindow("M24Pro_4_56");
TitleWindow("M24Pro_4_57");
TitleWindow("M24Pro");
TitleWindow("Launcher");
TitleWindow("Loader.dll");
TitleWindow("M24Pro_4_57");
TitleWindow("M24Pro.net");
}

void Window_Tekrar_Tara()
{
again:
	TitleCheckWindow();
	Sleep(1000);
	goto again;
}

void Windows_Tarama(){
	CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(Window_Tekrar_Tara), NULL, 0, 0);
}

