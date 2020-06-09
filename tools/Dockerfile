FROM ubuntu:18.04

RUN export DEBIAN_FRONTEND=noninteractive && \
        dpkg --add-architecture i386 && apt-get update -y && \
        apt-get install git gcc make bison flex wine32 -y

# docker build -f tools/Dockerfile -t pokemon_dp .
# docker run --rm -t -v $PWD:/pokemon_dp pokemon_dp /bin/bash -c '/pokemon_dp/tools/docker.sh'
