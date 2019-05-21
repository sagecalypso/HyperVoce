#ifndef FFT_LOOKUP_TABLE_H
#define FFT_LOOKUP_TABLE_H

#include <stdlib.h>
#include <math.h>

#ifndef FFT_N
#error "FFT_N (FFT resolution) not defined."
#endif
#ifndef FFT_LOG2N
#error "FFT_LOG2N (log2(FFT resolution)) not defined."
#endif

u_int16_t indices_LUT[FFT_N];
float W_Re_LUT[FFT_N];
float W_Im_LUT[FFT_N];

u_int16_t reverseBits(u_int16_t n, u_int16_t bitCount) {
	u_int16_t out = 0;
	u_int16_t i;

	for (i = 0; i < bitCount; i++)
		if (n & (1 << i))
			out |= 1 << (bitCount - 1 - i);

	return out;
}


void fftGetIndexLookupTable(void) {
	//fill in the global array to create a lookup table of input indices for quick sorting
	u_int16_t i;

	for (i = 0; i < FFT_N; i++)
		indices_LUT[i] = reverseBits(i, FFT_LOG2N);
}

void fftGetWLookupTable(void) {
	//fill in the global distributed complex array to create a lookup table of W values
	u_int16_t i;
	float angle;

	for (i = 0; i < FFT_N; i++) {
		angle = 6.283185307179586 * (float)(i) / (float)(FFT_N);
		W_Re_LUT[i] = cos(angle);
		W_Im_LUT[i] = -sin(angle);
	}
}

void fftGenerateLookupTables(void) {
	//generate necessary lookup tables
	fftGetIndexLookupTable();
	fftGetWLookupTable();
}

#endif