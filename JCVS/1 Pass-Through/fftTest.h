#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void fft(u_int16_t N, float rex[], float imx[]) {
	//fast Fourier transform
	u_int16_t i, j, k, l;
	float tr, ti, le, le2;
	//set constants
	u_int16_t Nm1 = N - 1;
	u_int16_t Nd2 = N / 2;
	float M = log(N) / log(2);
	j = Nd2;
	//bit reversal sorting
	for (i = 1; i < N - 1; i++) {
l1120:
		if (i >= j) goto l1190;
		tr = rex[j];
		ti = imx[j];
		rex[j] = rex[i];
		imx[j] = imx[i];
		rex[i] = tr;
		imx[i] = ti;
l1190:
		k = Nd2;
l1200:
		if (k > j) goto l1240;
		j = j - k;
		k = k / 2;
		goto l1200;
l1240:
		j = j + k;
	}
	//print rex and imx
	printf("REX:\n");
	for (i = 0; i < N; i++)
		printf("%f\t", rex[i]);
	printf("\nIMX:\n");
	for (i = 0; i < N; i++)
		printf("%f\t", imx[i]);
	printf("\n");
}
