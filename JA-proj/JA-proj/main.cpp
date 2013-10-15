#include <iostream>
#include <Windows.h>

using namespace std;

typedef void (*gamma)();



int main()
{
	HMODULE lib = LoadLibrary("CppImpl.dll");
	if (lib == NULL)
		return -1;
	gamma fun = (gamma)GetProcAddress(lib, "CorrectGamma");
	if (fun == NULL)
		return -2;

	fun();
	int ay;
	cin >> ay;
	if (lib != NULL)
		FreeLibrary(lib);
	return 0;
}