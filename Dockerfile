# CLion remote docker environment (How to build docker container, run and stop it)
#
# Build and run:
#   docker image build -t ubuntu2004_gcc10 -f Dockerfile .
#   docker run -d --cap-add sys_ptrace -p127.0.0.1:2000:22 --name clion_remote_env ubuntu2004_gcc10
#   Windows:
#   ssh-keygen -f "%HOMEPATH%/.ssh/known_hosts" -R "[localhost]:2000"
#   Rest:
#   ssh-keygen -f "$HOME/.ssh/known_hosts" -R "[localhost]:2000"
#
# stop:
#   docker stop clion_remote_env
#
# ssh credentials (test user):
#   user@password

FROM ubuntu:20.04

SHELL ["/bin/bash", "-c"]
# install sys requirements
RUN apt-get update && apt-get upgrade -y \
    && apt-get install -y gnupg software-properties-common

RUN apt-get update \
  && apt-get install -y ssh \
      build-essential \
      ccache \
      clang \
      cmake \
      curl \
      gcc-10 \
      gcc-9 \
      gcc-8 \
      g++-10 \
      g++-9 \
      g++-8 \
      gdb \
      git \
      python3.8 \
      python3-distutils \
      rsync \
      tar \
      wget \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*

RUN ln -s /usr/bin/python3 /usr/bin/python

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 60 --slave /usr/bin/g++ g++ /usr/bin/g++-10 \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 50 --slave /usr/bin/g++ g++ /usr/bin/g++-9 \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 40 --slave /usr/bin/g++ g++ /usr/bin/g++-8 \
    && yes '' | update-alternatives --force --all

RUN curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py && python get-pip.py

RUN git clone https://github.com/maichmueller/libtorchload.git \
    && cd libtorchload \
    && pip install -r requirements.txt \
    && python dl_torch.py --targetdir=/home

RUN pip install conan

RUN apt-get update \
  && apt-get install -y ssh \
      doxygen \
      graphviz \
      libgraphviz-dev \
      pkg-config \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*


RUN ( \
    echo 'LogLevel DEBUG2'; \
    echo 'PermitRootLogin yes'; \
    echo 'PasswordAuthentication yes'; \
    echo 'Subsystem sftp /usr/lib/openssh/sftp-server'; \
  ) > /etc/ssh/sshd_config_clion \
  && mkdir /run/sshd

RUN useradd -m user \
    && yes password | passwd user

CMD ["/usr/sbin/sshd", "-D", "-e", "-f", "/etc/ssh/sshd_config_clion"]
