/*Autor: Krystian Konias
  Data rozpocz�cia projektu 17-03-2018 */

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "tm1637.h"
#include "my_config.h"
#include "Enkoder/enkoder.h"

//DEKLARACJA ZMIENNYCH W PRZERWANIU
volatile uint16_t timer1;
volatile uint16_t timer2;
volatile uint16_t timer3;

int8_t mm,ss;
uint8_t start=0;
uint8_t miganie;
uint8_t koniec;
uint8_t alarm;
uint8_t sleep_time=0;



// Inicjalizacja enkodera.
static inline void encoderInit() {
        ENKODER_DDR &= ~(ENKODER_A | ENKODER_B | ENKODER_SW); // Piny enkodera jako wej�cie.
        ENKODER_PORT |= ENKODER_A | ENKODER_B | ENKODER_SW; // Wewn�trzne pullupy.
}

// Odczyt stanu enkodera.
encState_t readEncoder() {
        encState_t tmp;
        if (ENKODER_PIN & ENKODER_A) { // Stan na pinie A
                tmp.A = 1;
        } else {
                tmp.A = 0;
        }
        if (ENKODER_PIN & ENKODER_B) { // Stan na pinie B
                tmp.B = 1;
        } else {
                tmp.B = 0;
        }
        if (ENKODER_PIN & ENKODER_SW) { // Je�li przycisk nie wci�ni�ty stan wysoki.
                tmp.SW = 1; // Przycisk nie wci�ni�ty.
        } else {
                tmp.SW = 0; // Przycisk wci�ni�ty.
        }
        return tmp;
}
// Obs�uga zdarze� z enkodera. Jako argument ma przyjmowa� zmienn� typu encEvent_t.
void encoderEvent(encEvent_t const event) {
        char liczba[4];
        switch (event.rotate) {
        case CW: // Ruch w kierunku wskaz�wek zegara.
                mm++;
                BUZ_ON;
				_delay_ms(2);
				BUZ_OFF;
                break;
        case CCW: // Ruch w przeciwnym kierunku do wskaz�wek zegara.
                mm--;
                if (mm>99) mm=0;
					if (mm<0)
					{
						mm=0;
						ss=0;
					}
                BUZ_ON;
                _delay_ms(2);
                BUZ_OFF;
                break;
        default:
                break;
        }
        switch (event.button) {
        case SHORT: // Kr�tkie wci�ni�cie przycisku.
                start = !start;
                if(alarm) alarm=0;
                if (start && alarm)
                	{
                		start=0;
                		BUZ_ON; _delay_ms(100); BUZ_OFF;
                	}

                if (start && !mm && !ss) start=0;
                break;
        case LONG: // D�u�sze wci�ni�cie przycisku.

        		mm=0;
        		ss=0;

                break;
        default:
                break;
        }
}

int main(void)
{
	// Rejestracja u�ywanych callback�w.
	regEncState(readEncoder); // Rejestracja funkcji odczytuj�ce stan enkodera.
	regEncAction(encoderEvent); // Rejestracja funkcji wywo�ywanej podczas jakiego� zdarzenia enkodera.

	// Inicjalizacja peryferii.
	encoderInit(); // Inicjalizacja pin�w enkodera.

	TM1637_init();
	TM1637_set_brightness(7);
	timer_config();
	io_init();


	sei();

	while (1) {


		// DEBUGGING
		/*if (start)
		{
			TM1637_display_colon(true);  // Wy�wietl kropk�
		}
		else  TM1637_display_colon(false);  // Zga� kropk�*/



		if (!timer1)
		{
			timer1 = 10;  //bylo 199


			if ((mm || ss) && start)  // Je�li minuty LUB sekundy maj� jak�kolwiek wartosc I start=1
			{
				 ss--;                // zmniejsz liczb� sekund o jeden
				 miganie=0;           // nie migaj wy�wietlaczem

				 	 if (ss<0)        // je�li sekundy s� mniejsze od 0
				 	 {
						mm--;         // zmniejsz liczb� minut
						ss=59;        // usta sekundy spowrotem na 59
				 	 }

					 if (!ss && !mm)  // je�li czas dojdzie do zera (minuty I sekundy b�d� r�wne zero)
					 {
						start = 0;   // zresetuj zmienn� odpowiedzialn� za przycisk
						alarm=20;    // uruchom alarm

					 }


			}
			else if (start==0) // Je�eli zatrzymano odliczanie
			{
				miganie=1;     // Migaj wy�wietlaczem
			}



			if (miganie && ss && start==0) // Je�li miganie = 1 I sekundy maj� jak�kolwiek warto�c I zatrzymano odliczanie
			{
				TM1637_blink();
			}
			else
			{
				TM1637_wyswietl_czas(mm,ss);
				TM1637_enable(true);
			}

		}


		if (!timer2)
		{
			// obs�uga alarmu
			timer2 = 100;          // by�o 2
			if (alarm) alarm--;
			if (alarm%2) {
					BUZ_ON;

			}
			else {
					BUZ_OFF;

			}
		}


		TM1637_wyswietl_czas(mm,ss);
		TM1637_display_colon(true);



		if (!timer3)
		{
			timer3 = 1000;
			sleep_time++;
		}

		if (!ss && !mm && sleep_time)
		{
			//TM1637_enable(false);
		}



	}
}


ISR (TIMER0_COMPA_vect)
{
	encPool();
	//////////////////////// TIMERY PROGRAMOWE/////////////////
	uint8_t x,y;
	uint16_t z;

	   x = timer1;
	   y = timer2;
	   z = timer3;
	   if (x) timer1 = --x;
	   if (y) timer2 = --y;
	   if (z) timer3 = --z;
}
