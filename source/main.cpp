#include <iostream>
#include <thread>

#include "src/isocket.h" 

// --- 宏定义测试参数 ---
const std::string TEST_IP = "0.0.0.0";
const uint16_t TEST_PORT = 8080;

int main() {
    using namespace std::chrono;
    
    std::cout << "--- ISocket 服务器测试程序启动 ---" << std::endl;

    try {
        // 1. 创建 ISocket
        ISocket server_socket;

        // 2. 绑定地址
        InetAddress bind_addr(TEST_IP, TEST_PORT);
        server_socket.bind(bind_addr);

        // 3. 开始监听
        server_socket.listen();

        server_socket.set_nonblocking();


        // 5. 主循环：轮询 accept()
        bool running = true;
        int connections = 0;
        auto start_time = high_resolution_clock::now();

        while (running) {
            
            // 尝试接受连接
            std::pair<int, InetAddress> client_pair = server_socket.accept();
            int client_fd = client_pair.first;

            if (client_fd > 0) {
                // 成功接受连接
                connections++;
                
                // 仅输出核心连接信息，不包含 InetAddress::to_string() 的调用
                std::cout << "\n[INFO] Accepted client " << connections 
                          << " (New FD: " << client_fd << ")" << std::endl;
                
                // 关闭新连接
                close(client_fd);
                // 移除日志调用: logger::logger::instance().show_debug_log("...");

            } else if (client_fd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                // 非阻塞模式下，没有连接到来，这是正常情况
                
            } else if (client_fd == -1) {
                // 遇到其他错误，使用 std::cerr 输出
                // ⚠️ 保持 std::strerror(errno) 的调用，但请确保其返回值不是 nullptr
                std::cerr << "[ERROR] Accept failed: " << std::strerror(errno) << std::endl;
            }


            // 控制循环速度，避免占用过多CPU
            std::this_thread::sleep_for(milliseconds(10));
            
            // 运行一段时间后退出
            if (connections >= 3 || high_resolution_clock::now() - start_time > seconds(30)) {
                running = false;
            }
        }
        
        std::cout << "--- 测试结束. 共处理 " << connections << " 个连接. ---" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[FATAL] Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
