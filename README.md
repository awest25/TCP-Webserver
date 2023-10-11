# TCP-Like Webserver over UDP

**Language**: C
**Date**: Feb 2023
**Repository**: [github.com/awest25/TCP-Like-Webserver](https://github.com/awest25/TCP-Like-Webserver)

## Overview
This webserver project aims to mimic TCP's reliable data transfer behavior using the inherently unreliable UDP protocol. By recreating the essence of TCP over UDP, the project offers profound insights into packet loss handling, sequence acknowledgment, and data transfer reliability.

## Key Features
- **Large File Transmission**: Supports pipelining for efficient data transfer.
- **Loss Recovery Options**: 
  - Go-Back-N (GBN)
  - Selective Repeat (SR)
- **Server-Client Architecture**: Client sends a file upon establishing a connection with the server.
- **Reliable Data Transfer**: Mimicking TCP's reliability using UDP.

## Project Simplifications
- No need for checksum computation or packet verification.
- Absence of packet corruption, reordering, or duplication; only packet loss is simulated.
- No RTT estimation or RTO updates; uses a fixed retransmission timer.
- Sequential (non-parallel) connections.

## Technical Stack
- **Environment**: Linux (for simulated packet loss testing).
- **Networking**: BSD sockets in C.
- **Additional Libraries**: Only standard C libraries allowed.

## Getting Started
1. Clone the repo: `git clone https://github.com/awest25/TCP-Like-Webserver.git`
2. Navigate to directory: `cd TCP-Like-Webserver`
3. Compile and run server.
4. Compile and run client.
5. Alternitiveley to using a client, use TELNET to request hosted files.
