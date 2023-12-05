FROM amd64/ubuntu:22.04

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update -y && \
    apt-get install -y --no-install-recommends \
	make \
        pkg-config \
        gdb \
	gcc \
        libsdl2-dev \
	libpipewire-0.3-dev \
        libspa-0.2-dev \
        bash \
        neovim \
	ffmpeg \
	pipewire \
        libsdl2-2.0-0 && \
    apt-get clean

WORKDIR /opt/visualizer

COPY . ./

RUN tar -zxvf ./fftw-3.3.4.tar.gz && \
	cd fftw-3.3.4 && \
	./configure && make && make install

RUN mkdir Music

RUN make

