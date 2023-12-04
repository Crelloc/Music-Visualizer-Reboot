FROM amd64/ubuntu:22.04

ENV DEBIAN_FRONTEND noninteractive
#ENV XDG_RUNTIME_DIR="/tmp"
#ENV DBUS_SESSION_BUS_ADDRESS="unix:path=/tmp/bus"

# Install package dependencies
# Using PipeWire instead of pulseaudio:
# References:
# https://gist.github.com/the-spyke/2de98b22ff4f978ebf0650c90e82027e?permalink_comment_id=3976309
# https://stackoverflow.com/questions/28985714/run-apps-using-audio-in-a-docker-container/75775875#75775875
# https://leimao.github.io/blog/Docker-Container-Audio/

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
#	wireplumber \
#	pipewire-media-session- \
#	pipewire-audio-client-libraries \
#	libldacbt-{abr,enc}2 \
#	libspa-0.2-bluetooth \
#	pulseaudio-module-bluetooth- \
        libsdl2-2.0-0 && \
    apt-get clean

WORKDIR /opt/visualizer

COPY . ./

RUN tar -zxvf ./fftw-3.3.4.tar.gz && \
	cd fftw-3.3.4 && \
	./configure && make && make install

RUN mkdir Music

RUN make

