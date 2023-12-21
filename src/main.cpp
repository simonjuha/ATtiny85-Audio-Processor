/*
    ATtiny85 Audio processor
    This project uses the ATtiny85 to read an analog signal from the ADC and output a PWM signal.
    Processing of the signal can be done in the main loop.

    Simon Juhl, 2023 - Simonjuhl.net
*/

#include "attiny-audio-processor.h"

// clip the input to the range -128 to 127
void gainWithoutOverflow(int8_t& input) {
    int16_t temp = input * 2;
    input = temp > 127 ? 127 : temp < -128 ? -128 : temp;
}

// let gain overflow
void gainWithOverflow(int8_t& input) {
    input *= 2;
}

int main() {
    init_attiny85_audio_processor(gainWithoutOverflow);

    /* Main Loop */
    while (1)
    {
        processAudio(); // 
    }

    return 0;
}