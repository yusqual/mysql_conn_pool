#include "connectionPool.h"
#include <thread>

void op1(int begin, int end) {
    for (int i = begin; i < end; ++i) {
        mysqlConn conn;
        conn.connect("root", "0", "db_test", "localhost");
        std::string sql = "insert into person(name, sex, age) values('尻比', 'man', 24)";
        conn.update(sql);
    }
}

void op2(connectionPool* pool, int begin, int end) {
    for (int i = begin; i < end; ++i) {
        auto conn = pool->getConn();
        std::string sql = "insert into person(name, sex, age) values('尻比', 'man', 24)";
        conn->update(sql);
    }
}

void test1() {
#if 1
    auto begin = std::chrono::steady_clock::now();
    op1(0, 5000);
    auto end = std::chrono::steady_clock::now();
    auto length = end - begin;
    std::cout << "非连接池单线程用时: " << length.count() << "纳秒, "
              << length.count() / 1000000 << " 毫秒\n";
#else
    connectionPool* pool = connectionPool::getConnectPool();
    auto begin = std::chrono::steady_clock::now();
    op2(pool, 0, 5000);
    auto end = std::chrono::steady_clock::now();
    auto length = end - begin;
    std::cout << "连接池单线程用时: " << length.count() << "纳秒, "
              << length.count() / 1000000 << " 毫秒\n";
#endif
}

void test2() {
#if 0
    mysqlConn conn;
    conn.connect("root", "0", "db_test", "localhost");  // 提前连接一次防止多个连接同时连接出问题
    auto begin = std::chrono::steady_clock::now();
    std::thread t1(op1, 0, 1000);
    std::thread t2(op1, 1000, 2000);
    std::thread t3(op1, 2000, 3000);
    std::thread t4(op1, 3000, 4000);
    std::thread t5(op1, 4000, 5000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    auto end = std::chrono::steady_clock::now();
    auto length = end - begin;
    std::cout << "非连接池多线程用时: " << length.count() << "纳秒, "
              << length.count() / 1000000 << " 毫秒\n";
#else
    connectionPool* pool = connectionPool::getConnectPool();
    auto begin = std::chrono::steady_clock::now();
    std::thread t1(op2, pool, 0, 1000);
    std::thread t2(op2, pool, 1000, 2000);
    std::thread t3(op2, pool, 2000, 3000);
    std::thread t4(op2, pool, 3000, 4000);
    std::thread t5(op2, pool, 4000, 5000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    auto end = std::chrono::steady_clock::now();
    auto length = end - begin;
    std::cout << "连接池多线程用时: " << length.count() << "纳秒, "
              << length.count() / 1000000 << " 毫秒\n";
#endif
}

void test() {
    mysqlConn ac;
    ac.connect("root", "0", "db_test", "localhost");
    std::string sql = "insert into person(name, sex, age) values('尻比', 'man', 24)";
    ac.update(sql);
    sql = "select * from person";
    ac.query(sql);
    while (ac.next()) {
        std::cout << ac.value(0) << " " << ac.value(1) << " " << ac.value(2) << " " << ac.value(3) << std::endl;
    }
}

int main() {
    test2();
    return 0;
}