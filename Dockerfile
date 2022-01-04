FROM ubuntu:18.04

RUN apt-get update && apt-get install -y \
    gcc \
    libpng-dev \
    zlib1g-dev \
    build-essential \
    bison \
    flex \
    cmake \
    python3-pip

RUN pip3 install flask

RUN mkdir /code
WORKDIR /code

ADD CMakeLists.txt CMakeLists.txt
ADD main main
ADD lib_include lib_include
ADD src src
ADD include include

RUN mkdir out \
    && cd out \
    && cmake -DCMAKE_BUILD_TYPE=Release .. \
    && make \
    && make install \
    && mv pixelopolis .. \
    && mv libpixelopolis* .. \
    && cd .. \
    && rm -rf out

ADD textures /code/textures
ADD examples /code/examples
ADD web /code/web

WORKDIR /code/web
ENV FLASK_APP=web.py
ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8
ENV PORT=80
EXPOSE 80
CMD python3 -m flask run -h 0.0.0.0 -p $PORT
