#include <iostream>
#include <vector>
#include <functional>

#include "src/threadpool.h"
#include "src/logger.h"

int main()
{
    ThreadPool pool;
    std::vector<std::future<int>> futures; // 存储所有任务的 future

    // 假设 MAX_TASKS 是一个合理的并发数量，例如 1000
    const int MAX_TASKS = 1000;
    
    // 任务：模拟耗时的计算，并返回结果
    auto long_running_task = [](int id) -> int {
        // 模拟计算，无需锁，让线程并行执行
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return id * 2;
    };
    
    std::cout << "主线程: 开始提交任务...\n";

    // 1. 批量提交任务
    for (int i = 0; i < MAX_TASKS; ++i) {
        // 提交任务，将 future 存储起来
        futures.push_back(pool.submit_task(long_running_task, i));
    }

    std::cout << "主线程: 所有任务已提交，继续执行其他操作...\n";
    // 主线程可以在这里做其他工作，例如处理 UI 事件等

    // 2. 集中等待结果 (同步点)
    std::cout << "主线程: 等待所有任务完成并获取结果...\n";
    for (auto& future : futures) {
        try {
            int result = future.get(); // 此时才阻塞等待
            std::cout << "Task result: " << result << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Task threw exception: " << e.what() << std::endl;
        }
    }

    std::cout << "主线程: 所有任务完成，线程池测试结束。\n";
    return 0;
}
