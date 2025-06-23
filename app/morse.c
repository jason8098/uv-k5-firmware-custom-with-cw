#include "morse.h"

#include "driver/bk4819.h"

#include "driver/system.h"

#include <stdbool.h>
#include <string.h>
#include "driver/st7565.h"
#include "external/printf/printf.h"

#include "misc.h"
#include "ui/helper.h"
#include "ui/ui.h"
#include "ui/inputbox.h"


#include "app/generic.h"

// Function to play Morse code tones
void PlayMorseTone(const char *morse) {
    while (*morse) {
        if (*morse == '.') {
            BK4819_TransmitTone(false, 600); // Short tone for dot
            BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, true);
            SYSTEM_DelayMs(100);
            BK4819_EnterTxMute();
        } else if (*morse == '-') {
            BK4819_TransmitTone(false, 600); 
            BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, true);
            SYSTEM_DelayMs(340);
            BK4819_EnterTxMute();
        } else if (*morse == ' ') {
            SYSTEM_DelayMs(840); // Space between characters
            
            BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, false);
        } else if (*morse == '/') {
            SYSTEM_DelayMs(20); // Space between words
        }
        SYSTEM_DelayMs(45); // Intra-character spacing
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

        SYSTEM_DelayMs(380); // Gap between letters
        char buffer[10]; // Temporary buffer for Morse characters
        
        while (*text) {
            const char *morse = MorseCode(*text);
            strcpy(buffer, morse);
            PlayMorseTone(buffer);
            SYSTEM_DelayMs(380); // Gap between letters
            text++;
        }
        
        BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, false);
        
}


static void MORSE_Key_DIGITS(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld)
{
    if (!bKeyHeld && bKeyPressed &&  Key)
    {

        return;
    }
}
static void MORSE_Key_MENU(bool bKeyPressed, bool bKeyHeld)
{
    if (!bKeyHeld && bKeyPressed)
    {
    return;

    }
}
static void MORSE_Key_EXIT(bool bKeyPressed, bool bKeyHeld)
{
    if (!bKeyHeld && bKeyPressed) { // short pressed
       gRequestDisplayScreen    = DISPLAY_MAIN;
    return;
    }
}
static void MORSE_Key_UP_DOWN(bool bKeyPressed, bool pKeyHeld, int8_t Direction)
{
    if (!pKeyHeld && bKeyPressed &&  Direction){

    }
    return;
}

static void MORSE_Key_STAR(bool bKeyPressed, bool bKeyHeld)
{
    if (!bKeyHeld && bKeyPressed) {
    }
    return;
}

void MORSE_ProcessKeys(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld)
{
    switch (Key) {
        case KEY_0...KEY_9:
            MORSE_Key_DIGITS(Key, bKeyPressed, bKeyHeld);
            break;
        case KEY_MENU:
            MORSE_Key_MENU(bKeyPressed, bKeyHeld);
            break;
        case KEY_UP:
            MORSE_Key_UP_DOWN(bKeyPressed, bKeyHeld,  1);
            break;
        case KEY_DOWN:
            MORSE_Key_UP_DOWN(bKeyPressed, bKeyHeld, -1);
            break;
        case KEY_EXIT:
            MORSE_Key_EXIT(bKeyPressed, bKeyHeld);
            break;
        case KEY_STAR:
            MORSE_Key_STAR(bKeyPressed, bKeyHeld);
            break;
        case KEY_PTT:
            GENERIC_Key_PTT(bKeyPressed);
            break;
        default:
            if (!bKeyHeld && bKeyPressed)
            break;
    }
}


