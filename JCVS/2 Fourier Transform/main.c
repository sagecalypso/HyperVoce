#include "../portaudio.h"
#include "../pa_linux_alsa.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define IO_BUFFER_SIZE 1024
#define CIRCULAR_BUFFER_SIZE 512
#define WINDOW_SIZE 641
#define SAMPLE_RATE 44100
#define SAMPLE_FORMAT paInt16
#define SAMPLE_TYPE int16_t

#include "circularBuffer.h"

const u_int16_t WINDOW_N = (WINDOW_SIZE - 1) / 2;

int direction = 1;
float lastMean = 0;
int16_t lastGCI = 0;
float freq = -1;

float windowFunction[WINDOW_SIZE];

void fillBlackmanWindow(float wFunc[WINDOW_SIZE]);

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
	circularBuffer cBuf; circularBufferInit(&cBuf);
	fillBlackmanWindow(windowFunction);

	printf("sample rate:\t\t%d Hz\n", SAMPLE_RATE);
	printf("input buffer size:\t%d samples\n", IO_BUFFER_SIZE);
	printf("circular buffer size:\t%d samples\n", CIRCULAR_BUFFER_SIZE);
	printf("lowest frequency:\t%f Hz\n", (float)(SAMPLE_RATE) / (float)(CIRCULAR_BUFFER_SIZE));
	printf("minimum latency:\t%f ms\n", (float)(IO_BUFFER_SIZE * 3000) / (float)(SAMPLE_RATE));
	printf("\n");

	err = Pa_Initialize();
	if (err != paNoError) goto error;

	if (!openDefaultDuplexStream(&stream, &cBuf, &err)) goto error;
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
	circularBuffer* cBuf;
	long li;
	int16_t i, j, k, meanCount;
	float meanSum, curMean;

	if ( inputBuffer == NULL || outputBuffer == NULL ) return paContinue;

	in = ((SAMPLE_TYPE**)inputBuffer)[0];
	out = ((SAMPLE_TYPE**)outputBuffer)[0];
	cBuf = ((circularBuffer*)userData);

//	circularBufferWrite(cBuf, in);

	freq = -1;
	int framesElapsed;

	for (i = 0; i < framesPerBuffer; i++) {
		//determine the mean-based signal at this point
		meanSum = 0.;
		meanCount = 0;
		for (j = 0; j < WINDOW_SIZE; j++) {
			k = i + j - WINDOW_N;
			if (0 <= k && k < framesPerBuffer) {
				meanSum += windowFunction[j] * in[k];
				meanCount++;
			}
		}
		curMean = meanSum / (float)(meanCount);

		//determine if the mean-based signal changed direction
		if (direction) {
			if (lastMean >= curMean) {//change slope downward
				direction = 0;
				if (curMean >= 0.003) {
					framesElapsed = (int)(i) - lastGCI;
					lastGCI = i;
					while (framesElapsed <= 0)
						framesElapsed += framesPerBuffer;
					if (freq == -1)
						freq = (float)(SAMPLE_RATE) / (float)(framesElapsed);
					else
						freq = (.01 * (float)(SAMPLE_RATE) / (float)(framesElapsed) + 1.99 * freq) / 2.;
				}
			}
		}
		else {
			if (lastMean <= curMean) {//change slope upward
				direction = 1;
			}
		}
		lastMean = curMean;
	}

	if (freq != -1) printf("%f\n", freq);

	//write the input buffer to the output buffer
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
			IO_BUFFER_SIZE,
			paClipOff,
			wireCallback,
			userData);

	return 1;
}

void fillBlackmanWindow(float wFunc[WINDOW_SIZE]) {
	u_int16_t n;
	float nOverM;

	for (n = 0; n < WINDOW_SIZE; n++) {
		nOverM = (float)(n) / (float)(WINDOW_SIZE);
		wFunc[n] = 0.42 - 0.5*cos(2*M_PI*nOverM) + 0.08*cos(4*M_PI*nOverM);
	}
}