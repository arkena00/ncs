#ifndef INCLUDE_NCS_UTILITY_HPP_NCS
#define INCLUDE_NCS_UTILITY_HPP_NCS

#include <string>

namespace ncs
{
    inline bool str_starts_with(const std::vector<char>& data, const std::string& input)
    {
        return std::equal(input.begin(), input.end(), data.begin());
    }

    //template<class T>
    inline void str_replace(std::string& str, const std::string& from, const std::string& to)
    {
        if (from.empty()) return;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }

    inline void str_replace(std::vector<char>& data, const std::string& from, const std::string& to)
    {
        if (from.empty()) return;
        if (data.size() < from.size() + to.size()) return;

        while (true)
        {
            std::vector<char> out;
            out.resize(data.size() - from.size() + to.size());

            auto found_pos = std::search(data.begin(), data.end(), from.begin(), from.end());
            if (found_pos == data.end()) return;

            auto out_pos = std::copy(data.begin(), found_pos, out.begin());
            std::copy(to.begin(), to.end(), out_pos);
            std::copy(found_pos + from.size(), data.end(), out_pos + to.size());

            std::swap(data, out);
        }
    }
} // ncs

#endif // INCLUDE_NCS_UTILITY_HPP_NCS