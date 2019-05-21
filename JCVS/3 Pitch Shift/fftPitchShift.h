#ifndef FFT_PITCH_SHIFT_H
#define FFT_PITCH_SHIFT_H

#include <math.h>
#include <stdio.h>


void fftPitchShift(float X_Re[FFT_N], float X_Im[FFT_N], float intervalRatio) {
    //shift the given fft frequency spectrum to change the pitch of any notes
    u_int16_t pivotFreq = FFT_N / 2;
    u_int16_t i, from_i, to_i, stop_i;
    float m;

    if (intervalRatio < 1.) {//shift the pitch down
        stop_i = (float)(pivotFreq) * intervalRatio;//lowest mappable frequency bin

        //clear the destination frequency bins
        for (i = 1; i < stop_i; i++) {
            X_Re[i] = X_Im[i] = 0;
            X_Re[FFT_N - i] = X_Im[FFT_N - i] = 0;
        }

        //map the upper frequencies down
        for (from_i = pivotFreq; from_i >= stop_i; from_i--) {
            to_i = round((float)(from_i) * intervalRatio);

            X_Re[to_i] += X_Re[from_i] / intervalRatio;
            X_Im[to_i] += X_Im[from_i] / intervalRatio;

//            X_Re[FFT_N - to_i] += X_Re[FFT_N - from_i] * intervalRatio;
//            X_Im[FFT_N - to_i] += X_Im[FFT_N - from_i] * intervalRatio;

            //clear the upper frequencies
            X_Re[from_i] = 0;
            X_Im[from_i] = 0;

            X_Re[FFT_N - from_i] = 0;
            X_Im[FFT_N - from_i] = 0;
        }
    }
    else if (intervalRatio >= 1.) {//shift the pitch up
        m = 1. / intervalRatio;

        for (to_i = pivotFreq; to_i >= 2; to_i--) {
            from_i = round((float)(to_i) * m);

            X_Re[to_i] = X_Re[from_i] * m;
            X_Im[to_i] = X_Im[from_i] * m;

            X_Re[FFT_N - to_i] = 0;// = X_Re[FFT_N - from_i] * m;
            X_Im[FFT_N - to_i] = 0;// = X_Im[FFT_N - from_i] * m;
        }

        for (i = 1; i < 2; i++) {
            X_Re[i] = 0;
            X_Im[i] = 0;

            X_Re[FFT_N - i] = 0;
            X_Im[FFT_N - i] = 0;
        }
    }
}

#endif