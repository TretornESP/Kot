#include <arch/x86-64/cpu/cpu.h>
#include <core/core.h>


namespace CPU{ 
    void cpuid(uint32_t reg, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
    {
        __asm__ volatile("cpuid"
            : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
            : "0" (reg));
    }

    void GetMSR(uint32_t index, uint32_t* low, uint32_t* high){
        asm volatile("rdmsr" : "=a"(*low), "=d"(*high) : "c"(index));
    }
    
    void SetMSR(uint32_t index, uint32_t low, uint32_t high){
        asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(index));
    }

    void InitCPU(ArchInfo_t* ArchInfo){
        ArchInfo->architecture = 808664;
        ArchInfo->IRQLineStart = IRQ_START;
        ArchInfo->IRQSize = MAX_IRQ;
        return;
    }

    void InitCore(){
        CPUContext* context = (CPUContext*)kcalloc(sizeof(CPUContext));
        GetFeatures(context->FeaturesECX, context->FeaturesEDX);
        context->ID = GetAPICID();
        ReloadGSFS();
        SetCPUGSBase((uint64_t)context);
        return;
    }

    void GetFeatures(uint32_t FeaturesECX, uint32_t FeaturesEDX){
        uint32_t eax, ebx, standardfunc;
        char* tmp[13];
        cpuid(0, &standardfunc, (uint32_t *)(tmp + 0), (uint32_t *)(tmp + 8), (uint32_t *)(tmp + 4));
        
        if(standardfunc >= 1) {
            cpuid(1, &eax, &ebx, &FeaturesECX, &FeaturesEDX);
        } 
    }

    uint8_t GetCodeRing(ContextStack* Registers){
        return (Registers->cs & 0b11);
    }
}