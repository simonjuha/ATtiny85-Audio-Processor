#ifndef ATTINY_AUDIO_PROCESSOR_H
#define ATTINY_AUDIO_PROCESSOR_H

#define F_CPU 8000000UL // 8 MHz
#include <avr/io.h>
#include <avr/interrupt.h>

volatile bool sampleRead = true; // Flag for when a sample has been read by Timer0 ISR
volatile uint8_t tempSample = 0; // Temporary sample value that has not yet been processed
volatile uint8_t sample = 0;     // Sample value to be send to PWM
uint8_t sample_2lsb = 0;         // 2 least significant bits of 10 bit ADC value

typedef void (*UserFunctionType)(uint8_t& input); // Function pointer type
extern UserFunctionType process = nullptr;         // function called in main loop

// initialize the audio processor. pass a function pointer to the function that should be called in the main loop
void init_attiny85_audio_processor(UserFunctionType processFunction){
    // set the process function to the passed function pointer, or to a default function that does nothing (pass through)
    process = processFunction != nullptr ? processFunction : [](uint8_t& input){}; // do nothing

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
}

// process the audio signal. call this function in the main loop
void processAudio() {
    if(sampleRead) {
        uint8_t tempSample = ADC >> 2;  // MSB 8 bit ADC value
        sample_2lsb = ADC & 0x03;       // 2 LSB of 10 bit ADC value

        // Call the function pointer
        process(tempSample);

        sample = tempSample;
        sampleRead = false;
    }
}

// interrupt sample rate = 8 MHz / (1 * (1 + 24)) = 320 kHz
ISR(TIMER0_COMPA_vect) {
    // output value to PWM
    OCR1A = sample;
    sampleRead = true;
}

#endif // ATTINY_AUDIO_PROCESSOR_H