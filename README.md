# Snowpack Proxy Server

Snowpack Proxy Server is a simple proxy server implementation using Boost.Asio.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Tests](#tests)


## Introduction

Snowpack Proxy Server is a lightweight proxy server built with Boost.Asio, designed to facilitate communication between multiple users. It allows users to connect and exchange messages through a central proxy, providing a basic foundation for distributed systems and communication testing.

## Features

- Basic proxy server functionality
- Asynchronous communication using Boost.Asio
- Command handling for managing user connections

## Getting Started

To get started with the Snowpack Proxy Server, follow these steps:

1. Clone the repository: `git clone https://github.com/KazeTachinuu/Snowpack-proxy-test.git`
2. Navigate to the project directory: `cd Snowpack-Proxy-Server`
3. Build the project using CMake: `mkdir build && cd build && cmake .. && make main`
4. Run the proxy server: `./main --mode Proxy`

## Usage

Once the Snowpack Proxy Server is running, users can connect to it using a TCP client. The server handles user connections and facilitates communication between them.

Example usage:

1. Start the proxy server: `./main --mode Proxy`
2. Connect users to the server using TCP clients: `./main --mode User`
3. Exchange messages between connected users through the proxy.

## Tests

### Test Case 1: Manage absence of correspondent
- P is waiting for incoming connection
- A connects to P, P confirms that the connection is ok but that there is no other connected user, A is waiting up to 30 secs. After 30 secs, A disconnects from P because nobody has connected to the proxy.

To run Test Case 1, first run the proxy in a Terminal with :
```shell
./main --mode Proxy
```
Then run the test_case_1 executable and watch the magic
```bash
make test_case_1
./test_case_1
```

### Test Case 2: Manage basic communication
- P is waiting for incoming connection
- A connects to P, P confirms that the connection is ok but that there is no other connected user, A is waiting up to 30 secs.
- B connects to P before the remaining time of 30 secs. P informs B that he will be connected with the connected user A.
- A is informed that it has been connected to B by P.
- A sends a message “hello” to B relayed by P (P prints the message in its terminal before sending the message to B).
- B prints the message in its terminal

To run Test Case 2, first run the proxy in a Terminal with :
```shell
./main --mode Proxy
```
Then run the test_case_2 executable and watch the magic
```bash
make test_case_2
./test_case_2
```