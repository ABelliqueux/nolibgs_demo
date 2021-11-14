#include "../common.h"

int ovl_main_hello(void)
{   
    uint16_t i = 0;
    init();
    while(1)
    {
        i++;
        #ifndef STANDALONE
        if (i == timeout){
            next_overlay = MOTHERSHIP;
            break;
        }
            FntPrint(0, "Hello world ! %d\n", i);
        #else
            FntPrint(0, "Hello standalone ! %d\n", i);
            
        #endif
        FntFlush(0);
        display();
    }
    return next_overlay;
};

