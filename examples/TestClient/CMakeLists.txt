project("Open62541Cpp")
cmake_minimum_required(VERSION 3.11)
# Build Test Server
set(APPNAME TestClient)

include(../examples_common.cmake)
add_example(TestClient main.cpp)
