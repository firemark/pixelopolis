FROM ubuntu:18.04

RUN apt-get update && apt-get install -y \
    gcc \
    libpng-dev \
    zlib1g-dev \
    build-essential \
    bison \
    flex \
    python3-pip

RUN pip3 install flask
    
RUN mkdir /code
ADD Makefile /code/Makefile
ADD src /code/src
ADD include /code/include
ADD textures /code/textures
ADD examples /code/examples
ADD web /code/web

WORKDIR /code
ARG VERSION=PROD
RUN make all

WORKDIR /code/web
ENV FLASK_APP=web.py
ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8
ENV PORT=80
EXPOSE 80
CMD python3 -m flask run -h 0.0.0.0 -p $PORT
