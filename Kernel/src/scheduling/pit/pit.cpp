#include "pit.h"

namespace PIT{
    double TimeSinceBoot = 0;

    uint16_t Divisor = uint16_Limit;

    void Sleepd(double seconds){
        double startTime = TimeSinceBoot;
        while(TimeSinceBoot < startTime + seconds){
            asm("hlt");
        }
    }

    void Sleep(uint64_t milliSeconds){
        Sleepd((double)milliSeconds / 1000);
    }

    void SetDivisor(uint16_t divisor){
        if(divisor < 100) divisor = 100;
        Divisor = divisor;
        IoWrite8(0x40, (uint8_t)(divisor & 0x00ff));
        io_wait();
        IoWrite8(0x40, (uint8_t)(divisor & 0xff00) >> 8);
    }

    uint64_t GetFrequency(){
        return BaseFrequency / Divisor;
    }

    void SetFrequency(uint64_t frequency){
        SetDivisor(BaseFrequency / frequency);
    }

    void Tick(){  
        //TimeSinceBoot += 1 / (long double)GetFrequency();
        TimeSinceBoot += 1 / (double)GetFrequency();
    }
}