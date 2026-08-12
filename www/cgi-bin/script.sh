#!/usr/bin/env bash

echo -e 'content-type: text/html\r\n\r\n'
# echo -e 'content-type: text/html\r\n'
i=0
while read -r hey
do
	echo "$i: $hey";
	let i+=1
done
