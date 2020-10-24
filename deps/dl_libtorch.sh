#!/bin/bash -x

base_url=https://download.pytorch.org/libtorch/cu102
version=1.6.0
filename=libtorch-cxx11-abi-shared-with-deps-$version.zip
torch_url=$base_url/$filename
target_dir=$(dirname $(readlink -f $0))
if [ ! -d "$target_dir"/libtorch ]; then \
  wget --directory-prefix="$target_dir" $torch_url; \
  unzip "$target_dir"/$filename -d "$target_dir"; \
  rm "$target_dir"/$filename; \
else
  echo "folder 'libtorch' already exists -> exiting."; \
fi