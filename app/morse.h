#ifndef MORSE_CODE_H
#define MORSE_CODE_H

#include <string.h>
#include <ctype.h>

// Function Prototypes
void PlayMorseTone(const char *morse);
const char* MorseCode(char c);
void TransmitMorse(const char *text);

#endif // MORSE_CODE_H
