FROM ubuntu:20.04

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    libssl-dev \
    wget \
    unzip \
    && apt-get clean

RUN wget https://github.com/weidai11/cryptopp/archive/refs/tags/CRYPTOPP_8_9_0.tar.gz -O cryptopp-8.9.0.tar.gz && \
    tar -xvf cryptopp-8.9.0.tar.gz && \
    cd cryptopp-CRYPTOPP_8_9_0 && \
    make -j$(nproc) && \
    make install && \
    ldconfig && \
    cd .. && rm -rf cryptopp-8.9.0.tar.gz cryptopp-CRYPTOPP_8_9_0

RUN mkdir -p /usr/share/servercryptomadnessvortex/server/queue
RUN mkdir -p /usr/share/servercryptomadnessvortex/server/rooms
RUN mkdir -p /usr/share/servercryptomadnessvortex/server/users_key
RUN mkdir -p /usr/share/servercryptomadnessvortex/server/users_key_serv

WORKDIR /app

COPY ./src /app/src
COPY ./include /app/include

RUN g++ -std=c++2a -o CryptoMadness_Server -Iinclude -I/usr/local/include/cryptopp src/*.cpp src/command/*.cpp -lssl -lcrypto -lcryptopp -lpthread

EXPOSE 9999

CMD ["./CryptoMadness_Server"]
