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
#include <numeric>
//#pragma comment(linker, "/export:CorrectGamma=_CorrectGamma@16")
typedef unsigned char byte;
#undef max
extern "C"
{
	using namespace std;
	void __declspec(dllexport) __cdecl CorrectGamma(unsigned char* bitmap, int size, float gamma)
	{
		for (int i = 0; i < size; i++)
		{
			byte pixel = bitmap[i];
			bitmap[i] = static_cast<byte>(pow((float)pixel / (float)255, gamma) * (float)255);
			
		}
	}
}
