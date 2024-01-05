#ifndef ATTINY_AUDIO_PROCESSOR_H
#define ATTINY_AUDIO_PROCESSOR_H

#define F_CPU 16000000UL  // 16 MHz
#define SAMPLE_RATE 80000 // 80 kHz
#include <avr/io.h>
#include <avr/interrupt.h>

#define MS_TIMER_TICKS SAMPLE_RATE / 1000

volatile bool sampleRead = true;        // Flag for when a sample has been read by Timer0 ISR
volatile int8_t sample = 0;             // Sample value to be send to PWM
int8_t sample_2lsb = 0;                 // 2 least significant bits of 10 bit ADC value
volatile static uint32_t timePassed = 0;// Milliseconds since start (can count up to 49.71 days)

typedef void (*UserFunctionType)(int8_t& input); // Function pointer type
extern UserFunctionType process = nullptr;         // function called in main loop

uint32_t millis() {
    return timePassed;
}

// initialize the audio processor. pass a function pointer to the function that should be called in the main loop
void init_attiny85_audio_processor(UserFunctionType processFunction){
    // set the process function to the passed function pointer, or to a default function that does nothing (pass through)
    process = processFunction != nullptr ? processFunction : [](int8_t& input){}; // do nothing

    // !!!!! Remember to set fueses to use PLL: !!!!!
    // lfuse = 0xC1
    // hfuse = 0xD4
    CLKPR = (1 << CLKPCE);  // Enable changes to CLKPR (changing bit 7 to enable changes to the other bits)
    CLKPR = 0;              // Set the clock division to 1 (no division)

    /* -------- PWM setup (Timer1) -------- */
    // F_PWM = F_timer / (prescale * 256)
    // 64 MHz / (2 * 256) = 125 kHz
    DDRB |= 1<<PB1;     // Enable PWM output pins
    PLLCSR = 1<<PLLE;   // Enable PLL
    PLLCSR |= 1<<PCKE;  // Use 64 MHz PLL output as timer clock
    TCCR1 |= 1<<PWM1A;  // Enable PWM A
    TCCR1 |= 1<<COM1A1; // Clear OC1A on compare match
    TCCR1 |= 1<<CS11;   // Set prescale to 2
    
    /* -------- Timer setup (Timer0) -------- */
    // Output sample rate formula: (F_CPU) / (prescale * (1 + OCR0A))
    // 16 MHz / (8 * 25) = 80 kHz = 1/12.5 us
    TCCR0A = 1<<WGM00 | 1<<WGM01;   // Fast PWM, OCR0A as TOP
    TCCR0B = 1<<WGM02;
    // CS 0:2 bits in TCCR0B sets the prescale
    // 0b000: no clock source, 0b001: no prescaler, 0b010: 8, 0b011: 64, 0b100: 256, 0b101: 1024 
    TCCR0B |= 1<<CS01;  // Set prescale to 8
    TIMSK  = 1<<OCIE0A; // Enable interrupt on compare match A
    OCR0A  = 24;        // Compare value

    /* -------- ADC setup -------- */
    // F_ADC: F_CPU / prescale = 16 MHz / 16 = 1 MHz
    // (ATtiny85 conversion time is approx. 13.5 ADC clock cycles)
    // ADC sample rate: 1 MHz / 13.5 = 74.07 kHz 

    DDRB &= ~(1<<PB4);      // Enable ADC input on pin 3 (A2/PB4)
    ADMUX = 0b00000010;     // Use ADC2
    ADCSRA = 1<<ADEN;       // Enable ADC
    ADCSRA |= 1<<ADATE;     // Enable auto trigger
    ADCSRA |= 1<<ADPS1 | 1<<ADPS0; // Set prescale to 8
    // ADPS 0:2 bits in ADCSRA sets the prescale
    // 0b000: 2, 0b001: 2, 0b010: 4, 0b011: 8, 0b100: 16, 0b101: 32, 0b110: 64, 0b111: 128
    ADCSRB |= 1<<ADTS1 | 1<<ADTS0;  // Set trigger source to Timer0 compare match A
    ADCSRB = 1<<ACME;       // Enable analog comparator multiplexer
    DIDR0 |= 1<<ADC1D;      // Disable digital input on ADC2
    ADCSRA |= 1<<ADSC;      // Start conversion
    

    sei(); // Enable interrupts
}

// process the audio signal. call this function in the main loop
void processAudio() {
    if(sampleRead) {
        int16_t centeredReading = ADC - 512; // Center the reading around 0
        int8_t scaledReading = centeredReading >> 2; // Scale to -128 to 127 range
        sample_2lsb = (scaledReading < 0 ? -scaledReading : scaledReading) & 0b11; // Get the 2 least significant bits of the 10 bit ADC value

        // Call the function pointer
        process(scaledReading);

        sample = scaledReading;
        sampleRead = false;
    }
}

// interrupt sample rate = 80 kHz
ISR(TIMER0_COMPA_vect) {
    static uint8_t msCounter = 0; // Counter for milliseconds
    if(++msCounter == MS_TIMER_TICKS){
        msCounter = 0;
        timePassed++; // 1 ms has passed
    }
    // output value to PWM
    OCR1A = sample + 128; // 
    sampleRead = true;
}

#endif // ATTINY_AUDIO_PROCESSOR_H