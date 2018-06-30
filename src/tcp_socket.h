#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "inet_packet.h"

namespace AO {

    class NetworkManager;
    class NetworkThread;

    class Socket : public boost::enable_shared_from_this<Socket> {
    public:
        Socket(NetworkManager* pManager, NetworkThread* pOwner);
        ~Socket();

        bool Open();
        void Close();

        bool IsClosed() const;
        const std::string& GetRemoteAddress() const;

        bool EnableTCPNoDelay(bool enable);

        boost::asio::ip::tcp::socket& GetSocket();
        NetworkThread* GetOwner();

    private:
        void m_StartAsyncSend();
        void m_StartAsyncRead();

        void m_OnWriteComplete(const boost::system::error_code& err, unsigned int bytesTransferred);
        void m_OnReadComplete(const boost::system::error_code& err, unsigned int bytesTransferred);
        void m_OnError(const boost::system::error_code& err);

        bool m_ProcessIncoming();

        std::string m_ObtainRemoteAddress() const;

        NetworkManager* m_pManager;
        NetworkThread* m_pOwner;
        std::mutex m_OutBufferMutex;
        std::auto_ptr<InetPacket> m_pOutBuffer;
        std::auto_ptr<InetPacket> m_pInBuffer;
        boost::asio::ip::tcp::socket m_Socket;
        unsigned int m_InBufferSize;
        unsigned int m_OutBufferSize;
        std::string m_RemoteAddress;
        bool m_WriteOperation;
        bool m_Closed;
    };

}

#endif // !TCP_SOCKET_H
