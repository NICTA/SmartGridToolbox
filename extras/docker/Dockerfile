# Dockerfile for SmartGridToolbox development and testing on Debian Stretch.
#
# To build:
# docker build -t sgt-debian-stretch .

FROM debian:stretch

RUN DEBIAN_FRONTEND=noninteractive apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends pkg-config

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends apt-utils
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends automake
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends build-essential
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends ca-certificates
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends cmake
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends gfortran
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends git
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends libboost-all-dev
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends libblas-dev
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends liblapack-dev
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends libtool
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends ssh
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends sudo

# Set up user and group
RUN groupadd -g 999 sgt
RUN useradd -r -u 999 -g sgt sgt
RUN mkdir /home/sgt
RUN chown sgt:sgt /home/sgt
RUN adduser sgt sudo
RUN echo "sgt ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

USER sgt

RUN mkdir /home/sgt/src
WORKDIR /home/sgt/src

RUN git clone --depth 1 --recurse-submodules https://github.com/NICTA/SmartGridToolbox.git

WORKDIR /home/sgt/src/SmartGridToolbox/third_party
RUN ./install_third_party.sh g++ /usr/local

WORKDIR /home/sgt/src/SmartGridToolbox
    
RUN ./autogen.sh
RUN ./configure
RUN make -j2

USER root

RUN make install

USER sgt

ENV LD_LIBRARY_PATH /usr/local/lib

WORKDIR /home/sgt
