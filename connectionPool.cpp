#include "connectionPool.h"
#include <json/json.h>
#include <fstream>
#include <thread>

connectionPool* connectionPool::getConnectPool() {
    static connectionPool pool;
    return &pool;
}

bool connectionPool::parseJsonFile() {
    std::ifstream ifs("dbConfig.json");
    Json::Value root;
    Json::Reader rd;
    rd.parse(ifs, root);
    if (root.isObject()) {
        m_ip = root["ip"].asString();
        m_user = root["userName"].asString();
        m_passwd = root["password"].asString();
        m_dbName = root["dbName"].asString();
        m_port = root["port"].asUInt();
        m_minSize = root["minSize"].asInt();
        m_maxSize = root["maxSize"].asInt();
        m_maxIdleTime = root["maxIdleTime"].asInt();
        m_timeout = root["timeout"].asInt();
        return true;
    }
    return false;
}

void connectionPool::addConn() {
    mysqlConn* conn = new mysqlConn();
    if (!conn->connect(m_user, m_passwd, m_dbName, m_ip, m_port)) {
        err_exit("mysql connect");
    }
    conn->refreshAliveTime();
    m_connQueue.push(conn);
    ++m_allAliveNum;
}

void connectionPool::produceConn() {
    while (m_open) {
        std::unique_lock<std::mutex> locker(m_mutexQ);
        m_cv_producer.wait(locker, [&] { return (m_connQueue.size() < m_minSize) || !m_open; });
        if (m_open) addConn();
        m_cv_consumer.notify_all();
    }
}

void connectionPool::recycleConn() {
    while (m_open) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::unique_lock<std::mutex> locker(m_mutexQ);
        while (m_connQueue.size() > m_minSize) {
            mysqlConn* conn = m_connQueue.front();
            if (conn->getAliveTime() >= m_maxIdleTime) {
                m_connQueue.pop();
                --m_allAliveNum;
                delete conn;
            } else {
                break;
            }
        }
    }
}

std::shared_ptr<mysqlConn> connectionPool::getConn() {
    std::unique_lock<std::mutex> locker(m_mutexQ);
    while (m_connQueue.empty()) {
        if (std::cv_status::timeout == m_cv_consumer.wait_for(locker, std::chrono::milliseconds(m_timeout))) {
            if (m_connQueue.empty()) {
                // return nullptr;
                continue;
            }
        }
    }
    // 指定共享指针删除器为归还数据库连接
    std::shared_ptr<mysqlConn> connptr(m_connQueue.front(), [this](mysqlConn* conn) {
        std::lock_guard<std::mutex> locker(m_mutexQ);
        conn->refreshAliveTime();
        m_connQueue.push(conn);
    });
    m_connQueue.pop();
    m_cv_producer.notify_all();
    return connptr;
}

connectionPool::~connectionPool() {
    // 关闭生产和回收线程
    m_open = false;
    m_cv_producer.notify_all();
    // 关闭所有数据库连接
    while (m_allAliveNum) {
        while (!m_connQueue.empty()) {
            auto conn = m_connQueue.front();
            m_connQueue.pop();
            --m_allAliveNum;
            delete conn;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

connectionPool::connectionPool() {
    // 加载配置文件
    if (!parseJsonFile()) {
        return;
    }
    m_open = true;
    for (int i = 0; i < m_minSize; ++i) {
        addConn();
    }
    std::thread producer(&connectionPool::produceConn, this);
    std::thread recycler(&connectionPool::recycleConn, this);
    producer.detach();
    recycler.detach();
}