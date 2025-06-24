#ifndef MORSE_CODE_H
#define MORSE_CODE_H

#include <string.h>
#include <ctype.h>
#include "driver/keyboard.h"

// Function Prototypes
void Morse_Init(void);
void StartMorseTransmission(const char *text);
void MorseTask(void);
const char* MorseCode(char c);
void MORSE_ProcessKeys(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld);
void GUI_ShowMorseScreen(void);

// Global Variables
extern int txstatus;
extern char* cwid_m;

#endif // MORSE_CODE_H
