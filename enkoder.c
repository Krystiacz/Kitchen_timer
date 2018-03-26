/*
 * encoder.c
 *
 *  Created on: 1 maj 2015
 *      Author: Piotr Rudzki ryba.lodz@gmail.com
 *      Version: 1.0
 *  Changelog:
 *  1.0 - small code optimizations, and cleanups
 *      - small changes in button code it only reacts for release
 *  0.9 - split encPool function code for better code readable
 *      - added encoder pulse divisor macro for encoders with many pulses between steady states
 *      - added macro to disable encoder button routines
 *  0.8 - initial release
 */

#include "..//Enkoder/enkoder.h"



// Odczyt kierunku obrotu enkodera.
// Jeœli przypiszemy jednemu pinowi wagê 1 a drugiemu 2 to sekwencja
// w kierunku wskazówek zegara wygl¹da tak:
// w kodzie Gray-a:     0, 1, 3, 2, 0 ...
// w binarnym:          0, 1, 2, 3, 0 ...
static inline enum encRotate readEncRotate(encState_t const * const state) {
        static uint8_t encLast = 0; // Ostatnia pozycja enkodera.
        static uint8_t encDivisor = 0; // Licznik impulsów enkodera.
        uint8_t last = 0, actual = 0; // Zmienne pomocnicze.
// Zmiana kodu Gray-a na binarny.
        if (!state->A) {
                actual++; // 1
        }
        if (!state->B) {
                actual ^= 3; // XOR 3
        }
        last = encLast;
        encLast = actual; // Zapisujemy aktualn¹ pozycjê.
// Ustalamy czy nast¹pi³a zmiana pozycji. Wynik odejmowania wskazuje czy nast¹pi³ ruch,
// oraz kierunek tego ruchu. Bit 0 wskazuje czy nast¹pi³ ruch, a bit 1 jego kierunek.
// Kierunek wskazówek: 2 - 3 = 255 (0b11111111) (poniewa¿ pos³ugujemy siê zmiennymi typu unsigned)
// Przeciwnie:             0 - 3 = 253 (0b11111101) (j.w.)
        actual -= last;
        if (actual & 1) {
                if (actual & 2) {
                        if (++encDivisor == ENC_DIVISOR) { // Bierzemy pod uwagê licznik impulsów.
                                encDivisor = 0;
                                return CW; // Kierunek wskazówek zegara.
                        }
                } else {
                        if (--encDivisor == (uint8_t) (0 - ENC_DIVISOR)) { // J.w.
                                encDivisor = 0;
                                return CCW; // Kierunek przeciwny do wskazówek zegara.
                        }
                }
        }
        return STOP;
}
#ifdef ENC_BUTTON
// Obs³uga przycisku enkodera.
static inline enum encButton readEncButton(encState_t const * const state) {
        static uint16_t pushTime = 0; // Czas trzymania przycisku.
        enum encButton tmp;
        if (pushTime && state->SW) { // Zwolnienie przycisku.
                if ((SHORT_TIME < pushTime) && (pushTime < LONG_TIME)) {
                        tmp = SHORT;
                }
                if ((LONG_TIME < pushTime) && (pushTime < VERY_LONG_TIME)) {
                        tmp = LONG;
                }
                if (VERY_LONG_TIME < pushTime) {
                        tmp = VERY_LONG;
                }
                pushTime = 0;
        } else {
                tmp = NONE;
        }
        if (!state->SW) { // Trzymanie przycisku.
                ++pushTime;
        }
        return tmp;
}
#endif
// Callback do odczytu stanu enkodera.
encState_t (*_encState)(void);
// Rejestracja powy¿szej funkcji.
void regEncState(encState_t (*callback)(void)) {
        _encState = callback;
}
// Callback wywo³ywany w przypadku wyst¹pienia zdarzenia enkodera.
void (*_encEvent)(encEvent_t const event);
// Rejestracja powy¿szej funkcji.
void regEncAction(void (*callback)(encEvent_t const event)) {
        _encEvent = callback;
}
// Funkcja obs³ugi enkodera wywo³ywana w przerwaniu timera.
void encPool(void) {
        encState_t state; // Odczytany stan enkodera.
        encEvent_t event; // Zdarzenie enkodera.
        if (_encState) {
                state = (*_encState)(); // Pobierz aktualny status enkodera.
        }
// Sprawdzenie czy enkoder nie zosta³ obrócony.
        event.rotate = readEncRotate(&state);
#ifdef ENC_BUTTON
// Sprawdzenie stanu przycisku.
        event.button = readEncButton(&state);
#endif
// Wywo³ujemy callback tylko przy jakimœ zdarzeniu.
        if (
#ifdef ENC_BUTTON
        (event.button != NONE) ||
#endif
                        (event.rotate != STOP)) {
                if (_encEvent) {
                        (*_encEvent)(event);
                }
        }
}
