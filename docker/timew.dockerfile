FROM ubuntu:22.04 AS base

FROM base AS builder

ENV DEBIAN_FRONTEND noninteractive

RUN ( apt-get update && yes | unminimize ) && \
    apt-get install -y \
            cmake \
            g++ \
            git

# Setup language environment
ENV LC_ALL en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US.UTF-8

# Add source directory
ADD .. /root/code/
WORKDIR /root/code/

# Build Timewarrior
RUN git clean -dfx && \
    git submodule init && \
    git submodule update && \
    cmake -DCMAKE_BUILD_TYPE=release . && \
    make -j8

FROM base AS runner

# Install Timewarrior
COPY --from=builder /root/code/src/timew /usr/local/bin

# Initialize Timewarrior
RUN timew :yes
