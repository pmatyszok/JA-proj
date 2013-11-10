#include <iostream>
#include <Windows.h>
#include <string>
#include <fstream>
#include <assert.h>
#include <ctime>
using namespace std;

typedef void __declspec(dllimport) (*__cdecl gamma)(unsigned char*, int, float);


__declspec(noinline) int nic(int a, int b, int d)
{
	return a + b+ d;
}

typedef long long int64;
typedef unsigned long long uint64;

BITMAPINFOHEADER info;
BITMAPFILEHEADER header;



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


gamma fun;
gamma fun2;
HMODULE lib;
HMODULE asmLib;


int PrepareLibs()
{
	lib = LoadLibrary("CppImpl.dll");
	if (lib == NULL)
		return -1;
	fun = (gamma)GetProcAddress(lib, "CorrectGamma");
	if (fun == NULL)
		return -2;
	HMODULE asmLib = LoadLibrary("AsmImpl.dll");
	if (asmLib == NULL)
		return -3;
	fun2 = (gamma)GetProcAddress(asmLib, "CorrectGamma");
	if (fun2 == NULL)
		return -4;
	return 0;
}

unsigned char* LoadImageData(unsigned long& actualSize)
{
	const string file = "C:\\Users\\Pawel\\Desktop\\JA-proj\\JA-proj\\Debug\\test.bmp";
	fstream fh;
	fh.open(file, ios::in | ios::ate | ios::binary);
	if (!fh.is_open())
		throw "cannot open file";
	long beg, end;
	ifstream::pos_type size;
	size = fh.tellg();
	fh.seekg(0, ios::beg);
	
	
	fh.read(reinterpret_cast<char*>(&header), sizeof(header));
	cout<<header.bfSize << endl;
	cout.flush();
	
	fh.read(reinterpret_cast<char*>(&info), sizeof(info));
	assert(sizeof(info) + sizeof(header) == 54);
	fh.seekg(0, ios::beg);
	fh.seekg(54);
	actualSize = size-(ifstream::pos_type)54;
	unsigned char* data = new unsigned char[actualSize];

	fh.read((char*)data, actualSize);
	fh.close();
	return data;
}

struct arguments
{
	unsigned char* data;
	unsigned long size;
	float gamma;
};

DWORD WINAPI CppWrapper(LPVOID arg)
{
	arguments* args = (arguments*)arg;
	fun(args->data, args->size, args->gamma);
	return 0;
}

DWORD WINAPI AsmWrapper(LPVOID arg)
{
	arguments* args = (arguments*)arg;
	fun2(args->data, args->size, args->gamma);
	return 0;
}

typedef DWORD (WINAPI * wrapper)(LPVOID);

void RunInMultipleThreads(wrapper funct, unsigned char* data, unsigned long size, float gamma, int thread_no)
{
	uint64 timems;
	time_t clock_t;
	
	cout << "Measure time in three ways (does not include thread creation)..." << endl;
	int chunk = size / thread_no;
	arguments args;
	args.size = 0;
	args.data = data;
	HANDLE* threads = new HANDLE[thread_no];
	for (int i = 0; i < /*thread_no -*/ thread_no; i++)
	{
		if (args.size + chunk < size)
		{
			args.size = chunk;
			if (i > 0) args.data += chunk;
		}
		else
		{
			args.size = size - (i * chunk);
			args.data += chunk;
		}
		
		args.gamma = gamma;
		threads[i] = CreateThread(NULL, 0, funct, &args, CREATE_SUSPENDED, NULL);
	}
	/*if (chunk * thread_no == size)
	{
		args.data += chunk;
		threads[thread_no - 1] = CreateThread(NULL, 0, funct, &args, CREATE_SUSPENDED, NULL);
	}
	else if (chunk * thread_no < size)
	{
		args.data += size - (chunk * thread_no);
		threads[thread_no - 1] = CreateThread(NULL, 0, funct, &args, CREATE_SUSPENDED, NULL);
	}
	else if (size < chunk * thread_no )
	{
		args.data += chunk - (thread_no*chunk - (thread_no-1)*chunk);
		threads[thread_no - 1] = CreateThread(NULL, 0, funct, &args, CREATE_SUSPENDED, NULL);
	}*/

	

	timems = GetTimeMs64();
	clock_t= clock();
	DWORD time = GetTickCount();
	/*
		Parrallel execution!
	*/

	for (int i = 0; i < thread_no; i++)
	{
		ResumeThread(threads[i]);
	}
	//funct((LPVOID)&args);
	int t = WaitForMultipleObjects(thread_no, threads, TRUE, INFINITE);

	time = GetTickCount() - time;
	clock_t = clock() - clock_t;
	timems = GetTimeMs64() - timems;

	//cout << "Thread amount: " << thread_no << endl;
	cout << "GetTimeMs64: " << timems << "\tsec's:" << double(timems) / CLOCKS_PER_SEC << endl;
	cout << "Clock: " << clock_t << endl;
	cout << "GetTickCount: " << time << endl;
	delete[] threads;
}

int CalcThreadAmount(unsigned long size)
{
	//lets asssume that sensible minimal amount of pixels to correct is 1024
	/*double ratio = size / (double)chunk;
	int integral = (int)ratio;
	
	if (ratio - integral > 0)
	{
		integral++;
	}*/

	return 8;
}



int main()
{
	int init = PrepareLibs();
	if (init != 0)
		return init;
	unsigned long actualSize;
	unsigned char* data = LoadImageData(actualSize);
	unsigned char* dataAsm = new unsigned char[actualSize];
	memcpy((void*)dataAsm,(void*)data, actualSize);

	fstream fh;
	fh.open("C:\\Users\\Pawel\\Desktop\\JA-proj\\JA-proj\\Debug\\dupa.bmp", ios::out | ios::ate | ios::binary);

	float gamma = 2.0f;
	cout << "Cpp" << endl;
	RunInMultipleThreads(CppWrapper, data, actualSize, gamma, CalcThreadAmount(actualSize));

	fh.write((char*)&header, sizeof(header));
	fh.write((char*)&info, sizeof(info));
	fh.write((char*)data,  actualSize);
	fh.close();

	cout << "Asm" << endl;
	RunInMultipleThreads(AsmWrapper, dataAsm, actualSize, gamma, CalcThreadAmount(actualSize));
	

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

//321 x 320