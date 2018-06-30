#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <mutex>
#include <boost/lexical_cast.hpp>

#include "../singleton.h"

namespace AO
{

    struct ConfElement
    {
        ConfElement() : value(""), index(0U) {}
        ConfElement(const std::string& val, unsigned int idx) : value(val), index(idx) {}
        std::string value;
        unsigned int index;
    };

    class ConfParser : public ISingleton<ConfParser>
    {
    public:
        ConfParser();
        ~ConfParser() {}

        void SetFilename(const std::string& filename);
        bool Read();
        void Reset();
        bool GetBoolOption(const std::string& name, bool default_value = false) const;
        short GetShortOption(const std::string& name, short default_value = 0) const;
        unsigned short GetUShortOption(const std::string& name, unsigned short default_value = 0U) const;
        int GetIntOption(const std::string& name, int default_value = 0) const;
        unsigned int GetUIntOption(const std::string& name, unsigned int default_value = 0U) const;
        long long GetLongOption(const std::string& name, long long default_value = 0L) const;
        unsigned long long GetULongOption(const std::string& name, unsigned long long default_value = 0UL) const;
        float GetFloatOption(const std::string& name, float default_value = 0.0f) const;
        double GetDoubleOption(const std::string& name, double default_value = 0.0) const;
        std::string GetStringOption(const std::string& name, std::string default_value = "") const;

    private:
        template <typename T>
        T m_GetOption(const std::string& name, T default_value) const
        {
            auto it = m_Storage.find(name);
            if(it == m_Storage.end())
            {
                return default_value;
            }
            T val;
            try
            {
                val = boost::lexical_cast<T>(it->second.value);
                if(val < std::numeric_limits<T>::min() || val > std::numeric_limits<T>::max())
                {
                    return default_value;
                }
            }
            catch(boost::bad_lexical_cast)
            {
                return default_value;
            }
            return val;
        }

        bool m_Parsed;
        unsigned short m_OptionsCount;
        std::string m_Filename;
        std::map<std::string, ConfElement> m_Storage;
        mutable std::mutex m_Mutex;
    };

}

#define sConf AO::ConfParser::Instance()

#endif // !CONFIG_H
