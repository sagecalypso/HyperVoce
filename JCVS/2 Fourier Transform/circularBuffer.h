#ifndef FOURIER_TRANSFORM_H
#define FOURIER_TRANSFORM_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef IO_BUFFER_SIZE
#error "IO_BUFFER_SIZE not defined."
#endif
#ifndef CIRCULAR_BUFFER_SIZE
#error "CIRCULAR_BUFFER_SIZE not defined."
#endif
#ifndef SAMPLE_TYPE
#error "SAMPLE_TYPE not defined."
#endif

typedef struct {
	//structure to hold a large buffer for Fourier transform
	SAMPLE_TYPE inputBuffer[CIRCULAR_BUFFER_SIZE];
	u_int32_t readWritePos;
} circularBuffer;

void circularBufferInit(circularBuffer* cBuf) {
	//initialize all critical values to zero
	u_int16_t i;

	//initialize input buffer to all zeros
	for (i = 0; i < CIRCULAR_BUFFER_SIZE; i++) {
		cBuf->inputBuffer[i] = 0;
	}

	//initialize read/write head position to zero
	cBuf->readWritePos = 0;
}

void circularBufferWrite(circularBuffer* cBuf, SAMPLE_TYPE inBuf[IO_BUFFER_SIZE]) {
	//write the given small data buffer to the given Fourier buffer, wrapping it around starting at the current read/write head position
	u_int16_t i;

	//iterate the data buffer
	for (i = 0; i < IO_BUFFER_SIZE; i++) {
		//write the current dataBuf value to the current fourBuf value
		cBuf->inputBuffer[cBuf->readWritePos++] = inBuf[i];
		//wrap the read/write head back if necessary
		if (cBuf->readWritePos >= CIRCULAR_BUFFER_SIZE)
			cBuf->readWritePos = 0;
	}
}

float circularBufferPitchDetect(circularBuffer* cBuf) {
	//return the estimated most recent frequency of the input voice
	u_int16_t i;

	for (i = 0; i < CIRCULAR_BUFFER_SIZE; i++) {
		cBuf->inputBuffer[cBuf->readWritePos--];
		//wrap the read/write head back if necessary
		if (cBuf->readWritePos >= CIRCULAR_BUFFER_SIZE)
			cBuf->readWritePos = CIRCULAR_BUFFER_SIZE - 1;
	}
}

#endif
