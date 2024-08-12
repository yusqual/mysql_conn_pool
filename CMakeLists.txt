cmake_minimum_required(VERSION 3.10)

project(mysql_conn_pool)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 包含vcpkg
set(CMAKE_TOOLCHAIN_FILE "/home/ausnty/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_TARGET_TRIPLET "x64-linux")

# 在构建时将 dbConfig.json 文件复制到正确的输出目录
configure_file(dbConfig.json ${CMAKE_CURRENT_BINARY_DIR}/dbConfig.json COPYONLY)

add_executable(main main.cpp mysqlConn.cpp connectionPool.cpp)

# 添加链接库
find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(main PRIVATE mysqlclient JsonCpp::JsonCpp)