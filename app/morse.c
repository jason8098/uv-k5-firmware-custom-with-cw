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
uint8_t morse_wpm_effective = MORSE_EFF_WPM_DEFAULT;
uint16_t morse_stop_interval_ms = MORSE_STOP_INTERVAL_DEFAULT_MS;
uint16_t morse_beep_ms = MORSE_BEEP_INTERVAL_DEFAULT_MS;

#define MORSE_PARIS_WORD_UNITS    50u
#define MORSE_PARIS_ELEMENT_UNITS 31u
#define MORSE_PARIS_GAP_UNITS     19u

static uint16_t MORSE_GetDotMs(void)
{
    uint8_t wpm = morse_wpm;

    if (wpm < MORSE_WPM_MIN || wpm > MORSE_WPM_MAX)
        wpm = MORSE_WPM_DEFAULT;
    if (wpm == 0)
        wpm = MORSE_WPM_DEFAULT;

    return (uint16_t)(1200u / wpm);
}

static uint16_t MORSE_GetGapUnitMs(void)
{
    const uint16_t dot_ms = MORSE_GetDotMs();
    uint8_t char_wpm = morse_wpm;
    uint8_t eff_wpm = morse_wpm_effective;

    if (char_wpm < MORSE_WPM_MIN || char_wpm > MORSE_WPM_MAX)
        char_wpm = MORSE_WPM_DEFAULT;
    if (eff_wpm < MORSE_EFF_WPM_MIN || eff_wpm > MORSE_EFF_WPM_MAX)
        eff_wpm = MORSE_EFF_WPM_DEFAULT;
    if (eff_wpm >= char_wpm || eff_wpm == 0)
        return dot_ms;

    {
        const uint32_t total_word_ms = ((1200u * MORSE_PARIS_WORD_UNITS) + (eff_wpm / 2u)) / eff_wpm;
        const uint32_t element_ms = (uint32_t)dot_ms * MORSE_PARIS_ELEMENT_UNITS;
        if (total_word_ms <= element_ms)
            return dot_ms;

        {
            const uint32_t gap_total_ms = total_word_ms - element_ms;
            uint32_t gap_unit_ms = (gap_total_ms + (MORSE_PARIS_GAP_UNITS / 2u)) / MORSE_PARIS_GAP_UNITS;
            if (gap_unit_ms < dot_ms)
                gap_unit_ms = dot_ms;
            if (gap_unit_ms > 0xFFFFu)
                gap_unit_ms = 0xFFFFu;
            return (uint16_t)gap_unit_ms;
        }
    }
}

void morseDelay(uint16_t tms){
        uint16_t last_seconds = 0xFFFFu;
        const bool show_countdown =
            (tms >= 1000u) &&
            ((txstatus == 2 && tms == morse_beep_ms) ||
             (txstatus == 3 && tms == morse_stop_interval_ms));

        if (tms == 0) {
            gCustomCountdown_10ms = 0;
            gCustomTimeoutReached = true;
            return;
        }

        gCustomCountdown_10ms     = (tms + 9u) / 10u;
        gCustomTimeoutReached = false;
        if (show_countdown) {
            last_seconds = (gCustomCountdown_10ms + 99u) / 100u;
            UI_DisplayMORSE();
        }
        while (!gCustomTimeoutReached) {
            if (KEYBOARD_Poll() == KEY_EXIT) {
                txstatus=0;
                txen=false;
                isHalted=1;
                UI_DisplayMORSE();
                return;
            }
            if(txstatus==0){
                return;
            }
            if (show_countdown) {
                const uint16_t seconds_left = (gCustomCountdown_10ms + 99u) / 100u;
                if (seconds_left != last_seconds) {
                    last_seconds = seconds_left;
                    UI_DisplayMORSE();
                }
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
    const uint16_t dash_ms = (uint16_t)(dot_ms * 3u);
    const uint16_t element_gap_ms = dot_ms;

    while (*morse) {
        if (txstatus == 0)
            break;

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
        } else {
            morse++;
            continue;
        }

        BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, false);
        if (txstatus == 0)
            break;

        morse++;
        if (*morse)
            morseDelay(element_gap_ms);
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
        case '/': return "-..-."; // Slash (/)
        case '-': return "-....-"; // Dash (-)
        default: return ""; // Ignore unknown characters
    }
}

// Function to transmit Morse code from a text string
void TransmitMorse(const char *text) {
        const uint16_t gap_unit_ms = MORSE_GetGapUnitMs();
        const uint16_t letter_gap_ms = (uint16_t)(gap_unit_ms * 3u);
        const uint16_t word_gap_ms = (uint16_t)(gap_unit_ms * 7u);
        
        BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, true);
        BK4819_TransmitTone(false, 600); 
        
        txstatus=2;
        UI_DisplayMORSE();
        morseDelay(morse_beep_ms);
        BK4819_EnterTxMute();
        BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, false);
        if (txstatus == 0) {
            UI_DisplayMORSE();
            return;
        }
        morseDelay(letter_gap_ms);
        if (txstatus == 0) {
            UI_DisplayMORSE();
            return;
        }
        txstatus=1;
        UI_DisplayMORSE();
        while (*text) {
            if (txstatus == 0)
                break;

            if (*text == ' ') {
                morseDelay(word_gap_ms);
                text++;
                continue;
            }

            const char *morse = MorseCode(*text);
            if (morse[0] != '\0') {
                PlayMorseTone(morse);
                if (txstatus == 0)
                    break;
            }

            if (text[1] != '\0' && text[1] != ' ')
                morseDelay(letter_gap_ms);
            text++;
        }
        
        if (txstatus != 0)
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


