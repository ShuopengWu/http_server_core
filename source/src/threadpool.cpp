#include "threadpool.h"

#include "logger.h"

ThreadPool::ThreadPool() : is_exit(false)
{
    // 使用emplaceback原地构造，而不是pushback拷贝(std::function不可复制)
    for (int i = 0; i < MAX_WORKER_NUMBER; i++)
        workers.emplace_back([this] {
            this->worker_main();
        });
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock;
        is_exit = true;
    }

    condition.notify_all();

    for (int i = 0; i < MAX_WORKER_NUMBER; i++)
    {
        if (workers.at(i).joinable())
            workers.at(i).join();
    }
}

void ThreadPool::worker_main()
{
    logger::logger &log = logger::logger::instance();
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::string thread_id = ss.str();

    for(;;)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mutex);

            // 退出时或有任务时唤醒
            condition.wait(lock, [this] {
                return !tasks.empty() || is_exit;
            });

            // 是否退出
            if (tasks.empty() && is_exit)
                return;

            task = std::move(tasks.front());
            tasks.pop();
        }

        try
        {
            task();
        }
        catch(const std::exception& e)
        {
            log.show_warning_log("Thread throws a std exception, thread id is : " + thread_id + " exception is : " + e.what());
        }
        catch (...)
        {
            log.show_warning_log("Thread throws a unknown exception, thread id is : " +  thread_id);
        }
    }
}
