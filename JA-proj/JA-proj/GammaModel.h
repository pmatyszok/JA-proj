#ifndef GAMMA_MODEL
#define GAMMA_MODEL

#include <string>
#include <Windows.h>

enum Implementation
{ 
	Cpp,
	Asm
};

struct results
{
	results() : ticks(0), gamma(0), threads(0){}
	bool operator == (const results& rv)
	{
		return rv.ticks == ticks && rv.gamma == gamma && rv.threads == threads;
	}
	bool operator != (const results& rv)
	{
		return !(this->operator==(rv));
	}
	int ticks;
	float gamma;
	int threads;
};

class GammaModel;

struct arguments
	{
		void operator= (const arguments& rv)
		{
			if (this == &rv)
				return;
			thisptr = rv.thisptr;
			data = rv.data;
			size = rv.size;
			gamma = rv.gamma;
		}
		GammaModel* thisptr;
		unsigned char* data;
		unsigned long size;
		float gamma;
	};

typedef void __declspec(dllimport) (*__cdecl gammaF)(unsigned char*, int, float);	
class GammaModel
{


typedef DWORD (WINAPI wrapper)(LPVOID);

private:
	float gamma;
	int threads;
	Implementation impl;
	std::string basePath, previouslySavedPath;
	unsigned char* data;
	unsigned char* dataCpp;
	unsigned char* dataAsm;
	unsigned long actualSize;
	BITMAPINFOHEADER info;
	BITMAPFILEHEADER header;
	HMODULE lib, asmLib;
	gammaF fun, fun2;

	results resAsm, resCpp;

	

	static DWORD WINAPI CppWrapper(LPVOID arg)
	{
		arguments* args = (arguments*)arg;
		args->thisptr->fun(args->data, args->size, args->gamma);
		return 0;
	}

	static DWORD WINAPI AsmWrapper(LPVOID arg)
	{
		arguments* args = (arguments*)arg;
		args->thisptr->fun2(args->data, args->size, args->gamma);
		return 0;
	}

	int prepare_libs();
	results run_in_multiple_threads(wrapper funct, unsigned char* data, unsigned long size, float gamma, int thread_no);
	bool save_image(unsigned char* bytes);
public:
	GammaModel()
	{
		data = dataAsm = dataCpp = nullptr;
	}

	void set_gamma(float _gamma) { gamma = _gamma;}
	void set_threads(float _threads) { threads = _threads;}
	void set_implementation(Implementation _impl) { impl = _impl;}
	void set_basic_path(std::string _path) { basePath = _path; }
	void load_image(std::string path);
	std::string get_previously_saved_path() {return previouslySavedPath;}

	int get_optimal_thread_amount();
	Implementation get_implementation(){return impl;}
	int init();
	void run_correction();

	results get_results(Implementation);
	
	virtual ~GammaModel()
	{
		delete data;
		data = nullptr;
		if (lib != NULL)
			FreeLibrary(lib);
		if (asmLib != NULL)
			FreeLibrary(asmLib);
		delete[] dataCpp;
		dataCpp = nullptr;
		delete[] dataAsm;
		dataAsm = nullptr;
	}
};

#endif