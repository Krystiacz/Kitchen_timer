/*
 * ds3231.h
 *
 *  Created on: 21 mar 2018
 *      Author: Krystiacz
 */

#ifndef DS3231_DS3231_H_
#define DS3231_DS3231_H_

#define DS3231_ADDR 0xD0

// WYBÓR RODZAJU WYŒWIETLACZA ////
// 1 - 7 segmentowy zwyk³y
// 2 - 7 segmentowy TM1637
// 3 - LCD HD44780
// 4 - Wyœwietlacz matrycowy
// 5 - OLED

#define TYP_WYSWIETLACZA 2

typedef union {
	uint8_t bytes[7];
	struct {
		uint8_t ss;
		uint8_t mm;
		uint8_t hh;
		uint8_t dayofweek;
		uint8_t day;
		uint8_t month;
		uint8_t year;

	};
}TDATETIME;

typedef struct {
	int8_t cel;   // int8_t poniewa¿ ca³kowita czêœc temperatury mo¿e byc ujemna
	uint8_t fract;  // czesc u³amnkowa temperatury
}TTEMP;


void ds3231_init(void);
uint8_t dec2bcd(uint8_t dec);
uint8_t bcd2dec(uint8_t bcd);
void show_time (TDATETIME * dt);
void DS3231_get_datetime (TDATETIME * dt);
void DS3231_set_time (uint8_t hh , uint8_t mm , uint8_t ss);




#endif /* DS3231_DS3231_H_ */
