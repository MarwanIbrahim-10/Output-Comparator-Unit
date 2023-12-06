/*
 * lab6.cpp
 *
 * Created: 7/12/2023 6:03:00 pm
 * Author : Marwan & Ahmed
 * Pin 8 => PB0 => PinA of the encoder => PCINT0
 * Pin 9 => PB1 => PinB of the encoder => PCINT1
 * Pin 11 => PB3 => PWM signal to motor
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t pos = 0;	// index of the OCR2A axis register initialized

uint8_t axis[14] = {10,12,14,16,18,20,22,24,26,28,30,32,34,36};		//array that stores values that will be used to set the OCR2A register

int main(void){
	
	DDRB &= ~(1 << DDB0)|(1 << DDB1);	// PinA and PinbB set as input
	DDRB |= (1 << DDB3);	// PB3 set as output to the motor as the PWN signal
	
	cli();
	
	PCICR |= (1 << PCIE0);		// Enable pin change interrupt for PCINT[0:7]
	PCMSK0 |= (1 << PCINT0)|(1 << PCINT1);	// Enable pin change interrupt for PB0 and PB1

	TCCR2A |= (1 << WGM21) | (1 << WGM20);	// Fast PWM mode enabled on Timer2
	TCCR2A |= (1 << COM2A1);				// non-inverting PWM mode established
	
	TIMSK2 |= (1 << TOIE2);		//Time2 overflow interrupt enabled
	
	
	sei();
	
	OCR2A = axis[0];									// initialize OCR2A register
	
	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);	// prescale of 1024
	
	while(1){ 	 }
}


ISR(PCINT0_vect){
	
	
	if(!(PINB & (1 << PINB0))){		//if PinA become zero
		pos++;		//increment the axis index
		if(pos == 14) {		//overflow correction
			pos = 13;
		}
		while((!(PINB & (1 << PINB0))) || (!(PINB & (1 << PINB1))))		//hold the count until PinA and PinB are fully rotated and in high idle state again
		_delay_ms(20);
	}
	if(!(PINB & (1 << PINB1))){		//if PinB becomes zero
		pos--;		//decrement the position index
		if(pos == -1) {		//overflow correction
			pos = 0;
		}
		while((!(PINB & (1 << PINB0))) || (!(PINB & (1 << PINB1))))
		_delay_ms(20);
	}
}


ISR(TIMER2_OVF_vect){
	OCR2A = axis[pos];		//OCR2A updated according to the new position in the axis register, happens only when timer2 overflows
}
