#!/bin/bash

args=$@

docker run --rm -it \
    -v `pwd`:/code \
    almeidaos-build:latest /bin/bash -c "cd /code && make ${args}"
