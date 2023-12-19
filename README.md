# ATtiny85 Audio Processor
This project is an attempt to use a ATtiny85 microcontroller as an audio processor, despite its limitations.

The ATtiny85 is very limited compared to other microcontroller having some of the following specifications:
- Allow clock frequency from 1MHz up to 20MHz clock frequency
- 8KB flash
- 512B SRAM
- 512 EEPROM
- 2 timers

The project uses two of the available timers, one for PWM output and one for keeping a consistent sample rate. With 512B of SRAM it is very limited what kind of recursive processing that can be done. The ADC is properly the biggest bottleneck in this project not being able to read with the same speed that the MCU can output samples. Below is a plot showing the frequency response when sweeping a sine wave from _50Hz_ to _100kHz_. Note that this measurement was done without any filtering after the pwm, which would also further dampen the signal. The plot shows that we can get quite a good response up to at least _20kHz_.

<p align="center">
<img 
  alt="ATtiny85 frequency response between ADC input and PWM output (now pwm output filter)" 
  src="https://github.com/simonjuha/ATtiny85-Audio-Processor/blob/main/media/frequencyResponsePlot.png" width=75% height=75%>
</p>

Using a _10kohm_ resistor and a _47nF_ capacitor to filter the pwm signal, an output as seen below, can be achieved. The filter LP in this case might be too strong, but avoiding a jagged signal from the pwm vs amplitude loss is a compromise that has to be made.

<p align="center">
<img alt="Comparison of the input/output of a 10kHz sine (filtered pwm)" src="https://github.com/simonjuha/ATtiny85-Audio-Processor/blob/main/media/sineinput.png" width=75% height=75%>
</p>
