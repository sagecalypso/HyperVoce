#!/usr/bin/python3

import pyaudio
import time

CHUNK = 64
WIDTH = 2
CHANNELS = 1
RATE= 44100

#initialize pyaudio
p = pyaudio.PyAudio()

#declare callback function
def callback(in_data, frame_count, time_info, status):
	return (in_data, pyaudio.paContinue) #output the input data

#set up a stream for input and output
stream = p.open(format=p.get_format_from_width(WIDTH),
		channels=CHANNELS,
		rate=RATE,
		input=True,
		output=True,
		stream_callback=callback,
		frames_per_buffer=CHUNK)

stream.start_stream() #start the stream

#wait until the stream is no longer active
while stream.is_active():
	time.sleep(.1)

#stop and close the stream
stream.stop_stream()
stream.close()

#shut down pyaudio
p.terminate()
