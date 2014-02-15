Preparation
-----
* brew install portaudio
* brew install fftw

Usage
-----
g++ portaudio-test.cpp fftw/libfftw3.a portaudio/libportaudio.a -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreServices
