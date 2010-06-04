#ifndef SL_ERR_MESSAGES_HPP
#define SL_ERR_MESSAGES_HPP

#include <ostream>
#include <sstream>
#include <boost/preprocessor/seq/enum.hpp>
#include <sl/err/Message.hpp>

namespace sl
{

namespace err
{

#define SL_ERROR_MESSAGE(name, id, args, message) \
inline Message name(const FilePosition& pos, BOOST_PP_SEQ_ENUM(args) ) \
{ \
    std::ostringstream os; \
    os << message; \
    return Message(pos, #id, os.str()); \
}

#define SL_ERROR_MESSAGE_NP(name, id, message) \
inline Message name(const FilePosition& pos) \
{ \
    std::ostringstream os; \
    os << message; \
    return Message(pos, #id, os.str()); \
}

SL_ERROR_MESSAGE(unknown_identifier, E0001, (const std::string& identifier), "Unknown identifier: " << identifier)
SL_ERROR_MESSAGE(function_already_declared, E0002, (const std::string& identifier), "Function already declared: " << identifier)
SL_ERROR_MESSAGE(function_earlier_declaration, E0002, (const std::string& identifier), "Earlier declaration of " << identifier)
SL_ERROR_MESSAGE(variable_already_declared, E0003, (const std::string& identifier), "Variable already declared: " << identifier)
SL_ERROR_MESSAGE(variable_earlier_declaration, E0003, (const std::string& identifier), "Earlier declaration of " << identifier)
SL_ERROR_MESSAGE(syntax_error, E0004, (const std::string& what), "Syntax error. Exprected " << what)
SL_ERROR_MESSAGE(function_not_found, E0005, (const std::string& what), "Function not found: " << what)
SL_ERROR_MESSAGE(void_variable, E0006, (const std::string& name), "Cannot declare a variable of type void: " << name)
SL_ERROR_MESSAGE(no_variable_type, E0007, (const std::string& name), "Cannot declare a variable with no type: " << name)
SL_ERROR_MESSAGE(not_convertible, E0008, (const std::string& from)(const std::string& to), "Cannot convert an expression of type " << from << " into type " << to)
SL_ERROR_MESSAGE_NP(bool_expr_expected, E0009, "Expected a boolean expression")
SL_ERROR_MESSAGE_NP(module_declaration_missing, E0010, "Module declaration missing");
SL_ERROR_MESSAGE_NP(missing_main, E0011, "Missing main()");
SL_ERROR_MESSAGE_NP(invalid_main_type, E0012, "main() must be void");

std::ostream& operator<<(std::ostream& os, const Message& m);

}

}

#endif /* SL_ERR_MESSAGES_HPP */