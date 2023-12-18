/*
    ATtiny85 Audio processor
    This project uses the ATtiny85 to read an analog signal from the ADC and output a PWM signal.
    Processing of the signal can be done in the main loop.

    Simon Juhl, 2023 - Simonjuhl.net
*/

#include "attiny-audio-processor.h"

// custom process function
void gain(uint8_t& input) {
    input = input * 2;
}

// using sample_2lsb to get 10 bit ADC value
void lsbTest(uint8_t& input) {
    uint16_t sample_10bit = (input<<2) + sample_2lsb;
    if(sample_10bit > 512) {
        input = 255;
    } else {
        input = input - (sample_2lsb<<2);
    }
}

int main() {
    init_attiny85_audio_processor(lsbTest);

    /* Main Loop */
    while (1)
    {
        processAudio(); // 
    }

    return 0;
}