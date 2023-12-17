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
    /* -------- PWM setup -------- */
    DDRB |= 1<<PB1;                 // Enable PWM output pins
    PLLCSR = 1<<PCKE | 1<<PLLE;     // Enable PLL
    TCCR1 = 1<<PWM1A | 2<<COM1A0 | 1<<CS10; // PWM A, clear on match, 1:1 prescale

    /* -------- Timer setup -------- */
    TCCR0A = 3<<WGM00;              // Fast PWM
    TCCR0B = 1<<WGM02 | 2<<CS00;    // 1/8 prescale
    TIMSK = 1<<OCIE0A;              // Enable compare match, disable overflow
    OCR0A = 49;

    /* -------- ADC setup -------- */
    DDRB &= ~(1<<PB4);              // Enable ADC input on pin 3 (A2/PB4)
    ADMUX = 0b00000010;             // Use ADC2
    ADCSRA = 1<<ADEN | 1<<ADATE | 1<<ADPS2 | 1<<ADPS1;
    ADCSRB = 0x40;                  // Enable free running mode
    DIDR0 = 0x04;                   // Disable digital input on ADC2
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

// sample rate = 8 MHz / 8 / 50 = 20 kHz
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