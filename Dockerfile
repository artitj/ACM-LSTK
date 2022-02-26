FROM ubuntu:16.04
# Install git
RUN apt-get update && apt-get -y upgrade && \
    DEBIAN_FRONTEND=noninteractive \
    TZ=America/New_York \ 
    apt-get install -y git wget sudo apt-utils software-properties-common && \
    apt-get -y install g++ build-essential cmake-curses-gui python-minimal && \
    apt-get install -y autoconf autogen automake pkg-config libgtk-3-dev libtool llvm-dev && \
    apt-get install -y mesa-common-dev freeglut3 freeglut3-dev libglew1.5-dev libglm-dev libgles2-mesa libgles2-mesa-dev && \
    mkdir -p /home/ubuntu/Dev && \
    cd /home/ubuntu/Dev && \
    git clone https://github.com/artitj/ACM-LSTK.git
RUN cd /home/ubuntu/Dev/ACM-LSTK && \
    bash build.sh
