#include <iostream>
#include <Windows.h>
#include <string>
#include <fstream>
#include <assert.h>
using namespace std;

typedef void __declspec(dllimport) (*__cdecl gamma)(unsigned char*, int, double);

typedef void __declspec(dllimport) (*__stdcall gamma2)(unsigned char*, int, double);
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
	
	unsigned char* data = new unsigned char[size-(ifstream::pos_type)54];
	
	fh.read((char*)data, size-(ifstream::pos_type)54);
	/*byte* pixes = reinterpret_cast<byte*>(data);*/
	fh.close();
	fh.open("C:\\Users\\Pawel\\Desktop\\JA-proj\\JA-proj\\Debug\\dupa.bmp", ios::out | ios::ate | ios::binary);
	//for(int t = size-(ifstream::pos_type)54; t > size-(ifstream::pos_type)954; t-=3)
	//{
	//	cout << (unsigned char)data[t] << " " << (unsigned char)data[t+1] << " " << (unsigned char)data[t+2] << endl;
	//	int a;
	//	cin >> a;
	//}
	fun(data,  size-(ifstream::pos_type)54, 0.4);
	fh.write((char*)&header, sizeof(header));
	fh.write((char*)&info, sizeof(info));
	fh.write((char*)data,  size-(ifstream::pos_type)54);
	fh.close();
	
	fun2(data, size-(ifstream::pos_type)54, 0.4);
	int ay;
	cin >> ay;
	if (lib != NULL)
		FreeLibrary(lib);
	if (asmLib != NULL)
		FreeLibrary(asmLib);
	return 0;
}