/*
    ATtiny85 Audio processor
    This project uses the ATtiny85 to read an analog signal from the ADC and output a PWM signal.
    Processing of the signal can be done in the main loop.

    Simon Juhl, 2023 - Simonjuhl.net
*/

#include "attiny-audio-processor.h"

// custom process function
void gain(uint16_t& input) {
    input = input * 2;
}

int main() {
    init_attiny85_audio_processor(gain);

    /* Main Loop */
    while (1)
    {
        processAudio(); // 
    }

    return 0;
}