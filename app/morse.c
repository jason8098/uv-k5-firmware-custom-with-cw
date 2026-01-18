#include "morse.h"

#include "driver/bk4819.h"

#include "driver/system.h"

#include "radio.h"
#include "app/app.h"
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
#include "functions.h"
#include "settings.h"
#include "driver/uart.h"

int txstatus =0;
bool txen = false;
int isHalted = 0;
char cwid_m[MORSE_CWID_MAX_LEN + 1] = MORSE_CWID_DEFAULT;

char* morseVersion = "1.1.0";
uint8_t morse_wpm = MORSE_WPM_DEFAULT;
uint16_t morse_stop_interval_ms = MORSE_STOP_INTERVAL_DEFAULT_MS;

#define MORSE_DOT_BASE_MS         80u
#define MORSE_DASH_BASE_MS        290u
#define MORSE_ELEMENT_GAP_BASE_MS 50u
#define MORSE_LETTER_GAP_BASE_MS  280u
#define MORSE_WORD_GAP_BASE_MS    790u

static uint16_t MORSE_GetDotMs(void)
{
    if (morse_wpm == 0)
        return MORSE_DOT_BASE_MS;

    return (uint16_t)(1200u / morse_wpm);
}

static uint16_t MORSE_ScaleMs(uint16_t base_ms)
{
    const uint32_t dot_ms = MORSE_GetDotMs();
    return (uint16_t)((dot_ms * base_ms + (MORSE_DOT_BASE_MS / 2u)) / MORSE_DOT_BASE_MS);
}

void morseDelay(uint16_t tms){
        gCustomCountdown_10ms     = tms/10;   
        gCustomTimeoutReached = false;
        while (!gCustomTimeoutReached) {
            if (KEYBOARD_Poll() == KEY_EXIT) {
                txstatus=0;
                txen=false;
                isHalted=1;
            }
            if(txstatus==0){
                return;
            }
        }
}

static bool MORSE_StartTx(void)
{
    RADIO_PrepareTX();
    return gCurrentFunction == FUNCTION_TRANSMIT;
}

static void MORSE_StopTx(void)
{
    if (gCurrentFunction != FUNCTION_TRANSMIT)
        return;

    if (!gFlagEndTransmission) {
        APP_EndTransmission();

        if (gEeprom.REPEATER_TAIL_TONE_ELIMINATION > 0)
            morseDelay((uint16_t)gEeprom.REPEATER_TAIL_TONE_ELIMINATION * 100);
    }

    FUNCTION_Select(FUNCTION_FOREGROUND);
    gFlagEndTransmission = false;
#ifdef ENABLE_VOX
    gVOX_NoiseDetected = false;
#endif
    RADIO_SetVfoState(VFO_STATE_NORMAL);
}

void PlayMorseTone(const char *morse) {
    const uint16_t dot_ms = MORSE_GetDotMs();
    const uint16_t dash_ms = MORSE_ScaleMs(MORSE_DASH_BASE_MS);
    const uint16_t element_gap_ms = MORSE_ScaleMs(MORSE_ELEMENT_GAP_BASE_MS);
    const uint16_t word_gap_ms = MORSE_ScaleMs(MORSE_WORD_GAP_BASE_MS);

    while (*morse) {
        if(txstatus !=0){
            if (*morse == '.') {
                BK4819_TransmitTone(false, 600); 
                BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, true);
                morseDelay(dot_ms);
                BK4819_EnterTxMute();
            } else if (*morse == '-') {
                BK4819_TransmitTone(false, 600); 
                BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, true);
                morseDelay(dash_ms);
                BK4819_EnterTxMute();
            } else if (*morse == ' ' || *morse == '/') {
                morseDelay(word_gap_ms); 
                
                BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, false);
            }
            morseDelay(element_gap_ms); 
            BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, false);
            morse++;
        }else{
             break;
        }
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
        const uint16_t letter_gap_ms = MORSE_ScaleMs(MORSE_LETTER_GAP_BASE_MS);
        
        BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, true);
        BK4819_TransmitTone(false, 600); 
        
        txstatus=2;
        UI_DisplayMORSE();
        morseDelay(7500); 
        BK4819_EnterTxMute();
        BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, false);
        morseDelay(letter_gap_ms); 
        txstatus=1;
        UI_DisplayMORSE();
        while (*text) {
            const char *morse = MorseCode(*text);
            strcpy(buffer, morse);
            PlayMorseTone(buffer);
            morseDelay(letter_gap_ms); // Gap between letters
            text++;
        }
        
       txstatus =3;
       
        UI_DisplayMORSE();
        BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, false);
        
}


static void MORSE_Key_MENU(bool bKeyPressed, bool bKeyHeld) {
    if (!bKeyHeld && bKeyPressed) {
        txen=true;
        while(txen){
            if (!MORSE_StartTx()) {
                txstatus = 0;
                UI_DisplayMORSE();
                txen = false;
                break;
            }
            TransmitMorse(cwid_m);
            MORSE_StopTx();
            morseDelay(morse_stop_interval_ms); 
        }
    }
}
static void MORSE_Key_EXIT(bool bKeyPressed, bool bKeyHeld)
{
    isHalted++;
    if (!bKeyHeld && bKeyPressed) {
        if(!txen && isHalted==1){
            gRequestDisplayScreen = DISPLAY_MAIN;
            isHalted=0;
        }
        else if(!txen && txstatus == 0 && isHalted>2){
            gRequestDisplayScreen = DISPLAY_MAIN;
            isHalted=0;
        }
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
    morseDelay(10);
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


