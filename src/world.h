#ifndef WORLD_H
#define WORLD_H

#include <list>
#include <string>

namespace AO {

    class World {
    public:
        class Session;
        class Socket;

        World();

        void Start();
        void Update();
        void Stop();

        bool IsRunning();

        void CreateSession(Socket* pSock, );

    private:
        void m_Run();

        std::list<Session*> m_Sessions;

    };

}

#endif // !WORLD_H
