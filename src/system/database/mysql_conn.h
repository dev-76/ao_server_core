#ifndef MYSQL_CONN_H
#define MYSQL_CONN_H

#include <mutex>
#include <string>

#include "../logger/logger.h"
#include "common_mysql.h"
#include "mysql_result_set.h"

namespace AO
{
    class MySQLConnection {
    public:
        MySQLConnection(const std::string& dbHost, const std::string& dbPort, const std::string& dbUser, const std::string& dbPassword, const std::string& dbName);

        virtual bool Connect() = 0;

        virtual void Disconnect() = 0;

        virtual bool Ping() = 0;

        virtual ~MySQLConnection() {}

    protected:
        MYSQL* m_GetConnectionNative();
        std::string m_GetTracebackInfo() const;

        std::string m_DatabaseHost;
        std::string m_DatabasePort;
        std::string m_DatabaseUser;
        std::string m_DatabasePassword;
        std::string m_DatabaseName;
        MYSQL* m_pMYSQL;
        bool m_Connected;
    };

    class MySQLConnectionSync : public MySQLConnection {
    public:
        MySQLConnectionSync(const std::string& dbHost, const std::string& dbPort, const std::string& dbUser, const std::string& dbPassword, const std::string& dbName);

        bool Connect() override final;

        void Disconnect() override final;

        bool Ping() override final;

        std::shared_ptr<MySQLResultSet> Query(const std::string& sql);

        unsigned long long Execute(const std::string& sql);

        void Lock();
        bool TryLock();
        void Unlock();

        unsigned long ThreadID();

        void EscapeString(char* dest, const char* source, size_t length);

    private:
        std::mutex m_Mutex;
    };

    class MySQLConnectionAsync : public MySQLConnection {
    public:
        MySQLConnectionAsync(const std::string& dbHost, const std::string& dbPort, const std::string& dbUser, const std::string& dbPassword, const std::string& dbName);

        bool Connect() override final { return true; }

        void Disconnect() override final {}

        bool Ping() override final { return true; }
    };
}

#endif // !MYSQL_CONN_H
