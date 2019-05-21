#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 3
#define FOURIER_RESOLUTION 8

#include "fourierBuffer.h"

int main() {
	fourierBuffer fBuf; fourierBufferInit(&fBuf);
	int i, j;
	SAMPLE_TYPE in[BUFFER_SIZE] = {10, 20, 30};
	SAMPLE_TYPE out[BUFFER_SIZE] = {1, 1, 1};

	//print the contents of the fourier buffer
	for (j = 0; j < FOURIER_RESOLUTION; j++)
		printf("%d\t", fBuf.inputBuffer[j]);
	printf("\n");

	for (i = 0; i < 5; i++) {
		//write the input buffer to the fourierBuffer
		fourierBufferWrite(&fBuf, in);

		//print the contents of the fourier buffer
		for (j = 0; j < FOURIER_RESOLUTION; j++)
			printf("%d\t", fBuf.inputBuffer[j]);
		printf("\n");

		//compute the DFT
		fourierBufferGetDFT(&fBuf);

		//print the DFT
		for (j = 0; j < FOURIER_RESOLUTION; j++)
			printf("%f\t", fBuf.amplitudeBuffer[j]);
		printf("\n");

		//get the inverse DFT
		fourierBufferGetInverseDFT(&fBuf, out);

		//print the inverse DFT
		for (j = 0; j < BUFFER_SIZE; j++)
			printf("%d\t", out[j]);
		printf("\n");
	}

	return 0;
}
