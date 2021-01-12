FROM emscripten/emsdk:2.0.2

RUN apt update && apt install -y ninja-build cmake 

ADD https://dl.bintray.com/boostorg/release/1.75.0/source/boost_1_75_0.tar.gz .
RUN tar zxvf boost_1_75_0.tar.gz -C /opt && rm boost_1_75_0.tar.gz

ENV BOOST_ROOT /opt/boost_1_75_0

WORKDIR /app
USER 1000
COPY . ./
CMD ["./build.sh"]
