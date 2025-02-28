#ifndef _SRV_TIME_H
#define _SRV_TIME_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>

#define FEMOSECOND_IN_MILLISECOND     1000000000000 // femosecond 10E15 to millisecond 10E3

#if defined(__cplusplus)
extern "C" {
#endif

typedef KResult (*TimeCallbackHandler)(KResult Status, struct srv_time_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

typedef struct{
    uint64_t Year;
    uint64_t Month;
    uint64_t Day;
    uint64_t WeekDay;
    uint64_t Hour;
    uint64_t Minute;
    uint64_t Second;
}time_t;

struct srv_time_callback_t{
    thread_t Self;
    uint64_t Data;
    size64_t Size;
    bool IsAwait;
    KResult Status;
    TimeCallbackHandler Handler;
};

void Srv_Time_Initialize();

void Srv_Time_Callback(KResult Status, struct srv_time_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct srv_time_callback_t* Srv_Time_SetTimePointerKey(time_t** Time, bool IsAwait);
struct srv_time_callback_t* Srv_Time_SetTickPointerKey(uint64_t* TimePointer, uint64_t TickPeriod, bool IsAwait);


uint64_t GetYear();
uint64_t GetMonth();
uint64_t GetDay();
uint64_t GetWeekDay();
uint64_t GetHour();
uint64_t GetMinute();
uint64_t GetSecond();
KResult Sleep(uint64_t duration);
KResult SleepFromTick(uint64_t* tick, uint64_t duration);
KResult GetActualTick(uint64_t* tick);
KResult GetTimeFromTick(uint64_t* time, uint64_t tick);
KResult GetFemosecondTimeFromTick(uint64_t* time, uint64_t tick);
KResult GetTickFromTime(uint64_t* tick, uint64_t time);
KResult GetTime(uint64_t* time);
KResult CompareTime(uint64_t* compare, uint64_t time0, uint64_t time1);

uint64_t GetPosixTime();

#if defined(__cplusplus)
}
#endif

#endif