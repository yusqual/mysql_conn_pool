# MySQL连接池
## 参考链接

<a herf="https://www.bilibili.com/video/BV1Fr4y1s7w4?p=16&vd_source=9d3fcd844b0ad55a330b3afec653d682" title="基于C++11的数据库连接池【C++/数据库/多线程/MySQL】">基于C++11的数据库连接池【C++/数据库/多线程/MySQL】</a>

## 环境和工具
    # WSL2 (Ubuntu22.04)
    # MySQL
    # CMake
    # vcpkg (三方库管理工具)

### CMake 
    # 使用vcpkg需要设置其路径等参数
    set(CMAKE_TOOLCHAIN_FILE "<path to vcpkg>/scripts/buildsystems/vcpkg.cmake")
    set(VCPKG_TARGET_TRIPLET "x64-linux")
    # 添加链接库
    find_package(jsoncpp CONFIG REQUIRED)
    target_link_libraries(main PRIVATE mysqlclient JsonCpp::JsonCpp)

    # 在构建时需要将 dbConfig.json 文件复制到正确的输出目录
    configure_file(dbConfig.json ${CMAKE_CURRENT_BINARY_DIR}/dbConfig.json COPYONLY)

### vcpkg
    # 安装三方库的最后,会提示该库在CMakeLists.txt中需要添加的参数,如:

    # JsonCpp: 
    find_package(jsoncpp CONFIG REQUIRED)
    target_link_libraries(PROJECT PRIVATE JsonCpp::JsonCpp)

    # oatpp:
    find_package(oatpp CONFIG REQUIRED)
    target_link_libraries(PROJECT PRIVATE oatpp::oatpp oatpp::oatpp-test)


## 部分自定义的头文件

### myheads/share_tools.h
    #include <myheads/base.h>
    #include <mutex>
    #include <condition_variable>
    #include <atomic>

### myheads/base.h
    #ifndef _MYHEADS_BASE_
    #define _MYHEADS_BASE_

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <math.h>
    #include <errno.h>
    #include <unistd.h>

    static inline void err_exit(const char* msg, bool quit = true) {
        perror(msg);
        if (quit) exit(-1);
    }

    #endif  // _MYHEADS_BASE_