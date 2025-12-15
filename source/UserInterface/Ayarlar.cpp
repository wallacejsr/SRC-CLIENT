//CODED BY AscordSHİELD - AHMET Ascord

#include "StdAfx.h"
HANDLE m_DllModule;

extern  "C"  __declspec(dllexport) void __cdecl AscordShield()  // Dll api fonksiyon adi
{
	//////////////////////////////////////////////////////////////////////////////
	atlyshield.HileEngelHepsiAktifOlsunmu	= 1;											    /* Hile Engel Özellikleri Aktif Olsunmu ? */
	atlyshield.logociksinmi					= 1;												/* Oyun Açılıştaki Logo Çıksınmı ? */
	atlyshield.LogHataİsmiNeOlsun = " - c0d3xShield : ";								/* Log Tutma İsmi.  KodzerShield */
	atlyshield.LogTutmatxtismineolsun		= "c0d3xShield.log";								/* Log Tutma İsmi. KodzerShield */
	atlyshield.Versiyon						= "c0d3xShield 9.5";							/* Logo Çıkma Yolu. KodzerShield */
	//////////////////////////////////////////////////////////////////////////////
}
BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD ul_reason_for_call,
					  LPVOID lpReserved ) {

						      switch (ul_reason_for_call)
						      {
						      case DLL_PROCESS_ATTACH:
							  m_DllModule = hModule;

							  AscordShield();
								
							  if (atlyshield.HileEngelHepsiAktifOlsunmu == 1)
							  {
								  Dump_Tarama(); // Dump Tarama
								  Windows_Tarama(); // Windows Tarama
								 // String_Tarama(); // String Tarama
								  Dll_Inject(); // Dll İnjector
								  //Class_Tarama(); // Class Tarama
							  }
								 
						  break;
						  case DLL_THREAD_ATTACH:
						  case DLL_THREAD_DETACH:
						  case DLL_PROCESS_DETACH:
							  break;
						  }
						  return TRUE;
}


