#ifndef SINGLETON_H
#define SINGLETON_H

#include <boost/noncopyable.hpp>

namespace AO
{

    template <class T>
    class ISingleton : public boost::noncopyable {
    public:
        static T* Instance() {
            static T instance;
            return &instance;
        }
    };

}

#endif // !SINGLETON_H
