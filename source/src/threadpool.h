#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <functional>
#include <sstream>
#include <future>
#include <stdlib.h>

namespace
{
static size_t MAX_WOKRER_NUMBER = 10;
}

class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();
    template <typename function, typename ...args>
    std::future<std::invoke_result_t<function, args...>> submit_task(function &&func, args&&... a)
    {
        using return_type = std::invoke_result_t<function, args...>;

        auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<function>(func), std::forward<args>(a)...));

        std::future<return_type> result = task_ptr->get_future();

        {
            std::unique_lock<std::mutex> lock(mutex);

            if (is_exit)
                throw std::runtime_error("thread pool exit");
            
            tasks.emplace([task_ptr] {
                (*task_ptr)();
            });

            condition.notify_one();
        }

        return result;
    }
private:
    void worker_main();
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mutex;
    std::condition_variable condition;
    bool is_exit;
};

#endif
