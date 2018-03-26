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
	uint8_t ctrl=0; // Utworzenie zmiennej ctrl (nazwa rejestru w scalaku) kt�ra potem zostanie przekazana przez wska�nik do funkcji TWI_write_buf

	TWI_write_buf (DS3231_ADDR ,0x0e,1, &ctrl); // Funkcja obs�uguj�ca bufor (wieloelementowe tablice), ale w tym przypadku buforem jest jednoelementowa tablica

	// TWI_write_buf( uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *buf ) Adres scalaka , Adres kom�rki pami�ci scalaka, 1, pobranie adresu zmiennej ctrl
	// Pobrany ADRES zmiennej ctrl trafia do funkcji za pomoc� gwiazdki pobrana jest warto�c tej zmiennej odwo�uj�c si�
	// BEZPO�REDNIO do ADRESU pod kt�rym znajduje si� ta zmienna. Gwiazdka wyci�ga warto�c zmiennej kt�ra znajduje si� pod adresem wskazanym przez "&"
     // Mo�na tej zmiennej u�yc w wielu miejscach.

	// Gdy korsyztamy z funkcji i j� wywo�ujemy w g��wnym programie - u�ywamy " & "
	// Gdy piszemy funkcj� i chcemy do niej przekazac wartosc poprzez wskaznik - uzywamy " * "


	// Wpisujemy 0 do rejestru 0x0e scalaka bo dzi�ki temu uruchamiamy cz�stotliwo�c 1Hz i w�aczamy pojawienie si� jej na wyj�ciu scalaka
	// Wszystkie bity w rejestrze 8 bitowym 0x0e ustawiamy na 0
}

void DS3231_get_datetime (TDATETIME * dt)
// Podajemy wska�nik do ca�ej struktury i unii TDATETIME. W razie modyfikacji zmiennych w strukturze nie trzeba
// tutaj nic zmieniac poniewaz i tak dostaniemy sie do struktury bo znajduje sie ona pod konkretnym adresem (dlatego uzywany jest wska�nik)
{
	uint8_t i;
	uint8_t buf[7];
	TWI_read_buf( DS3231_ADDR, 0x00, 7, buf);
	for (i=0; i<7; i++) dt->bytes[i] = bcd2dec (buf[i]);   // Strza�ka oznacza odwo�anie si� do p�l struktury u�ywaj�c wska�nik�w. Normalnie uzywa sie kropki
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

// KONWERSJA LICZBY DZIESI�TNEJ NA BCD

uint8_t dec2bcd(uint8_t dec)
{
	return ((dec / 10)<<4) | (dec % 10);
}

// KONWERSJA LICZBY BCD NA DZIESI�TN�

uint8_t bcd2dec(uint8_t bcd)
{
	return ((((bcd) >> 4) & 0x0F) * 10 ) + ((bcd) & 0x0F);
}


