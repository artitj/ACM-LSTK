FROM ubuntu:16.04
# Install git
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive \
    TZ=America/New_York \ 
    apt-get install -y git wget sudo apt-utils software-properties-common && \
    mkdir -p /home/ubuntu/Dev && \
    cd /home/ubuntu/Dev && \
    git clone https://github.com/artitj/ACM-LSTK.git
RUN cd /home/ubuntu/Dev/ACM-LSTK && \
    bash build.sh
