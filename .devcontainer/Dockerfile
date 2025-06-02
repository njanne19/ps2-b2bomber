FROM ps2dev/ps2dev:latest

# Update and install bash
USER root
# install bash, make, cmake, plus the full build-base (incl. shared GMP/MPFR/MPC)
RUN apk update && apk add --no-cache \
      bash \
      make \
      cmake \
      git \
      build-base

# Clone PS2Link into $PS2DEV/ps2link and build it immediately -- for uploading programs to your PS2 over ethernet. 
RUN git clone https://github.com/ps2dev/ps2link.git $PS2DEV/ps2link && \ 
      cd $PS2DEV/ps2link && \
      make clean all 

# Symlink into /usr/local/bin so "ps2client" "fsclient" works everywhere 
RUN ln -s $PS2DEV/bin/ps2client /usr/local/bin/ps2client
RUN ln -s $PS2DEV/bin/fsclient /usr/local/bin/fsclient

WORKDIR /workspace
