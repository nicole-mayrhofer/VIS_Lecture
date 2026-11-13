cmake_minimum_required(VERSION 3.26)
project(VIS-2026)

set(CMAKE_CXX_STANDARD 14)

# Shortcuts
set(PREP socket/cpp/Hello/src/main)
set(DemoC socket/cpp/Demo_SocketClient/src/main)
set(DemoS socket/cpp/Demo_SocketServer/src/main)
set(PrimC socket/cpp/Primitive_SocketClient/src/main)
set(PrimS socket/cpp/Primitive_SocketServer/src/main)
set(EnvS socket/cpp/Environment_SocketServer/src/main)

# Executable inkl. beteiligte Sourcen
add_executable(Hello
        ${PREP}/cpp/main.cpp
        ${PREP}/cpp/Foo.cpp
        ${PREP}/headers/Foo.h)

# Exercise 1.1 a, b
add_executable(Demo_SocketServer
        ${DemoS}/cpp/Demo_SocketServer.cpp)
add_executable(Demo_SocketClient
        ${DemoC}/cpp/Demo_SocketClient.cpp)

# Exercise 1.2 a, b
add_executable(Primitive_SocketServer
        ${PrimS}/cpp/Primitive_SocketServer.cpp)
add_executable(Primitive_SocketClient
        ${PrimC}/cpp/Primitive_SocketClient.cpp)

# Exercise 1.3 a, b
add_executable(Environment_SocketServer
        ${EnvS}/cpp/Environment_SocketServer.cpp)

# Header Files bekannt machen
target_include_directories(Hello PRIVATE ${PREP}/headers)
target_include_directories(Primitive_SocketServer PRIVATE ${PrimS}/headers)
target_include_directories(Environment_SocketServer PRIVATE ${EnvS}/headers)

