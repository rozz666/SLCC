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

    typedef std::vector<err::Message> Errors;

    ErrorLogger(const std::string& filename) : filename_(filename) { }

    ErrorLogger& operator<<(err::Message&& msg)
    {
        errors_.push_back(msg);

        return *this;
    }

    void print(std::ostream& os);

    const Errors& errors() const { return errors_; }

	const std::string& filename() const { return filename_; }

private:

    std::string filename_;
    Errors errors_;
};

}

#endif /* SL_ERRORLOGGER_HPP */