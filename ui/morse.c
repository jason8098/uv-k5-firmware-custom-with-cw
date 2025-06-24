#include <stdbool.h>
#include <string.h>
#include "app/morse.h"
#include "dcs.h"
#include "driver/st7565.h"
#include "external/printf/printf.h"
#include "misc.h"
#include "ui/helper.h"
#include "ui/morse.h"
#include "radio.h"
#include "ui/ui.h"
#include "driver/bk4819.h"
void UI_DisplayMORSE(void)
{

    
    char  String[32] = {0};
    char *pPrintStr = String;

    UI_DisplayClear();
    UI_PrintString("FOX TX v0.1", 0, 0, 0, 8);
    UI_PrintStringSmallBold("CWID: DE 4S7RS", 0, 0, 2);

    if(txstatus==1){
        
    UI_PrintStringSmallBold("STATUS: TX CWID..", 0, 0, 3);
    }else if(txstatus==2){
        
    UI_PrintStringSmallBold("STATUS: TX TONE..", 0, 0, 3);
    }else if(txstatus==3){
        
    UI_PrintStringSmallBold("STATUS: WAITING", 0, 0, 3);
    }
    else{
    UI_PrintStringSmallBold("STATUS: QRV", 0, 0, 3);

    }
    uint32_t frequency = gTxVfo->pTX->Frequency;
    uint32_t mhz = frequency / 100000;
    uint32_t khz = (frequency % 100000) / 10;

    snprintf_(String, sizeof(String), "FREQ: %u.%uMHz", mhz, khz);
    pPrintStr = String;
    UI_PrintStringSmallBold(pPrintStr, 0, 0, 4);


    UI_PrintStringSmallNormal("Menu - Start", 0, 0, 5);
    UI_PrintStringSmallNormal("Turn off to Stop", 0, 0, 6);
    ST7565_BlitFullScreen();

    
}