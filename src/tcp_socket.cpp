#include "tcp_socket.h"

#include "logger/logger.h"
#include "network_manager.h"
#include "network_thread.h"
#include "inet_packet.h"

namespace AO {

    Socket::Socket(NetworkManager* pManager, NetworkThread* pOwner) : m_pManager(pManager), m_pOwner(pOwner), m_Socket(m_pOwner->GetService()), m_OutBufferSize(4096), m_InBufferSize(4096),
        m_RemoteAddress(""), m_WriteOperation(false), m_Closed(true) {}

    Socket::~Socket() {}

    bool Socket::Open() {
        m_RemoteAddress = m_ObtainRemoteAddress();
        if(m_RemoteAddress.empty()) {
            return false;
        }
        if(!m_pManager->OnSocketOpen(shared_from_this())) {
            return false;
        }
        m_Closed = false;
        m_pOutBuffer.reset(new InetPacket());
        m_pInBuffer.reset(new InetPacket());
        m_StartAsyncRead();

        return true;
    }

    void Socket::Close() {
        if(IsClosed()) {
            return;
        }
        m_Closed = true;
        try {
            if(m_Socket.is_open()) {
                m_Socket.shutdown(boost::asio::socket_base::shutdown_both);
                m_Socket.close();
            }
        } catch(const boost::system::error_code& err) {
            Trace(L_ERROR, "Error when close socket: %s", err.message().c_str());
        }
    }

    bool Socket::EnableTCPNoDelay(bool enable) {
        try {
            m_Socket.set_option(boost::asio::ip::tcp::no_delay(enable));
        } catch(const boost::system::error_code& err) {
            Trace(L_ERROR, "Error when try to set TCPNoDelay option: %s", err.message().c_str());
            return false;
        }
        return true;
    }



}
