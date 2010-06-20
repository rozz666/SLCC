#ifndef SL_AST_VARIABLE_HPP
#define SL_AST_VARIABLE_HPP

#include <string>
#include <sl/basicTypes.hpp>
#include <sl/FilePosition.hpp>

namespace sl
{
namespace ast
{

class Variable
{
public:

    Variable(const std::string& name, const FilePosition& pos, BasicType type, bool ref) : name_(name), pos_(pos), type_(type), ref_(ref) { }

    const std::string& name() const { return name_; }
    FilePosition pos() const { return pos_; }
    BasicType type() const { return type_; }
    bool ref() const { return ref_; }

private:

    std::string name_;
    FilePosition pos_;
    BasicType type_;
    bool ref_;
};

inline bool operator<(const Variable& left, const Variable& right)
{
    return left.pos() < right.pos();
}

}
}
#endif /* SL_AST_VARIABLE_HPP */
