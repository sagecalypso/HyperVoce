#include "../include/portaudio.h"
#include "../include/pa_linux_alsa.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 32
#define FOURIER_RESOLUTION 4800
#define SAMPLE_RATE 48000
#define SAMPLE_FORMAT paInt16
#define SAMPLE_TYPE u_int16_t

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

	err = Pa_Initialize();
	if (err != paNoError) goto error;

	if (!openDefaultDuplexStream(&stream, NULL, &err)) goto error;
	if (err != paNoError) goto error;

	err = Pa_StartStream(stream);
	if (err != paNoError) goto error;

	printf("Now recording and playing. - Hit ENTER to exit.");
	getchar();

	err = Pa_StopStream(stream);
	if (err != paNoError) goto error;
	err = Pa_CloseStream(stream);
	if (err != paNoError) goto error;

	goto done;

done:
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
	long i;

	if ( inputBuffer == NULL || outputBuffer == NULL ) return paContinue;

	in = ((SAMPLE_TYPE**)inputBuffer)[0];
	out = ((SAMPLE_TYPE**)outputBuffer)[0];

	for (i = 0; i < framesPerBuffer; i++) {
		*out++ = *in++;
	}

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
			BUFFER_SIZE,
			paClipOff,
			wireCallback,
			userData);

	return 1;
}
