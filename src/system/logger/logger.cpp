#include "logger.h"

namespace AO
{
    Logger::Logger() {}

    Logger::~Logger() {}

    bool Logger::Open(const std::string& logfile, E_LOG_LEVEL logLevel) {
        m_Filename = logfile;
        m_LogLevel = logLevel;

        m_pFile = new std::ofstream();
        m_pFile->open(m_Filename, std::ios_base::app);
        if(!m_pFile->is_open()) {
            m_pFile->open(m_Filename, std::ios_base::out);
            if(!m_pFile->is_open())
                return false;
        }
        return true;
    }

    void Logger::Close() {
        if(m_pFile) {
            if(m_pFile->is_open()) {
                m_pFile->close();
            }
            delete m_pFile;
        }
    }
}
