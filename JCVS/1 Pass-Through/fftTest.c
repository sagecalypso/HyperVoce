#include <stdlib.h>
#include "fftTest.h"

int main() {
	u_int16_t N = 8;
	float REX[8] = {1, 2, 1, 2, 1, 2, 1, 2};
	float IMX[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	fft(N, REX, IMX);

	return 0;
}
