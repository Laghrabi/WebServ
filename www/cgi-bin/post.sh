#!/bin/bash

i=0
echo -e "Content-Type: text/html\n"
while read -r line
do
	echo "$i $line"
	# echo "$line" >> file.sh
	let i+=1
done
#
# echo hey
# # /usr/bin/docker  build -t hey . 2>&1
# /usr/bin/docker  buildx ls 2>&1
# docker run -t hey 2>&1
