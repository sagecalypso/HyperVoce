/*
//
// fastFourierTransform.h
//
// Author: Weston Cook
//
// Distributed under the Mozilla Public License 2.0
//
// Description:
//  Defines
//
*/
#ifndef FAST_FOURIER_TRANSFORM_H
#define FAST_FOURIER_TRANSFORM_H


#include <stdlib.h>
#include "fftLookupTable.h"
//#include "fourierBuffer.h"


void fftDITbutterfly(float[], float[], u_int16_t);

void fft(float[FFT_N], float[FFT_N], float[FFT_N], float[FFT_N]);

void ifft(float[FFT_N], float[FFT_N], float[FFT_N], float[FFT_N]);

void rfft(float[FFT_N], float[FFT_N], float[FFT_N], u_int16_t)


/*
//
// void fftDITbutterfly(float[], float[], u_int16_t)
//
// Description:
//  Computes the radix-2 decimation in time FFT butterfly in-place using recursion.
//
// Arguments:
//  - float X_Re[] -- Real component of input array
//  - float X_Im[] -- Imaginary component of input array
//  - u_int16_t N -- Size of butterfly operation to perform
//
*/
void fftDITbutterfly(float X_Re[], float X_Im[], u_int16_t N)
{
  float tempVal_Re, tempVal_Im, tempVal2;
  u_int16_t halfN, W_stepSize, i, i2, temp_i;

  if (N == 2) {  // Base case
    // Combine the two single-point frequency-domain values

    tempVal_Re = X_Re[0] - X_Re[1];
    tempVal_Im = X_Im[0] - X_Im[1];

    X_Re[0] = X_Re[0] + X_Re[1];
    X_Im[0] = X_Im[0] + X_Im[1];

    X_Re[1] = tempVal_Re;
    X_Im[1] = tempVal_Im;
  }
  else {
    halfN = N / 2;  // Size of the arrays that are about to be merged
    W_stepSize = FFT_N / N;  // How far each index in the larger array goes in the weights LUT

    // Combine the four frequency domain spectra into two frequency domain spectra
    fftDITbutterfly(X_Re, X_Im, halfN);  // Perform butterfly on first half
    fftDITbutterfly(X_Re + halfN, X_Im + halfN, halfN);  // Perform butterfly on second half

    i2 = halfN;  // Second half of the array
    for (i = 0; i < halfN; i++) {
      // Perform the butterfly operation on the individual values of the two sub-arrays
      temp_i = (i * W_stepSize) % FFT_N;  // W value index
      tempVal_Re = X_Re[i] + COS_LUT[temp_i] * X_Re[i2] + SIN_LUT[temp_i] * X_Im[i2];
      tempVal_Im = X_Im[i] + COS_LUT[temp_i] * X_Im[i2] - SIN_LUT[temp_i] * X_Re[i2];

      temp_i = (i2 * W_stepSize) % FFT_N;  // W value index
      tempVal2 = X_Re[i] + COS_LUT[temp_i] * X_Re[i2] + SIN_LUT[temp_i] * X_Im[i2];
      X_Im[i2] = X_Im[i] + COS_LUT[temp_i] * X_Im[i2] - SIN_LUT[temp_i] * X_Re[i2];
      X_Re[i2] = tempVal2;

      X_Re[i] = tempVal_Re;
      X_Im[i] = tempVal_Im;

      i2++;  // Update corresponding value on second half
    }
  }
}


/*
//
// void fft(float[FFT_N], float[FFT_N], float[FFT_N], float[FFT_N])
//
// Description:
//  Computes the complex FFT (using decimation in time) of the X arrays and places it in the Y arrays.
//
// Arguments:
//  - float X_Re[FFT_N] -- Real component of input buffer
//  - float X_Im[FFT_N] -- Imaginary component of input buffer
//  - float Y_Re[FFT_N] -- Real component of output buffer
//  - float Y_Im[FFT_N] -- Imaginary component of output buffer
//
*/
void fft(float X_Re[FFT_N], float X_Im[FFT_N], float Y_Re[FFT_N], float Y_Im[FFT_N])
{
  u_int16_t i;

  // Copy the input buffers to the output buffers while sorting the data in reverse-bit order
  for (i = 0; i < FFT_N; i++) {
    Y_Re[RBO_LUT[i]] = X_Re[i];
    Y_Im[RBO_LUT[i]] = X_Im[i];
  }

  // Apply butterfly operations on output buffers
  fftDITbutterfly(Y_Re, Y_Im, FFT_N);
}


/*
//
// void ifft(float[FFT_N], float[FFT_N], float[FFT_N], float[FFT_N])
//
// Description:
//  Computes the inverse complex FFT (using decimation in time) of the X arrays and places it in the Y arrays.
//
// Arguments:
//  - float X_Re[FFT_N] -- Real component of input buffer
//  - float X_Im[FFT_N] -- Imaginary component of input buffer
//  - float Y_Re[FFT_N] -- Real component of output buffer
//  - float Y_Im[FFT_N] -- Imaginary component of output buffer
//
*/
void ifft(float X_Re[FFT_N], float X_Im[FFT_N], float Y_Re[FFT_N], float Y_Im[FFT_N])
{
  u_int16_t i;
  float scale = 1. / (float)(FFT_N);  // Scaling factor of the output

  // Copy the input buffers to the output buffers while sorting the data in reverse-bit order
  for (i = 0; i < FFT_N; i++) {
    Y_Re[RBO_LUT[i]] = X_Re[i];
    Y_Im[RBO_LUT[i]] = -X_Im[i];  // Conjugate the input
  }

  // Apply butterfly operations on the output buffers
  fftDITbutterfly(Y_Re, Y_Im, FFT_N);

  // Conjugate and scale down the output
  for (i = 0; i < FFT_N; i++) {
    Y_Re[i] *= scale;
    Y_Im[i] *= -scale;
  }
}


/*
//
// void rfft(float[FFT_N], float[FFT_N], float[FFT_N], u_int16_t)
//
// Description:
//  Computes the complex FFT (using decimation in time) of the X array and places it in the Y arrays.
//
// Arguments:
//  - float X_Re[FFT_N] -- Real component of input buffer
//  - float Y_Re[FFT_N] -- Real component of output buffer
//  - float Y_Im[FFT_N] -- Imaginary component of output buffer
//  - u_int16_t startPos -- Initial position of read/write head (reads from circular buffer)
//
*/
void rfft(float X_Re[FFT_N], float Y_Re[FFT_N], float Y_Im[FFT_N], u_int16_t startPos) {
  u_int16_t i;

  // Copy the input buffers to the output buffers while sorting the data in reverse-bit order
  for (i = 0; i < FFT_N; i++) {
    Y_Re[RBO_LUT[i]] = X_Re[startPos];
    Y_Im[i] = 0.;

    // Return to beginning of buffer if out of range
    if (++startPos >= FFT_N)
      startPos = 0;
  }

  // Apply butterfly operations
  fftDITbutterfly(Y_Re, Y_Im, FFT_N);
}

#endif
