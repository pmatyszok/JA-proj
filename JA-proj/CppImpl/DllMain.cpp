#include <Windows.h>


BOOLEAN WINAPI DllMain( IN HINSTANCE hDllHandle, 
		 IN DWORD     nReason, 
		 IN LPVOID    Reserved )
 {

  switch ( nReason )
   {
	case DLL_PROCESS_ATTACH:
	  break;
	case DLL_PROCESS_DETACH:
	  break;
   }
  return TRUE;
 }

#include <iostream>

extern "C"{
	void __declspec(dllexport) __cdecl CorrectGamma()
	{
		std::cout << "dupa;";
	}
}
