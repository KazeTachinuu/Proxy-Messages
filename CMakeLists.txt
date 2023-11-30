cmake_minimum_required(VERSION 3.12)

project(YourProjectName)

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
        # Add other source files as needed
)

set(HEADERS
        include/BasicUser.hpp
        include/ProxyServer.hpp
        include/CommandHandler.hpp
        # Add other header files as needed
)

# Build main executable in the root directory
add_executable(main src/main.cpp ${SOURCES} ${HEADERS})

# Link against Boost libraries
target_link_libraries(main ${Boost_LIBRARIES})

# Build test executable in the test directory
add_executable(test_case_1 tests/test_case_1.cpp src/BasicUser.cpp include/BasicUser.hpp)
target_link_libraries(test_case_1 ${Boost_LIBRARIES})

# Define test
add_test(NAME TestCase1 COMMAND test_case_1)

# Optionally, you can add more tests similarly
 add_executable(test_case_2 tests/test_case_2.cpp src/BasicUser.cpp include/BasicUser.hpp)
 target_link_libraries(test_case_2 ${Boost_LIBRARIES})
 add_test(NAME TestCase2 COMMAND test_case_2)
