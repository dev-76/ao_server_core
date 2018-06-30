#include "network_thread.h"
#include "logger/logger.h"
#include "database/mysql_interface.h"

namespace AO {

    NetworkThread::NetworkThread(unsigned short threadID) : m_ThreadID(threadID), m_ConnectionsCount(0U) {}

    NetworkThread::~NetworkThread() {}

    void NetworkThread::Start() {
        m_pServiceWork.reset(new boost::asio::io_service::work(m_Service));
        m_pThread.reset(new std::thread(&NetworkThread::m_Work, this));
    }

    void NetworkThread::Stop() {
        m_pServiceWork.reset();
        m_Service.stop();
        if(m_pThread.get() && m_pThread->joinable()) {
            m_pThread->join();
            m_pThread.reset();
        }
    }

    void NetworkThread::AddSocket(const std::shared_ptr<Socket>& sock) {
        m_ConnectionsCount++;
        std::lock_guard<std::mutex> guard(m_Mutex);
        m_Sockets.push_back(sock);
    }

    void NetworkThread::RemoveSocket(const std::shared_ptr<Socket>& sock) {
        m_ConnectionsCount--;
        std::lock_guard<std::mutex> guard(m_Mutex);
        m_Sockets.erase(sock);
    }

    boost::asio::io_service& NetworkThread::GetService() {
        return m_Service;
    }

    unsigned int NetworkThread::GetConnectionsCount() const {
        return m_ConnectionsCount;
    }

    void NetworkThread::m_Work() {
        Trace(L_DEBUG, "Network thread with ID: %i starting", m_ThreadID);
        AO::AccountsDatabase->ThreadStart();
        m_Service.run();
        AO::AccountsDatabase->ThreadEnd();
        Trace(L_DEBUG, "Network thread with ID: %i stopping", m_ThreadID);
    }

}
