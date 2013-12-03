#include "GammaModel.h"
#include <fstream>
#include <iostream>
#include <assert.h>
#include <time.h>
using namespace std;

int GammaModel::prepare_libs()
{
	lib = LoadLibrary(TEXT("CppImpl.dll"));
	if (lib == NULL)
		return -1;
	fun = (gammaF)GetProcAddress(lib, "CorrectGamma");
	if (fun == NULL)
		return -2;
	asmLib = LoadLibrary(TEXT("AsmImpl.dll"));
	if (asmLib == NULL)
		return -3;
	fun2 = (gammaF)GetProcAddress(asmLib, "CorrectGamma");
	if (fun2 == NULL)
		return -4;
	return 0;
}

void GammaModel::load_image(string path)
{
	string file = path;
	fstream fh;
	fh.open(file, ios::in | ios::ate | ios::binary);
	if (!fh.is_open())
		throw "cannot open file";

	ifstream::pos_type size;
	size = fh.tellg();
	fh.seekg(0, ios::beg);
	
	//bmp header loading
	fh.read(reinterpret_cast<char*>(&header), sizeof(header));
	cout<<header.bfSize << endl;
	cout.flush();
	

	fh.read(reinterpret_cast<char*>(&info), sizeof(info));
	//cheks if this is correct bmp file
	assert(sizeof(info) + sizeof(header) == 54);
	fh.seekg(0, ios::beg);
	fh.seekg(54);
	//size without header
	actualSize = size-(ifstream::pos_type)54;
	data = new unsigned char[actualSize];

	fh.read((char*)data, actualSize);
	fh.close();

	dataCpp = new unsigned char[actualSize];
	dataAsm = new unsigned char[actualSize];
}

results GammaModel::run_in_multiple_threads(wrapper funct, unsigned char* data, unsigned long size, float gamma, int thread_no)
{
	time_t clock_t;
	results res;
	cout << "Measure time in three ways (does not include thread creation)..." << endl;
	int chunk = size / thread_no;
	
	HANDLE* threads = new
		HANDLE[thread_no];

	//prepare arguments for each thread
	arguments* args = new arguments[thread_no];
	fill(args, args + thread_no, arguments());
	//fill results struct
	res.threads = thread_no;
	res.gamma = gamma;

	for (int i = 0; i < thread_no; i++)
	{
		args[i].thisptr = this;
		args[i].size = 0;
		args[i].data = data;
		if (args[i].size + chunk <= size)
		{//handles image division across threads
			args[i].size = chunk;
			args[i].data += i * chunk;
		}
		else
		{
			args[i].size = size - (i * chunk);
			args[i].data += chunk;
		}
		
		args[i].gamma = gamma;
		threads[i] = CreateThread(NULL, 0, funct, &(args[i]), CREATE_SUSPENDED, NULL);
	}

	
	clock_t= clock();
	DWORD time = GetTickCount();
	/*
		Parrallel execution!
		Time measuring starts!
	*/

	for (int i = 0; i < thread_no; i++)
	{
		ResumeThread(threads[i]);
	}

	/*
		Parrallel execution ends!
		Time measuring stops!
	*/

	int t = WaitForMultipleObjects(thread_no, threads, TRUE, INFINITE);

	time = GetTickCount() - time;
	clock_t = clock() - clock_t;


	cout << "Thread amount: " << thread_no << endl;
	cout << "Clock: " << clock_t << endl;
	cout << "GetTickCount: " << time << endl;
	res.ticks = time;
	delete[] threads;
	delete[] args;
	return res;
}

int GammaModel::get_optimal_thread_amount()
{
	SYSTEM_INFO sys;
	GetSystemInfo(&sys);
	return sys.dwNumberOfProcessors;
}

int GammaModel::init()
{
	return prepare_libs();
}

bool GammaModel::save_image(unsigned char* bytes)
{
	fstream fh;
	string path = basePath.substr(0, basePath.rfind(".bmp", 0, string::npos));
	path += (string)(impl == Implementation::Asm ? "_asm" : "_cpp") + ".bmp";
	previouslySavedPath = path;
	fh.open(path, ios::out | ios::ate | ios::binary);
	fh.write((char*)&header, sizeof(header));
	fh.write((char*)&info, sizeof(info));
	fh.write((char*)bytes,  actualSize);
	previouslySavedPath = path;
	fh.close();
	return true;
}

void GammaModel::run_correction()
{
	switch(impl)
	{
	case Implementation::Asm:
		memcpy(dataAsm, data, actualSize);
		resAsm = run_in_multiple_threads(AsmWrapper, dataAsm, actualSize, gamma, threads);
		this->save_image(dataAsm);
		break;
	case Implementation::Cpp:
		memcpy(dataCpp, data, actualSize);
		resCpp = run_in_multiple_threads(CppWrapper, dataCpp, actualSize, gamma, threads);
		this->save_image(dataCpp);
		break;
	}
	
}

results GammaModel::get_results(Implementation i)
{
	results tmp;
	switch(i)
	{
	case Implementation::Asm:
		tmp = resAsm;
		resAsm = results();
		return tmp;
		break;
	case Implementation::Cpp:
		tmp = resCpp;
		resCpp = results();
		return tmp;
		break;
	}
}