#!/usr/bin/python3

import pyaudio

CHUNK = 64 #buffer size
WIDTH = 2 #number of bytes per sample
CHANNELS = 1 #number of channels to record and play
RATE = 44100 #sample rate
RECORD_SECONDS = 10 #number of seconds to operate

#initialize pyaudio
p = pyaudio.PyAudio()

#set up an audio stream which has both input and output
stream = p.open(format=p.get_format_from_width(WIDTH),
		channels=CHANNELS,
		rate=RATE,
		input=True,
		output=True,
		frames_per_buffer=CHUNK)

try:
	print("* recording")

	while True: #for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
		data = stream.read(CHUNK) #read audio data
		stream.write(data, CHUNK) #write audio data

	print("* done recording")
except KeyboardInterrupt:
	print("* stopped recording")
finally:
	#stop and close the stream
	stream.stop_stream()
	stream.close()
	print("stream closed")

	#shut down pyaudio
	p.terminate()
	print("pyaudio terminated")
