cmake_minimum_required(VERSION 3.15...3.26)

# Name the CMake project; the IDE will show this as the solution/project name.
project(cachesim LANGUAGES CXX)

# Prevent in‑source builds
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR
    "In‑source builds are not allowed.\n"
    "Please create a separate build directory and run CMake there.")
endif()

# Require C++11 or later
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands (useful for clang‑tools, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# List your source files (only .cpp’s here)
set(SOURCE_FILES
  cachesim.cpp
  cacheClass.cpp
  cacheClass.hpp
)

# Declare the executable target
add_executable(cachesim ${SOURCE_FILES})

# Make sure your header directory (current dir) is on the include path
target_include_directories(cachesim PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
