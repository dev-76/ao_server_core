#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <map>
#include <fstream>
#include <queue>
#include <atomic>
#include <mutex>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <stdarg.h>

#include "../singleton.h"
#include "../utilities.h"

namespace AO {

    static const std::string ERROR =     "[ERROR]";
    static const std::string WARNING =   "[WARNING]";
    static const std::string VERBOSE =   "[VERBOSE]";
    static const std::string DEBUG =     "[DEBUG]";

    enum E_LOG_LEVEL {
        L_ERROR,
        L_WARNING,
        L_VERBOSE,
        L_DEBUG
    };

    class Logger : public ISingleton<Logger> {
    public:
        Logger();
        ~Logger();
        bool Open(const std::string& logfile, E_LOG_LEVEL logLevel);
        void Close();
        bool CheckLogPermissions(E_LOG_LEVEL required) const {
            return (required <= m_LogLevel);
        }

        inline void WriteMessage(const std::string& message) {
            if(m_pFile && m_pFile->is_open()) {
                m_pFile->write(message.c_str(), message.size());
                m_pFile->flush();
            }
        }

        inline std::string MakeTimeStamp() const {
            return boost::posix_time::to_iso_extended_string(boost::posix_time::second_clock::local_time());
        }

    private:
        std::string m_Filename;
        std::ofstream* m_pFile;
        std::mutex m_Mutex;
        E_LOG_LEVEL m_LogLevel;
    };

    #define sLog AO::Logger::Instance()

    template <typename... Arguments>
    void TraceMsg(E_LOG_LEVEL l, const char* func,  int line, const char* str, Arguments&&... args) {
        if(Logger::Instance()->CheckLogPermissions(l)) {
            switch(l) {
            case L_ERROR:
                sLog->WriteMessage(AO_StrFormat("[%s]%s[%s:%i]: ", sLog->MakeTimeStamp().c_str(), ERROR.c_str(), func, line) + AO_StrFormat(str, args ...) + '\n');
                break;
            case L_WARNING:
                sLog->WriteMessage(AO_StrFormat("[%s]%s[%s:%i]: ", sLog->MakeTimeStamp().c_str(), WARNING.c_str(), func, line) + AO_StrFormat(str, args ...) + '\n');
                break;
            case L_VERBOSE:
                sLog->WriteMessage(AO_StrFormat("[%s]%s[%s:%i]: ", sLog->MakeTimeStamp().c_str(), VERBOSE.c_str(), func, line) + AO_StrFormat(str, args ...) + '\n');
                break;
            case L_DEBUG:
                sLog->WriteMessage(AO_StrFormat("[%s]%s[%s:%i]: ", sLog->MakeTimeStamp().c_str(), DEBUG.c_str(), func, line) + AO_StrFormat(str, args ...)+ '\n');
                break;
            }
        }
    }
}

#define Trace(l, ...) AO::TraceMsg(l, __FUNCTION__, __LINE__, __VA_ARGS__)

#endif // LOGGER_H
