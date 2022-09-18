#include <mouse/mouse.h>

uint8_t MouseMaxCycles = 0;
uint8_t MousePacket[4];
uint8_t MouseCycle = 0;

PS2Port_t* MousePS2Port;
uint8_t MouseType;
arguments_t* MouseEventParameters;
thread_t Mousethread = NULL;

event_t MouseRelativeEvent; // We don't use absolute for ps2

KResult MouseInitalize(){
    for(int i = 0; i < PS2_PORT_NUMBER; i++){
        if(PS2Ports[i].IsPresent){
            if(PS2Ports[i].Type == PS2_TYPE_MOUSE
            || PS2Ports[i].Type == PS2_TYPE_MOUSE_SCROLL
            || PS2Ports[i].Type == PS2_TYPE_MOUSE_5BUTTONS){
                Printlog("[PS2] Mouse device found");
                
                MouseEventParameters = (arguments_t*)malloc(sizeof(arguments_t));


                MousePS2Port = &PS2Ports[i];
                IRQRedirectionsArray[MousePS2Port->PortNumber] = MouseHandler;
                
                MouseRelativeEvent = GetMouseRelativeEvent();

                MouseType = mousePortTypePS2;

                MouseMaxCycles = 3;

                if(PS2Ports[i].Type == PS2_TYPE_MOUSE_SCROLL){
                    EnableMouseScroll(MousePS2Port);
                    MouseMaxCycles = 4;
                }
                if(PS2Ports[i].Type == PS2_TYPE_MOUSE_5BUTTONS){
                    EnableMouse5Buttons(MousePS2Port);
                    MouseMaxCycles = 4;
                }

                MouseCycle = MouseMaxCycles - 2; // ignore the first packet and remove 1 because we add one before in the handler
                
                // clear mouse packet data
                memset(MousePacket, NULL, sizeof(uint8_t) * 4);

                Srv_System_BindIRQLine(MousePS2Port->IRQ, InterruptthreadHandler, false, true);

                break; // Enable only one mouse
            }            
        }
    }

    return KSUCCESS;
} 

void MouseSetRate(uint8_t rate, PS2Port_t* Self){
    Self->PS2SendDataPort(0xF3);
    PS2WaitOutput();
    PS2GetData();
    Self->PS2SendDataPort(rate);
    PS2WaitOutput();
    PS2GetData();
}

uint8_t MouseGetID(PS2Port_t* Self){
    Self->PS2SendDataPort(0xF2);
    PS2WaitOutput();
    return PS2GetData();
}

void EnableMouseScroll(PS2Port_t* Self){
    MouseSetRate(200, Self);
    MouseSetRate(100, Self);
    MouseSetRate(80, Self);
    MouseGetID(Self);
}

void EnableMouse5Buttons(PS2Port_t* Self){
    EnableMouseScroll(Self);
    MouseSetRate(200, Self);
    MouseSetRate(200, Self);
    MouseSetRate(80, Self);
    MouseGetID(Self);
}

void MouseParser(uint8_t data){
    if(MouseCycle == PacketGlobalInfo){
        if(((data & (1 << 3)) >> 3) & 1 == 0){
            return;
        }
    }

    MousePacket[MouseCycle] = data;
    MouseCycle++;

    if(MouseCycle == MouseMaxCycles){
        MouseCycle = 0;

        if(MousePacket[PacketGlobalInfo] & (1 << 6)){
            MousePacket[PacketXPosition] = 0xff;
        }

        if(MousePacket[PacketGlobalInfo] & (1 << 7)){
            MousePacket[PacketYPosition] = 0xff;
        }   

        bool leftClick    = ((MousePacket[PacketGlobalInfo] & (1 << 0)) >> 0) & 1;
        bool rightClick   = ((MousePacket[PacketGlobalInfo] & (1 << 1)) >> 1) & 1;
        bool middleClick  = ((MousePacket[PacketGlobalInfo] & (1 << 2)) >> 2) & 1;
        bool button4Click = ((MousePacket[ExtendedInfos] & (1 << 4)) >> 4) & 1;
        bool button5Click = ((MousePacket[ExtendedInfos] & (1 << 5)) >> 5) & 1;

        /* Relative position */
        bool IsXNegative = ((MousePacket[PacketGlobalInfo] & (1 << 4)) >> 4) & 1;
        bool IsYNegative = ((MousePacket[PacketGlobalInfo] & (1 << 5)) >> 5) & 1;
        MouseEventParameters->arg[0] = (int64_t)((uint64_t)MousePacket[PacketXPosition] | ((uint64_t)IsXNegative << 63)); // add signed bit
        MouseEventParameters->arg[1] = (int64_t)((uint64_t)MousePacket[PacketYPosition] | ((uint64_t)IsYNegative << 63)); // add signed bit

        bool IsZNegative = ((MousePacket[ExtendedInfos] & (1 << 3)) >> 3) & 1;
        MouseEventParameters->arg[2] = (int64_t)(((uint64_t)MousePacket[ExtendedInfos] & 0b111) | ((uint64_t)IsZNegative << 63)); // add signed bit

        /* Buttons status */
        MouseEventParameters->arg[3] = NULL;
        MouseEventParameters->arg[3] |= leftClick << 0;
        MouseEventParameters->arg[3] |= rightClick << 1;
        MouseEventParameters->arg[3] |= middleClick << 2;
        MouseEventParameters->arg[3] |= button4Click << 3;
        MouseEventParameters->arg[3] |= button5Click << 4;

        Sys_kevent_trigger(MouseRelativeEvent, MouseEventParameters);
    }
}

KResult MouseHandler(uint8_t data){
    MouseParser(data);
    
    return KSUCCESS;
}