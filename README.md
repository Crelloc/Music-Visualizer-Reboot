# Music-Visualizer-Reboot 

  - Terminal-based Music Visualizer that displays the power spectrum of a .wav file
  - Music visualizer using SDL2 for audio play and FFTW for Fourier Transformation

![alt tag](https://github.com/Crelloc/Music-Visualizer-Reboot/blob/master/rebooted.gif)

Video Demonstration: https://www.youtube.com/watch?v=YaeXZrTrX-Y&t=790s

For simpler version of this code, checkout:
https://github.com/Crelloc/terminal-music-visualizer

# Motivation

  - Was planning to design and build a LED music visualizer as an Arduino project but wanted a better understanding of how music visualization software actually works.
  
Video Demonstration for LED music visualizer: https://www.youtube.com/watch?v=___XwMbhV4k

# Software (prerequisites) to run the program using Docker containers
- Docker server: https://docs.docker.com/engine/install/#server
- Docker Compose: https://docs.docker.com/compose/install/linux/#install-the-plugin-manually

# Software (prerequisites) if not using Docker

  - SDL2: https://wiki.libsdl.org/SDL2/Installation
      - Note: make sure to install sdl2, alsa, and pulseaudio development (or any development files like pipewire that your systems uses to play sound ) files before installing SDL2.
  - FFTW: http://www.fftw.org/download.html
  - pkg-config: https://www.freedesktop.org/wiki/Software/pkg-config/
  - GDB: for debugging on Linux
  - gcc: C compiler for Linux
  - make: build automation tool
  - Optional (ffmpeg): will be used to convert audio to .wav


# How To Build and Run with Docker

Make sure you install docker and docker compose from the links given in the **Software (prerequisites) to run the program using Docker containers**


## Add path to your music folder on your host machine:

edit the [.env](.env) file:

the [.env](.env) file should be read by [docker-compose.yml](docker-compose.yml) by default

```bash
ENV_MUSIC_PATH="/home/some_user/Music"
```

Set path to music folder that has audio files.

Note: this visualizer runs .wav files. If you only have mp3 files
you can convert the mp3 files into .wav files using ffmpeg which is 
automatically installed inside the container using the command:

ffmpeg -i path/to/audio/file output/path/for/.wav

for example:

```bash
ffmpeg -i Music/song.mp3 Music/song.wav

```
**MAKE SURE THE FILE PATH AND NAME OF FILE DOESN'T HAVE SPACES AND SPECIAL CHARACTERS IN IT**

## Build and run docker image:

```bash
sudo docker compose build --no-cache && sudo docker compose up --force-recreate --remove-orphans -d
```
## Log into the container:

```bash
docker exec -ti visualizer-container bash

```

## Synopsis (run the visualizer):

./visual.exe [-f] /PATH/TO/WAV/AUDIO/FILE

For example:

```bash
./visual.exe -f Music/Classic.wav

```
Note: Supports only wav audio files and make sure that the "/PATH/TO/WAV/AUDIO/FILE" doesn't have spaces in it.

## Stop container:

```bash
sudo docker stop visualizer-container

```

## Troubleshooting audio not playing:

Check for the correct path of audio socket on your system:

```bash
pactl info | grep '^Server String';

```

and add the correct path in [docker-compose.yml](docker-compose.yml)


```yaml
...
volumes:
    ...
  - /run/user/1000/pulse/native:/tmp/pulse/native
...

```

Other troubleshooting references:
- https://gist.github.com/the-spyke/2de98b22ff4f978ebf0650c90e82027e?permalink_comment_id=3976309
- https://stackoverflow.com/questions/28985714/run-apps-using-audio-in-a-docker-container/75775875#75775875
- https://leimao.github.io/blog/Docker-Container-Audio/

# How To Build and Run without docker

## Install prerequisites:

### For Ubuntu (or debian-based):

```bash
sudo apt-get update && sudo apt-get install gdb gcc pkg-config make

```
if your system is using **pulseaudio** to play sound install the development files for
pulseaudio and alsa

```bash
sudo apt-get install libasound2-dev libpulse-dev 

```
if your system is using **pipewire** to play sound install the development files for
pipewire

```bash
sudo apt-get install libpipewire-0.3-dev libspa-0.2-dev 

```

Install sdl2 development files and sdl application

```bash
sudo apt-get install libsdl2-dev libsdl2-2.0-0

```
## clone repo and change directory into it:

```bash
git clone https://github.com/Crelloc/Music-Visualizer-Reboot.git && cd Music-Visualizer-Reboot

```
 ## Install fftw3.3.4.tar.gz from project directory or install updated version from http://www.fftw.org/download.html
```bash
tar -xvf fftw-3.3.4.tar.gz && \
cd fftw-3.3.4 && \
chmod +x ./configure && \
./configure && \
make && \
sudo make install


```
## Go back to project directory and reate executable for visualize.c

```bash
cd .. && make

```

## Synopsis:


```bash
./visual.exe [-f] /PATH/TO/WAV/AUDIO/FILE

For example:
./visual.exe -f ~/Music/Classic.wav
```
Note: Supports only wav audio files and make sure that the "/PATH/TO/WAV/AUDIO/FILE" doesn't have spaces in it.

# Brief overview about how the program works
  - Analyzes the digital information in the wav file.
  - Computes the fourier transformation every 'N' frames. Eg. computes the fourier transformation (FT) on 'N' L channel data and also computes the FT on 'N' R channel data.
  - Gathers frequency and magnitude information from the results of the fourier transformation and records it by storing into an array.
  - After analysis is finished the program will play music and display the corresponding information at the terminal.
