#!/bin/bash -x

base_url=https://download.pytorch.org/libtorch/cu101/
version=1.3.1
filename=libtorch-cxx11-abi-shared-with-deps-$version.zip
torch_url=$base_url/$filename
if [ ! -d ./libtorch ]; then \
  wget $torch_url deps; \
  unzip deps/$filename; \
  rm deps/$filename; \
fi