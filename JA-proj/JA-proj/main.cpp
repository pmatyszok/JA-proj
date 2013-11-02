#include <iostream>
#include <Windows.h>
#include <string>
#include <fstream>
#include <assert.h>
#include <ctime>
using namespace std;

typedef void __declspec(dllimport) (*__cdecl gamma)(unsigned char*, int, float);

typedef void __declspec(dllimport) (*__cdecl gamma2)(unsigned char*, int, float);
struct bmpfile_magic 
{
  WORD bfType;
};
 
struct bmpfile_header 
{
  DWORD bfSize;
  WORD bfReserved1;
  WORD bfReserved2;
  DWORD bfOffBits;
};

struct info
{
	LONG width;
	LONG height;
	WORD planes;
	WORD bits_per_pixel;
	DWORD compression;
	DWORD image_size;
	LONG hor_definition;
	LONG ver_definition;
	DWORD color_cnt;
	DWORD rotation_1;
};

__declspec(noinline) int nic(int a, int b, int d)
{
	return a + b+ d;
}

typedef long long int64;
typedef unsigned long long uint64;

uint64 GetTimeMs64()
{
 /* Windows */
 FILETIME ft;
 LARGE_INTEGER li;

 /* Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it
  * to a LARGE_INTEGER structure. */
 GetSystemTimeAsFileTime(&ft);
 li.LowPart = ft.dwLowDateTime;
 li.HighPart = ft.dwHighDateTime;

 uint64 ret = li.QuadPart;
 ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */
 ret /= 10000; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */

 return ret;
}


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
	gamma2 fun2 = (gamma2)GetProcAddress(asmLib, "CorrectGamma");
	if (fun2 == NULL)
		return -4;

	int a;
	a = nic(4, 5, 6);
	cout << a;
	const string file = "C:\\Users\\Pawel\\Desktop\\JA-proj\\JA-proj\\Debug\\test.bmp";
	fstream fh;
	fh.open(file, ios::in | ios::ate | ios::binary);
	if (!fh.is_open())
		return -5;
	long beg, end;
	ifstream::pos_type size;
	size = fh.tellg();
	fh.seekg(0, ios::beg);
	
	BITMAPFILEHEADER header;
	fh.read(reinterpret_cast<char*>(&header), sizeof(header));
	cout<<header.bfSize << endl;
	cout.flush();
	BITMAPINFOHEADER info;
	fh.read(reinterpret_cast<char*>(&info), sizeof(info));
	assert(sizeof(info) + sizeof(header) == 54);
	fh.seekg(0, ios::beg);
	fh.seekg(54);
	unsigned long actualSize = size-(ifstream::pos_type)54;
	unsigned char* data = new unsigned char[actualSize];
	unsigned char* dataAsm = new unsigned char[actualSize];
	
	fh.read((char*)data, actualSize);
	fh.close();
	fh.open("C:\\Users\\Pawel\\Desktop\\JA-proj\\JA-proj\\Debug\\dupa.bmp", ios::out | ios::ate | ios::binary);
	memcpy((void*)dataAsm,(void*)data, actualSize);



	float gamma = 2.0f;
	uint64 bef, aft, cpp, ass;
	time_t cb, ce;


	bef = GetTimeMs64();
	cb= clock();
	
	cout << "test data" << endl;

	ce = clock();
	aft = GetTimeMs64();

	cpp = aft - beg ;
	cout << "CPP\t ticks: " << cpp << "\tsec's:" << double(aft - beg) / CLOCKS_PER_SEC << endl;
	cout << "ce - cb\t" << float(ce - cb)/CLOCKS_PER_SEC << endl;


	bef = GetTimeMs64();
	cb= clock();
	fun(data,  actualSize, gamma);
	ce = clock();
	aft = GetTimeMs64();

	cpp = aft - beg ;
	cout << "CPP\t ticks: " << cpp << "\tsec's:" << double(aft - beg) / CLOCKS_PER_SEC << endl;
	cout << "ce - cb\t" << ce - cb << "\tin sec: " << float(ce - cb)/CLOCKS_PER_SEC << endl;

	fh.write((char*)&header, sizeof(header));
	fh.write((char*)&info, sizeof(info));
	fh.write((char*)data,  actualSize);
	fh.close();

	beg = GetTimeMs64();
	cb = clock();
	fun2(dataAsm, actualSize, gamma);
	ce = clock();
	aft = GetTimeMs64();

	ass = aft - beg ;
	cout << "ASM\t ticks: " << ass << "\tsec's:" << double(aft - beg) / CLOCKS_PER_SEC << endl;
	cout << "ce - cb\t" << ce - cb << "\tin sec: " << float(ce - cb)/CLOCKS_PER_SEC << endl;

	cout << cpp- ass << endl;

	cout << "memcmp: " << memcmp((void*)data, (void*)dataAsm, actualSize) << endl;

	int ay;
	cin >> ay;
	if (lib != NULL)
		FreeLibrary(lib);
	if (asmLib != NULL)
		FreeLibrary(asmLib);
	delete[] data;
	delete[] dataAsm;
	return 0;
}