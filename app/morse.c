#include "morse.h"

#include "driver/bk4819.h"

#include "driver/system.h"

#include "radio.h"
#include <stdbool.h>
#include <string.h>
#include "driver/st7565.h"
#include "external/printf/printf.h"

#include "misc.h"
#include "ui/helper.h"
#include "ui/ui.h"
#include "ui/inputbox.h"
#include "ui/morse.h"
#include "app/generic.h"
#include "settings.h"
#include <pthread.h>

int txstatus =0;
char* cwid_m = "VVV"; //Edit this Message

void PlayMorseTone(const char *morse) {
    while (*morse) {
        if (*morse == '.') {
            BK4819_TransmitTone(false, 600); /
            BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, true);
            SYSTEM_DelayMs(80);
            BK4819_EnterTxMute();
        } else if (*morse == '-') {
            BK4819_TransmitTone(false, 600); 
            BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, true);
            SYSTEM_DelayMs(290);
            BK4819_EnterTxMute();
        } else if (*morse == ' ') {
            SYSTEM_DelayMs(790); 
            
            BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, false);
        } else if (*morse == '/') {
            SYSTEM_DelayMs(20);
        }
        SYSTEM_DelayMs(50); 
        BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, false);
        morse++;
    }
}

// Function to get Morse code for a character
const char* MorseCode(char c) {
    switch (toupper(c)) {
        case 'A': return ".-";
        case 'B': return "-...";
        case 'C': return "-.-.";
        case 'D': return "-..";
        case 'E': return ".";
        case 'F': return "..-.";
        case 'G': return "--.";
        case 'H': return "....";
        case 'I': return "..";
        case 'J': return ".---";
        case 'K': return "-.-";
        case 'L': return ".-..";
        case 'M': return "--";
        case 'N': return "-.";
        case 'O': return "---";
        case 'P': return ".--.";
        case 'Q': return "--.-";
        case 'R': return ".-.";
        case 'S': return "...";
        case 'T': return "-";
        case 'U': return "..-";
        case 'V': return "...-";
        case 'W': return ".--";
        case 'X': return "-..-";
        case 'Y': return "-.--";
        case 'Z': return "--..";
        case '1': return ".----";
        case '2': return "..---";
        case '3': return "...--";
        case '4': return "....-";
        case '5': return ".....";
        case '6': return "-....";
        case '7': return "--...";
        case '8': return "---..";
        case '9': return "----.";
        case '0': return "-----";
        case ' ': return "/";   // Space between words
        case '/': return "-..-."; // Slash (/)
        case '-': return "-....-"; // Dash (-)
        default: return ""; // Ignore unknown characters
    }
}

// Function to transmit Morse code from a text string
void TransmitMorse(const char *text) {
        char buffer[10]; // Temporary buffer for Morse characters
        
        BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, true);
        BK4819_TransmitTone(false, 600); 
        
        txstatus=2;
        UI_DisplayMORSE();
        SYSTEM_DelayMs(7500); 
        BK4819_EnterTxMute();
        BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, false);
        SYSTEM_DelayMs(280); 
        txstatus=1;
        UI_DisplayMORSE();
        while (*text) {
            const char *morse = MorseCode(*text);
            strcpy(buffer, morse);
            PlayMorseTone(buffer);
            SYSTEM_DelayMs(280); // Gap between letters
            text++;
        }
        
       txstatus =3;
       
        UI_DisplayMORSE();
        BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, false);
        
}


static void MORSE_Key_MENU(bool bKeyPressed, bool bKeyHeld)
{
    if (!bKeyHeld && bKeyPressed)
    {
        
        UI_DisplayMORSE();
        RADIO_SetTxParameters();
        while(1){

            TransmitMorse(cwid_m);
            
            BK4819_Disable();
            SYSTEM_DelayMs(45000); // Gap 

        }
    return;

    }
}
static void MORSE_Key_EXIT(bool bKeyPressed, bool bKeyHeld)
{
    if (!bKeyHeld && bKeyPressed) {
        gRequestDisplayScreen = DISPLAY_MAIN;
        txstatus = 0;
        BK4819_Disable();
    }
}
static void MORSE_Key_UP_DOWN(bool bKeyPressed, bool pKeyHeld, int8_t Direction)
{
    if (!pKeyHeld && bKeyPressed && Direction==1) {
        gTxVfo->OUTPUT_POWER++;
    } else if(!pKeyHeld && bKeyPressed && Direction==-1){
        gTxVfo->OUTPUT_POWER--;
    }

    if(gTxVfo->OUTPUT_POWER==8){
        
        gTxVfo->OUTPUT_POWER=1;
    }
    else if(gTxVfo->OUTPUT_POWER==0){
        
        gTxVfo->OUTPUT_POWER=7;
    }
    UI_DisplayMORSE();
    SYSTEM_DelayMs(10);
}
void MORSE_ProcessKeys(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld)
{
    switch (Key) {
        case KEY_MENU:
            MORSE_Key_MENU(bKeyPressed, bKeyHeld);
            break;
        case KEY_EXIT:
            MORSE_Key_EXIT(bKeyPressed, bKeyHeld);
            break;
        case KEY_UP:
            MORSE_Key_UP_DOWN(bKeyPressed, bKeyHeld,  1);
            break;
        case KEY_DOWN:
            MORSE_Key_UP_DOWN(bKeyPressed, bKeyHeld, -1);
            break;
        default:
            if (!bKeyHeld && bKeyPressed)
            break;
    }
}


