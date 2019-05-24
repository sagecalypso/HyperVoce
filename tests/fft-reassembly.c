/*
//
// fft-reassembly.c
//
// Author: Weston Cook
// Derived from tutorials at portaudio.com
//
// Distributed under the Mozilla Public License 2.0
//
// Description:
//	Demonstrates FFT capabilities by piping the input stream through an FFT and back
//	 through an inverse FFT and then passing the resulting audio to the output device.
//
*/
#include "../portaudio.h"
#include "../pa_linux_alsa.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>


#define FFT_N 512
#define FFT_LOG2N 9
#define CIRCULAR_BUFFER_SIZE 1024
#define INPUT_BUFFER_SIZE 256
#define SAMPLE_RATE 44100
#define SAMPLE_FORMAT paInt16
#define SAMPLE_TYPE int16_t
#define FOURIER_TYPE float


#include "../include/fourierBuffer.h"


static int wireCallback(
		const void* inputBuffer,
		void* outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void* userData);


int openDefaultDuplexStream(PaStream** stream, void* userData, PaError* err);


int main() {
	PaStream* stream = NULL;
	PaError err;
	fourierBuffer fBuf; fourierBufferInit(&fBuf);
	generateLookupTables();

	printf("sample rate:\t\t%d Hz\n", SAMPLE_RATE);
	printf("input buffer size:\t%d samples\n", INPUT_BUFFER_SIZE);
	printf("circular buffer size:\t%d samples\n", CIRCULAR_BUFFER_SIZE);
	printf("lowest frequency:\t%f Hz\n", (float)(SAMPLE_RATE) / (float)(CIRCULAR_BUFFER_SIZE));
	printf("minimum latency:\t%f ms\n", (float)(INPUT_BUFFER_SIZE * 3000) / (float)(SAMPLE_RATE));
	printf("\n");

	// Initialize portaudio
	err = Pa_Initialize();
	if (err != paNoError) goto error;

	// Open a stream using the default input and output devices
	if (!openDefaultDuplexStream(&stream, &fBuf, &err)) goto error;
	if (err != paNoError) goto error;

	// Start the stream
	err = Pa_StartStream(stream);
	if (err != paNoError) goto error;

	printf("Now recording and playing. - Hit ENTER to exit.");
	getchar();

	// Stop and close the stream
	err = Pa_StopStream(stream);
	if (err != paNoError) goto error;
	err = Pa_CloseStream(stream);
	if (err != paNoError) goto error;

	goto done;

done:
	// Shut down portaudio
	Pa_Terminate();
	return EXIT_SUCCESS;

error:
	fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
	return EXIT_FAILURE;
}


static int wireCallback(
		const void* inputBuffer,
                void* outputBuffer,
                unsigned long framesPerBuffer,
                const PaStreamCallbackTimeInfo* timeInfo,
                PaStreamCallbackFlags statusFlags,
                void* userData) {
	SAMPLE_TYPE* in;
	SAMPLE_TYPE* out;
	fourierBuffer* fBuf;
	long i;

	if ( inputBuffer == NULL || outputBuffer == NULL ) return paContinue;

	in = ((SAMPLE_TYPE**)inputBuffer)[0];
	out = ((SAMPLE_TYPE**)outputBuffer)[0];
	fBuf = ((fourierBuffer*)userData);

	// Write the new data to the fourierBuffer
	fourierBufferWrite(fBuf, in);

	// Compute the fourier transform
	fourierBufferComputeFFT(fBuf);

	// Compute the inverse fourier transform
	fourierBufferComputeIFFT(fBuf, out, framesPerBuffer);

	return paContinue;
}


int openDefaultDuplexStream(PaStream** stream, void* userData, PaError* err) {
	PaStreamParameters inputParameters, outputParameters;

	inputParameters.device = Pa_GetDefaultInputDevice();
	if (inputParameters.device == paNoDevice) {
		fprintf(stderr, "Error: No default input device.\n");
		return 0;
	}
	inputParameters.channelCount = 1;
	inputParameters.sampleFormat = SAMPLE_FORMAT | paNonInterleaved;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	outputParameters.device = Pa_GetDefaultOutputDevice();
	if (outputParameters.device == paNoDevice) {
		fprintf(stderr, "Error: No default output device.\n");
		return 0;
	}
	outputParameters.channelCount = 1;
	outputParameters.sampleFormat = SAMPLE_FORMAT | paNonInterleaved;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	*err = Pa_OpenStream(stream,
			&inputParameters,
			&outputParameters,
			SAMPLE_RATE,
			INPUT_BUFFER_SIZE,
			paClipOff,
			wireCallback,
			userData);

	return 1;
}