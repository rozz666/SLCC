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

enum RelOp
{
    less_,
    lessEqual_,
    greater_,
    greaterEqual_
};

enum EqOp
{
    equal_,
    notEqual_
};

struct Variable
{
    std::string name;
};

struct SignedUnaryExpression;
struct FunctionCall;
struct Expression;

typedef boost::variant<
    Constant, 
    Variable, 
    boost::recursive_wrapper<FunctionCall>,
    boost::recursive_wrapper<Expression>,
    boost::recursive_wrapper<SignedUnaryExpression>
> UnaryExpression;

struct SignedUnaryExpression
{
    Sign sign;
    UnaryExpression expr;
};

struct MulOpUnaryExpression
{
    MulOp op;
    UnaryExpression expr;
};

struct MultiplicativeExpression
{
    UnaryExpression first;
    std::vector<MulOpUnaryExpression> next;
};

struct SignMultiplicativeExpression
{
    Sign sign;
    MultiplicativeExpression expr;
};

struct AdditiveExpression
{
    MultiplicativeExpression first;
    std::vector<SignMultiplicativeExpression> next;
};

struct RelOpAdditiveExpression
{
    RelOp op;
    AdditiveExpression expr;
};

struct RelationalExpression
{
    AdditiveExpression first;
    std::vector<RelOpAdditiveExpression> next;
};

struct EqOpRelationalExpression
{
    EqOp op;
    RelationalExpression expr;
};

struct EqualityExpression
{
    RelationalExpression first;
    std::vector<EqOpRelationalExpression> next;
};

struct LogicalAndExpression
{
    EqualityExpression first;
    std::vector<EqualityExpression> next;
};

struct Expression
{
    LogicalAndExpression first;
    std::vector<LogicalAndExpression> next;
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
    ::sl::ast::SignedUnaryExpression,
    (::sl::ast::Sign, sign)
    (::sl::ast::UnaryExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::MulOpUnaryExpression,
    (::sl::ast::MulOp, op)
    (::sl::ast::UnaryExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::MultiplicativeExpression,
    (::sl::ast::UnaryExpression, first)
    (std::vector<::sl::ast::MulOpUnaryExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::SignMultiplicativeExpression,
    (::sl::ast::Sign, sign)
    (::sl::ast::MultiplicativeExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::AdditiveExpression,
    (::sl::ast::MultiplicativeExpression, first)
    (std::vector<::sl::ast::SignMultiplicativeExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::RelOpAdditiveExpression,
    (::sl::ast::RelOp, op)
    (::sl::ast::AdditiveExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::RelationalExpression,
    (::sl::ast::AdditiveExpression, first)
    (std::vector<::sl::ast::RelOpAdditiveExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::EqOpRelationalExpression,
    (::sl::ast::EqOp, op)
    (::sl::ast::RelationalExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::EqualityExpression,
    (::sl::ast::RelationalExpression, first)
    (std::vector<::sl::ast::EqOpRelationalExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::LogicalAndExpression,
    (::sl::ast::EqualityExpression, first)
    (std::vector<::sl::ast::EqualityExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::Expression,
    (::sl::ast::LogicalAndExpression, first)
    (std::vector<::sl::ast::LogicalAndExpression>, next)
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