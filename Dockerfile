FROM amd64/ubuntu:22.04

ENV DEBIAN_FRONTEND noninteractive

# Install package dependencies
RUN apt-get update -y && \
    apt-get install -y --no-install-recommends \
	make \
	cmake \
        pkg-config \
        gdb \
	gcc \
        libsdl2-dev \
	libasound2-dev \
	libpulse-dev \
        bash \
        neovim \
	pulseaudio \
        alsa-utils \
        pulseaudio \
	ffmpeg \
        libsdl2-2.0-0 && \
    apt-get clean

WORKDIR /visualizer

COPY . ./

RUN tar -zxvf ./fftw-3.3.4.tar.gz && \
	cd fftw-3.3.4 && \
	./configure && make && make install

RUN mkdir Music

RUN make

