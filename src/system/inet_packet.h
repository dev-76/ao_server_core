#ifndef INET_PACKET_H
#define INET_PACKET_H

#include <vector>
#include <map>
#include <list>
#include <utility>
#include <string>
#include <cstring>
#include <exception>

namespace AO {

    struct PacketHeader {
        PacketHeader() {}

        unsigned int packetSize;
        unsigned short opcode;

    };

    class InetPacket {
        friend class IOBuffer;
    public:
        InetPacket(unsigned short opcode = 0) : m_ReadPos(0U), m_WritePos(0U) {
            m_Storage.clear();
            m_Header.opcode = opcode;
            m_Header.packetSize = 0U;
            m_WritePos = m_ReadPos = 0U;
        }

        InetPacket(const InetPacket& other) {
            m_Storage.clear();
            m_Header = other.m_Header;
            m_WritePos = other.m_WritePos;
            m_ReadPos = other.m_ReadPos;
            if(m_Header.packetSize) {
                m_Storage.resize(m_Header.packetSize);
                memcpy((void*)&m_Storage[0], (void*)&other.m_Storage[0], m_Header.packetSize);
            }
        }

        ~InetPacket() {}

        inline PacketHeader& Header() {
            return m_Header;
        }

        inline unsigned int Size() const {
            return m_Storage.size();
        }

        inline void* Data() {
            return (void*)&m_Storage[0];
        }

        inline void Confirm() {
            m_Header.packetSize = m_Storage.size();
        }

        // Just move m_ReadPos to n bytes. Protected
        inline void ReadSkip(unsigned int n) {
            if(m_Storage.size() >= m_ReadPos + n)
                m_ReadPos += n;
        }

        template <typename T>
        inline void ReadSkip() {
            // Prevent memory access exceptions
            if((m_Storage.size() - m_ReadPos) < sizeof(T))
                return;

            m_ReadPos += sizeof(T);
        }

        inline InetPacket& operator<<(const char& val) {
            m_Append<char>(val);
            return *this;
        }

        inline InetPacket& operator<<(const unsigned char& val) {
            m_Append<unsigned char>(val);
            return *this;
        }

        inline InetPacket& operator<<(const short& val) {
            m_Append<unsigned char>(val);
            return *this;
        }

        inline InetPacket& operator<<(const unsigned short& val) {
            m_Append<unsigned short>(val);
            return *this;
        }

        inline InetPacket& operator<<(const int& val) {
            m_Append<int>(val);
            return *this;
        }

        inline InetPacket& operator<<(const unsigned int& val) {
            m_Append<unsigned int>(val);
            return *this;
        }

        inline InetPacket& operator<<(const long long& val) {
            m_Append<long long>(val);
            return *this;
        }

        inline InetPacket& operator<<(const unsigned long long& val) {
            m_Append<unsigned long long>(val);
            return *this;
        }

        inline InetPacket& operator<<(const float& val) {
            m_Append<float>(val);
            return *this;
        }

        inline InetPacket& operator<<(const double& val) {
            m_Append<double>(val);
            return *this;
        }

        inline InetPacket& operator<<(const char* val) {
            unsigned int size = strlen(val);
            unsigned int i = 0U;
            while(size--) {
                m_Append<char>(*(val + i));
                i++;
            }
            m_Append<char>('\0');
            return *this;
        }

        inline InetPacket& operator<<(const std::string& val) {
            unsigned int size = val.size();
            unsigned int i = 0U;
            while(size--) {
                m_Append<char>(val[i]);
                i++;
            }
            m_Append<char>('\0');
            return *this;
        }

        inline InetPacket& operator>>(char& target) {
            m_Read<char>(target);
            return *this;
        }

        inline InetPacket& operator>>(unsigned char& target) {
            m_Read<unsigned char>(target);
            return *this;
        }

        inline InetPacket& operator>>(short& target) {
            m_Read<short>(target);
            return *this;
        }

        inline InetPacket& operator>>(unsigned short& target) {
            m_Read<unsigned short>(target);
            return *this;
        }

        inline InetPacket& operator>>(int& target) {
            m_Read<int>(target);
            return *this;
        }

        inline InetPacket& operator>>(unsigned int& target) {
            m_Read<unsigned int>(target);
            return *this;
        }

        inline InetPacket& operator>>(long long& target) {
            m_Read<long long>(target);
            return *this;
        }

        inline InetPacket& operator>>(unsigned long long& target) {
            m_Read<unsigned long long>(target);
            return *this;
        }

        inline InetPacket& operator>>(float& target) {
            m_Read<float>(target);
            return *this;
        }

        inline InetPacket& operator>>(double& target) {
            m_Read<double>(target);
            return *this;
        }

        inline InetPacket& operator>>(std::string& target) {
            char c;
            std::string tmp("");
            while(m_ReadPos < m_Header.packetSize) {
                m_Read<char>(c);
                if(c == '\0') {
                    target = tmp;
                    return *this;
                }
                tmp += c;
            }
            return *this;
        }

        // IMPORTANT!!! Methods for serialize\deserialize STL containers
        // can to use only for the basic types of containers, user-defined types not allowed
        template <typename T>
        inline void SerializeVector(const std::vector<T>& val) {
            unsigned int vecSize = val.size();
            if(!vecSize)
                return;

            (*this) << vecSize;
            for(typename std::vector<T>::iterator it = val.begin(); it != val.end(); it++)
                (*this) << (*it);
        }

        template <typename T>
        inline void DeserializeVector(std::vector<T>& target) {
            unsigned int vecSize;
            (*this) >> vecSize;

            T value;
            while(vecSize--) {
                (*this) >> value;
                target.push_back(value);
            }
        }

        template <typename T>
        inline void SerializeList(const std::list<T>& val) {
            unsigned int listSize = val.size();
            if(!listSize)
                return;

            (*this) << listSize;
            for(typename std::list<T>::iterator it = val.begin(); it != val.end(); it++)
                (*this) << (*it);
        }

        template <typename T>
        inline void DeserializeList(std::list<T>& target) {
            unsigned int listSize;
            (*this) >> listSize;

            T value;
            while(listSize--) {
                (*this) >> value;
                target.push_back(value);
            }
        }

        template <typename t_Key, typename t_Value>
        inline void SerializeMap(const std::map<t_Key, t_Value>& val) {
            unsigned int mapSize = val.size();
            if(!mapSize)
                return;

            (*this) << (unsigned int)mapSize;
            for(typename std::map<t_Key, t_Value>::iterator it = val.begin(); it != val.end(); it++)
                (*this) << it->first << it->second;
        }

        template <typename t_Key, typename t_Value>
        inline void DeserializeMap(std::map<t_Key, t_Value>& target) {
            unsigned int mapSize;
            (*this) >> mapSize;

            t_Key key;
            t_Value value;
            while(mapSize--) {
                (*this) >> key >> value;
                target.insert(std::make_pair<t_Key, t_Value>(key, value));
            }
        }

    private:
        template <typename T>
        inline void m_Append(const T& val) {
            m_Storage.resize(m_Storage.size() + sizeof(T));
            memcpy((void*)&m_Storage[m_WritePos], (void*)&val, sizeof(T));
            m_WritePos += sizeof(T);
        }

        template <typename T>
        inline void m_Read(T& val) {
            // Prevent memory access exceptions
            if((m_Storage.size() - m_ReadPos) < sizeof(T))
                return;

            val = (T)(*(&m_Storage[m_ReadPos]));
            m_ReadPos += sizeof(T);
        }

        PacketHeader m_Header;
        std::vector<unsigned char> m_Storage;
        unsigned int m_WritePos;
        unsigned int m_ReadPos;
    };

    /*
    **      ______________________________________________________
    **      |   2 bytes   |   4 bytes      |  <packetSize> bytes  |
    **      |   opcode    |   packetSize   |   content            |
    **      ______________________________________________________
    */

    class IOBuffer {
    public:
        IOBuffer(unsigned int size) : m_BytesUsed(0U), m_WritePos(0U), m_ReadPos(0U), m_Size(size), m_BinaryData(nullptr) {
            m_BinaryData = new char[size];
        }

        ~IOBuffer() {
            if(m_BinaryData)
                delete[] m_BinaryData;
        }

        bool AttachPacket(InetPacket* packet) {
            if(!packet->Size())
                return false;

            if(packet->Size() > m_Size - m_BytesUsed)
                return false;

            memcpy((char*)m_BinaryData + m_WritePos, (void*)&packet->Header().opcode, 2);
            m_WritePos += 2;
            memcpy((char*)m_BinaryData + m_WritePos, (void*)&packet->Header().packetSize, 4);
            m_WritePos += 4;
            memcpy((char*)m_BinaryData + m_WritePos, packet->Data(), packet->Size());
            m_WritePos += packet->Size();
            m_BytesUsed += 2 + 4 + packet->m_Storage.size();
            return true;
        }

        bool DetachPacket(InetPacket** packet) {
            // Prevent memory access error
            if(m_ReadPos + 6 > m_BytesUsed)
                return false;

            unsigned short opcode = (unsigned short)*(m_BinaryData + m_ReadPos);
            m_ReadPos += 2;
            unsigned int pktSize = (unsigned int)*(m_BinaryData + m_ReadPos);
            m_ReadPos += 4;

            *packet = new InetPacket(opcode);
            (*packet)->Header().packetSize = pktSize;

            // Prevent memory access error
            if(m_ReadPos + pktSize > m_BytesUsed)
                return false;

            // Fill binary data. Packet's storage must be empty here
            // If packet size is zero then skip extraction of data
            if((*packet)->Header().packetSize) {
                (*packet)->m_Storage.resize(pktSize);
                memcpy((void*)&(*packet)->m_Storage[0], (char*)m_BinaryData + m_ReadPos, pktSize);
                (*packet)->m_ReadPos = (*packet)->m_WritePos = 0U;
                m_ReadPos += pktSize;
            }
            return true;
        }

        unsigned int Size() const {
            return m_BytesUsed;
        }

        void* Data() {
            return (void*)m_BinaryData;
        }

        inline bool IsReadable() const {
            return m_ReadPos < m_BytesUsed;
        }

        inline bool IsWritable() const {
            return m_BytesUsed < m_Size;
        }

        void Reset() {
            memset((void*)m_BinaryData, 0x00, m_Size);
            m_BytesUsed = 0U;
            m_ReadPos = m_WritePos = 0U;
        }

    private:
        char* m_BinaryData;
        unsigned int m_BytesUsed;
        unsigned int m_WritePos;
        unsigned int m_ReadPos;
        unsigned int m_Size;
    };

}

#endif // INET_PACKET_H
