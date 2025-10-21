target_sources(http_server_core PRIVATE
                "isocket.cpp"
                "inetaddress.cpp"
                "logger.cpp"
                "ifile.cpp")

target_include_directories(http_server_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")