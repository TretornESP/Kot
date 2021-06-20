#include "smp.h" 

extern uint64_t IdleTaskStart;
extern uint64_t IdleTaskEnd;

extern "C" void TrampolineMain(int CoreID){
    gdtInitCores(CoreID);
    asm ("lidt %0" : : "m" (idtr));
    
    APIC::localApicEOI();
    APIC::StartLapicTimer();
    printf("I am %u\n", CoreID);
    globalGraphics->Update();
    
    asm("sti");

    //void* NewLocationIdleTask = globalAllocator.RequestPage();
    //memcpy(NewLocationIdleTask, (void*)IdleTaskStart, IdleTaskEnd - IdleTaskStart);
    globalTaskManager.EnabledScheduler(CoreID, (void*)task2);

    while(true){
        asm("hlt");
    }
}
