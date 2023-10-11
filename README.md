# TCP-Like Webserver over UDP

**Language**: C <br/>
**Date**: Feb 2023 <br/>
**Repository**: [github.com/awest25/TCP-Like-Webserver](https://github.com/awest25/TCP-Like-Webserver)

## Overview
This webserver project involves two key parts. In the first part, located in the "project1" directory, we implemented an unreliable version of a TCP-like protocol using the socket API from scratch. This involved the intricate processes of setting up and tearing down connections, emphasizing the foundational understanding of TCP protocol. 

In the second part, the essence of the project, located in the "project2" directory, dives deep into mimicking TCP's reliable data transfer behavior using the inherently unreliable UDP protocol. By recreating the essence of TCP over UDP, the project offers profound insights into packet loss handling, sequence acknowledgment, and data transfer reliability.

## Key Features
- **From Scratch Protocol**: Developed a TCP-like protocol from the ground up in "project1".
- **Large File Transmission**: Supports pipelining for efficient data transfer.
- **Loss Recovery Options**: 
  - Go-Back-N (GBN)
  - Selective Repeat (SR)
- **Server-Client Architecture**: Client sends a file upon establishing a connection with the server.
- **Reliable Data Transfer**: Mimicking TCP's reliability using UDP in "project2".

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
3. For "project1", navigate to its directory and compile it.
4. For "project2", compile and run the server.
5. Compile and run the client.
6. Alternatively, for "project2", use TELNET to request hosted files.
