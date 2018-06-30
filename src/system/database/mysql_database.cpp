#include "mysql_database.h"

#include <thread>
#include <stdio.h>

#include "../utilities.h"

namespace AO
{
    MySQLDatabase::MySQLDatabase(const std::string& dbHost, const std::string& dbPort, const std::string& dbUser, const std::string& dbPassword, const std::string& dbName,
        size_t syncConnCount, size_t asyncConnCount)
        : m_DatabaseHost(dbHost), m_DatabasePort(dbPort), m_DatabaseUser(dbUser), m_DatabasePassword(dbPassword), m_DatabaseName(dbName),
          m_SyncConnectionsCount(syncConnCount), m_AsyncConnectionsCount(asyncConnCount), m_LastSyncConnectionExecution(-1){
    }

    bool MySQLDatabase::InitializeConnections() {
        unsigned int mysqlThreadSafe = mysql_thread_safe();
        if(!mysqlThreadSafe) {
            Trace(L_ERROR, "MYSQL client compiled with non thread-safe mode");
            return false;
        }
        for(size_t i = 0; i < m_SyncConnectionsCount; i++) {
            MySQLConnectionSync* ptr = new MySQLConnectionSync(m_DatabaseHost, m_DatabasePort, m_DatabaseUser, m_DatabasePassword, m_DatabaseName);
            if(!ptr->Connect()) {
                return false;
            }
            m_SyncConnections.push_back(ptr);
        }

        for(size_t i = 0; i < m_SyncConnectionsCount; i++) {
            MySQLConnectionAsync* ptr = new MySQLConnectionAsync(m_DatabaseHost, m_DatabasePort, m_DatabaseUser, m_DatabasePassword, m_DatabaseName);
            if(!ptr->Connect()) {
                return false;
            }
            m_AsyncConnections.push_back(ptr);
        }

        m_IsInitialized = true;
        return true;
    }

    void MySQLDatabase::DestroyConnections() {
        for(auto it = m_SyncConnections.begin(); it != m_SyncConnections.end(); it++) {
            (*it)->Disconnect();
            delete *it;
        }
        m_SyncConnections.clear();

        for(auto it = m_AsyncConnections.begin(); it != m_AsyncConnections.end(); it++) {
            (*it)->Disconnect();
            delete *it;
        }
        m_AsyncConnections.clear();
        m_IsInitialized = false;
    }

    void MySQLDatabase::ThreadStart() {
        mysql_thread_init();
    }

    void MySQLDatabase::ThreadEnd() {
        mysql_thread_end();
    }

    std::shared_ptr<MySQLResultSet> MySQLDatabase::Query(const std::string& sql)
    {
        bool inTransaction = false;
        MySQLConnectionSync* pConn = m_GetNextExecutor(inTransaction);
        if(!inTransaction) {
            ConnectionLockHelper lock(pConn);
            return pConn->Query(sql);
        }
        return pConn->Query(sql);
    }

    unsigned long long MySQLDatabase::Execute(const std::string& sql)
    {
        bool inTransaction = false;
        MySQLConnectionSync* pConn = m_GetNextExecutor(inTransaction);
        if(!inTransaction) {
            ConnectionLockHelper lock(pConn);
            return pConn->Execute(sql);
        }
        return pConn->Execute(sql);
    }

    void MySQLDatabase::BeginTransaction() {
        size_t threadID = ThreadID();

        // Prevent dublicate transactions
        auto it = m_TransactionsStorage.find(threadID);
        if(it != m_TransactionsStorage.end())
            return;

        bool tmp;
        MySQLConnectionSync* pConn = m_GetNextExecutor(tmp);
        TransactionHelper* pTransacton = new TransactionHelper(threadID, pConn);
        m_TransactionsStorage[threadID] = pTransacton;
        pConn->Execute("START TRANSACTION");
    }

    void MySQLDatabase::CommitTransaction() {
        auto it = m_TransactionsStorage.find(ThreadID());
        if(it == m_TransactionsStorage.end())
            return;

        MySQLConnectionSync* pConn = it->second->GetConnection();
        pConn->Execute("COMMIT");
        pConn->Unlock();
        delete it->second;
        m_TransactionsStorage.erase(it);
    }

    void MySQLDatabase::RollbackTransaction() {
        auto it = m_TransactionsStorage.find(ThreadID());
        if(it == m_TransactionsStorage.end())
            return;

        MySQLConnectionSync* pConn = it->second->GetConnection();
        pConn->Execute("ROLLBACK");
        pConn->Unlock();
        delete it->second;
        m_TransactionsStorage.erase(it);
    }

    void MySQLDatabase::EscapeString(std::string &sql) {
        if(sql.empty())
            return;

        char* buff = new char[sql.size() * 2 + 1];
        bool tmp;
        MySQLConnectionSync* pConn = m_GetNextExecutor(tmp);
        pConn->EscapeString(buff, sql.c_str(), sql.size());
        sql = buff;
        delete[] buff;
    }

    MySQLConnectionSync* MySQLDatabase::m_GetNextExecutor(bool& intoTransaction) {
        MySQLConnectionSync* ptr = nullptr;
        // Select connection for transaction
        // If transaction found then return connection which locked for this transaction
        auto it = m_TransactionsStorage.find(ThreadID());
        if(it != m_TransactionsStorage.end()) {
            ptr = it->second->GetConnection();
            intoTransaction = true;
            return ptr;
        }

        // If not in transaction - search free connection and lock it
        for(auto it = m_SyncConnections.begin(); it != m_SyncConnections.end(); it++) {
            ptr = *it;
            if(ptr->TryLock())
                return ptr;
        }

        // If free connections are not found - use round-robin connection selection and lock it
        if(m_LastSyncConnectionExecution == m_SyncConnectionsCount - 1)
            m_LastSyncConnectionExecution = -1;
        ptr = m_SyncConnections[++m_LastSyncConnectionExecution];
        ptr->Lock();
        return ptr;
    }
}
