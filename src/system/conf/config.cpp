#include "config.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <boost/algorithm/string.hpp>

namespace AO {

    ConfParser::ConfParser() : m_Parsed(false), m_OptionsCount(0), m_Filename("") {}

    void ConfParser::SetFilename(const std::string& filename)
    {
        std::lock_guard<std::mutex> guard(m_Mutex);
        if(filename.empty())
            std::cout << "Configuration file not specifyed. Exit...\n";
        m_Filename = filename;
    }

    bool ConfParser::Read()
    {
        std::lock_guard<std::mutex> guard(m_Mutex);
        if(!m_Storage.empty())
        {
            m_Storage.clear();
            m_Parsed = false;
        }
        if(m_Filename.empty())
        {
            return false;
        }

        std::ifstream file;
        file.open(m_Filename, std::ios_base::in);
        if(!file.is_open())
        {
            return false;
        }
        std::vector<std::string> splitted;
        std::string line;
        std::getline(file, line);
        do
        {
            if(file.eof())
            {
                break;
            }
            if(line != "" && line != "\n" && (!boost::starts_with(line, "#")))
            {
                boost::split(splitted, line, boost::is_any_of("="));
                if(splitted.size() == 2) m_Storage[splitted[0]] = ConfElement(splitted[1], ++m_OptionsCount);
                splitted.clear();
            }
            splitted.clear();
            line.clear();
            std::getline(file, line);
        } while(true);

        file.close();
        m_Parsed = true;
        return true;
    }

    void ConfParser::Reset()
    {
        std::lock_guard<std::mutex> guard(m_Mutex);
        m_Filename = "";
        m_Parsed = false;
        m_OptionsCount = 0;
        m_Storage.clear();
    }

    bool ConfParser::GetBoolOption(const std::string& name, bool default_value) const
    {
        std::lock_guard<std::mutex> guard(m_Mutex);
        auto it = m_Storage.find(name);
        if(it == m_Storage.end())
        {
            return default_value;
        }
        std::string value = it->second.value;
        if(value == "True" || value == "true" || value == "1")
        {
            return true;
        }
        else if(value == "False" || value == "false" || value == "0")
        {
            return false;
        }
        else
        {
            return default_value;
        }
    }

    short ConfParser::GetShortOption(const std::string& name, short default_value) const
    {
        std::lock_guard<std::mutex> guard(m_Mutex);
        return m_GetOption<short>(name, default_value);
    }

    unsigned short ConfParser::GetUShortOption(const std::string& name, unsigned short default_value) const
    {
        std::lock_guard<std::mutex> guard(m_Mutex);
        return m_GetOption<unsigned short>(name, default_value);
    }

    int ConfParser::GetIntOption(const std::string& name, int default_value) const
    {
        std::lock_guard<std::mutex> guard(m_Mutex);
        return m_GetOption<int>(name, default_value);
    }

    unsigned int ConfParser::GetUIntOption(const std::string& name, unsigned int default_value) const
    {
        std::lock_guard<std::mutex> guard(m_Mutex);
        return m_GetOption<unsigned int>(name, default_value);
    }

    long long ConfParser::GetLongOption(const std::string& name, long long default_value) const
    {
        std::lock_guard<std::mutex> guard(m_Mutex);
        return m_GetOption<long long>(name, default_value);
    }

    unsigned long long ConfParser::GetULongOption(const std::string& name, unsigned long long default_value) const
    {
        std::lock_guard<std::mutex> guard(m_Mutex);
        return m_GetOption<unsigned long long>(name, default_value);
    }

    float ConfParser::GetFloatOption(const std::string& name, float default_value) const
    {
        std::lock_guard<std::mutex> guard(m_Mutex);
        return m_GetOption<float>(name, default_value);
    }

    double ConfParser::GetDoubleOption(const std::string& name, double default_value) const
    {
        std::lock_guard<std::mutex> guard(m_Mutex);
        return m_GetOption<double>(name, default_value);
    }

    std::string ConfParser::GetStringOption(const std::string& name, std::string default_value) const
    {
        std::lock_guard<std::mutex> guard(m_Mutex);
        auto it = m_Storage.find(name);
        if(it == m_Storage.end())
        {
            return default_value;
        }
        return it->second.value;
    }

}
