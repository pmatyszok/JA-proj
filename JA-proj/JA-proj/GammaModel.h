#ifndef GAMMA_MODEL
#define GAMMA_MODEL

#include <string>
#include <Windows.h>

//Enables to choose language which will execute algorithm.
enum Implementation
{ 
	Cpp,
	Asm
};


//helper struct to store and exchange execution results
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

//Helper structure used to sent arguments to executive threads
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

//Main algorithm wrapping class
class GammaModel
{

//typedef for winapi CreateThread function
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

	
	//Wraps cpp implementation when executed in thread
	static DWORD WINAPI CppWrapper(LPVOID arg)
	{
		arguments* args = (arguments*)arg;
		args->thisptr->fun(args->data, args->size, args->gamma);
		return 0;
	}

	//Wraps asm implementation when executed in thread
	static DWORD WINAPI AsmWrapper(LPVOID arg)
	{
		arguments* args = (arguments*)arg;
		args->thisptr->fun2(args->data, args->size, args->gamma);
		return 0;
	}

	//initialization - loads libraries and function pointers
	int prepare_libs();
	//main executibe function - creates threads and execute operation
	results run_in_multiple_threads(wrapper funct, unsigned char* data, unsigned long size, float gamma, int thread_no);
	//saves image to file
	bool save_image(unsigned char* bytes);
public:
	
	GammaModel()
	{
		data = dataAsm = dataCpp = nullptr;
	}

	//setters
	void set_gamma(float _gamma) { gamma = _gamma;}
	void set_threads(float _threads) { threads = _threads;}
	void set_implementation(Implementation _impl) { impl = _impl;}
	void set_basic_path(std::string _path) { basePath = _path; }
	std::string get_previously_saved_path() {return previouslySavedPath;}
	Implementation get_implementation(){return impl;}
	results get_results(Implementation);
	//loads image from path
	void load_image(std::string path);
	//returns thread number related to CPU cores
	int get_optimal_thread_amount();
	//public initialization, must be called
	int init();
	//executes algorithm
	void run_correction();

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