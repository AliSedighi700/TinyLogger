#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <iostream>
#include <queue>
#include <thread>
#include <atomic>
#include <memory>
#include <future>
#include <vector>
#include <concepts>
#include <type_traits>
#include <functional>
#include <condition_variable>
#include <stdexcept>

class ThreadPool
{
private: 
    std::mutex mtx_; 
    std::condition_variable cv_; 
    std::queue<std::function<void()>> tasks_;
    std::vector<std::thread> workers_; 
    std::atomic<bool> stop_{false};
    
    void worker(); 
    
public: 

    explicit ThreadPool(size_t numThread = 5); 
    ~ThreadPool(); 

    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;


    void shutdown(); 
 
};

#endif
