#ifndef _TIMER_H
#define _TIMER_H

#ifdef __cplusplus
extern "C" {
#endif
//返回两次调用该函数之间逝去的时间，以微秒数计
long GetElapsedTime(void);
#ifdef __cplusplus
}
#endif

#endif//_TIMER_H
