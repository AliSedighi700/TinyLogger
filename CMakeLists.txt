cmake_minimum_required(VERSION 4.0)
project(ConcurrentPriceFetcher)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(hftapp main.cpp)
