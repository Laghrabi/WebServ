#!/bin/bash -e

echo -n > file.c
echo -e "Content-Type: text/html\n"

export DOCKER_HOST='unix:///run/user/102540/docker.sock'
export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export USER=hsacr
export HOME=/home/hsacr


while read -r line
do
	echo "$line" >> file.c
done

# docker buildx ls
docker build -t compile -f compile.dockerfile . 
docker run -t compile

