FROM ghcr.io/gothenburgbitfactory/timew:stable

# Install dependencies
RUN apt-get update && apt-get install -y \
    wget \
    sudo \
    jq \
    && rm -rf /var/lib/apt/lists/*
    
RUN wget https://github.com/sharkdp/hyperfine/releases/download/v1.19.0/hyperfine_1.19.0_amd64.deb
RUN sudo dpkg -i hyperfine_1.19.0_amd64.deb

# Create directories for volume mounting
RUN mkdir -p /performance-test

# Copy performance test scripts
COPY performance-test.sh /performance-test/

# Make script executable
RUN chmod +x /performance-test/performance-test.sh

# Set working directory
WORKDIR /performance-test

# Execute performance tests
CMD ["/performance-test/performance-test.sh", "/performance-test/output"]
