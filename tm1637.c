/**
 * Copyright (c) 2017, £ukasz Marcin Podkalicki <lpodkalicki@gmail.com>
 *
 * This is ATtiny13/25/45/85 library for 4-Digit LED Display based on TM1637 chip.
 *
 * Features:
 * - display digits
 * - display colon
 * - display raw segments
 * - display on/off
 * - brightness control
 *
 * References:
 * - library: https://github.com/lpodkalicki/attiny-tm1637-library
 * - documentation: https://github.com/lpodkalicki/attiny-tm1637-library/README.md
 * - TM1637 datasheet: https://github.com/lpodkalicki/attiny-tm1637-library/blob/master/docs/TM1637_V2.4_EN.pdf
 */

#include <avr/io.h>
#include <util/delay.h>
#include "tm1637.h"

#define	TM1637_DIO_HIGH()		(PORTD |= _BV(TM1637_DIO_PIN))
#define	TM1637_DIO_LOW()		(PORTD &= ~_BV(TM1637_DIO_PIN))
#define	TM1637_DIO_OUTPUT()		(DDRD |= _BV(TM1637_DIO_PIN))
#define	TM1637_DIO_INPUT()		(DDRD &= ~_BV(TM1637_DIO_PIN))
#define	TM1637_DIO_READ() 		(((PIND & _BV(TM1637_DIO_PIN)) > 0) ? 1 : 0)
#define	TM1637_CLK_HIGH()		(PORTD |= _BV(TM1637_CLK_PIN))
#define	TM1637_CLK_LOW()		(PORTD &= ~_BV(TM1637_CLK_PIN))

#define	TM1637_FLAG_ENABLED		(1 << 0)
#define	TM1637_FLAG_SHOWCOLON		(1 << 1)


static void TM1637_configure(void);
static void TM1637_cmd(uint8_t value);
static void TM1637_start(void);
static void TM1637_stop(void);
static uint8_t TM1637_write_byte(uint8_t value);

static const uint8_t _digit2segments[] =
{
	0x3F, // 0
	0x06, // 1
	0x5B, // 2
	0x4F, // 3
	0x66, // 4
	0x6D, // 5
	0x7D, // 6
	0x07, // 7
	0x7F, // 8
	0x6F,  // 9
	0x80  // kropka
};

static uint8_t _brightness = TM1637_DEFAULT_BRIGHTNESS;
static uint8_t _digit = 0xff;
static uint8_t _flags = 0x00;

void
TM1637_init(void)
{

	DDRD |= (_BV(TM1637_DIO_PIN)|_BV(TM1637_CLK_PIN));
	PORTD &= ~(_BV(TM1637_DIO_PIN)|_BV(TM1637_CLK_PIN));
	_flags |= TM1637_FLAG_ENABLED;
	TM1637_clear();
}

void
TM1637_display_digit(const uint8_t addr, const uint8_t digit)
{
	uint8_t segments = digit < 11 ? _digit2segments[digit] : 0x00;

	if (addr == CYFRA_3) {  // sztywnie przypisana funkcja zapalania kropki
		_digit = digit; // przypisanie _digit wartoœci digit ¿eby w funkcji showcolon dalej wyswietla³a siê cyfra
		if (_flags & TM1637_FLAG_SHOWCOLON) { // jeœli w funkcji showcolon zapali siê flaga wykonaj pêtlê
			segments |= 0x80;  // adres kropki (trzeba poznac adres dwukropka)
		}
	}

	TM1637_display_segments(addr, segments);
}

void TM1637_wyswietl_liczbe (uint16_t liczba)
{
	uint8_t jednosci, dziesiatki, setki, tysiace;


	jednosci =    liczba%10;
	dziesiatki = (liczba/10)%10;
	setki =      (liczba/100)%10;
	tysiace =     liczba/1000;


	TM1637_display_digit(CYFRA_1, jednosci);
	TM1637_display_digit(CYFRA_2, dziesiatki);
	TM1637_display_digit(CYFRA_3, setki);
	TM1637_display_digit(CYFRA_4, tysiace);

}

void TM1637_wyswietl_czas (uint8_t minuty, uint8_t sekundy)
{
	uint8_t ss1, ss2, mm1, mm2;


	ss1 =    sekundy%10;
	ss2 = (sekundy/10)%10;
	mm1 =      minuty%10;
	mm2 =     (minuty/10)%10;


	TM1637_display_digit(CYFRA_1, ss1);
	TM1637_display_digit(CYFRA_2, ss2);
	TM1637_display_digit(CYFRA_3, mm1);
	TM1637_display_digit(CYFRA_4, mm2);

}

void TM1637_blink()
{
	TM1637_enable(true);
	_delay_ms(500);
	TM1637_enable(false);
	_delay_ms(500);

}

void test_wyswietlacza()
{
	for (uint8_t l=0 ; l<=9;){

				TM1637_display_digit(CYFRA_4, l);
				//TM1637_display_digit(TM1637_SET_ADR_00H, 10);

				for (uint8_t k=0 ; k<=9;){

					TM1637_display_digit(CYFRA_3, k);

					for (uint8_t j=0 ; j<=9;)
					{
						TM1637_display_digit(CYFRA_2, j);

						for (uint8_t i=0;i<=9;i++)
						{
							TM1637_display_digit(CYFRA_1, i);
							_delay_ms(100);
						}

						j++;

					}
						k++;
					}
				l++;
			}
}

void
TM1637_display_segments(const uint8_t addr, const uint8_t segments)
{

	TM1637_cmd(TM1637_CMD_SET_DATA | TM1637_SET_DATA_F_ADDR);
	TM1637_start();
	TM1637_write_byte(TM1637_CMD_SET_ADDR | addr);
	TM1637_write_byte(segments);
	TM1637_stop();
	TM1637_configure();
}

void TM1637_display_colon(bool value)
{

	if (value) {
		_flags |= TM1637_FLAG_SHOWCOLON;
	} else {
		_flags &= ~TM1637_FLAG_SHOWCOLON;
	}
	TM1637_display_digit(CYFRA_3, _digit);
}



void
TM1637_clear(void)
{

	TM1637_display_colon(false);
	TM1637_display_segments(CYFRA_4, 0x00);
	TM1637_display_segments(CYFRA_3, 0x00);
	TM1637_display_segments(CYFRA_2, 0x00);
	TM1637_display_segments(CYFRA_1, 0x00);
}

void
TM1637_set_brightness(const uint8_t brightness)
{

	_brightness = brightness & 0x07;
	TM1637_configure();
}

void
TM1637_enable(bool value)
{

	if (value) {
		_flags |= TM1637_FLAG_ENABLED;
	} else {
		_flags &= ~TM1637_FLAG_ENABLED;
	}
	TM1637_configure();
}

void
TM1637_configure(void)
{
	uint8_t cmd;

	cmd = TM1637_CMD_SET_DSIPLAY;
	cmd |= _brightness;
	if (_flags & TM1637_FLAG_ENABLED) {
		cmd |= TM1637_SET_DISPLAY_ON;
	}

	TM1637_cmd(cmd);
}

void
TM1637_cmd(uint8_t value)
{

	TM1637_start();
	TM1637_write_byte(value);
	TM1637_stop();
}

void
TM1637_start(void)
{

	TM1637_DIO_HIGH();
	TM1637_CLK_HIGH();
	_delay_us(TM1637_DELAY_US);
	TM1637_DIO_LOW();
}

void
TM1637_stop(void)
{

	TM1637_CLK_LOW();
	_delay_us(TM1637_DELAY_US);

	TM1637_DIO_LOW();
	_delay_us(TM1637_DELAY_US);

	TM1637_CLK_HIGH();
	_delay_us(TM1637_DELAY_US);

	TM1637_DIO_HIGH();
}

uint8_t
TM1637_write_byte(uint8_t value)
{
	uint8_t i, ack;

	for (i = 0; i < 8; ++i, value >>= 1) {
		TM1637_CLK_LOW();
		_delay_us(TM1637_DELAY_US);

		if (value & 0x01) {
			TM1637_DIO_HIGH();
		} else {
			TM1637_DIO_LOW();
		}

		TM1637_CLK_HIGH();
		_delay_us(TM1637_DELAY_US);
	}

	TM1637_CLK_LOW();
	TM1637_DIO_INPUT();
	TM1637_DIO_HIGH();
	_delay_us(TM1637_DELAY_US);

	ack = TM1637_DIO_READ();
	if (ack) {
		TM1637_DIO_OUTPUT();
		TM1637_DIO_LOW();
	}
	_delay_us(TM1637_DELAY_US);

	TM1637_CLK_HIGH();
	_delay_us(TM1637_DELAY_US);

	TM1637_CLK_LOW();
	_delay_us(TM1637_DELAY_US);

	TM1637_DIO_OUTPUT();

	return ack;
}
