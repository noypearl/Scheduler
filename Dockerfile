# Use Ubuntu 20.04 as base image
FROM ubuntu:20.04

# Avoid prompts from apt during build
ARG DEBIAN_FRONTEND=noninteractive

# Install gcc and gdb
RUN apt-get update && \
    apt-get install -y gcc gdb vim build-essential && \
    rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /home/scheduler

# Set the entry point to bash
ENTRYPOINT ["/bin/bash"]

