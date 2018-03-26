/*
 * my_config.c
 *
 *  Created on: 18 mar 2018
 *      Author: Krystiacz
 */

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "my_config.h"


///////////////////// KONFIGURACJA TIMERA ////////////////////////
void io_init()
{
	DDRD |= BUZ_PIN;
}


void timer_config()
{
	    // Czestotliwosc taktowania 16 MHz
		TCCR0A |= (1<<WGM01);  // Tryb CTC
		TCCR0B |= (1<<CS02) | (1<<CS00); // Preskaler = 1024
		OCR0A = 77 ; // przerwanie porównania co 5 ms (200 Hz)
		TIMSK0 |= (1<<OCIE0A); //zezwolenie na przerwanie COMPARE MATCH
}
