#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include <thread>
#include <atomic>

namespace AO {

    class SpinLock {

    public:
        SpinLock() {
            m_LockFlag.clear();
        }

        bool try_lock() {
            return !m_LockFlag.test_and_set(std::memory_order_acquire);
        }

        void lock() {
            for (size_t i = 0; !try_lock(); ++i) if (i % 100 == 0) std::this_thread::yield();
        }

        void unlock() {
            m_LockFlag.clear(std::memory_order_release);
        }

    private:
        std::atomic_flag m_LockFlag;
    };

}

#endif // SPIN_LOCK_H
