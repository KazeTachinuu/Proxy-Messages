cmake_minimum_required(VERSION 3.12)
project(main)

set(CMAKE_CXX_STANDARD 17)


# Find Boost
find_package(Boost COMPONENTS program_options system)

# Include Boost headers
include_directories(${Boost_INCLUDE_DIRS})

# Source files
set(SRC_DIR src)
set(SRC_FILES
    ${SRC_DIR}/BasicUser.cpp
    ${SRC_DIR}/ProxyServer.cpp
    ${SRC_DIR}/main.cpp
)

# Create an executable in the build directory
add_executable(main ${SRC_FILES})

# Link Boost libraries
target_link_libraries(main ${Boost_LIBRARIES})

