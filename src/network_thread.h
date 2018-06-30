#ifndef NETWORK_THREAD_H
#define NETWORK_THREAD_H

#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include <thread>

#include "tcp_socket.h"

namespace AO {

    class NetworkThread {
    public:
        NetworkThread(unsigned short threadID);
        ~NetworkThread();

        void Start();
        void Stop();

        void AddSocket(const std::shared_ptr<Socket>& sock);
        void RemoveSocket(const std::shared_ptr<Socket>& sock);

        unsigned int GetConnectionsCount() const;
        boost::asio::io_service& GetService();

    private:
        void m_Work();

        boost::asio::io_service m_Service;
        std::auto_ptr<boost::asio::io_service::work> m_pServiceWork;
        std::auto_ptr<std::thread> m_pThread;
        std::vector<std::shared_ptr<Socket> m_Sockets;
        std::mutex m_Mutex;
        unsigned short m_ThreadID;
        unsigned int m_ConnectionsCount;
        bool m_IsWorked;
    };

}

#endif // !NETWORK_THREAD_H
