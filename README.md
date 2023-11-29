# Snowpack Proxy Test

Snowpack Proxy Test is a simple C++ project that demonstrates a basic proxy-server and user-client communication using Boost.Asio. The project allows you to run either in "Proxy" mode or "User" mode.

## Prerequisites

Before you begin, ensure you have met the following requirements:

- C++ compiler (g++)
- Boost C++ Libraries installed

## Build and Run

To build and run the project, follow these steps:

1. Clone the repository:

    ```bash
    git clone https://github.com/KazeTachinuu/Snowpack-proxy-test.git
    ```

2. Change into the project directory:

    ```bash
    cd Snowpack-proxy-test
    ```

3. Build the project:

    ```bash
    mkdir build
    cd build
    cmake ..
    make -j4
    ```

4. Run the Proxy or User:

    - Proxy Mode:

        ```bash
        ./main --mode=Proxy
        ```

    - User Mode:

        ```bash
        ./main --mode=User
        ```

## Usage

- Proxy Mode: Launches the proxy server on port 12345.

- User Mode: Initiates a basic user that connects to the proxy server. Ensure the proxy server is running before starting the user.

## Options

- `--mode`: Specify the mode as "Proxy" or "User".

## Error Handling

The project includes error handling for various scenarios, such as connection refusal and invalid options.

If an error occurs, the application will provide informative error messages to guide you.


