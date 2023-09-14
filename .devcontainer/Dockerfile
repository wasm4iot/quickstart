FROM mcr.microsoft.com/devcontainers/base:ubuntu-22.04
RUN apt update
RUN apt install -y python3.10 python3-pip python3.10-venv python3.10 curl sudo 
WORKDIR /tmp
RUN curl -fsSL -o get-platformio.py https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py
RUN sudo -u \#1000 python3 get-platformio.py
RUN sudo -u \#1000 python3 -m pip install nrfutil
RUN apt install -y gcc-arm-none-eabi cmake wabt libncurses5 minicom