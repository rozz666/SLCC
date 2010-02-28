#ifndef SL_GRAMMAR_HPP
#define SL_GRAMMAR_HPP

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/tuple/tuple.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/home/classic/iterator/position_iterator.hpp>

#include "ErrorLogger.hpp"
#include "ast.hpp"

namespace boost
{
namespace spirit
{
namespace traits
{
    
template <typename Exposed, typename Transformed>
struct transform_attribute<
    Exposed,
    Transformed,
    typename enable_if_c<
    fusion::result_of::size<typename enable_if<fusion::traits::is_sequence<Exposed>, Exposed>::type>::type::value == 1 &&
        is_convertible<
            typename fusion::result_of::at_c<Exposed, 0>::type,
            Transformed
        >::value &&
        is_convertible<
            Transformed,
            typename fusion::result_of::at_c<Exposed, 0>::type
        >::value 
    >::type
>
{
    typedef Transformed type;

    static Transformed pre(Exposed const& val)
    {
        return boost::fusion::at_c<0>(val);
    }                                         

    static void post(Exposed& val, Transformed const& attr)
    {
        boost::fusion::at_c<0>(val) = attr;
    }
};

}
}
}

namespace sl
{
    BOOST_SPIRIT_TERMINAL(input_pos);
}

namespace boost
{
namespace spirit 
{ 
    template <>
    struct use_terminal<qi::domain, sl::tag::input_pos> : mpl::true_  { }; 
}
}

namespace sl
{
    struct input_pos_parser : boost::spirit::qi::primitive_parser<input_pos_parser>
    {
        template <typename Context, typename Iterator>
        struct attribute
        {
            typedef sl::FilePosition type;
        };

        template <typename Iterator, typename Context, typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last, Context&, Skipper const& skipper, Attribute& attr) const
        {
            boost::spirit::qi::skip_over(first, last, skipper);
            boost::spirit::traits::assign_to(sl::FilePosition(first.get_position().line, first.get_position().column), attr);
            return true;
        }

        template <typename Context>
        boost::spirit::info what(Context&) const
        {
            return boost::spirit::info("input_pos");
        }
    };
}

namespace boost
{
namespace spirit
{
namespace qi
{
    template <typename Modifiers>
    struct make_primitive<sl::tag::input_pos, Modifiers>
    {
        typedef sl::input_pos_parser result_type;

        result_type operator()(unused_type, unused_type) const
        {
            return result_type();
        }
    };
}
}
}

namespace sl
{

namespace qi = ::boost::spirit::qi;
namespace ascii = ::boost::spirit::ascii;

namespace detail
{

struct Type : qi::symbols<char, ast::Type>
{
    Type()
    {
        add
            ("int", ast::int_)
            ("float", ast::float_)
            ("bool", ast::bool_);
    }
};

struct ReturnType : qi::symbols<char, ast::Type>
{
    ReturnType()
    {
        add
            ("int", ast::int_)
            ("float", ast::float_)
            ("bool", ast::bool_)
            ("void", ast::void_);
    }
};

struct Sign : qi::symbols<char, ast::Sign>
{
    Sign()
    {
        add
            ("+", ast::plus_)
            ("-", ast::minus_);
    }
};

struct MulOp : qi::symbols<char, ast::MulOp>
{
    MulOp()
    {
        add
            ("*", ast::mul_)
            ("/", ast::div_)
            ("%", ast::mod_);
    }
};

struct RelOp : qi::symbols<char, ast::RelOp>
{
    RelOp()
    {
        add
            ("<", ast::less_)
            ("<=", ast::lessEqual_)
            (">", ast::greater_)
            (">=", ast::greaterEqual_);
    }
};

struct EqOp : qi::symbols<char, ast::EqOp>
{
    EqOp()
    {
        add
            ("==", ast::equal_)
            ("!=", ast::notEqual_);
    }
};

struct BoolLit : qi::symbols<char, bool>
{
    BoolLit()
    {
        add
            ("true", true)
            ("false", false);
    }
};

}

struct errorMessageImpl
{
    template <typename Logger, typename It, typename What>
    struct result { typedef void type; };

    template <typename Logger, typename It, typename What>
    void operator()(Logger *logger, It it, What what) const
    {
        std::ostringstream os;
        os << what;
        *logger << err::syntax_error(FilePosition(it.get_position().line, it.get_position().column), os.str());
    }
};

boost::phoenix::function<errorMessageImpl> errorMessage;



template <typename Iterator>
struct Grammar : qi::grammar<Iterator, ast::Module(), ascii::space_type> 
{
    Grammar(ErrorLogger& errorLogger) : Grammar::base_type(module, "module"), errorLogger_(&errorLogger)
    {
        using qi::int_;
        using qi::float_;
        using qi::char_;
        using qi::lit;
        using qi::attr;
        using qi::lexeme;
        using qi::on_error;
        using qi::fail;
        using boost::phoenix::construct;
        using boost::phoenix::val;
        using boost::phoenix::ref;
        using boost::phoenix::at_c;
        using boost::phoenix::push_back;
        using namespace qi::labels;            

        identifier %= input_pos >> lexeme[char_("a-zA-Z_") >> *char_("a-zA-Z0-9_")];
        constant %= (int_ >> !char_('.')) | float_ | boolLit;
        variable %= identifier;
        unaryExpression %= constant | functionCall | variable | ('(' >> expression >> ')') | signedUnaryExpression;
        signedUnaryExpression %= sign >> unaryExpression;
        multiplicativeExpression %= unaryExpression >> *(mulOp > unaryExpression);
        additiveExpression %= multiplicativeExpression >> *(sign > multiplicativeExpression);
        relationalExpression %= additiveExpression >> *(relOp > additiveExpression);
        equalityExpression %= relationalExpression >> *(eqOp > relationalExpression); 
        andExpression = equalityExpression >> *(lit("&&") >> equalityExpression); 
        expression = andExpression >> *(lit("||") > andExpression); 
        variableDecl %= "new" > -type > identifier > -('=' > expression) > ';';
        variableDelete %= "delete" > identifier > ';';
        functionParameter %= (lit("ref") >> attr(true) | attr(false)) >> type >> identifier;
        assignment = identifier >> '=' >> expression;
        functionCall %= identifier >> '(' >> -(expression % ',') > ')';
        returnStatement %= "return" > -expression > ';';
        statement %=
            compoundStatement |
            returnStatement |
            (assignment > ';') |
            (functionCall > ';') |
            variableDecl |
            variableDelete;
        compoundStatement = '{' >> (*statement)[_val = _1] > '}';
        function %= identifier >> '(' >> -(functionParameter % ',') >> ')' >> "->" >> (returnType | (lit("typeof") >> '(' >> expression >> ')')) >> compoundStatement;
        module %= "module" > identifier > ';' >> *function >> qi::eoi;

        identifier.name("identifier");
        constant.name("constant");
        variable.name("variable");
        unaryExpression.name("unaryExpression");
        signedUnaryExpression.name("signedUnaryExpression");
        multiplicativeExpression.name("multiplicativeExpression");
        expression.name("expression");
        variableDecl.name("variable declaration");
        functionParameter.name("function parameter");
        assignment.name("assignment");
        functionCall.name("function call");
        returnStatement.name("return statement");
        statement.name("statement");
        compoundStatement.name("compound statement");
        function.name("function definition");
        module.name("module body");

        on_error<fail>
        (
            module,
            errorMessage(errorLogger_, _3, _4)
        );
    }

    ErrorLogger *errorLogger_;
    detail::Type type;
    detail::ReturnType returnType;
    detail::Sign sign;
    detail::MulOp mulOp;
    detail::RelOp relOp;
    detail::EqOp eqOp;
    detail::BoolLit boolLit;
    qi::rule<Iterator, ast::Identifier(), ascii::space_type> identifier;
    qi::rule<Iterator, ast::Constant(), ascii::space_type> constant;
    qi::rule<Iterator, ast::Variable(), ascii::space_type> variable;
    qi::rule<Iterator, ast::UnaryExpression(), ascii::space_type> unaryExpression;
    qi::rule<Iterator, ast::SignedUnaryExpression(), ascii::space_type> signedUnaryExpression;
    qi::rule<Iterator, ast::MultiplicativeExpression(), ascii::space_type> multiplicativeExpression;
    qi::rule<Iterator, ast::AdditiveExpression(), ascii::space_type> additiveExpression;
    qi::rule<Iterator, ast::RelationalExpression(), ascii::space_type> relationalExpression;
    qi::rule<Iterator, ast::EqualityExpression(), ascii::space_type> equalityExpression;
    qi::rule<Iterator, ast::LogicalAndExpression(), ascii::space_type> andExpression;
    qi::rule<Iterator, ast::Expression(), ascii::space_type> expression;
    qi::rule<Iterator, ast::VariableDecl(), ascii::space_type> variableDecl;
    qi::rule<Iterator, ast::VariableDelete(), ascii::space_type> variableDelete;
    qi::rule<Iterator, ast::FunctionParameter(), ascii::space_type> functionParameter;
    qi::rule<Iterator, ast::Assignment(), ascii::space_type> assignment;
    qi::rule<Iterator, ast::FunctionCall(), ascii::space_type> functionCall;
    qi::rule<Iterator, ast::ReturnStatement(), ascii::space_type> returnStatement;
    qi::rule<Iterator, ast::Statement(), ascii::space_type> statement;
    qi::rule<Iterator, ast::CompoundStatement(), ascii::space_type> compoundStatement;
    qi::rule<Iterator, ast::Function(), ascii::space_type> function;
    qi::rule<Iterator, ast::Module(), ascii::space_type> module;
};

}

#endif /* SL_GRAMMAR_HPP */