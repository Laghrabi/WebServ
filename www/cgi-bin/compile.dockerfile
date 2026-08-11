FROM gcc

WORKDIR /src

COPY ./file.c .

RUN gcc /src/file.c -o /bin/a.out

CMD ["a.out"]
