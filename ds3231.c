/*
 * ds3231.c
 *
 *  Created on: 21 mar 2018
 *      Author: Krystiacz
 */


#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "../I2C_TWI/i2c_twi.h"
#include "ds3231.h"
#include "../tm1637.h"



void ds3231_init(void)
{
	uint8_t ctrl=0; // Utworzenie zmiennej ctrl (nazwa rejestru w scalaku) która potem zostanie przekazana przez wskaŸnik do funkcji TWI_write_buf

	TWI_write_buf (DS3231_ADDR ,0x0e,1, &ctrl); // Funkcja obs³uguj¹ca bufor (wieloelementowe tablice), ale w tym przypadku buforem jest jednoelementowa tablica

	// TWI_write_buf( uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *buf ) Adres scalaka , Adres komórki pamiêci scalaka, 1, pobranie adresu zmiennej ctrl
	// Pobrany ADRES zmiennej ctrl trafia do funkcji za pomoc¹ gwiazdki pobrana jest wartoœc tej zmiennej odwo³uj¹c siê
	// BEZPOŒREDNIO do ADRESU pod którym znajduje siê ta zmienna. Gwiazdka wyci¹ga wartoœc zmiennej która znajduje siê pod adresem wskazanym przez "&"
     // Mo¿na tej zmiennej u¿yc w wielu miejscach.

	// Gdy korsyztamy z funkcji i j¹ wywo³ujemy w g³ównym programie - u¿ywamy " & "
	// Gdy piszemy funkcjê i chcemy do niej przekazac wartosc poprzez wskaznik - uzywamy " * "


	// Wpisujemy 0 do rejestru 0x0e scalaka bo dziêki temu uruchamiamy czêstotliwoœc 1Hz i w³aczamy pojawienie siê jej na wyjœciu scalaka
	// Wszystkie bity w rejestrze 8 bitowym 0x0e ustawiamy na 0
}

void DS3231_get_datetime (TDATETIME * dt)
// Podajemy wskaŸnik do ca³ej struktury i unii TDATETIME. W razie modyfikacji zmiennych w strukturze nie trzeba
// tutaj nic zmieniac poniewaz i tak dostaniemy sie do struktury bo znajduje sie ona pod konkretnym adresem (dlatego uzywany jest wskaŸnik)
{
	uint8_t i;
	uint8_t buf[7];
	TWI_read_buf( DS3231_ADDR, 0x00, 7, buf);
	for (i=0; i<7; i++) dt->bytes[i] = bcd2dec (buf[i]);   // Strza³ka oznacza odwo³anie siê do pól struktury u¿ywaj¹c wskaŸników. Normalnie uzywa sie kropki
}

void DS3231_set_time (uint8_t hh , uint8_t mm , uint8_t ss)
{
	uint8_t buf[3];

	buf[0]=dec2bcd(ss);
	buf[1]=dec2bcd(mm);
	buf[2]=dec2bcd(hh);
	TWI_write_buf (DS3231_ADDR , 0x00 , 3, buf);

}


void show_time (TDATETIME * dt)
{
#if TYP_WYSWIETLACZA ==2

	TM1637_wyswietl_czas(dt->hh,dt->mm);


#endif



}

// KONWERSJA LICZBY DZIESIÊTNEJ NA BCD

uint8_t dec2bcd(uint8_t dec)
{
	return ((dec / 10)<<4) | (dec % 10);
}

// KONWERSJA LICZBY BCD NA DZIESIÊTN¥

uint8_t bcd2dec(uint8_t bcd)
{
	return ((((bcd) >> 4) & 0x0F) * 10 ) + ((bcd) & 0x0F);
}


