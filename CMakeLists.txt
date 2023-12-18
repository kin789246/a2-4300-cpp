cmake_minimum_required(VERSION 3.15)

# for LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add MacOS frameworks path for LLVM 17
set(CMAKE_CXX_COMPILER 
    /opt/homebrew/opt/llvm/bin/clang++ -F$ENV{SDKROOT}/System/Library/Frameworks
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-result -pedantic-errors")

project(
    a2
    LANGUAGES CXX
    VERSION 0.1.0
)

add_subdirectory(dependencies)

# make executable app
set(SRC_FILES
    src/main.cpp
    src/Vec2.cpp
    src/Game.cpp
    src/Entity.cpp
    src/EntityManager.cpp
)
set(exec_name a2)
add_executable(${exec_name} ${SRC_FILES})
target_link_libraries(${exec_name} PRIVATE ImGui-SFML::ImGui-SFML)
include_directories(include)

# copy assets to binary directory
file(
    COPY ${CMAKE_SOURCE_DIR}/assets/fonts/TwilioSansMNerdFont-Regular.ttf 
    DESTINATION ${CMAKE_BINARY_DIR}/assets/fonts
)

file(
    COPY ${CMAKE_SOURCE_DIR}/assets/config.txt 
    DESTINATION ${CMAKE_BINARY_DIR}/assets
)
