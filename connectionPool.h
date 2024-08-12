#ifndef _CONNECTIONPOOL_H_
#define _CONNECTIONPOOL_H_

#include <myheads/share_tools.h>
#include "mysqlConn.h"
#include <queue>

class connectionPool {
public:
    static connectionPool* getConnectPool();

    connectionPool(const connectionPool& obj) = delete;

    connectionPool& operator=(const connectionPool& obj) = delete;

    std::shared_ptr<mysqlConn> getConn();  // 消费者, 取出一个连接

    ~connectionPool();
private:
    connectionPool();
    bool parseJsonFile();  // 解析json配置文件dbConfig.json

    void addConn();
    void produceConn();  // 生产者
    void recycleConn();

    std::string m_ip;
    std::string m_user;
    std::string m_passwd;
    std::string m_dbName;
    unsigned short m_port;
    int m_minSize;
    int m_maxSize;
    int m_timeout;
    int m_maxIdleTime;
    std::atomic<int> m_allAliveNum;     // 所有连接数量,包括队列中的以及被取出的
    std::atomic<bool> m_open;
    std::queue<mysqlConn*> m_connQueue;
    std::mutex m_mutexQ;
    std::condition_variable m_cv_producer, m_cv_consumer;
};

#endif  // _CONNECTIONPOOL_H_