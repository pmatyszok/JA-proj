#include <iostream>
#include <Windows.h>

using namespace std;

typedef void (*__stdcall gamma)();



int main()
{
	HMODULE lib = LoadLibrary("CppImpl.dll");
	if (lib == NULL)
		return -1;
	gamma fun = (gamma)GetProcAddress(lib, "CorrectGamma");
	if (fun == NULL)
		return -2;
	HMODULE asmLib = LoadLibrary("AsmImpl.dll");
	if (asmLib == NULL)
		return -3;
	gamma fun2 = (gamma)GetProcAddress(asmLib, "TestFunction");
	if (fun2 == NULL)
		return -4;

	fun();
	fun2();
	int ay;
	cin >> ay;
	if (lib != NULL)
		FreeLibrary(lib);
	if (asmLib != NULL)
		FreeLibrary(asmLib);
	return 0;
}