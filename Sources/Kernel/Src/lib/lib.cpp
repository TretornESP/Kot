#include <lib/lib.h>


bool ReadBit(uint8_t byte, int position){
    return (byte >> position) & 0x1;
}

uint8_t WriteBit(uint8_t byte, int position, bool value){
    if(value){
        byte |= 1 << position;
    }else{
        byte &= ~(1 << position);
    }

    return byte;    
}

uint64_t DivideRoundUp(uint64_t value, uint64_t divider){
    uint64_t returnValue = value / divider;
    if((value % divider) != 0){
        returnValue++;
    }
    return returnValue;
}