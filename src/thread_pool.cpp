#include "thread_pool.h"
#include <iostream>



ThreadPool::ThreadPool(size_t numThread)
{
    workers_.reserve(numThread);
    for(int i = 0 ; i < numThread; ++i)
    {
        workers_.emplace_back(&ThreadPool::worker, this);
    }
}


ThreadPool::~ThreadPool()
{
    shutdown(); 
}

template<typename F, typename... Args>
auto ThreadPool::submit(F&& f, Args&&... args) 
    -> std::future<std::invoke_result_t<F, Args...>>
{

    using returnType = std::future<std::invoke_result_t<F, Args...>>;

    auto task_ptr = std::make_shared<std::packaged_task<returnType()>> (
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));


    auto future = task_ptr.get_future();

    {
        std::unique_lock<std::mutex> lock(mtx_);
        if(stop_)
        {
            throw std::runtime_error("cannot push task to a stoped pool");
        }
        tasks_.emplace([task_ptr = std::move(task_ptr)]()mutable { (*task_ptr)(); });
    }
    cv_.notify_all();

    return future;
}




void ThreadPool::worker()
{
    while(true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mtx_);

            cv_.wait(lock, [this]
                    {
                        return stop_ || !tasks_.empty();
                    });

            if(stop_ && tasks_.empty()){return ;}
            if(!tasks_.empty()){ continue ;}

            auto task = std::move(tasks_.front());
            tasks_.pop(); 
        }

        try{
            task(); 
        }catch(const std::exception& e){
            std::cerr << "Task threw exception : " << e.what() << '\n';
        }catch(...){
            std::cerr << "Task threw unknown exception" << '\n';
        }
        
    }
}



void ThreadPool::shutdown()
{
    {
        std::unique_lock<std::mutex> lock(mtx_);
        stop_ = true;
    }

    cv_.notify_all();

    for(auto& thread : workers_)
    {
        if(thread.joinable()){thread.join();}
    }
}

