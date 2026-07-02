FROM debian

RUN apt -y update && apt -y install make gcc clang valgrind vim

WORKDIR /src

COPY . .

RUN make && cp ./webserver /bin/
