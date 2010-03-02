#ifndef SL_ERRORLOGGER_HPP
#define SL_ERRORLOGGER_HPP

#include <sstream>
#include <vector>
#include <string>
#include <boost/preprocessor/seq/enum.hpp>
#include "FilePosition.hpp"

namespace sl
{

namespace warn
{
}

namespace err
{

struct Message
{
    FilePosition pos;
    std::string id;
    std::string text;    

    Message() { }
    Message(const FilePosition& pos, const std::string& id, const std::string& text) : pos(pos), id(id), text(text) { }
};

#define SL_ERROR_MESSAGE(name, id, args, message) \
inline Message name(const FilePosition& pos, BOOST_PP_SEQ_ENUM(args) ) \
{ \
    std::ostringstream os; \
    os << message; \
    return Message(pos, #id, os.str()); \
}

SL_ERROR_MESSAGE(unknown_identifier, E0001, (const std::string& identifier), "Unknown identifier: " << identifier)
SL_ERROR_MESSAGE(function_already_declared, E0002, (const std::string& identifier), "Function already declared: " << identifier)
SL_ERROR_MESSAGE(variable_already_declared, E0003, (const std::string& identifier), "Variable already declared: " << identifier)
SL_ERROR_MESSAGE(variable_earlier_declaration, E0003, (const std::string& identifier), "Earlier declaration of " << identifier)
SL_ERROR_MESSAGE(syntax_error, E0004, (const std::string& what), "Syntax error. Exprected " << what)
SL_ERROR_MESSAGE(function_not_found, E0005, (const std::string& what), "Function not found: " << what)
SL_ERROR_MESSAGE(void_variable, E0006, (const std::string& name), "Cannot declare a variable of type void: " << name)
SL_ERROR_MESSAGE(no_variable_type, E0007, (const std::string& name), "Cannot declare a variable with no type: " << name)
SL_ERROR_MESSAGE(not_convertible, E0008, (const std::string& from)(const std::string& to), "Cannot convert an expression of type " << from << " into type " << to)

std::ostream& operator<<(std::ostream& os, const Message& m);

}

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