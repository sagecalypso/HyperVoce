#ifndef FAST_FOURIER_TRANSFORM_H
#define FAST_FOURIER_TRANSFORM_H

#include <stdlib.h>
#include "fftLookupTable.h"
//#include "fourierBuffer.h"


void fftDITbutterfly(float X_Re[], float X_Im[], u_int16_t N) {
  //compute the radix-2 FFT butterfly in-place using recursion
  float tempVal_Re, tempVal_Im, tempVal2;
  u_int16_t Nd2, W_stepSize, i, i2, temp_i;

  if (N == 2) {//base case
    //combine the two single-point frequency-domain values
    tempVal_Re = X_Re[0] + X_Re[1];
    tempVal_Im = X_Im[0] + X_Im[1];

    X_Re[1] = X_Re[0] - X_Re[1];
    X_Im[1] = X_Im[0] - X_Im[1];

    X_Re[0] = tempVal_Re;
    X_Im[0] = tempVal_Im;
  }
  else {
    Nd2 = N / 2;//size of the arrays that are about to be merged
    W_stepSize = FFT_N / N;//how far each index in the larger array goes in the W LUT

    //combine the four frequency domain spectra into two frequency domain spectra
    fftDITbutterfly(X_Re, X_Im, Nd2);
    fftDITbutterfly(X_Re+Nd2, X_Im+Nd2, Nd2);

    i2 = Nd2;//second half of the array
    for (i = 0; i < Nd2; i++) {
      //perform the butterfly operation on the individual values of the two sub-arrays
      temp_i = (i * W_stepSize) % FFT_N;//W value index
      tempVal_Re = X_Re[i] + W_Re_LUT[temp_i] * X_Re[i2] - W_Im_LUT[temp_i] * X_Im[i2];
      tempVal_Im = X_Im[i] + W_Re_LUT[temp_i] * X_Im[i2] + W_Im_LUT[temp_i] * X_Re[i2];

      temp_i = (i2 * W_stepSize) % FFT_N;//W value index
      tempVal2 = X_Re[i] + W_Re_LUT[temp_i] * X_Re[i2] - W_Im_LUT[temp_i] * X_Im[i2];
      X_Im[i2] = X_Im[i] + W_Re_LUT[temp_i] * X_Im[i2] + W_Im_LUT[temp_i] * X_Re[i2];
      X_Re[i2] = tempVal2;

      X_Re[i] = tempVal_Re;
      X_Im[i] = tempVal_Im;

      i2++;//update corresponding value on second half
    }
  }
}

void fft(float X_Re[FFT_N], float X_Im[FFT_N], float Y_Re[FFT_N], float Y_Im[FFT_N]) {
  //compute the complex FFT (using decimation in time) of the X arrays and place it in the Y arrays
  u_int16_t i;

  //sort the complex input using reversed bit order
  for (i = 0; i < FFT_N; i++) {
    Y_Re[indices_LUT[i]] = X_Re[i];
    Y_Im[indices_LUT[i]] = X_Im[i];
  }

  //apply butterfly operations
  fftDITbutterfly(Y_Re, Y_Im, FFT_N);
}
void ifft(float X_Re[FFT_N], float X_Im[FFT_N], float Y_Re[FFT_N], float Y_Im[FFT_N]) {
  //compute the complex inverse FFT (using decimation in time) of the X arrays and place it in the Y arrays
  u_int16_t i;
  float m = 1. / (float)(FFT_N);

  //sort the complex input using reversed bit order
  for (i = 0; i < FFT_N; i++) {
    Y_Re[indices_LUT[i]] = X_Re[i];
    Y_Im[indices_LUT[i]] = -X_Im[i];//conjugate the input
  }

  //apply butterfly operations
  fftDITbutterfly(Y_Re, Y_Im, FFT_N);

  //conjugate and scale down the output
  for (i = 0; i < FFT_N; i++) {
    Y_Re[i] *= m;
    Y_Im[i] *= -m;
  }
}
void rfft(float X_Re[FFT_N], float Y_Re[FFT_N], float Y_Im[FFT_N], u_int16_t startPos) {
  //compute the complex FFT (using decimation in time) of the X array and place it in the Y arrays
  u_int16_t i;

  //sort the complex input using reversed bit order
  for (i = 0; i < FFT_N; i++) {
    Y_Re[indices_LUT[i]] = X_Re[startPos];
    Y_Im[i] = 0.;

    if (++startPos >= FFT_N)
      startPos = 0;
  }

  //apply butterfly operations
  fftDITbutterfly(Y_Re, Y_Im, FFT_N);
}

#endif
