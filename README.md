# Music-Visualizer-Reboot 

  - Terminal-based Music Visualizer that displays the power spectrum of a .wav file
  - Music visualizer using SDL2 for audio play and FFTW for Fourier Transformation

![alt tag](https://github.com/Crelloc/Music-Visualizer-Reboot/blob/master/rebooted.gif)

Video Demonstration: https://www.youtube.com/watch?v=f6q0LmagfIU

# Motivation

  - Was planning to design and build a LED music visualizer as an Arduino project but wanted a better understanding of how music visualization software actually works.
  
Video Demonstration for LED music visualizer: https://www.youtube.com/watch?v=___XwMbhV4k

#Software

  - SDL2: https://wiki.libsdl.org/Installation
  - FFTW: http://www.fftw.org/download.html
  - pkg-config: https://www.freedesktop.org/wiki/Software/pkg-config/
  - avconv: https://libav.org/download/ if running debian based distro run "sudo apt-get install libav-tools"
  - GDB: for debugging on Linux
  - gcc: C compiler for Linux
  
#How To Build and Run:

##From terminal:

```bash
git clone https://github.com/Crelloc/Music-Visualizer-Reboot.git && cd Music-Visualizer-Reboot && make
```
##Synopsis:

```bash
./visual.exe [--file|-f] /PATH/TO/WAV/AUDIO/FILE

For example:

1. ./visual.exe --file ~/Music/Classic.wav
2. ./visual.exe -f ~/Music/Classic.wav
```
