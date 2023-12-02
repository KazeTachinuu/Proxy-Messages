cmake_minimum_required(VERSION 3.12)

project(SnowpackProxy)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Locate Boost libraries
find_package(Boost REQUIRED COMPONENTS program_options system thread)

# Include directories
include_directories(include ${Boost_INCLUDE_DIRS})

# Add your source files and headers
set(SOURCES
        src/BasicUser.cpp
        src/ProxyServer.cpp
        src/CommandHandler.cpp

)

set(HEADERS
        include/BasicUser.hpp
        include/ProxyServer.hpp
        include/CommandHandler.hpp

)

# Build main executable in the root directory
add_executable(main src/main.cpp ${SOURCES} ${HEADERS})

# Link against Boost libraries
target_link_libraries(main ${Boost_LIBRARIES})

