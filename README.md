# ATtiny85 Audio Processor
This project is an attempt to use a ATtiny85 microcontroller as an audio processor, despite its limitations.

The ATtiny85 is very limited compared to other microcontroller having some of the following specifications:
- Allow clock frequency from 1MHz up to 20MHz clock frequency
- 8KB flash
- 512B SRAM
- 512 EEPROM
- 2 timers

The project uses two of the available timers, one for PWM output and one for keeping a consistent samplerate. With 512B of SRAM it is very limited what kind of recursive processing that can be done. The ADC is properbly the biggest bottleneck in this project not being able to read with the same speed that the MCU can output samples. Below is a plot showing the frequency response when sweeping a sine wave from _200Hz_ to _10kHz_. Note that this measurement was done without any filtering after the pwm, which would also further dampen the signal.

![Plot showing frequency input of a sinewave and its response](https://github.com/simonjuha/ATtiny85-Audio-Processor/blob/main/media/frequencyResponsePlot.png)
