#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <iostream>
#include <thread>
#include <atomic>
#include <future>
#include <vector>
#include <concepts>
#include <type_traits>
#include <functional>
#include <condition_variable>

class ThreadPool
{
private: 
    std::mutex mtx_{}; 
    std::condition_variable cv_{}; 
    std::queue<std::function<void>> tasks_{};
    std::vector<std::thread> workers_{}; 
    std::atomic<bool> stop_{false};
    
    void worker(); 
    
Public: 

    explicit ThreadPool(size_t numThread = 5); 
    ~ThreadPool(); 

    template<typename F>
    auto submit(F&& f) -> std::future<decltype(f())>;

    template<typename F>
    auto submit(F&& f) -> std::future<decltype(f)>;
    requires std::is_void_v<decltype(f())>;

    void shutdown(); 
 
}

#endif
