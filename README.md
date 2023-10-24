# TCP-Like Webserver over UDP

**Language**: C/C++ <br/>
**Date**: Feb 2023 <br/>
**Repository**: [github.com/awest25/TCP-Webserver](https://github.com/awest25/TCP-Webserver)

## Overview

This project comprises two essential parts:

1. **Web Server in C/C++** (`project1` directory): A simple web server developed in C/C++ to understand the workings of a web browser and server. This server parses HTTP requests, generates an HTTP response message with the requested file and its headers, and then dispatches the response back to the client.

2. **TCP-like Protocol over UDP** (`project2` directory): This part delves into simulating TCP's reliable data transfer over the inherently unreliable UDP protocol. The aim is to mimic TCP over UDP, diving deep into packet loss handling, sequence acknowledgment, and ensuring data transfer reliability.

## Key Features

- **Web Server Implementation**: Capable of responding to HTTP requests with supported file formats in "project1".
- **From Scratch Protocol**: Developed a TCP-like protocol from the ground up in "project2".
- **Supported File Formats**: 
  - Text: `*.html`, `*.txt`
  - Images: `*.jpg`, `*.png`
  - Documents: `*.pdf`
  - Binary files with no specific extensions.
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
- Server does not handle subdirectory file requests, non-existing files, or filenames with special characters other than alphabets, periods, spaces, or % signs.

## Technical Stack

- **Environment**: Ubuntu 22.04 LTS (Jammy Jellyfish) - Recommended for development and testing.
- **Networking**: BSD sockets in C/C++.
- **Additional Libraries**: Only standard C/C++ libraries and extensions up to C++14 (for non-networking tasks, such as string parsing) are permitted.

## Getting Started

1. Clone the repo: `git clone https://github.com/awest25/TCP-Webserver.git`
2. Navigate to directory: `cd TCP-Webserver`
3. For "project1", navigate to its directory, compile, and start the server.
4. For "project2", compile and run the server.
5. Compile and run the client.
6. Alternatively, for "project1", use a standard web browser like Safari to connect to the web server. For "project2", use TELNET to request hosted files.

## Sample Usage

```bash
$ cat /dev/urandom | head -c 1000000 > binaryfile        # generate a 1MB file
$ curl -o downloadfile <machinename>:<port>/binaryfile   # request the file
$ diff downloadfile binaryfile                           # check if the downloaded file is intact
```
