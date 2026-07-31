FROM debian

RUN apt -y update && apt -y install make gcc clang valgrind vim

WORKDIR /src

# COPY . .

# RUN make re && make re CPP=clang++ && cp ./webserver /bin/
