Prepare
-----
* brew install portaudio
* brew install fftw

Usage
-----
g++ portaudio-test.cpp portaudio/libportaudio.a -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreServices