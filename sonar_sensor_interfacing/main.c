// /*
//  * sonar_sensor_interfacing.c
//  *
//  * Created: 9/29/2024 11:32:11 PM
//  * Author : muntasir
//  */ 

#define F_CPU 8000000UL  // 8 MHz clock frequency
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "lcd_lib.h"

void init_hcsr04();
uint16_t measure_distance();

volatile uint16_t pulse_width = 0;
volatile uint8_t rising_edge = 1;

int main(void) {
	uint16_t distance;
	
	LCDinit();
	_delay_ms(50);

	// Initialize the HC-SR04 sensor and INT0
	init_hcsr04();

	// Enable global interrupts
	sei();

	while (1) {
		// Measure distance
		distance = measure_distance();

		// Send the measured distance via UART
		char buffer[16];
		sprintf(buffer, "Distance: %ucm    ", distance);

		LCDWriteStringXY(0, 0, buffer);
		// Delay between measurements
		_delay_ms(500);
	}
}


// HC-SR04 Initialization
void init_hcsr04() {
	// Set PB0 (Trigger) as output
	DDRB |= (1 << PB4);
	PORTB &= ~(1 << PB4);  

	// Set INT0 (PD2, Echo pin) as input
	DDRD &= ~(1 << PD2);

	// Configure external interrupt INT0 for any logical change
	MCUCR |= (1 << ISC00);   
	GICR |= (1 << INT0);     
}

uint16_t measure_distance() {
	uint16_t distance;

	// Send 10us trigger pulse
	PORTB |= (1 << PB4);
	_delay_us(10);
	PORTB &= ~(1 << PB4);

	// Wait for the pulse to complete
	while (pulse_width == 0);  // Wait for INT0 to calculate pulse_width

	// Calculate distance (Speed of sound is ~343m/s => 58us/cm round trip)
	distance = pulse_width / 58;

	// Reset pulse_width for next measurement
	pulse_width = 0;

	return distance;
}

// INT0 Interrupt Service Routine
ISR(INT0_vect) {
	if (rising_edge) {
		// Rising edge detected, start the timer
		TCNT1 = 0;
		TCCR1B |= (1 << CS11);  // Start Timer1 with prescaler of 8 (1 tick = 1µs)
		rising_edge = 0;
	} else {
		// Falling edge detected, stop the timer
		TCCR1B = 0;  // Stop the timer
		pulse_width = TCNT1;  // Capture the pulse width
		rising_edge = 1;
	}
}
