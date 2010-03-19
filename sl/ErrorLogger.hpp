#ifndef SL_ERRORLOGGER_HPP
#define SL_ERRORLOGGER_HPP

#include <sstream>
#include <vector>
#include <string>
#include <sl/FilePosition.hpp>
#include <sl/err/Message.hpp>

namespace sl
{

class ErrorLogger
{
public:

    ErrorLogger(const std::string& filename) : filename_(filename) { }

    ErrorLogger& operator<<(err::Message&& msg)
    {
        errors_.push_back(msg);

        return *this;
    }

    void sort();

    void print(std::ostream& os);

    bool hasErrors() { return !errors_.empty(); }

private:

    std::string filename_;
    std::vector<err::Message> errors_;
};

}

#endif /* SL_ERRORLOGGER_HPP */