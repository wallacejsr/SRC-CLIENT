//CODED BY AscordSHÝELD - AHMET Ascord
#include "StdAfx.h"
#include "ayarlar.h"
/*
int file_exists(const char *filename)
{
  FILE *arquivo;
  if(arquivo = fopen(filename, "r"))
  {	 
    fclose(arquivo);	
	return 1;	
  }
    return 0;
}
*/

void Injetado(){

	{
		using namespace std;
		fstream textfile;
		textfile.open(atlyshield.LogTutmatxtismineolsun, ios::out | ios::app);
		textfile << "----------------------------------------------------------------- " << endl;
		textfile << atlyshield.LogHataÝsmiNeOlsun << "c0d3xShield Log detected." << endl;
		textfile << atlyshield.LogHataÝsmiNeOlsun << "Duvidas entre em contato com um ADMINISTRADOR." << endl;
		ShellExecute(NULL, "open", atlyshield.LogTutmatxtismineolsun, NULL, NULL, SW_SHOWNORMAL);
		exit(0);
	}
}

void I_loop(){ 
// Dica: [NOME.extenção] não é case sensitive.
	 if (GetModuleHandle("speedhack.dll")    || 
		(GetModuleHandle("speed-hack.dll"))  ||
		(GetModuleHandle("speed-hack.dll"))  ||
		(GetModuleHandle("speed_hack.dll"))  ||
		(GetModuleHandle("hack_speed.dll"))  ||
		(GetModuleHandle("hack-speed.dll"))  ||
		(GetModuleHandle("hackspeed.dll"))   ||
		(GetModuleHandle("hack.dll"))        ||
		(GetModuleHandle("wpepro.dll"))      ||
		(GetModuleHandle("Cr4ck3r.dll"))     ||
		(GetModuleHandle("wpeprospy.dll"))   ||
		(GetModuleHandle("engine.dll"))      || 
		(GetModuleHandle("CheatEngine.dll")) || 
		(GetModuleHandle("c.e.dll"))         || 
		(GetModuleHandle("cheat.dll"))		 ||
		(GetModuleHandle("hile.dll"))		 ||
		(GetModuleHandle("hack.dll"))		 ||
		(GetModuleHandle("karacabay.dll"))	 ||
		(GetModuleHandle("bypass.dll"))		 ||
		(GetModuleHandle("ProDamage++.dll")) ||
		(GetModuleHandle("ProDamage.dll"))	 ||
		(GetModuleHandle("kagb22.dll"))	 ||
		(GetModuleHandle("kagb27.dll"))	 ||
		(GetModuleHandle("Loader.dll"))	 ||
		// (GetModuleHandle("FoxAnti.dll"))     ||  
		(GetModuleHandle("smoll.dll"))        )
	{
	Injetado();
	}
	if(!FindWindow(NULL,"Nada acontece..."))
	{
}
}

void Dll_Inject(){
	CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(I_loop), NULL, 0, 0);
}
