#include "morse.h"

#include "driver/bk4819.h"

#include "driver/system.h"

// Function to play Morse code tones
void PlayMorseTone(const char *morse) {
    while (*morse) {
        if (*morse == '.') {
            BK4819_TransmitTone(false, 600); // Short tone for dot
            SYSTEM_DelayMs(90);
            BK4819_EnterTxMute();
        } else if (*morse == '-') {
            BK4819_TransmitTone(false, 600); // Long tone for dash
            SYSTEM_DelayMs(250);
            BK4819_EnterTxMute();
        } else if (*morse == ' ') {
            SYSTEM_DelayMs(10); // Space between characters
        } else if (*morse == '/') {
            SYSTEM_DelayMs(10); // Space between words
        }
        SYSTEM_DelayMs(10); // Intra-character spacing
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

        SYSTEM_DelayMs(200); // Gap between letters
        char buffer[10]; // Temporary buffer for Morse characters
        
        BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, true);
        while (*text) {
            const char *morse = MorseCode(*text);
            strcpy(buffer, morse);
            PlayMorseTone(buffer);
            SYSTEM_DelayMs(200); // Gap between letters
            text++;
        }
        
        BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, false);
}
