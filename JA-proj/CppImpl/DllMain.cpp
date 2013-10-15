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
#include <math.h>

typedef unsigned char byte;

extern "C"
{
	void __declspec(dllexport) __cdecl CorrectGamma(byte bitmap[], int size, double gamma)
	{
		for (int i = 0; i < size; i++)
		{
			bitmap[i] = static_cast<byte>((static_cast<double>(bitmap[i]), gamma));
		}
	}
}
