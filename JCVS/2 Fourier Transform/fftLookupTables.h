/*
//
// fftLookupTables.h
//
// Author: Weston Cook
//
// Distributed under the Mozilla Public Licence 2.0
//
// Description:
//	Generates global arrays containing lookup tables of:
//		- Reverse-bit order sorting index-transform
//		- Cosine values with period of <FFT_N>
//		- Negative sine values with period <FFT_N>
//
*/
#ifndef VOCAL_SYNTH_LOOKUP_TABLES_H
#define VOCAL_SYNTH_LOOKUP_TABLES_H


/*
//
// Required preprocessor definitions.
//
// FFT_N -- Resolution of Fourier Transform
// FFT_LOG2N -- Log base 2 of FFT_N
//
*/
#ifndef FFT_N
#error "FFT_N (FFT resolution) not defined."
#endif
#ifndef FFT_LOG2N
#error "FFT_LOG2N (log2(FFT resolution)) not defined."
#endif


#include <stdlib.h>
#include <math.h>


const u_int16_t RBO_LUT[FFT_N];
const float COS_LUT[FFT_N];
const float NEG_SIN_LUT[FFT_N];


u_int16_t reverseBits(u_int16_t, u_int16_t);

void getRBOLookupTable();

void getSinCosLookupTable();

void generateLookupTables();


/*
//
// u_int16_t reverseBits(u_int16_t, u_int16_t)
//
// Description:
//	Reverses the bits of the given integer.
//
// Arguments:
//	- u_int16_t n -- integer whose bits to reverse
//	- u_int16_t bitCount -- number of bits in the byte to reverse
//
*/
u_int16_t reverseBits(u_int16_t n, u_int16_t bitCount) {
	u_int16_t out = 0;
	u_int16_t i;

	for (i = 0; i < bitCount; i++)
		if (n & (1 << i))
			out |= 1 << (bitCount - 1 - i);

	return out;
}


/*
//
// void getRBOLookupTable()
//
// Description:
//	Fills in the global array "RBO_LUT" as a reverse-bit-order lookup table for quick sorting.
//
*/
void getRBOLookupTable(void)
{
	u_int16_t i;

	for (i = 0; i < FFT_N; i++)
		RBO_LUT[i] = reverseBits(i, FFT_LOG2N);
}

/*
//
// void getSinCosLookupTable()
//
// Description:
//	Fills in the global distributed complex arrays as a lookup table of cosine and sine values with period of FFT_N.
//
*/
void getSinCosLookupTable(void)
{
	u_int16_t i;
	double angle;

	// Generate sine/cosine values for every element of the array, with a period spanning the array.
	for (i = 0; i < FFT_N; i++) {
		angle = 6.283185307179586 * (double)(i) / (double)(FFT_N);
		COS_LUT[i] = (float)(cosf32x(angle));
		NEG_SIN_LUT[i] = (float)(-sinf32x(angle));
	}
}


/*
//
// void generateLookupTables()
//
// Description:
//	Generates all lookup tables.
//
*/
void generateLookupTables(void)
{
	fftGetIndexLookupTable();
	fftGetWLookupTable();
}

#endif