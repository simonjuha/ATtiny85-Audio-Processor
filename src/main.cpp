/*
    ATtiny85 Audio processor
    This project uses the ATtiny85 to read an analog signal from the ADC and output a PWM signal.
    Processing of the signal can be done in the main loop.

    Simon Juhl, 2023 - Simonjuhl.net
*/

#define F_CPU 8000000UL // 8 MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ringbuffer.h" 

#define PASSTHROUGH 0

// main loop
int main() {
    /* -------- PWM setup (Timer1) -------- */
    DDRB |= 1<<PB1;                         // Enable PWM output pins
    PLLCSR = 1<<PCKE | 1<<PLLE;             // Enable PLL
    TCCR1 = 1<<PWM1A | 2<<COM1A0 | 1<<CS10; // PWM A, clear on match, 1:1 prescale

    /* -------- Timer setup (Timer0) -------- */
    // Output sample rate formula: F_CPU / (prescale * (1 + OCR0A))
    // 8 MHz / ( 1 * (1 * 24) ) = 320 kHz
    TCCR0A = 3<<WGM00;              // Fast PWM
    TCCR0B = 1<<WGM02;              // Fast PWM
    // CS 0:2 bits in TCCR0B sets the prescale
    // 0b000: no clock source, 0b001: no prescaler, 0b010: 8, 0b011: 64, 0b100: 256, 0b101: 1024 
    TCCR0B |= 2<<CS00;              // no prescaler
    TIMSK = 1<<OCIE0A;              // Enable compare match, disable overflow
    OCR0A = 24;                     // Compare value

    /* -------- ADC setup -------- */
    // Input sample rate formula: F_CPU / (prescale * (1 + (ADCSRA & 0x07)))
    // 8 MHz / (8 * 3) = 333.333 kHz
    DDRB &= ~(1<<PB4);              // Enable ADC input on pin 3 (A2/PB4)
    ADMUX = 0b00000010;             // Use ADC2
    ADCSRA = 1<<ADEN;               // Enable ADC
    ADCSRA |= 1<<ADATE;             // Enable auto trigger
    // ADPS 0:2 bits in ADCSRA sets the prescale
    // 0b000: 2, 0b001: 2, 0b010: 4, 0b011: 8, 0b100: 16, 0b101: 32, 0b110: 64, 0b111: 128
    ADCSRB |= 1<<ADPS1 | 1<<ADPS0;  // Set prescale to 8
    ADCSRB = 1<<ACME;               // Enable analog comparator multiplexer
    DIDR0 |= 1<<ADC1D;              // Disable digital input on ADC2
    ADCSRA |= 1<<ADSC;              // Start conversion
    

    sei(); // Enable interrupts

    /* Main Loop */
    while (1)
    {
        #if PASSTHROUGH
            uint16_t adcValue = ADC >> 2;
            writeToBuffer(adcValue);
        #else
        uint16_t adcValue = ADC >> 2;

        /* PROCESS VALUE... */

        writeToBuffer(adcValue);
        #endif
    }

    return 0;
}

// sample rate = 8 MHz / (1 * (1 + 24)) = 320 kHz
ISR(TIMER0_COMPA_vect) {
    #if PASSTHROUGH
        uint16_t value = ADC >> 2;
        OCR1A = value;
        OCR1B = value;
    #else
        // input from buffer
        uint16_t value = readFromBuffer();
        // output value to PWM
        OCR1A = value;
        OCR1B = value;
    #endif
}