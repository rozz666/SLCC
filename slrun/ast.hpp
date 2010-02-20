#ifndef SL_AST_HPP
#define SL_AST_HPP

#include <string>
#include <vector>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>

namespace sl
{
namespace ast
{

typedef boost::variant<int, float, bool> Constant;

enum Type
{
    int_,
    float_,
    bool_
};

enum Sign
{
    plus_,
    minus_
};

enum MulOp
{
    mul_,
    div_,
    mod_
};

struct Variable
{
    std::string name;
};

struct SignedFactor;
struct FunctionCall;
struct Expression;

typedef boost::variant<
    Constant, 
    Variable, 
    boost::recursive_wrapper<FunctionCall>,
    boost::recursive_wrapper<Expression>,
    boost::recursive_wrapper<SignedFactor>
> Factor;

struct SignedFactor
{
    Sign sign;
    Factor factor;
};

struct MulOpFactor
{
    MulOp op;
    Factor factor;
};

struct Term
{
    Factor first;
    std::vector<MulOpFactor> next;
};

struct SignTerm
{
    Sign sign;
    Term term;
};

struct Expression
{
    Term first;
    std::vector<SignTerm> next;
};

struct VariableDecl
{
    boost::optional<Type> type;
    std::string name;
    boost::optional<Expression> expr;
};

struct VariableDelete
{
    std::string name;
};

struct FunctionParameter
{
    bool ref;
    Type type;
    std::string name;
};

struct Assignment
{
    std::string var;
    Expression expr;
};

struct FunctionCall
{
    std::string name;
    std::vector<Expression> expr;
};

struct ReturnStatement
{
    Expression expr;
};

struct CompoundStatement;

typedef boost::variant<
    boost::recursive_wrapper<CompoundStatement>, 
    Assignment, 
    FunctionCall, 
    ReturnStatement,
    VariableDecl,
    VariableDelete
> Statement;

struct CompoundStatement
{
    std::vector<Statement> statements;

    CompoundStatement() { }
    CompoundStatement(const std::vector<Statement>& s) : statements(s) { }
};

typedef boost::variant<Type, Expression> FunctionReturnType;

struct Function
{
    std::string name;
    std::vector<FunctionParameter> parameters;
    FunctionReturnType type;
    CompoundStatement body;
};

struct Module
{
    std::string name;
    std::vector<Function> functions;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::Variable,
    (std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::SignedFactor,
    (::sl::ast::Sign, sign)
    (::sl::ast::Factor, factor)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::MulOpFactor,
    (::sl::ast::MulOp, op)
    (::sl::ast::Factor, factor)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::Term,
    (::sl::ast::Factor, first)
    (std::vector<::sl::ast::MulOpFactor>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::SignTerm,
    (::sl::ast::Sign, sign)
    (::sl::ast::Term, term)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::Expression,
    (::sl::ast::Term, first)
    (std::vector<::sl::ast::SignTerm>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::VariableDecl,
    (boost::optional<::sl::ast::Type>, type)
    (std::string, name)
    (boost::optional<::sl::ast::Expression>, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::VariableDelete,
    (std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::FunctionParameter,
    (bool, ref)
    (::sl::ast::Type, type)
    (std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::Assignment,
    (std::string, var)
    (::sl::ast::Expression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::FunctionCall,
    (std::string, name)
    (std::vector<::sl::ast::Expression>, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::ReturnStatement,
    (::sl::ast::Expression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::CompoundStatement,
    (std::vector<::sl::ast::Statement>, statements)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::Function,
    (std::string, name)
    (std::vector<::sl::ast::FunctionParameter>, parameters)
    (::sl::ast::FunctionReturnType, type)
    (::sl::ast::CompoundStatement, body)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::Module,
    (std::string, name)
    (std::vector<::sl::ast::Function>, functions)
)

#endif /* SL_AST_HPP */