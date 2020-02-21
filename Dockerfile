FROM ubuntu:19.04

SHELL ["/bin/bash", "-c"]
# install sys requirements
RUN apt-get update
RUN cat /etc/*release
RUN apt-get install -y gnupg software-properties-common

RUN apt-get install -y build-essential \
    && rm -rf /var/lib/apt/lists/* \
    && apt-get upgrade
RUN add-apt-repository ppa:ubuntu-toolchain-r/ppa \
    && apt-get update \
    && apt-get -y install gcc-9 g++-9

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 60 --slave /usr/bin/g++ g++ /usr/bin/g++-9 \
    && yes '' | update-alternatives --force --all

RUN /usr/bin/gcc -v && /usr/bin/g++ -v

RUN apt-get install -y build-essential git wget cmake

WORKDIR .
RUN pwd
#################################################
# AlphaZeroEnvironment
#################################################
COPY . /aze
WORKDIR aze
RUN ls -a deps
RUN git submodule init \
    && git submodule update
RUN chmod u+x ./deps/dl_libtorch.sh

#RUN ls
RUN mkdir build
WORKDIR build
RUN cmake -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/root/install
RUN make -j4
RUN make install
