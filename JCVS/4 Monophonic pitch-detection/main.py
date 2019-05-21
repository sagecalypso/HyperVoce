#!/usr/bin/python3

import pyaudio
import numpy as np
from sampleQueue import SampleQueue

CHUNK = 8
WIDTH = 2
CHANNELS = 1
RATE = 44100
FFT_RESOLUTION = 4

NP_SAMPLE_DTYPE = {1: np.uint8, 2: np.uint16, 4: np.uint32, 8: np.uint64}[WIDTH]

LOWEST_FFT_INDEX = int(20 * FFT_RESOLUTION / RATE * 2)
def getFreq(fftIndex):
	return int(np.ceil(fftIndex / 2)) * RATE / FFT_RESOLUTION
def getIndex(freq):
	return int(freq * FFT_RESOLUTION / RATE * 2)

def pitchShift(ft, shift):
	out = np.roll(ft, shift)
	out[0:shift] = 0
	return out

#initialize pyaudio
p = pyaudio.PyAudio()

#create a sample queue for fft
q = SampleQueue(FFT_RESOLUTION, CHUNK, NP_SAMPLE_DTYPE)

#set up a stream for input and output
stream = p.open(format=p.get_format_from_width(WIDTH),
		channels=CHANNELS,
		rate=RATE,
		input=True,
		output=True,
		frames_per_buffer=CHUNK)

print("* recording")

try:
	while True:
		data = stream.read(CHUNK)
		q.push(data)
		ft = q.fft()
		print(len(ft))
		stream.write(q.ifft(ft), CHUNK) #play back the inverse fft of the fft
except KeyboardInterrupt:
	print("* stopped recording")
finally:
	#stop and close the stream
	stream.stop_stream()
	stream.close()
	#shut down pyaudio
	p.terminate()
