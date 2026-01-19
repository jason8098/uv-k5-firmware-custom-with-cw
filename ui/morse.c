#include <stdbool.h>
#include <string.h>
#include "app/morse.h"
#include "driver/st7565.h"
#include "external/printf/printf.h"
#include "misc.h"
#include "ui/helper.h"
#include "ui/morse.h"
#include "app/morse.h"
#include "radio.h"
#include "ui/ui.h"
#include "driver/bk4819.h"

void UI_DisplayMORSE(void)
{

    
    char  String[64] = {0};
    char *pPrintStr = String;

    UI_DisplayClear();

    snprintf_(String, sizeof(String), "FOX TX v%s", morseVersion);
    pPrintStr = String;
    UI_PrintStringSmallBold(pPrintStr, 0, 0, 0);
    snprintf_(String, sizeof(String), "CW: %s", cwid_m);
    pPrintStr = String;
    UI_PrintStringSmallBold(String, 0, 0, 1);

    if (morse_wpm_effective != morse_wpm)
        snprintf_(String, sizeof(String), "WPM: %u/%u", morse_wpm, morse_wpm_effective);
    else
        snprintf_(String, sizeof(String), "WPM: %u", morse_wpm);
    UI_PrintStringSmallBold(String, 0, 0, 2);

    {
        uint16_t seconds_left = 0;
        if (txstatus == 2 || txstatus == 3)
            seconds_left = (gCustomCountdown_10ms + 99u) / 100u;

        if(txstatus==1){
            UI_PrintStringSmallBold("STATUS: TX CWID", 0, 0, 3);
        }else if(txstatus==2){
            if (seconds_left > 0)
                snprintf_(String, sizeof(String), "STATUS: TONE %us", seconds_left);
            else
                snprintf_(String, sizeof(String), "STATUS: TONE");
            UI_PrintStringSmallBold(String, 0, 0, 3);
        }else if(txstatus==3){
            if (seconds_left > 0)
                snprintf_(String, sizeof(String), "STATUS: WAIT %us", seconds_left);
            else
                snprintf_(String, sizeof(String), "STATUS: WAIT");
            UI_PrintStringSmallBold(String, 0, 0, 3);
        }
        else{
            UI_PrintStringSmallBold("STATUS: QRV", 0, 0, 3);
        }
    }
    uint32_t frequency = gTxVfo->pTX->Frequency;
    uint32_t mhz = frequency / 100000;
    uint32_t khz = (frequency % 100000);

    snprintf_(String, sizeof(String), "FREQ: %u.%uMHz", mhz, khz);
    pPrintStr = String;
    UI_PrintStringSmallBold(pPrintStr, 0, 0, 4);
    

    char* pw;
    int power=gTxVfo->OUTPUT_POWER;
    if(power==1){
        pw="20mW";
    }
    else if(power==2){
        pw="125mW";
    }
    else if(power==3){
        pw="250mW";
    }
    else if(power==4){
        pw="500mW";
    }
    else if(power==5){
        pw="1W";
    }
    else if(power==6){
        pw="2W";
    }
    else if(power==7){
        pw="5W";
    }
    snprintf_(String, sizeof(String), "PW: %s", pw);
    pPrintStr = String;
    UI_PrintStringSmallBold(pPrintStr, 0, 0, 5);
    ST7565_BlitFullScreen();

    
}
