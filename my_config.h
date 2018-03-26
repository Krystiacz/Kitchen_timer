/*
 * my_config.h
 *
 *  Created on: 18 mar 2018
 *      Author: Krystiacz
 */

#ifndef MY_CONFIG_H_
#define MY_CONFIG_H_

#define BUZ_PIN (1<<PD7) //Dioda podlaczona do pinu PB5
#define BUZ_TOG PORTD ^= BUZ_PIN // Zmiana stanu diody
#define BUZ_ON  PORTD |= BUZ_PIN // Stan wysoki na pinie PB5  (00100000)
#define BUZ_OFF PORTD &= ~BUZ_PIN // Stan niski na pinie PB5


void timer_config();


#endif /* MY_CONFIG_H_ */
