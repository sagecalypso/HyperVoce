/*
// Author: Weston Cook
//
// Distributed under the Mozilla Public Licence 2.0
//
// Description:
//	Defines struct circularBuffer, which uses a circular array to store samples.
//	Also defines two helper functions for circularBuffer:
//		circularBufferInit(circularBuffer*) -- initializes a circularBuffer object
//		circularBufferWrite(circularBuffer*, SAMPLE_TYPE[INPUT_BUFFER_SIZE]) -- writes data to a circularBuffer object
//
*/
#ifndef VOCAL_SYNTH_CIRCULAR_BUFFER_H
#define VOCAL_SYNTH_CIRCULAR_BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/*
//
// Required preprocessor definitions.
//
// CIRCULAR_BUFFER_SIZE (int) -- size of circular buffer
//
// INPUT_BUFFER_SIZE (int) -- size of buffer used for adding to the circular buffer
//
// SAMPLE_TYPE (typename) -- type of samples to store
//
*/
#ifndef CIRCULAR_BUFFER_SIZE
#error "CIRCULAR_BUFFER_SIZE not defined."
#endif
#ifndef INPUT_BUFFER_SIZE
#error "IO_BUFFER_SIZE not defined."
#endif
#ifndef SAMPLE_TYPE
#error "SAMPLE_TYPE not defined."
#endif


/*
//
// struct circularBuffer
//
// Description:
//	Stores an array of samples that is written/read circularly for efficiency.
//
*/
typedef struct
{
	SAMPLE_TYPE buffer[CIRCULAR_BUFFER_SIZE];
	u_int16_t readWritePos;
} circularBuffer;


/*
//
// void circularBufferInit
//
// Description:
//	Initializes the values in the given circularBuffer to zeros.
//
// Arguments:
//	circularBuffer* cBuf -- a pointer to the circularBuffer object to be initialized
//
*/
void circularBufferInit(circularBuffer* cBuf)
{
	u_int16_t i;

	// Initialize buffer to all zeros
	for (i = 0; i < CIRCULAR_BUFFER_SIZE; i++) {
		cBuf->buffer[i] = 0;
	}

	// Initialize read/write head position to zero
	cBuf->readWritePos = 0;
}


/*
//
// void circularBufferWrite
//
// Description:
//	Appends the contents given small data buffer to the circularBuffer.
//
// Arguments:
//	circularBuffer* cBuf -- a pointer to the circularBuffer to be written to
//	SAMPLE_TYPE inBuf[INPUT_BUFFER_SIZE] -- a buffer containing input data
//
*/
void circularBufferWrite(circularBuffer* cBuf, SAMPLE_TYPE inBuf[INPUT_BUFFER_SIZE]) {
	u_int16_t i;

	for (i = 0; i < INPUT_BUFFER_SIZE; i++) {
		cBuf->inputBuffer[cBuf->readWritePos++] = inBuf[i];

		// Wrap the read/write head back if necessary
		if (cBuf->readWritePos >= CIRCULAR_BUFFER_SIZE)
			cBuf->readWritePos = 0;
	}
}


#endif
