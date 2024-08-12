#include "mysqlConn.h"

mysqlConn::mysqlConn() {
    // 初始化
    m_conn = mysql_init(nullptr);
    // 设置接口编码
    mysql_set_character_set(m_conn, "utf8");
}

mysqlConn::~mysqlConn() {
    if (m_conn != nullptr) {
        mysql_close(m_conn);
    }
    freeResult();
}

bool mysqlConn::connect(std::string user, std::string passwd, std::string dbName, std::string ip, unsigned short port) {
    auto ptr = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), passwd.c_str(), dbName.c_str(), port, nullptr, 0);
    return ptr != nullptr;
}

bool mysqlConn::update(std::string sql) {
    if (mysql_query(m_conn, sql.c_str())) {
        return false;
    }
    return true;
}

bool mysqlConn::query(std::string sql) {
    freeResult();
    if (mysql_query(m_conn, sql.c_str())) {
        return false;
    }
    if (!m_conn) return false;
    m_result = mysql_store_result(m_conn);
    return true;
}

bool mysqlConn::next() {
    if (m_result != nullptr) {
        m_row = mysql_fetch_row(m_result);
        return m_row != nullptr;
    }
    return false;
}

std::string mysqlConn::value(int index) {
    int colCount = mysql_num_fields(m_result);
    if (index >= colCount || index < 0) return std::string();
    char* val = m_row[index];
    unsigned long length = mysql_fetch_lengths(m_result)[index];
    return std::string(val, length);
}

bool mysqlConn::transaction() {
    return mysql_autocommit(m_conn, false);
}

bool mysqlConn::commit() {
    return mysql_commit(m_conn);
}

bool mysqlConn::rollback() {
    return mysql_rollback(m_conn);
}

void mysqlConn::refreshAliveTime() {
    m_aliveTime = std::chrono::steady_clock::now();
}

long long mysqlConn::getAliveTime() {
    std::chrono::nanoseconds res = std::chrono::steady_clock::now() - m_aliveTime;
    std::chrono::milliseconds millsec = std::chrono::duration_cast<std::chrono::milliseconds>(res);
    return millsec.count();
}

void mysqlConn::freeResult() {
    if (m_result) {
        mysql_free_result(m_result);
        m_result = nullptr;
        m_row = nullptr;
    }
}
