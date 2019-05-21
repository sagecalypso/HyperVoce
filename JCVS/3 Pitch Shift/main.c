#include "../portaudio.h"
#include "../pa_linux_alsa.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 64
#define FFT_N 1024
#define FFT_LOG2N 10
#define SAMPLE_RATE 44100
#define SAMPLE_FORMAT paFloat32
#define SAMPLE_TYPE float
#define FOURIER_TYPE float

#include "fourierBuffer.h"
#include "fastFourierTransform.h"
#include "fftPitchShift.h"

FOURIER_TYPE Y_Re[FFT_N];
FOURIER_TYPE Y_Im[FFT_N];
FOURIER_TYPE X_Re[FFT_N];
FOURIER_TYPE X_Im[FFT_N];

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
	fourierBuffer fourBuf; fourierBufferInit(&fourBuf);

	fftGenerateLookupTables();

	err = Pa_Initialize();
	if (err != paNoError) goto error;

	if (!openDefaultDuplexStream(&stream, &fourBuf, &err)) goto error;
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
	fourierBuffer* fBuf;
	long i;
	u_int16_t i2;

	if ( inputBuffer == NULL || outputBuffer == NULL ) return paContinue;

	in = ((SAMPLE_TYPE**)inputBuffer)[0];
	out = ((SAMPLE_TYPE**)outputBuffer)[0];
	fBuf = (fourierBuffer*)userData;

	//write the input data to a fancy buffer
	fourierBufferWrite(fBuf, in);

	//get the fast fourier transform of the entire circular buffer
	rfft(fBuf->inputBuffer, Y_Re, Y_Im, fBuf->readWritePos);

	u_int16_t maxFreq = 1;
	float maxAmp = sqrt(Y_Re[1]*Y_Re[1] + Y_Im[1]*Y_Im[1]), amp;

	for (i2 = 1; i2 <= FFT_N / 2; i2++) {
		amp = sqrt(Y_Re[i2]*Y_Re[i2] + Y_Im[i2]*Y_Im[i2]);
		if (amp > maxAmp) {
			maxFreq = i2;
			maxAmp = amp;
		}

//		Y_Re[i2] = amp * .5;
//		Y_Im[i2] = 0;
	}

//	for (; i2 <= FFT_N; i2++)
//		Y_Re[i2] = Y_Im[i2] = 0;
//	Y_Re[0] = Y_Im[0] = 0;

	if (maxAmp >= 1.4) {
//		Y_Re[maxFreq] = Y_Im[maxFreq] = sqrt(2.) * maxAmp;
//		for (i2 = 1; i2 <= FFT_N / 2; i2++)
//			if (i2 < maxFreq || i2 / 2. - maxFreq / 2. > 300) {
//				Y_Re[i2] = Y_Im[i2] = 0;
//				Y_Re[FFT_N - i2] = Y_Im[FFT_N - i2] = 0;
//			}
		printf("%d\t%f\t%f\n", maxFreq, (float)(maxFreq) * (float)(SAMPLE_RATE) / (float)(FFT_N), maxAmp);
	}

	//get the inverse fft of the frequency data
	ifft(Y_Re, Y_Im, X_Re, X_Im);

	//write the phase-corrected inverse fft to the output buffer
	i2 = fBuf->readWritePos;
	for (i = 0; i < framesPerBuffer; i++) {
		*out++ = X_Re[i];
		if (++i2 >= FFT_N) i2 = 0;
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
