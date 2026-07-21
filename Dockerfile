FROM debian

RUN apt -y update && apt -y install make gcc clang valgrind vim tmux

WORKDIR /src

