import numpy as np
from scipy.fftpack import rfft, irfft

class SampleQueue(object):
	def __init__(self, fftResolution, bufferSize, dtype):
		"""Stores a collection of small buffers and computes the fft of the collection as a whole"""
		self.__fftRes__ = fftResolution
		self.__bufSize__ = bufferSize
		self.__sampleDtype__ = dtype

		self.__ifftStartIndex__ = fftResolution - bufferSize

		self.__bufCount__ = int(np.ceil(fftResolution / bufferSize))
		self.__buffers__ = [np.zeros(bufferSize, dtype=dtype) for i in range(self.__bufCount__)]
	def push(self, buf):
		"""Appends a small buffer to the end of the large buffer"""
		self.__buffers__.append(np.frombuffer(buf, self.__sampleDtype__))
		self.__buffers__.pop(0)
	def fft(self):
		"""Gets the fft of the entire buffer"""
		return rfft(np.concatenate(self.__buffers__), n=self.__fftRes__)
	def ifft(self, ft):
		"""Gets the inverse fft of the given Fourier transform"""
		return irfft(ft).astype(self.__sampleDtype__)[self.__ifftStartIndex__:].tobytes()
