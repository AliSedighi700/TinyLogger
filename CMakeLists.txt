cmake_minimum_required(VERSION 4.0)
project(ConcurrentPriceFetcher)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(hftapp 
               src/main.cpp
               src/price_fetcher.cpp
               src/shared_prices.cpp
               src/thread_pool.cpp)

target_include_directories(hftapp PRIVATE src)
