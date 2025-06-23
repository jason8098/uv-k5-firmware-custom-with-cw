#ifndef MORSE_CODE_H
#define MORSE_CODE_H

#include <string.h>
#include <ctype.h>
#include "driver/keyboard.h"

// Function Prototypes
void PlayMorseTone(const char *morse);
const char* MorseCode(char c);
void TransmitMorse(const char *text);
void GUI_ShowMorseScreen(void);
void MORSE_ProcessKeys(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld);
#endif // MORSE_CODE_H
