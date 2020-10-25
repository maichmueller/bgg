#!/bin/bash

# do !/bin/bash -x to see all commands and variables expanded as they are called (-v for not expanded variables)

cuda=false
os=linux
build=debug  # by default we would grab the debug package of libtorch (windows only)
for i in "$@"
do
case $i in
    -c=*|--cuda=*)
    cuda="${i#*=}"
    shift # past argument=value
    ;;
    -s=*|--os=*)
    os="${i#*=}"
    shift # past argument=value
    ;;
    -b=*|--build=*)
    build="${i#*=}"
    shift # past argument=value
    ;;
    *)
          # unknown option
    ;;
esac
done

base_url=https://download.pytorch.org/libtorch/cpu
if [ "$cuda" = true ]; then \
  base_url=https://download.pytorch.org/libtorch/cu102
fi

version=1.6.0
os="${os,,}"
if [ "$os" == "lin" ] || [ "$os" == "linux" ] || [ "$os" == "ubuntu" ]; then \
  if [ "$cuda" = true ]; then \
    filename=libtorch-cxx11-abi-shared-with-deps-$version.zip; \
  else
    filename=libtorch-cxx11-abi-shared-with-deps-$version%2Bcpu.zip; \
  fi

elif [ "$os" == "mac" ] || [ "$os" == "apple" ] || [ "$os" == "macos" ]; then \
  if [ "$cuda" = true ]; then \
    echo "CUDA setting '$cuda' and operating system '$os' are incompatible. Stopping.";
  fi

  filename=libtorch-macos-$version.zip; \

elif [ "$os" == "win" ] || [ "$os" == "windows" ] || [ "$os" == "windows10" ] || [ "$os" == "win10" ]; then \
  if [ "$build" == "debug" ]; then \
      if [ "$cuda" = true ]; then \
        filename=libtorch-win-shared-with-deps-debug-$version.zip; \
      else
        filename=libtorch-win-shared-with-deps-debug-$version%2Bcpu.zip; \
      fi
  elif [ "$build" == "release" ]; then \
      if [ "$cuda" = true ]; then \
        filename=libtorch-win-shared-with-deps-$version.zip; \
      else
        filename=libtorch-win-shared-with-deps-$version%2Bcpu.zip; \
      fi
  else
    echo "Build type $build not supported. Stopping."
  fi

else
  echo "Operating system $os not supported. Stopping."
fi
url=$base_url/$filename
bad_chars="\%2B"
replacement="+"
filename="${filename/$bad_chars/$replacement}"   # decode the potential hexadecimal %2B as the '+' char

target_dir=$(dirname $(readlink -f $0))  # target dir is the dir of this script

printf "%s\n" "Will attempt to download:" "OS: $os" "CUDA: $cuda" "BUILD: $build" "URL: $url" "TARGET DIR: $target_dir"

if [ ! -d "$target_dir"/libtorch ]; then \
  wget --directory-prefix="$target_dir" $url; \
  unzip "$target_dir"/$filename -d "$target_dir"; \
  rm "$target_dir"/$filename; \
else
  echo "Folder 'libtorch' already exists. Stopping."; \
fi