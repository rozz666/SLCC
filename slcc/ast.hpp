#ifndef SL_AST_HPP
#define SL_AST_HPP

#include <string>
#include <vector>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include "FilePosition.hpp"

namespace sl
{
namespace ast
{

struct Constant
{
    typedef boost::variant<int, float, bool> Value;
    FilePosition pos;
    Value value;
};

enum Type
{
    int_,
    float_,
    bool_,
    void_
};

enum UnOp
{
    plus_,
    minus_,
    lnot_
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

enum LAndOp
{
    and_
};

enum LOrOp
{
    or_
};

struct Identifier
{
    FilePosition pos;
    std::string str;
};

struct Variable
{
    Identifier name;
};

struct UnOpUnaryExpression;
struct FunctionCall;
struct Expression;

typedef boost::variant<
    Constant, 
    Variable, 
    boost::recursive_wrapper<FunctionCall>,
    boost::recursive_wrapper<Expression>,
    boost::recursive_wrapper<UnOpUnaryExpression>
> UnaryExpression;

struct UnOpUnaryExpression
{
    FilePosition opPos;
    UnOp op;
    FilePosition exprPos;
    UnaryExpression expr;
};

struct MulOpUnaryExpression
{
    FilePosition opPos;
    MulOp op;
    FilePosition exprPos;
    UnaryExpression expr;
};

struct MultiplicativeExpression
{
    FilePosition firstPos;
    UnaryExpression first;
    std::vector<MulOpUnaryExpression> next;
};

struct SignMultiplicativeExpression
{
    FilePosition signPos;
    UnOp sign;
    FilePosition exprPos;
    MultiplicativeExpression expr;
};

struct AdditiveExpression
{
    FilePosition firstPos;
    MultiplicativeExpression first;
    std::vector<SignMultiplicativeExpression> next;
};

struct RelOpAdditiveExpression
{
    FilePosition opPos;
    RelOp op;
    FilePosition exprPos;
    AdditiveExpression expr;
};

struct RelationalExpression
{
    FilePosition firstPos;
    AdditiveExpression first;
    std::vector<RelOpAdditiveExpression> next;
};

struct EqOpRelationalExpression
{
    FilePosition opPos;
    EqOp op;
    FilePosition exprPos;
    RelationalExpression expr;
};

struct EqualityExpression
{
    FilePosition firstPos;
    RelationalExpression first;
    std::vector<EqOpRelationalExpression> next;
};

struct LAndOpEqualityExpression
{
    FilePosition opPos;
    LAndOp op;
    FilePosition exprPos;
    EqualityExpression expr;
};

struct LogicalAndExpression
{
    FilePosition firstPos;
    EqualityExpression first;
    std::vector<LAndOpEqualityExpression> next;
};

struct LOrOpLogicalAndExpression
{
    FilePosition opPos;
    LOrOp op;
    FilePosition exprPos;
    LogicalAndExpression expr;
};

struct Expression
{
    FilePosition firstPos;
    LogicalAndExpression first;
    std::vector<LOrOpLogicalAndExpression> next;
};

struct VariableDecl
{
    boost::optional<Type> type;
    Identifier name;
    boost::optional<Expression> expr;
};

struct VariableDelete
{
    Identifier name;
};

struct FunctionParameter
{
    bool ref;
    Type type;
    Identifier name;
};

struct Assignment
{
    Identifier var;
    Expression expr;
};

struct FunctionCall
{
    Identifier name;
    std::vector<Expression> expr;
};

struct ReturnStatement
{
    Expression expr;
};

struct CompoundStatement;
struct IfStatement;

typedef boost::variant<
    boost::recursive_wrapper<CompoundStatement>, 
    boost::recursive_wrapper<IfStatement>,
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

struct IfStatement
{
    Expression cond;
    CompoundStatement onTrue;
    boost::optional<CompoundStatement> onFalse;
};

typedef boost::variant<Type, Expression> FunctionReturnType;

struct Function
{
    Identifier name;
    std::vector<FunctionParameter> parameters;
    FunctionReturnType type;
    CompoundStatement body;
};

struct Module
{
    Identifier name;
    std::vector<Function> functions;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::Constant,
    (::sl::FilePosition, pos)
    (::sl::ast::Constant::Value, value)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::Identifier,
    (::sl::FilePosition, pos)
    (std::string, str)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::Variable,
    (::sl::ast::Identifier, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::UnOpUnaryExpression,
    (::sl::FilePosition, opPos)
    (::sl::ast::UnOp, op)
    (::sl::FilePosition, exprPos)
    (::sl::ast::UnaryExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::MulOpUnaryExpression,
    (::sl::FilePosition, opPos)
    (::sl::ast::MulOp, op)
    (::sl::FilePosition, exprPos)
    (::sl::ast::UnaryExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::MultiplicativeExpression,
    (::sl::FilePosition, firstPos)
    (::sl::ast::UnaryExpression, first)
    (std::vector<::sl::ast::MulOpUnaryExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::SignMultiplicativeExpression,
    (::sl::FilePosition, signPos)
    (::sl::ast::UnOp, sign)
    (::sl::FilePosition, exprPos)
    (::sl::ast::MultiplicativeExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::AdditiveExpression,
    (::sl::FilePosition, firstPos)
    (::sl::ast::MultiplicativeExpression, first)
    (std::vector<::sl::ast::SignMultiplicativeExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::RelOpAdditiveExpression,
    (::sl::FilePosition, opPos)
    (::sl::ast::RelOp, op)
    (::sl::FilePosition, exprPos)
    (::sl::ast::AdditiveExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::RelationalExpression,
    (::sl::FilePosition, firstPos)
    (::sl::ast::AdditiveExpression, first)
    (std::vector<::sl::ast::RelOpAdditiveExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::EqOpRelationalExpression,
    (::sl::FilePosition, opPos)
    (::sl::ast::EqOp, op)
    (::sl::FilePosition, exprPos)
    (::sl::ast::RelationalExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::EqualityExpression,
    (::sl::FilePosition, firstPos)
    (::sl::ast::RelationalExpression, first)
    (std::vector<::sl::ast::EqOpRelationalExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::LAndOpEqualityExpression,
    (::sl::FilePosition, opPos)
    (::sl::ast::LAndOp, op)
    (::sl::FilePosition, exprPos)
    (::sl::ast::EqualityExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::LogicalAndExpression,
    (::sl::FilePosition, firstPos)
    (::sl::ast::EqualityExpression, first)
    (std::vector<::sl::ast::LAndOpEqualityExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::LOrOpLogicalAndExpression,
    (::sl::FilePosition, opPos)
    (::sl::ast::LOrOp, op)
    (::sl::FilePosition, exprPos)
    (::sl::ast::LogicalAndExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::Expression,
    (::sl::FilePosition, firstPos)
    (::sl::ast::LogicalAndExpression, first)
    (std::vector<::sl::ast::LOrOpLogicalAndExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::VariableDecl,
    (boost::optional<::sl::ast::Type>, type)
    (::sl::ast::Identifier, name)
    (boost::optional<::sl::ast::Expression>, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::VariableDelete,
    (::sl::ast::Identifier, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::FunctionParameter,
    (bool, ref)
    (::sl::ast::Type, type)
    (::sl::ast::Identifier, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::Assignment,
    (::sl::ast::Identifier, var)
    (::sl::ast::Expression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::FunctionCall,
    (::sl::ast::Identifier, name)
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
    ::sl::ast::IfStatement,
    (::sl::ast::Expression, cond)
    (::sl::ast::CompoundStatement, onTrue)
    (boost::optional<::sl::ast::CompoundStatement>, onFalse)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::Function,
    (::sl::ast::Identifier, name)
    (std::vector<::sl::ast::FunctionParameter>, parameters)
    (::sl::ast::FunctionReturnType, type)
    (::sl::ast::CompoundStatement, body)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::ast::Module,
    (::sl::ast::Identifier, name)
    (std::vector<::sl::ast::Function>, functions)
)

#endif /* SL_AST_HPP */