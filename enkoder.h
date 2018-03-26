 /*
 * encoder.c
 *
 *  Created on: 1 maj 2015
 *      Author: Piotr Rudzki ryba.lodz@gmail.com
 *      Version: 1.0
 *  Changelog:
 *  1.0 - small code optimizations, and cleanups
 *  0.9 - split encPool function code for better code readable
 *      - added encoder pulse divisor macro for encoders with many pulses between steady states
 *      - added macro to disable encoder button routines
 *  0.8 - initial release
 */


#ifndef ENKODER_ENKODER_H_
#define ENKODER_ENKODER_H_



#include <stdint.h>

#define ENKODER_DDR DDRD
#define ENKODER_PIN PIND
#define ENKODER_PORT PORTD
#define ENKODER_A (1 << PD5)
#define ENKODER_B (1 << PD4)
#define ENKODER_SW (1 << PD6)

#define ENC_BUTTON                              // Nale�y zakomentowa� je�li nie obs�ugujemy przycisku enkodera. Zmniejszy kod wynikowy usuwaj�c zb�dne funkcje.
#define ENC_DIVISOR             4               // Ile impuls�w generuje enkoder pomi�dzy stanami stabilnymi.
#define SHORT_TIME              250     // Kr�tkie wci�ni�cie czas wyra�ony w zale�no�ci od tempa wywo�ywania funkcji encPool.
#define LONG_TIME               500    // D�u�sze j.w.
#define VERY_LONG_TIME  3000    // Bardzo d�ugie j.w.

// Struktura opisuj�ca stan enkodera.
typedef struct{
        uint8_t A;
        uint8_t B;
        uint8_t SW;
} encState_t;

enum encRotate {STOP, CW, CCW}; // Zdarzenia obracania enkoderem.
#ifdef ENC_BUTTON
enum encButton {NONE, SHORT, LONG, VERY_LONG}; // Zdarzenia przycisku enkodera.
#endif
// Struktura opisuj�ca zdarzenia enkodera.
typedef struct{
        enum encRotate rotate;
#ifdef ENC_BUTTON
        enum encButton button;
#endif
} encEvent_t;

void regEncState(encState_t (*callback)(void)); // Rejestracja funkcji odczytuj�cej stan enkodera.
void regEncAction(void (*callback)(encEvent_t const event)); // Rejestracja funkcji obs�uguj�cej zdarzenia enkodera.
void encPool(void); // Wywo�ujemy w przerwaniu timera.
void TM1637_wyswietl_czas (uint8_t minuty, uint8_t sekundy);





#endif /* ENKODER_ENKODER_H_ */
