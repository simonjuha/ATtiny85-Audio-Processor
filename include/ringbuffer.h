#pragma once
#include <stdint.h>

/*
    THIS RINGBUFFER IS OBSOLETE AND DOES NOT HAVE ANY USE IN THE CURRENT PROJECT.

    The size of the buffer will affect the latency.
        buffer size 16 : 1ms
        buffer size 32 : 1.7ms
        buffer size 64 : 3.3ms
        buffer size 128: 6.5ms
    Buffer size larger than 128 might cause problems with RAM overflow.
*/

#ifndef BUFFER_SIZE
    #define BUFFER_SIZE 16
#endif

volatile uint16_t ringBuffer[BUFFER_SIZE];
volatile uint8_t  bufferCount = 0;
volatile uint8_t  bufferReadIndex = 0;
volatile uint8_t  bufferWriteIndex = 0;

void writeToBuffer(uint16_t value) {
    if(bufferCount >= BUFFER_SIZE) return; // buffer full
    ringBuffer[bufferWriteIndex] = value;
    bufferWriteIndex = (bufferWriteIndex + 1) % BUFFER_SIZE; // wrap around
    bufferCount++;
}

uint16_t readFromBuffer() {
    while(bufferCount <= 0) {}
    uint16_t value = ringBuffer[bufferReadIndex];
    bufferReadIndex = (bufferReadIndex + 1) % BUFFER_SIZE; // wrap around
    bufferCount--;
    return value;
}