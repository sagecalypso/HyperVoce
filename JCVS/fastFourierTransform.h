#ifndef FAST_FOURIER_TRANSFORM_H
#define FAST_FOURIER_TRANSFORM_H

#include <stdlib.h>
#include "fftLookupTable.h"

void fftSortInput(float X_Re[FFT_N], float X_Im[FFT_N], float Y_Re[FFT_N], float Y_Im[FFT_N]) {
  //sort the given complex input using reversed bit order
  u_int16_t i;

  for (i = 0; i < FFT_N; i++) {
    Y_Re[indices_LUT[i]] = X_Re[i];
    Y_Im[indices_LUT[i]] = X_Im[i];
  }
}
void ifftSortInput(float X_Re[FFT_N], float X_Im[FFT_N], float Y_Re[FFT_N], float Y_Im[FFT_N]) {
  //conjugate the given complex input and sort it using reversed bit order
  u_int16_t i;

  for (i = 0; i < FFT_N; i++) {
    Y_Re[indices_LUT[i]] = X_Re[i];
    Y_Im[indices_LUT[i]] = -X_Im[i];
  }
}
void rfftSortInput(float X_Re[FFT_N], float Y_Re[FFT_N], float Y_Im[FFT_N]) {
  //sort the given real input using reversed bit order and clear the imaginary output array
  u_int16_t i;

  for (i = 0; i < FFT_N; i++) {
    Y_Re[indices_LUT[i]] = X_Re[i];
    Y_Im[i] = 0;
  }
}

void fftButterfly(float X_Re[], float X_Im[], u_int16_t N) {
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
    fftButterfly(X_Re, X_Im, Nd2);
    fftButterfly(X_Re+Nd2, X_Im+Nd2, Nd2);

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
void ifftButterfly(float X_Re[], float X_Im[], u_int16_t N) {
  //compute the radix-2 IFFT butterfly in-place using recursion (and negate the imaginary value of W)
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
    ifftButterfly(X_Re, X_Im, Nd2);
    ifftButterfly(X_Re+Nd2, X_Im+Nd2, Nd2);

    i2 = Nd2;//second half of the array
    for (i = 0; i < Nd2; i++) {
      //perform the butterfly operation on the individual values of the two sub-arrays
      temp_i = (i * W_stepSize) % FFT_N;//W value index
      tempVal_Re = X_Re[i] + W_Re_LUT[temp_i] * X_Re[i2] + W_Im_LUT[temp_i] * X_Im[i2];
      tempVal_Im = X_Im[i] + W_Re_LUT[temp_i] * X_Im[i2] - W_Im_LUT[temp_i] * X_Re[i2];

      temp_i = (i2 * W_stepSize) % FFT_N;//W value index
      tempVal2 = X_Re[i] + W_Re_LUT[temp_i] * X_Re[i2] + W_Im_LUT[temp_i] * X_Im[i2];
      X_Im[i2] = X_Im[i] + W_Re_LUT[temp_i] * X_Im[i2] - W_Im_LUT[temp_i] * X_Re[i2];
      X_Re[i2] = tempVal2;

      X_Re[i] = tempVal_Re;
      X_Im[i] = tempVal_Im;

      i2++;//update corresponding value on second half
    }
  }
}

void fft(float X_Re[FFT_N], float X_Im[FFT_N], float Y_Re[FFT_N], float Y_Im[FFT_N]) {
  //compute the complex FFT (using decimation in time) of the X arrays and place it in the Y arrays
  fftSortInput(X_Re, X_Im, Y_Re, Y_Im);//put the input in reversed bit order
  fftButterfly(Y_Re, Y_Im, FFT_N);//perform all butterfly computations
}
void ifft(float X_Re[FFT_N], float X_Im[FFT_N], float Y_Re[FFT_N], float Y_Im[FFT_N]) {
  //compute the inverse complex FFT of the X arrays and place it in the Y arrays
  u_int16_t i;
  float floatFFT_N = (float)FFT_N;

  fftSortInput(X_Re, X_Im, Y_Re, Y_Im);//conjugate the input and put it in reversed bit order
  ifftButterfly(Y_Re, Y_Im, FFT_N);//perform all butterfly computations
  //divide the output by FFT_N
  for (i = 0; i < FFT_N; i++) {
    Y_Re[i] /= floatFFT_N;
    Y_Im[i] /= floatFFT_N;
  }
}
void rfft(float X_Re[FFT_N], float Y_Re[FFT_N], float Y_Im[FFT_N]) {
  //compute the real FFT (using decimation in time) of the X arrays and place it in the Y arrays
  rfftSortInput(X_Re, Y_Re, Y_Im);//put the input in reversed bit order
  fftButterfly(Y_Re, Y_Im, FFT_N);//perform all butterfly computations
}

#endif