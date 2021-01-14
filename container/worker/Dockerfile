FROM kalilinux/kali-rolling:latest

RUN apt-get update
RUN apt install git -y
RUN apt install gcc -y
RUN apt install make -y
RUN git clone https://github.com/MrPekar98/PekarDrive.git

WORKDIR /PekarDrive
RUN make worker MASTER_TKN=1 WORKER_TKN=2     # Example tokens. Must be the same as in worker's Dockerfile.

EXPOSE 55443
ENTRYPOINT ["./worker"]
