#ifndef FOURIER_TRANSFORM_H
#define FOURIER_TRANSFORM_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef BUFFER_SIZE
#error "BUFFER_SIZE not defined."
#endif
#ifndef FFT_N
#error "FFT_N not defined."
#endif
#ifndef SAMPLE_TYPE
#error "SAMPLE_TYPE not defined."
#endif
#ifndef FOURIER_TYPE
#error "FOURIER_TYPE not defined."
#endif

typedef struct {
	//structure to hold a large buffer for Fourier transform
	SAMPLE_TYPE inputBuffer[FFT_N];
	u_int32_t readWritePos;
	FOURIER_TYPE amplitudeBuffer[FFT_N];
} fourierBuffer;

void fourierBufferInit(fourierBuffer* fourBuf) {
	//initialize all critical values to zero
	u_int32_t i;

	//initialize input buffer to all zeros
	for (i = 0; i < FFT_N; i++) {
		fourBuf->inputBuffer[i] = 0;
	}

	//initialize read/write head position to zero
	fourBuf->readWritePos = 0;
}

void fourierBufferWrite(fourierBuffer* fourBuf, SAMPLE_TYPE dataBuf[BUFFER_SIZE]) {
	//write the given small data buffer to the given Fourier buffer, wrapping it around starting at the current read/write head position
	u_int32_t i;
	//iterate the data buffer
	for (i = 0; i < BUFFER_SIZE; i++) {
		//write the current dataBuf value to the current fourBuf value
		fourBuf->inputBuffer[fourBuf->readWritePos++] = dataBuf[i];
		//wrap the read/write head back if necessary
		if (fourBuf->readWritePos >= FFT_N)
			fourBuf->readWritePos = 0;
	}
}
void fourierBufferGetDFT(fourierBuffer* fourBuf) {
	//take the Fourier transform of fourBuf and write it to magBuf
        FOURIER_TYPE trigStepVal1, trigStepVal2, trigStepVal3, sizeFloat, cosDotProd, sinDotProd;
	sizeFloat = (FOURIER_TYPE)(FFT_N);
        trigStepVal1 = 6.283185307179586 / sizeFloat;
	u_int32_t i, j;

	//iterate the magnitude buffer
	for (i = 0; i < FFT_N; i++) {
		cosDotProd = sinDotProd = 0;

		trigStepVal2 = trigStepVal1 * (FOURIER_TYPE)(i);

		//iterate the fourierBuffer from first written to last written, starting from the current read/write pos
		for (j = 0; j < FFT_N; j++) {
			trigStepVal3 = trigStepVal2 * (FOURIER_TYPE)(j);

			//accumulate the dot product of the input data with cos and sin
			cosDotProd += (FOURIER_TYPE)(fourBuf->inputBuffer[fourBuf->readWritePos]) * cos(trigStepVal3);
			sinDotProd += (FOURIER_TYPE)(fourBuf->inputBuffer[fourBuf->readWritePos]) * sin(trigStepVal3);

			//wrap the read/write head back if necessary
			if (++(fourBuf->readWritePos) >= FFT_N)
				fourBuf->readWritePos = 0;
		}

		fourBuf->amplitudeBuffer[i] = sqrt(cosDotProd*cosDotProd + sinDotProd*sinDotProd) / sizeFloat;
	}
}
void fourierBufferGetInverseDFT(fourierBuffer* fourBuf, SAMPLE_TYPE outBuf[BUFFER_SIZE]) {
	//get the inverse Fourier transform of the given frequency magnitude data (with phase = 0), and write the last of its values to the output buffer
        FOURIER_TYPE trigStepVal1, trigStepVal2, sizeFloat, outVal;
	sizeFloat = (FOURIER_TYPE)(FFT_N);
        trigStepVal1 = 6.283185307179586 / sizeFloat;
	u_int32_t i, j = fourBuf->readWritePos, n;

	//iterate each value in the output buffer
        for (i = 0; i < BUFFER_SIZE; i++) {//iterate each value of the audio buffer
                outVal = 0;

                trigStepVal2 = trigStepVal1 * (FOURIER_TYPE)(j++);
		//wrap the read/write head back over if necessary
		if (j >= FFT_N) j = 0;

                for (n = 0; n < FFT_N; n++) {//iterate each frequency in the magnitude buffer
			//accumulate the values of each frequency wave at this point in the audio buffer
                        outVal += fourBuf->amplitudeBuffer[n] * cos(trigStepVal2 * (FOURIER_TYPE)(n));
                }

                outBuf[i] = round(outVal);
        }
}

#endif
