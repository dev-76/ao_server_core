#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <string>
#include <vector>
#include <boost/asio.hpp>

#include "singleton.h"
#include "network_thread.h"

namespace AO {
    class NetworkManager : public ISingleton<NetworkManager> {
    public:
        NetworkManager();

        bool StartNetwork(const std::string& host, unsigned short port, unsigned int maxConnections, unsigned short threadsCount);
        void StopNetwork();

    private:
        bool m_CreateThreads();
        void m_DestroyThreads();
        NetworkThread* m_GetAcceptorThread();
        NetworkThread* m_GetThreadForNewConnection();

        void OnNewConnection(std::shared_ptr<Socket>& sock, const boost::system::error_code& err);
        void OnSocketOpen(const std::shared_ptr<Socket>& sock);
        void OnSocketClose(const std::shared_ptr<Socket>& sock);

        std::shared_ptr<Socket> m_CreateSocket(NetworkThread* pThread);
        void m_AcceptNewConnection();

        std::vector<std::pair<unsigned short,NetworkThread*>> m_Threads;
        std::auto_ptr<boost::asio::ip::tcp::acceptor> m_pAcceptor;
        std::string m_Host;
        unsigned short m_Port;
        unsigned short m_ThreadsCount;
        unsigned int m_MaxConnections;
        unsigned int m_ConnectionsCount;
        bool m_IsWorked;

    };

}

#define sNetManager AO::NetworkManager::Instance()

#endif // !NETWORK_MANAGER_H
