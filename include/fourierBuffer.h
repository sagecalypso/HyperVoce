/*
//
// fourierBuffer.h
//
// Author: Weston Cook
//
// Distributed under the Mozilla Public License 2.0
//
*/
#ifndef VOCAL_SYNTH_FOURIER_BUFFER_H
#define VOCAL_SYNTH_FOURIER_BUFFER_H


#include "circularBuffer.h"
#include "fastFourierTransform.h"


#ifndef FOURIER_TYPE
#error "FOURIER_TYPE not defined."
#endif


#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct
{
    circularBuffer cBuf;

	FOURIER_TYPE FFT_Re_buffer[CIRCULAR_BUFFER_SIZE];
    FOURIER_TYPE FFT_Im_buffer[CIRCULAR_BUFFER_SIZE];

    SAMPLE_TYPE IFFT_Re_buffer[CIRCULAR_BUFFER_SIZE];
    SAMPLE_TYPE IFFT_Im_buffer[CIRCULAR_BUFFER_SIZE];
} fourierBuffer;


void fourierBufferInit(fourierBuffer* fBuf)
{
    circularBufferInit(&(fBuf->cBuf));
}


void fourierBufferWrite(fourierBuffer* fBuf, SAMPLE_TYPE inBuf[INPUT_BUFFER_SIZE])
{
    circularBufferWrite(&(fBuf->cBuf), inBuf);
}


void fourierBufferComputeFFT(fourierBuffer* fBuf)
{
    rfft(fBuf->cBuf.buffer, fBuf->FFT_Re_buffer, fBuf->FFT_Im_buffer, fBuf->cBuf.readWritePos);
}


void fourierBufferComputeIFFT(fourierBuffer* fBuf, SAMPLE_TYPE Y[], u_int16_t output_buffer_size)
{
    u_int16_t i, offset = CIRCULAR_BUFFER_SIZE - output_buffer_size;
    ifft(fBuf->FFT_Re_buffer, fBuf->FFT_Im_buffer, fBuf->IFFT_Re_buffer, fBuf->IFFT_Im_buffer);
    SAMPLE_TYPE x = fBuf->IFFT_Re_buffer[0];

    for (i = 0; i < output_buffer_size; i++) {
        Y[i] = fBuf->IFFT_Re_buffer[i + offset];
    }
}


#endif
