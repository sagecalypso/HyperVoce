# HyperVoce
HyperVoce is a real time vocoder/voice duplicator inspired by Jacob Collier's iconic choral synthesizer. It uses a MIDI input stream to transform the carrier voice to imitate a choir.

### Modes of Operation
HyperVoce has three distinct modes of operation:
  - Voice duplicator
  - Group vocoder
  - Independent vocoder

#### Voice Duplicator mode
HyperVoce polyphonically transposes the carrier voice according to each of the MIDI input pitches.

#### Group Vocoder
HyperVoce uses the transposed formants of the carrier to vocode an external audio source.

#### Independent Vocoder
HyperVoce uses the transposed formants of the carrier to independently vocode the voices of an internal multi-voice synthesizer.

## Dependencies
HyperVoce requires the following libraries for compilation:
  - <a href="www.portaudio.com">PortAudio</a>

## Compiling
To get started with HyperVoce, grab a copy of the code and compile the files in the "src" directory. These are each independent main executables for each of the modes of operation. This software has been written and tested using Ubuntu, though it's dependencies were selected with the goal of platform compatibility. See <a href="www.portaudio.com">PortAudio</a>'s cross-platform compatibility details for more information.

## Interested in Contributing?
If you would like to contribute to this project, see <a href="https://github.com/wwilliamcook/HyperVoce/blob/master/CONTRIBUTING.md">CONTRIBUTING</a> for guidelines on how to get started.
