#ifndef MYSQL_DATABASE_H
#define MYSQL_DATABASE_H

#include <string>
#include <vector>
#include <map>

#include "../logger/logger.h"
#include "../utilities.h"
#include "mysql_conn.h"
#include "mysql_result_set.h"


namespace AO
{
    class TransactionHelper {
    public:
        TransactionHelper(size_t threadID, MySQLConnectionSync* pConnection) : m_ThreadID(threadID), m_pConnection(pConnection) {}

        MySQLConnectionSync* GetConnection() const { return m_pConnection; }

    private:
        size_t m_ThreadID;
        MySQLConnectionSync* m_pConnection;
    };

    class ConnectionLockHelper {
    public:
        ConnectionLockHelper(MySQLConnectionSync* connPtr) : m_pConn(connPtr) {}

        ~ConnectionLockHelper() {
            m_pConn->Unlock();
        }

    private:
        MySQLConnectionSync* m_pConn;
    };

    class MySQLDatabase {
    public:
        MySQLDatabase(const std::string& dbHost, const std::string& dbPort, const std::string& dbUser, const std::string& dbPassword, const std::string& dbName,
            size_t syncConnCount, size_t asyncConnCount);

        bool InitializeConnections();

        void DestroyConnections();

        void ThreadStart();

        void ThreadEnd();

        bool IsInitialized() const {
            return m_IsInitialized;
        }

        // Sync queries
        std::shared_ptr<MySQLResultSet> Query(const std::string& sql);
        unsigned long long Execute(const std::string& sql);

        template <typename ... Arguments>
        std::shared_ptr<MySQLResultSet> Query(const std::string & sql, Arguments&& ... args)
        {
            bool inTransaction = false;
            MySQLConnectionSync* pConn = m_GetNextExecutor(inTransaction);
            if(!inTransaction) {
                ConnectionLockHelper lock(pConn);
                return pConn->Query(AO_StrFormat(sql.c_str(), args...));
            }
            return pConn->Query(AO_StrFormat(sql.c_str(), args...));
        }

        template <typename ... Arguments>
        unsigned long long Execute(const std::string & sql, Arguments&& ... args)
        {
            bool inTransaction = false;
            MySQLConnectionSync* pConn = m_GetNextExecutor(inTransaction);
            if(!inTransaction) {
                ConnectionLockHelper lock(pConn);
                return pConn->Execute(AO_StrFormat(sql.c_str(), args...));
            }
            return pConn->Execute(AO_StrFormat(sql.c_str(), args...));
        }

        void BeginTransaction();
        void CommitTransaction();
        void RollbackTransaction();

        void EscapeString(std::string& sql);

    private:
        MySQLConnectionSync* m_GetNextExecutor(bool& intoTransaction);

        std::string m_DatabaseHost;
        std::string m_DatabasePort;
        std::string m_DatabaseUser;
        std::string m_DatabasePassword;
        std::string m_DatabaseName;

        std::vector<MySQLConnectionSync*> m_SyncConnections;
        std::vector<MySQLConnectionAsync*> m_AsyncConnections;
        std::map<size_t, TransactionHelper*> m_TransactionsStorage;
        size_t m_SyncConnectionsCount;
        size_t m_AsyncConnectionsCount;
        unsigned int m_LastSyncConnectionExecution;
        bool m_IsInitialized;
    };

}

#endif // !MYSQL_DATABASE_H
