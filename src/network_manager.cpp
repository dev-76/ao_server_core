#include "network_manager.h"
#include "logger/logger.h"

namespace AO {

    NetworkManager::NetworkManager() {}

    NetworkManager::~NetworkManager() {}

    bool NetworkManager::StartNetwork(const std::string& host, unsigned short port, unsigned int maxConnections, unsigned short threadsCount) {
        m_Host = host;
        m_Port = port;
        m_MaxConnections = maxConnections;
        m_ThreadsCount = threadsCount;

        if(m_IsWorked) {
            Trace(L_ERROR, "NetworkManager already worked");
            return false;
        }

        if(m_ThreadsCount < 1) {
            Trace(L_ERROR, "Incorrect network threads count for initialize NetworkManager");
            return false;
        }

        if(!m_CreateThreads()) {
            Trace(L_ERROR, "Errow when starting network threads");
            return false;
        }

        try {
            boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(m_Host), m_Port);
            m_pAcceptor.reset(new boost::asio::ip::tcp::acceptor(m_GetAcceptorThread()->GetService(), ep));
        } catch(const boost::system::error_code& err) {
            Trace(L_ERROR, "Error when start network manager, details: %s", err.message().c_str());
            return false;
        }

        m_IsWorked = true;
        m_AcceptNewConnection();

        Trace(L_DEBUG, "NetworkManager started successfully");
        return true;
    }

    void NetworkManager::StopNetwork() {
        m_DestroyThreads();
        if(m_pAcceptor.get()) {
            m_pAcceptor->cancel();
            m_pAcceptor.reset();
        }
        Trace(L_DEBUG, "NetworkManager stopped");
    }

    bool NetworkManager::m_CreateThreads() {
        unsigned short lastThreadID = 1;
        for(int i = 0; i < m_ThreadsCount; i++) {
            m_Threads.push_back(std::make_pair<unsigned short, NetworkThread*>(lastThreadID, new NetworkThread(lastThreadID)));
            lastThreadID++;
        }
        for(auto it = m_Threads.begin(); it != m_Threads.end(); it++) {
            it->second->Start();
        }
    }

    void NetworkManager::m_DestroyThreads() {
        if(!m_Threads.empty()) {
            for(auto it = m_Threads.begin(); it != m_Threads.end(); it++) {
                if(it->second) {
                    if(it->second->IsWorked()) {
                        it->second->Stop();
                    }
                    delete it->second;
                }
            }
        }
    }

    NetworkThread* NetworkManager::m_GetAcceptorThread() {
        if(m_Threads.size() && m_Threads[0]->second) {
            return m_Threads[0]->second;
        } else {
            Trace(L_ERROR, "Acceptor thread is invalid");
            return nullptr;
        }
    }

    NetworkThread* NetworkManager::m_GetThreadForNewConnection() {
        if(m_Threads.empty()) {
            Trace(L_ERROR, "Threads pool is empty");
            return nullptr;
        }
        NetworkThread* ptr = m_Threads[0]->second;
        unsigned int minConnCount = m_Threads[0]->second->GetConnectionsCount();
        for(int i = 1; i < m_Threads.size(); i++) {
            unsigned int connCount = m_Threads[i]->second->GetConnectionsCount();
            if(connCount < minConnCount) {
                minConnCount = connCount;
                ptr = it->second;
            }
        }
        return ptr;
    }

    void NetworkManager::OnNewConnection(std::shared_ptr<Socket>& sock, const boost::system::error_code& err) {
        if(err) {
            Trace(L_ERROR, "Error when accept new connection");
            return;
        }
        if(!sock->Open()) {
            Trace(L_ERROR, "Error when open socket");
            sock->Close();
            return;
        }
        m_AcceptNewConnection();
    }

    void NetworkManager::OnSocketOpen(const std::shared_ptr<Socket>& sock) {
        NetworkThread* ptr = sock->GetOwner();
        ptr->AddSocket(sock);
    }

    void NetworkManager::OnSocketClose(const std::shared_ptr<Socket>& sock) {
        NetworkThread* ptr = sock->GetOwner();
        ptr->RemoveSocket(sock);
    }

    std::shared_ptr<Socket> NetworkManager::m_CreateSocket(NetworkThread* pThread) {}

    void NetworkManager::m_AcceptNewConnection() {
        NetworkThread* ptr = m_GetThreadForNewConnection();
        if(!ptr) {
            return;
        }
        std::shared_ptr<Socket> sock = m_CreateSocket(ptr);
        m_pAcceptor->async_accept(sock->GetSocket(), boost::bind(&NetworkManager::OnNewConnection, this, sock, boost::asio::placeholders::error));
    }

}
