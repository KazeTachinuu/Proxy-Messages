# Snowpack Proxy Server

Snowpack Proxy Server is a simple proxy server implementation using Boost.Asio.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Commands](#commands)


## Introduction

Snowpack Proxy Server is a lightweight proxy server built with Boost.Asio, designed to facilitate communication between multiple users. It allows users to connect and exchange messages through a central proxy, providing a basic foundation for distributed systems and communication testing.

## Features

- Basic proxy server functionality
- Asynchronous communication using Boost.Asio
- Command handling for managing user connections

## Getting Started

To get started with the Snowpack Proxy Server, follow these steps:

1. Clone the repository: `git clone https://github.com/KazeTachinuu/Snowpack-proxy-test.git`
2. Navigate to the project directory: `cd Snowpack-proxy-test`
3. Build the project using CMake: `mkdir build && cd build && cmake .. && make main`
4. Run the proxy server: `./main --mode Proxy`

## Usage

Once the Snowpack Proxy Server is running, users can connect to it using a TCP client. The server handles user connections and facilitates communication between them.

Example usage:

1. Start the proxy server: `./main --mode Proxy`
2. Connect users to the server using TCP clients: `./main --mode User --secret <secret>`
3. Exchange messages between connected users with the same secret on the proxy server.

## Commands

Once the Snowpack Proxy Server is running, the following commands can be used to manage user connections:

### Server Commands

- `/list` - Lists all connected users.
- `/kill <channelNum>` - Disconnects all users with the specified channel.
- `/kick <user>` - Disconnects the specified user.
- `/exit, /shutdown` - Stop the proxy server.
- `/help` - Displays a list of available commands.

### User Commands

- `/usercount` - Displays the number of connected users.
- `/ping <message>` - Sends a message to all connected users.
- `/exit` - Exits the user client.
- `/list` - List all users in the channel.
- `/help` - Displays a list of available commands.




