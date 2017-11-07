
double absoluteTime(void);

#ifdef _WIN32

#include <windows.h>

double absoluteTime() {
    //return timeGetTime() / (double) 1000;
	// hi-res timer courtesy gafferongames.com;
	static __int64 start = 0;
    static __int64 frequency = 0;

    if (start==0)
    {
        QueryPerformanceCounter((LARGE_INTEGER*)&start);
        QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
        return 0.0f;
    }

    __int64 counter = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&counter);
    return (float) ((counter - start) / double(frequency));
};

#elif __APPLE__

#import <mach/mach_time.h>

double absoluteTime() {
    static double sysTimebaseMult = -1;
    if (sysTimebaseMult == -1) {
        mach_timebase_info_data_t info;
        kern_return_t err = mach_timebase_info(&info);
        if (err == 0) {
            sysTimebaseMult = 1e-9 * (double) info.numer / (double) info.denom;
        }
    }
    return mach_absolute_time() * sysTimebaseMult;
};

#elif __linux__

#include <sys/time.h>
#include <unistd.h>

double absoluteTime(){
	struct timeval time;
	gettimeofday(&time, NULL);
	
	return (time.tv_sec*1000.0 + time.tv_usec/1000.0)/(double)1000.0;
}

#endif
