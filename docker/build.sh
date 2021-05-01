#!/bin/bash

args=$@

docker run --rm -it \
    -v `pwd`:/code \
    ghcr.io/paulomigalmeida/almeidaos-build:gcc-10.2.0 /bin/bash -c "cd /code && \
     make ${args}"
