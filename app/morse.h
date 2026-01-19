#ifndef MORSE_CODE_H
#define MORSE_CODE_H

#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "driver/keyboard.h"

#define MORSE_CWID_MAX_LEN 10u
#define MORSE_CWID_DEFAULT "DE N0CALL"
#define MORSE_WPM_DEFAULT 15u
#define MORSE_WPM_MIN 5u
#define MORSE_WPM_MAX 30u
#define MORSE_EFF_WPM_DEFAULT MORSE_WPM_DEFAULT
#define MORSE_EFF_WPM_MIN MORSE_WPM_MIN
#define MORSE_EFF_WPM_MAX MORSE_WPM_MAX
#define MORSE_STOP_INTERVAL_DEFAULT_MS 45000u
#define MORSE_STOP_INTERVAL_MIN_MS 1000u
#define MORSE_STOP_INTERVAL_MAX_MS 60000u

// Function Prototypes
void Morse_Init(void);
void StartMorseTransmission(const char *text);
void MorseTask(void);
const char* MorseCode(char c);
void MORSE_ProcessKeys(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld);
void GUI_ShowMorseScreen(void);

// Global Variables
extern int txstatus;
extern char cwid_m[MORSE_CWID_MAX_LEN + 1];
extern char* morseVersion;
extern uint8_t morse_wpm;
extern uint8_t morse_wpm_effective;
extern uint16_t morse_stop_interval_ms;
#endif // MORSE_CODE_H1
