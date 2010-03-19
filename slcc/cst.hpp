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
namespace cst
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
struct WhileLoop;

typedef boost::variant<
    boost::recursive_wrapper<CompoundStatement>, 
    boost::recursive_wrapper<IfStatement>,
    boost::recursive_wrapper<WhileLoop>,
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

struct WhileLoop
{
    Expression cond;
    CompoundStatement body;
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
    ::sl::cst::Constant,
    (::sl::FilePosition, pos)
    (::sl::cst::Constant::Value, value)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::Identifier,
    (::sl::FilePosition, pos)
    (std::string, str)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::Variable,
    (::sl::cst::Identifier, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::UnOpUnaryExpression,
    (::sl::FilePosition, opPos)
    (::sl::cst::UnOp, op)
    (::sl::FilePosition, exprPos)
    (::sl::cst::UnaryExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::MulOpUnaryExpression,
    (::sl::FilePosition, opPos)
    (::sl::cst::MulOp, op)
    (::sl::FilePosition, exprPos)
    (::sl::cst::UnaryExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::MultiplicativeExpression,
    (::sl::FilePosition, firstPos)
    (::sl::cst::UnaryExpression, first)
    (std::vector<::sl::cst::MulOpUnaryExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::SignMultiplicativeExpression,
    (::sl::FilePosition, signPos)
    (::sl::cst::UnOp, sign)
    (::sl::FilePosition, exprPos)
    (::sl::cst::MultiplicativeExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::AdditiveExpression,
    (::sl::FilePosition, firstPos)
    (::sl::cst::MultiplicativeExpression, first)
    (std::vector<::sl::cst::SignMultiplicativeExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::RelOpAdditiveExpression,
    (::sl::FilePosition, opPos)
    (::sl::cst::RelOp, op)
    (::sl::FilePosition, exprPos)
    (::sl::cst::AdditiveExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::RelationalExpression,
    (::sl::FilePosition, firstPos)
    (::sl::cst::AdditiveExpression, first)
    (std::vector<::sl::cst::RelOpAdditiveExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::EqOpRelationalExpression,
    (::sl::FilePosition, opPos)
    (::sl::cst::EqOp, op)
    (::sl::FilePosition, exprPos)
    (::sl::cst::RelationalExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::EqualityExpression,
    (::sl::FilePosition, firstPos)
    (::sl::cst::RelationalExpression, first)
    (std::vector<::sl::cst::EqOpRelationalExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::LAndOpEqualityExpression,
    (::sl::FilePosition, opPos)
    (::sl::cst::LAndOp, op)
    (::sl::FilePosition, exprPos)
    (::sl::cst::EqualityExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::LogicalAndExpression,
    (::sl::FilePosition, firstPos)
    (::sl::cst::EqualityExpression, first)
    (std::vector<::sl::cst::LAndOpEqualityExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::LOrOpLogicalAndExpression,
    (::sl::FilePosition, opPos)
    (::sl::cst::LOrOp, op)
    (::sl::FilePosition, exprPos)
    (::sl::cst::LogicalAndExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::Expression,
    (::sl::FilePosition, firstPos)
    (::sl::cst::LogicalAndExpression, first)
    (std::vector<::sl::cst::LOrOpLogicalAndExpression>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::VariableDecl,
    (boost::optional<::sl::cst::Type>, type)
    (::sl::cst::Identifier, name)
    (boost::optional<::sl::cst::Expression>, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::VariableDelete,
    (::sl::cst::Identifier, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::FunctionParameter,
    (bool, ref)
    (::sl::cst::Type, type)
    (::sl::cst::Identifier, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::Assignment,
    (::sl::cst::Identifier, var)
    (::sl::cst::Expression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::FunctionCall,
    (::sl::cst::Identifier, name)
    (std::vector<::sl::cst::Expression>, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::ReturnStatement,
    (::sl::cst::Expression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::CompoundStatement,
    (std::vector<::sl::cst::Statement>, statements)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::IfStatement,
    (::sl::cst::Expression, cond)
    (::sl::cst::CompoundStatement, onTrue)
    (boost::optional<::sl::cst::CompoundStatement>, onFalse)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::WhileLoop,
    (::sl::cst::Expression, cond)
    (::sl::cst::CompoundStatement, body)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::Function,
    (::sl::cst::Identifier, name)
    (std::vector<::sl::cst::FunctionParameter>, parameters)
    (::sl::cst::FunctionReturnType, type)
    (::sl::cst::CompoundStatement, body)
)

BOOST_FUSION_ADAPT_STRUCT(
    ::sl::cst::Module,
    (::sl::cst::Identifier, name)
    (std::vector<::sl::cst::Function>, functions)
)

#endif /* SL_AST_HPP */