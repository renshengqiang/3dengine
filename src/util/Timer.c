#include "Timer.h"
#include <time.h>
#include <stdio.h>

long GetElapsedTime(void)
{
	static struct timespec timePre,timeNow;
	static char isNotFirstCall = 0;
	float timeElapsed;
	
	if(clock_gettime(CLOCK_MONOTONIC, &timeNow)==-1){
		fprintf(stderr, "GetElapsedTime : Error Get Monotonic Time\n");
		return 0;
	}
	if(isNotFirstCall){	
		timeElapsed = (timeNow.tv_sec - timePre.tv_sec)*1000 + (timeNow.tv_nsec - timePre.tv_nsec)/1000000;
	}
	else{
		isNotFirstCall = 1;		
		timeElapsed = 0;
	}
	timePre = timeNow;
	return timeElapsed;
}