#include <stdint.h>
#include <math.h>

#ifndef SAMPLE_RATE
#define SAMPLE_RATE 80000 // 80 kHz
#endif

int16_t f_tone = 440; // tone frequency

// generate 440Hz square wave (samplerate = 80 kHz)
// top = f_timer/ f_signal = 80 kHz / 440 Hz = 181.8
void square(int8_t& input) {
    static uint8_t counter = 0;
    static int16_t f_tone_last = f_tone;
    static uint8_t top = SAMPLE_RATE/f_tone/2; // 50% duty cycle
    static bool output = false;
    if(f_tone != f_tone_last) {
        // recalculate top
        f_tone_last = f_tone;
        counter = 0;
        top = SAMPLE_RATE / f_tone / 2;
    }
    if(counter++ >= top) {
        output = !output;
        counter = 0;
    }
    input = output ? 127 : -128;
}

// absolute value
int8_t abs(int8_t value) {
    return value < 0 ? -value : value;
}