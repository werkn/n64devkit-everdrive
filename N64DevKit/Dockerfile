FROM werkncode/libdragon:toolchain

WORKDIR /libdragon

#add any tools you want here
RUN apt-get update
RUN apt-get install -y vim tmux htop

RUN git clone https://github.com/DragonMinded/libdragon libdragon-master

# Build the actual library here & build and install mikmod
WORKDIR /libdragon/libdragon-master
RUN ./build.sh